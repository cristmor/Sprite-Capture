#include "App.h"
#include "imgui.h"

App::App() {
	window = std::make_shared<sf::RenderWindow>(sf::VideoMode(windowSizeX, windowSizeY), windowTitle);
	if(!ImGui::SFML::Init(*window)) {
		std::cerr << "Error: Failed to Load ImGui" << std::endl;
	}

	for(const auto& entry: std::filesystem::recursive_directory_iterator(assetPath)) {
		if(entry.is_regular_file()) {
			fileList.push_back(entry.path().filename().string());
			filepathList.push_back(entry.path().string());
		}
	}
	for(const auto& file:fileList) {
		fileListChar.push_back(file.c_str());
	}

	texture.loadFromFile(filepathList[indexFile]);
	sprite = std::make_shared<sf::Sprite>(texture);
	sprite->setScale(scale, scale);

	p1.setRadius(radius);
	p2.setRadius(radius);
	p1.setFillColor(sf::Color(255, 0, 0, 255));
	p2.setFillColor(sf::Color(255, 0, 0, 255));
	p1.setOrigin({radius, radius});
	p2.setOrigin({radius, radius});

	rect.setFillColor(sf::Color(0, 0, 0, 0));
	rect.setOutlineColor(sf::Color(255, 255, 255, 255));
	rect.setOutlineThickness(2.0f);

	textureOutline.setFillColor(sf::Color(0, 0, 0, 0));
	textureOutline.setOutlineColor(sf::Color(255, 255, 255, 255));
	textureOutline.setOutlineThickness(2.0f);
	for(int i = 0;i < 10;i++) {
		sf::RectangleShape newRect;
		newRect.setFillColor(sf::Color(0, 0, 0, 0));
		newRect.setOutlineColor(sf::Color(0, 255, 0, 255));
		newRect.setOutlineThickness(2.0f);
		rects.push_back(newRect);
	}

	boundingBox.setFillColor(sf::Color(0, 0, 0, 0));
	boundingBox.setOutlineColor(sf::Color(0, 0, 255, 255));
	boundingBox.setOutlineThickness(2.0f);

	output.texturePath = filepathList[indexFile];
	output.textureTag = "textureTag";
	output.animationTag = "animationTag";
	output.frameCount = 0;
	output.frameTime = 0;
	output.size = 48;
	output.row = 1;
	imguiSize = {700, 900};
}

// Public
void App::run() {

	sf::Clock deltaClock;
	while(window->isOpen()) {

		// Inputs
		inputs();

		// ImGui
		ImGui::SFML::Update(*window, deltaClock.restart());
		ImGui::SetNextWindowPos(imguiPos);
		ImGui::SetNextWindowSize(imguiSize);
		ImGui::Begin("Control Panel");
		loadTexture();
		displayInfo();
		calculator();
		boxSettings();
		setBoundingBox();
		outputValues();
		viewOutputFile();
		ImGui::End();

		// Set Data
		mousePosition = sf::Mouse::getPosition(*window);
		rect.setPosition(p1.getPosition().x, p1.getPosition().y);
		rect.setSize({p2.getPosition().x - p1.getPosition().x, p2.getPosition().y - p1.getPosition().y});

		// Render
		windowChange();
		render();
	}
}

// Private
// ImGui
void App::loadTexture() {
	ImGui::SeparatorText("Load Texture");

	static std::string path = "Texture Path: " + filepathList[indexFile];
	static int size = path.size() + 1;
	char label[size];
	std::strncpy(label, path.c_str(), size);
	ImGui::Text(label, IM_ARRAYSIZE(label));

	if(!fileListChar.empty()) {
		if(ImGui::Combo("Selected File", &indexFile, fileListChar.data(), fileListChar.size())) {
			texture.loadFromFile(filepathList[indexFile]);
		}
	}
}

void App::displayInfo() {
	ImGui::SeparatorText("Display Info");
	ImGui::Text("Window Dimensions   x: %dpx y: %dpx", window->getSize().x, window->getSize().y);
	ImGui::Text("Image Dimensions   x: %dpx y: %dpx", sprite->getTexture()->getSize().x * static_cast<int>(scale), sprite->getTexture()->getSize().y * static_cast<int>(scale));
}

void App::calculator() {
	static int num[3] = {static_cast<int>(scale), output.size, static_cast<int>(scale)*output.size};

	ImGui::SeparatorText("Basic Multiplication Calculator");
	std::string label = std::to_string(num[2]);
	if(ImGui::InputInt2(" = ", num)) {
		num[2] = num[0] * num[1];
	}
	ImGui::SameLine();
	ImGui::Button(label.c_str());

	dragPayload("DRAG", num[2]);
}

void App::boxSettings() {

	// Circle Locks
	ImGui::SeparatorText("Box Settings");
	ImGui::Checkbox("Lock Point 1", &p1Lock);
	ImGui::SameLine();
	ImGui::Checkbox("Lock Point 2", &p2Lock);

	// Cicle Positions
	static int pos1[2] = {static_cast<int>(p1.getPosition().x), static_cast<int>(p1.getPosition().y)};
	static int pos2[2] = {static_cast<int>(p2.getPosition().x), static_cast<int>(p2.getPosition().y)};

	pos1[0] = p1.getPosition().x;
	pos1[1] = p1.getPosition().y;
	pos2[0] = p2.getPosition().x;
	pos2[1] = p2.getPosition().y;

	if(p1Lock) {
		ImGui::BeginDisabled();
	}
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":x1", &pos1[0]);
	dropTarget("DRAG", pos1[0]);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":y1", &pos1[1]);
	dropTarget("DRAG", pos1[1]);
	if(p1Lock) {
		ImGui::EndDisabled();
	}

	if(p2Lock) {
		ImGui::BeginDisabled();
	}
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":x2", &pos2[0]);
	dropTarget("DRAG", pos2[0]);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":y2", &pos2[1]);
	dropTarget("DRAG", pos2[1]);	
	if(p2Lock) {
		ImGui::EndDisabled();
	}


	// Box Row
	static int indexRow = 0;
	static int maxRow;
	maxRow = (texture.getSize().y)/(output.size);
	if(!maxRow) {
		maxRow = 1;
	}
	ImGui::SliderInt("Set Row", &indexRow, 0, maxRow);
	if(indexRow) {
		pos1[0] = 0;
		pos1[1] = (output.size * scale) * (indexRow - 1);
		pos2[0] = (texture.getSize().x * scale);
		pos2[1] = (output.size * scale) * indexRow;
		rActive = true;
	}

	// Box Splits
	ImGui::SliderInt("Set Splits", &splits, 0, 10);
	for(int i = 1; i <= splits;i++) {
		rects[i-1].setPosition(rect.getPosition());
		rects[i-1].setSize({(rect.getSize().x/splits)*i, rect.getSize().y});
	}	

	// Reset Button
	if(ImGui::Button("Reset")) {
		p1Lock = false;
		p2Lock = false;
		pos1[0] = 0;
		pos1[1] = 0;
		pos2[0] = 0;
		pos2[1] = 0;
		p1Active = false;
		p2Active = false;
		rActive = false;
		splits = 0;
	}

	p1.setPosition(pos1[0], pos1[1]);
	p2.setPosition(pos2[0], pos2[1]);	


}

void App::setBoundingBox() {
	ImGui::SeparatorText("Bounding Box");

	static int pos[2] = {0};
	static int boundingSize[2] = {0};

	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":x Offset", &pos[0]);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":y Offset", &pos[1]);

	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":x Size", &boundingSize[0]);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragInt(":y Size", &boundingSize[1]);


	if(rActive) {
		boundingBox.setOrigin({boundingBox.getSize().x/2.0f, boundingBox.getSize().y/2.0f});
		boundingBox.setPosition({rect.getPosition().x + rect.getSize().x/(2.0f * splits) + pos[0], rect.getPosition().y + rect.getSize().y/2.0f + pos[1]});
		boundingBox.setSize({static_cast<float>(boundingSize[0]), static_cast<float>(boundingSize[1])});
	}
	
}

void App::outputValues() {
	ImGui::SeparatorText("Output Values");

	output.frameCount = splits;
	output.row = p2.getPosition().y / (output.size * scale);

	char buf[32];

	std::strncpy(buf, output.textureTag.c_str(), 32);
	ImGui::InputText("Texture Tag", buf, IM_ARRAYSIZE(buf));
	output.textureTag = buf;

	std::strncpy(buf, output.animationTag.c_str(), 32);
	ImGui::InputText("AnimaitonTag", buf, IM_ARRAYSIZE(buf));
	output.animationTag = buf;

	ImGui::InputInt("Frame Time", &output.frameTime);

	static int count = output.frameCount;
	ImGui::SliderInt("Frame Count", &count, 0, splits);
	output.frameCount = count;


	enum sizeOptions {Size_8, Size_12, Size_16, Size_32, Size_48, Size_64, Size_COUNT};
	static int sizeIndex = Size_48;
	const char* sizeNames[Size_COUNT] = {"8x8 Pixels", "12x12 Pixels", "16x16 Pixels", "32x32 Pixels", "48x48 Pixels", "64x64 Pixels"};
	const char* sizeName = (sizeIndex >=0 && sizeIndex < Size_COUNT) ? sizeNames[sizeIndex]: "Size Unknowm";

	if(ImGui::SliderInt("Frame Size/Sprite Size", &sizeIndex, 0, Size_COUNT - 1, sizeName)) {
		switch (sizeIndex) {
			case Size_8:
				output.size = 8;
				break;
			case Size_12:
				output.size = 12;
				break;
			case Size_16:
				output.size = 16;
				break;
			case Size_32:
				output.size = 32;
				break;
			case Size_48:
				output.size = 48;
				break;
			case Size_64:
				output.size = 64;
				break;
		}
	}

	ImGui::BeginDisabled();
	ImGui::InputInt("Frame Row", &output.row);
	ImGui::EndDisabled();

	std::string condition = ((outputFile.is_open()) ? "(Opened)": "(Closed)");
	std::strncpy(buf, outputFilename.c_str(), 32);
	ImGui::InputText(("Output File " + condition).c_str(), buf, IM_ARRAYSIZE(buf));
	outputFilename = buf;

	if(ImGui::Button("Open File")) {
		outputFile.open(outputFilename, std::ios::app);
	}
	ImGui::SameLine();
	if(ImGui::Button("Append File")) {
		if(!outputFile.is_open()) {
			outputFile.open(outputFilename, std::ios::app);
			outputFile << output.textureTag << " " 
				   << output.animationTag << " " 
				   << output.frameCount << " " 
				   << output.frameTime << " " 
				   << output.size << " " 
				   << output.row << " " 
				   << boundingBox.getSize().x/scale << " "
				   << boundingBox.getSize().y/scale << " "
				   << ((splits) ? (boundingBox.getPosition().x - (rect.getPosition().x + rect.getSize().x/(2.0f * splits)))/scale : 0) << " "
				   << (boundingBox.getPosition().y - (rect.getPosition().y + rect.getSize().y/2.0f))/scale << " "
				   << filepathList[indexFile] << std::endl;

		}
		else {
			outputFile << output.textureTag << " " 
				   << output.animationTag << " " 
				   << output.frameCount << " " 
				   << output.frameTime << " " 
				   << output.size << " " 
				   << output.row << " " 
				   << boundingBox.getSize().x/scale << " "
				   << boundingBox.getSize().y/scale << " "
				   << ((splits) ? (boundingBox.getPosition().x - (rect.getPosition().x + rect.getSize().x/(2.0f * splits)))/scale : 0) << " "
				   << (boundingBox.getPosition().y - (rect.getPosition().y + rect.getSize().y/2.0f))/scale << " "
				   << filepathList[indexFile] << std::endl;
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Clear File")) {
		if(outputFile.is_open()) {
			outputFile.close();
			outputFile.open(outputFilename);
			outputFile.close();
			outputFile.open(outputFilename, std::ios::app);
		}
		else {
			outputFile.open(outputFilename);
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Close File")) {
		outputFile.close();
	}
}

void App::viewOutputFile() {
	ImGui::SeparatorText("Output File");

	static std::string fileContents;
	static std::string updateBuffer;
	static std::vector<char> textBuffer;
	
/*
		std::cout << updateBuffer << std::endl;
		ImGui::SameLine();
		if(ImGui::Button("Save")) {
			if(!outputFile.is_open()) {
				outputFile.open(outputFilename);
				outputFile << updateBuffer;
			}
			else {
				outputFile.close();
				outputFile.open(outputFilename);
				outputFile << updateBuffer;
			}
		}
*/

		std::ifstream file(outputFilename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		fileContents = buffer.str();
		textBuffer = std::vector<char>(fileContents.begin(), fileContents.end());
		textBuffer.push_back('\0');
		
		ImGui::InputTextMultiline("##source", textBuffer.data(), textBuffer.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
		updateBuffer = textBuffer.data();

}

void App::dropTarget(const std::string& targetName, int& targetValue) {
	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(targetName.c_str())) {
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payloadData = *(const int*)payload->Data;
			targetValue = payloadData;
		}
		ImGui::EndDragDropTarget();
	}
}

void App::dragPayload(const std::string& targetName, int& targetValue) {
	if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload(targetName.c_str(), &targetValue, sizeof(int));
		ImGui::Text("%d", targetValue);
		ImGui::EndDragDropSource();
	}
}

// SFML
void App::inputs() {
	sf::Event event;
	while(window->pollEvent(event)) {
		ImGui::SFML::ProcessEvent(*window, event);
		if(event.type == sf::Event::Closed) {
			outputFile.close();
			window->close();
		}
		if(event.type == sf::Event::MouseButtonPressed) {
			if(event.mouseButton.button == sf::Mouse::Left && !p1Active) {
				p1.setPosition(mousePosition.x, mousePosition.y);
				p1.setFillColor(sf::Color(255, 0, 0, 255));
				p1.setFillColor(sf::Color(255, 0, 0, 255));
				p1Active= true;
			}

			if(event.mouseButton.button == sf::Mouse::Right && p1Active&& !p2Active) {
				p1.setFillColor(sf::Color(0, 255, 0, 255));
				p2.setFillColor(sf::Color(0, 255, 0, 255));
				p2.setPosition(mousePosition.x, mousePosition.y);
				p2Active = true;
				rActive = true;
			}
		}
/*
		if(event.type == sf::Event::KeyPressed) {
			if(event.key.code == sf::Keyboard::Space) {
				p1Active = false;
				p2Active = false;
				rActive = false;
			}
		}
*/
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && (p1Active || p1Active)) {
			if((std::sqrt(std::pow(mousePosition.x - p1.getPosition().x, 2) + pow(mousePosition.y - p1.getPosition().y, 2)) <= radius*2 || p1Grab) && (!p2Grab && !p1Lock)) {
				p1.setPosition(mousePosition.x, mousePosition.y);
				rect.setPosition(p1.getPosition().x, p1.getPosition().y);
				if(!p2Lock) {
					rect.setSize({p2.getPosition().x - rect.getPosition().x, p2.getPosition().y - rect.getPosition().y});
				}
				else {
					p2.setPosition(rect.getPosition().x + rect.getSize().x, rect.getPosition().y + rect.getSize().y);
				}
				p1Grab = true;
			}
			if((std::sqrt(std::pow(mousePosition.x - p2.getPosition().x, 2) + pow(mousePosition.y - p2.getPosition().y, 2)) <= radius*2 || p2Grab) && (!p1Grab && !p2Lock)) {
				p2.setPosition(mousePosition.x, mousePosition.y);
				if(!p1Lock) {
					rect.setSize({p2.getPosition().x - rect.getPosition().x, p2.getPosition().y - rect.getPosition().y});
				  }
				else {
					rect.setPosition(p2.getPosition().x - rect.getSize().x, p2.getPosition().y - rect.getSize().y);
					p1.setPosition(rect.getPosition().x, rect.getPosition().y);
				}
				p2Grab = true;
			}
		}
		else {
			p1Grab = false;
			p2Grab = false;
		}
	}
}

void App::windowChange() {
	if((window->getSize().x - imguiSize.x) != texture.getSize().x * scale) {
		sf::View view = window->getView();
		sf::Vector2u size = {static_cast<unsigned int>(texture.getSize().x*scale + imguiSize.x), static_cast<unsigned int>(texture.getSize().y*scale)};

		if(imguiSize.y > size.y) {
			size.y = imguiSize.y;
		}
		
		view.setSize(size.x, size.y);
		view.setCenter(size.x/2.0f, size.y/2.0f);

		window->setView(view);
		window->setSize(size);

		sprite->setPosition(0, 0);
		sprite->setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
		imguiPos = {static_cast<float>(window->getSize().x - imguiSize.x), 0};

		textureOutline.setPosition(0, 0);
		textureOutline.setSize({static_cast<float>(texture.getSize().x * scale), static_cast<float>(texture.getSize().y * scale)});
	}
}

void App::render() {
	window->clear();
	window->draw(*sprite);
	window->draw(textureOutline);
	if(p1Active) {
		window->draw(p1);
	}
	if(p2Active) {
		window->draw(p2);
	}
	if(rActive) {
		window->draw(rect);
		window->draw(boundingBox);
	}
	if(splits) {
		for(int i = 0;i < splits;i++) {
			window->draw(rects[i]);
		}
	}

	ImGui::SFML::Render(*window);
	window->display();
}

