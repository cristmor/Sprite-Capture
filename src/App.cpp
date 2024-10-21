#include "App.h"
#include "imgui.h"
#include <algorithm>

App::App() {
	window = std::make_shared<sf::RenderWindow>(sf::VideoMode(windowSizeX, windowSizeY), windowTitle);
	if(!ImGui::SFML::Init(*window)) {
		std::cerr << "Error: Failed to Load ImGui" << std::endl;
	}

	for(const auto& entry: std::filesystem::recursive_directory_iterator(assetPath)) {
		if(entry.is_regular_file()) {
			paths.push_back(entry.path());
		}
	}
	std::sort(paths.begin(), paths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) { return a.filename().string() < b.filename().string(); });
	filenames.reserve(paths.size());
	filenamesChar.reserve(paths.size());
	for(const auto& path : paths) {
		filenames.push_back(path.filename().string());
		filenamesChar.push_back(filenames.back().c_str());
		std::cout << filenames.back().c_str() << std::endl;
	}

	texture.loadFromFile(paths[fileIndex].string());
	sprite = std::make_shared<sf::Sprite>(texture);
	sprite->setScale(scale, scale);

	rect.setFillColor(sf::Color(0, 0, 0, 0));
	rect.setOutlineColor(sf::Color(255, 255, 255, 255));
	rect.setOutlineThickness(2.0f);
	rect.setPosition(0, 0);
	rect.setSize({ 8 * scale, 8 * scale});

	textureOutline.setFillColor(sf::Color(0, 0, 0, 0));
	textureOutline.setOutlineColor(sf::Color(255, 255, 255, 255));
	textureOutline.setOutlineThickness(2.0f);
	for(int i = 0;i < 10;i++) {
		sf::RectangleShape newRect;
		newRect.setFillColor(sf::Color(0, 0, 0, 0));
		newRect.setOutlineColor(sf::Color(255, 255, 255, 255));
		newRect.setOutlineThickness(2.0f);
		rects.push_back(newRect);
	}

	boundingBox.setFillColor(sf::Color(0, 0, 0, 0));
	boundingBox.setOutlineColor(sf::Color(0, 0, 255, 255));
	boundingBox.setOutlineThickness(2.0f);

	output.texturePath = paths[fileIndex].string();
	output.textureTag = "textureTag";
	output.animationTag = "animationTag";
	output.count = 0;
	output.speed = 0;
	output.size = 8 * scale;
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

		imguiUI();

		ImGui::End();

		// Render
		windowChange();
		render();
	}
}

// Private
// ImGui
void App::imguiUI() {
	if(!filenamesChar.empty()) {
		if(ImGui::Combo("Selected File", &fileIndex, filenamesChar.data(), filenamesChar.size())) {
			texture.loadFromFile(paths[fileIndex].string());
		}
	}


	//
	// Box Settings
	//
	ImGui::SeparatorText("Box Settings");

	// Size Options
	enum sizeOptions {Size_8, Size_12, Size_16, Size_32, Size_48, Size_64, Size_COUNT};
	static int sizeIndex = Size_8;
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
		rect.setSize({ static_cast<float>(output.size * scale), static_cast<float>(output.size * scale) });
	}

	// Box Row and Column
	static int row = 1;
	static int column = 1;
	static int maxRow;
	static int maxColumn;
	maxRow = (texture.getSize().y * scale)/(rect.getSize().y);
	maxColumn = (texture.getSize().x * scale)/(rect.getSize().x);
	if(!maxRow) {
		maxRow = 1;
	}
	if(!maxColumn) {
		maxColumn = 1;
	}

	ImGui::SliderInt("Set Row", &row, 1, maxRow);
	ImGui::SliderInt("Set Column", &column, 1, maxColumn);

	// Box Splits
	ImGui::SliderInt("Set Splits", &splits, 1, 10);

	static bool fullWidth = false;
	static bool fullHeight = false;
	ImGui::Checkbox("Full Width", &fullWidth);
	ImGui::Checkbox("Full Height", &fullHeight);

	// Reset Button
	if(ImGui::Button("Reset")) {
		rect.setPosition({ 0,0 });
		rect.setSize({ output.size * scale, output.size * scale });
		row = 1;
		column = 1;
		fullWidth = false;
		fullHeight = false;
		splits = 1;
	}

	// Data
	output.row = row;
	rect.setPosition({ (output.size * scale) * (column - 1), (output.size * scale) * (row - 1) });
	rect.setSize({ (output.size * scale), (output.size * scale)});
	if(fullWidth) {
		rect.setSize({ texture.getSize().x * scale, rect.getSize().y });
	}
	if(fullHeight) {
		rect.setSize({ rect.getSize().x, texture.getSize().y * scale });
	}
	for(int i = 1; i <= splits;i++) {
		rects[i-1].setPosition(rect.getPosition());
		rects[i-1].setSize({(rect.getSize().x/splits)*i, rect.getSize().y});
	}


	//
	// Bounding Box
	//
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

	boundingBox.setOrigin({boundingBox.getSize().x/2.0f, boundingBox.getSize().y/2.0f});
	boundingBox.setPosition({rect.getPosition().x + rect.getSize().x/(2.0f * splits) + pos[0], rect.getPosition().y + rect.getSize().y/2.0f + pos[1]});
	boundingBox.setSize({static_cast<float>(boundingSize[0]), static_cast<float>(boundingSize[1])});


	//
	// Output Values
	//
	ImGui::SeparatorText("Output Values");

	output.count= splits;

	char buf[32];

	std::strncpy(buf, output.textureTag.c_str(), 32);
	ImGui::InputText("Texture Tag", buf, IM_ARRAYSIZE(buf));
	output.textureTag = buf;

	std::strncpy(buf, output.animationTag.c_str(), 32);
	ImGui::InputText("Animaiton Tag", buf, IM_ARRAYSIZE(buf));
	output.animationTag = buf;

	ImGui::InputInt("Speed", &output.speed);

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
				   << output.count << " "
				   << output.speed << " "
				   << output.size << " "
				   << output.row << " "
				   << output.column << " "
				   << boundingBox.getSize().x/scale << " "
				   << boundingBox.getSize().y/scale << " "
				   << ((splits) ? (boundingBox.getPosition().x - (rect.getPosition().x + rect.getSize().x/(2.0f * splits)))/scale : 0) << " "
				   << (boundingBox.getPosition().y - (rect.getPosition().y + rect.getSize().y/2.0f))/scale << " "
				   << paths[fileIndex].string() << std::endl;

		}
		else {
			outputFile << output.textureTag << " "
				   << output.animationTag << " "
				   << output.count << " "
				   << output.speed << " "
				   << output.size << " "
				   << output.row << " "
				   << output.column << " "
				   << boundingBox.getSize().x/scale << " "
				   << boundingBox.getSize().y/scale << " "
				   << ((splits) ? (boundingBox.getPosition().x - (rect.getPosition().x + rect.getSize().x/(2.0f * splits)))/scale : 0) << " "
				   << (boundingBox.getPosition().y - (rect.getPosition().y + rect.getSize().y/2.0f))/scale << " "
				   << paths[fileIndex].string() << std::endl;
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

	//
	// Output File
	//
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

// SFML
void App::inputs() {
	sf::Event event;
	while(window->pollEvent(event)) {
		ImGui::SFML::ProcessEvent(*window, event);
		if(event.type == sf::Event::Closed) {
			outputFile.close();
			window->close();
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
	window->draw(rect);
	window->draw(boundingBox);
	if(splits) {
		for(int i = 0;i < splits;i++) {
			window->draw(rects[i]);
		}
	}
	ImGui::SFML::Render(*window);
	window->display();
}

