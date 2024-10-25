#include "Interface.h"

Interface::Interface() {
	auto& window = AppState::getInstance().window();
	if(!ImGui::SFML::Init(*window)) {
		std::cerr << "Error: Failed to Load ImGui" << std::endl;
	}
	mSize = {700, 900};
}

Interface::~Interface() {
	ImGui::SFML::Shutdown();
}

void Interface::update() {
	auto& window = AppState::getInstance().window();

	// Start of Imgui UI
	ImGui::SFML::Update(*window, mClock.restart());
	ImGui::SetNextWindowPos(mPos);
	ImGui::SetNextWindowSize(mSize);
	ImGui::Begin("Control Panel");

	// App State Variables
	static auto& filenames = AppState::getInstance().filenames();
	static auto& paths = AppState::getInstance().paths();
	static auto& texture = AppState::getInstance().texture();
	static auto& data = AppState::getInstance().data();
	static auto& selectRect = AppState::getInstance().selectRect();
	static auto& splitRects = AppState::getInstance().splitRects();
	static auto& boundingBox = AppState::getInstance().boundingBox();
	static auto& dataFile = AppState::getInstance().dataFile();
	static auto& dataFilename = AppState::getInstance().dataFilename();

	if(!filenames.empty()) {
		if(ImGui::Combo("Selected File", &fileIndex, filenames.data(), filenames.size())) {
			texture.loadFromFile(paths[fileIndex].string());
		}
	}

	//
	// Box Settings
	//
	ImGui::SeparatorText("Box Settings");

	// Box Row and Column
	static int row = 1;
	static int column = 1;
	static int maxRow;
	static int maxColumn;
	maxRow = (texture.getSize().y * SCALE)/(selectRect.getSize().y);
	maxColumn = (texture.getSize().x * SCALE)/(selectRect.getSize().x);
	if(!maxRow) {
		maxRow = 1;
	}
	if(!maxColumn) {
		maxColumn = 1;
	}

	ImGui::Spacing();
	ImGui::Text("Fixed Mode");
	ImGui::Spacing();
	ImGui::BeginDisabled(mCustomMode);

	// Size Options
	enum sizeOptions {Size_8, Size_12, Size_16, Size_32, Size_48, Size_64, Size_COUNT};
	static int sizeIndex = Size_8;
	const char* sizeNames[Size_COUNT] = {"8x8 Pixels", "12x12 Pixels", "16x16 Pixels", "32x32 Pixels", "48x48 Pixels", "64x64 Pixels"};
	const char* sizeName = (sizeIndex >=0 && sizeIndex < Size_COUNT) ? sizeNames[sizeIndex]: "Size Unknowm";

	if(ImGui::SliderInt("Frame Size/Sprite Size", &sizeIndex, 0, Size_COUNT - 1, sizeName)) {
		switch (sizeIndex) {
			case Size_8:
				data.sizeX = 8;
				data.sizeY = 8;
				break;
			case Size_12:
				data.sizeX = 12;
				data.sizeY = 12;
				break;
			case Size_16:
				data.sizeX = 16;
				data.sizeY = 16;
				break;
			case Size_32:
				data.sizeX = 32;
				data.sizeY = 32;
				break;
			case Size_48:
				data.sizeX = 48;
				data.sizeY = 48;
				break;
			case Size_64:
				data.sizeX = 64;
				data.sizeY = 64;
				break;
		}
		selectRect.setSize({ static_cast<float>(data.sizeX * SCALE), static_cast<float>(data.sizeY * SCALE) });
	}
	ImGui::SliderInt("Set Row", &row, 1, maxRow);
	ImGui::SliderInt("Set Column", &column, 1, maxColumn);
	ImGui::SliderInt("Set Splits", &data.count, 1, 10);
	ImGui::EndDisabled();


	static int posX = 0;
	static int posY = 0;
	static int sizeX = 0;
	static int sizeY = 0;

	ImGui::Spacing();
	ImGui::Checkbox("Custom Mode", &mCustomMode);
	ImGui::Spacing();

	ImGui::BeginDisabled(!mCustomMode);
	ImGui::SetNextItemWidth(100);
	//ImGui::DragInt(":x", &posX, 4.0f, 0, texture.getSize().x * SCALE - selectRect.getSize().x);
	if(ImGui::DragInt(":x ##1", &posX, 2.0f, 0, (texture.getSize().x * SCALE))) {
		if(sizeX > (texture.getSize().x * SCALE - posX)) {
			sizeX = (texture.getSize().x * SCALE - posX);
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":y Select Rect Position", &posY, 2.0f, 0, (texture.getSize().y * SCALE))) {
		if(sizeY > (texture.getSize().y * SCALE - posY)) {
			sizeY = (texture.getSize().y * SCALE - posY);
		}
	}
	ImGui::SetNextItemWidth(100);

	if(ImGui::DragInt(":x ##2", &sizeX, 2.0f, 0, (texture.getSize().x * SCALE))) {
		if(posX > (texture.getSize().x * SCALE - sizeX)) {
			posX = (texture.getSize().x * SCALE - sizeX);
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":y Select Rect Size", &sizeY , 2.0f, 0, (texture.getSize().y * SCALE))) {
		if(posY > (texture.getSize().y * SCALE - sizeY)) {
			posY = (texture.getSize().y * SCALE - sizeY);
		}
	}
	ImGui::EndDisabled();	

	static bool fullWidthSelect = false;
	static bool fullHeightSelect = false;
	ImGui::Checkbox("Full Width##1", &fullWidthSelect);
	ImGui::Checkbox("Full Height##1", &fullHeightSelect);

	// Reset Button
	if(ImGui::Button("Reset##1")) {
		selectRect.setPosition({ 0,0 });
		selectRect.setSize({ 8 * SCALE, 8 * SCALE});
		sizeIndex = Size_8;
		data.count = 1;
		row = 1;
		column = 1;
		fullWidthSelect = false;
		fullHeightSelect = false;
		mCustomMode = false;
	}

	// Data
	if(!mCustomMode) {
		posX = selectRect.getPosition().x;
		posY = selectRect.getPosition().y;
		sizeX = selectRect.getSize().x;
		sizeY = selectRect.getSize().y;
		data.row = row;
		data.column = column;
		selectRect.setPosition({ (data.sizeX * SCALE) * (data.column - 1), (data.sizeY * SCALE) * (data.row - 1) });
		selectRect.setSize({ (data.sizeX * SCALE), (data.sizeY * SCALE)});
	}
	else {
		data.row = posX;
		data.column = posY;
		data.sizeX = sizeX;
		data.sizeY = sizeY;
		selectRect.setPosition({ static_cast<float>(posX), static_cast<float>(posY)});
		selectRect.setSize({ static_cast<float>(sizeX), static_cast<float>(sizeY)});
	}
	if(fullWidthSelect) {
		selectRect.setSize({ texture.getSize().x * SCALE, selectRect.getSize().y });
	}
	if(fullHeightSelect) {
		selectRect.setSize({ selectRect.getSize().x, texture.getSize().y * SCALE });
	}
	for(int i = 1; i <= data.count;i++) {
		splitRects[i-1].setPosition(selectRect.getPosition());
		splitRects[i-1].setSize({(selectRect.getSize().x/data.count)*i, selectRect.getSize().y});
	}

	//
	// Bounding Box
	//
	ImGui::SeparatorText("Bounding Box");

	static int pos[2] = {0};
	static int boundingSize[2] = {0};

	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":x ##3", &pos[0], 1.0f, - splitRects[0].getSize().x/2, splitRects[0].getSize().x/2)) {
		if(boundingSize[0]/2.0 > (splitRects[0].getSize().x/2 - abs(pos[0]))) {
			boundingSize[0] = 2 * (splitRects[0].getSize().x/2 - abs(pos[0]));
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":y Offset", &pos[1], 1.0f, -splitRects[0].getSize().y/2, splitRects[0].getSize().y/2)) {
		if(boundingSize[1]/2.0 > (splitRects[0].getSize().y/2 - abs(pos[1]))) {
			boundingSize[1] = 2 * (splitRects[0].getSize().y/2 - abs(pos[1]));
		}
	}

	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":x ##4", &boundingSize[0], 1.0f, 0, splitRects[0].getSize().x)) {
		if(pos[0] > (splitRects[0].getSize().x/2 - boundingSize[0]/2.0)) {
			pos[0] = (splitRects[0].getSize().x/2 - boundingSize[0]/2.0);
		}
		if(pos[0] < -(splitRects[0].getSize().x/2 - boundingSize[0]/2.0)) {
			pos[0] = -(splitRects[0].getSize().x/2 - boundingSize[0]/2.0);
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if(ImGui::DragInt(":y Size", &boundingSize[1], 1.0f, 0, splitRects[0].getSize().y)) {
		if(pos[1] > (splitRects[0].getSize().y/2 - boundingSize[1]/2.0)) {
			pos[1] = (splitRects[0].getSize().y/2 - boundingSize[1]/2.0);
		}
		if(pos[1] < -(splitRects[0].getSize().y/2 - boundingSize[1]/2.0)) {
			pos[1] = -(splitRects[0].getSize().y/2 - boundingSize[1]/2.0);
		}
	}

	static bool fullWidthBounding = false;
	static bool fullHeightBounding = false;
	ImGui::Checkbox("Full Width##2", &fullWidthBounding);
	ImGui::Checkbox("Full Height##2", &fullHeightBounding);

	if(fullWidthBounding) {
		pos[0] = 0;
		boundingSize[0] = splitRects[0].getSize().x;
	}
	if(fullHeightBounding) {
		pos[1] = 0;
		boundingSize[1] = splitRects[0].getSize().y;
	}

	// Reset Button
	if(ImGui::Button("Reset##2")) {
		pos[0] = 0;
		pos[1] = 0;
		boundingSize[0] = 0;
		boundingSize[1] = 0;
		fullWidthBounding = false;
		fullHeightBounding = false;
	}

	// Data
	boundingBox.setOrigin({boundingBox.getSize().x/2.0f, boundingBox.getSize().y/2.0f});
	boundingBox.setPosition({selectRect.getPosition().x + selectRect.getSize().x/(2.0f * data.count) + pos[0], selectRect.getPosition().y + selectRect.getSize().y/2.0f + pos[1]});
	boundingBox.setSize({static_cast<float>(boundingSize[0]), static_cast<float>(boundingSize[1])});

	//
	// Output Values
	//
	ImGui::SeparatorText("Output Values");

	char buf[32];

	std::strncpy(buf, data.textureTag.c_str(), 32);
	ImGui::InputText("Texture Tag", buf, IM_ARRAYSIZE(buf));
	data.textureTag = buf;

	std::strncpy(buf, data.animationTag.c_str(), 32);
	ImGui::InputText("Animaiton Tag", buf, IM_ARRAYSIZE(buf));
	data.animationTag = buf;

	ImGui::InputInt("Speed", &data.speed);

	std::string condition = ((dataFile.is_open()) ? "(Opened)": "(Closed)");
	std::strncpy(buf, dataFilename.c_str(), 32);
	ImGui::InputText(("Output File " + condition).c_str(), buf, IM_ARRAYSIZE(buf));
	dataFilename = buf;

	if(ImGui::Button("Open File")) {
		dataFile.open(dataFilename, std::ios::app);
	}
	ImGui::SameLine();
	if(ImGui::Button("Append File")) {
		if(!dataFile.is_open()) {
			dataFile.open(dataFilename, std::ios::app);
			dataFile << data.textureTag << " "
				   << data.animationTag << " "
				   << data.count << " "
				   << data.speed << " "
				   << data.sizeX << " "
				   << data.sizeY << " "
				   << data.row << " "
				   << data.column << " "
				   << data.isCustom << " "
				   << boundingBox.getSize().x/SCALE<< " "
				   << boundingBox.getSize().y/SCALE<< " "
				   << ((data.count) ? (boundingBox.getPosition().x - (selectRect.getPosition().x + selectRect.getSize().x/(2.0f * data.count)))/SCALE : 0) << " "
				   << (boundingBox.getPosition().y - (selectRect.getPosition().y + selectRect.getSize().y/2.0f))/SCALE<< " "
				   << paths[fileIndex].string() << std::endl;

		}
		else {
			dataFile << data.textureTag << " "
				   << data.animationTag << " "
				   << data.count << " "
				   << data.speed << " "
				   << data.sizeX << " "
				   << data.sizeY << " "
				   << data.row << " "
				   << data.column << " "
				   << data.isCustom << " "
				   << boundingBox.getSize().x/SCALE << " "
				   << boundingBox.getSize().y/SCALE << " "
				   << ((data.count) ? (boundingBox.getPosition().x - (selectRect.getPosition().x + selectRect.getSize().x/(2.0f * data.count)))/SCALE : 0) << " "
				   << (boundingBox.getPosition().y - (selectRect.getPosition().y + selectRect.getSize().y/2.0f))/SCALE << " "
				   << paths[fileIndex].string() << std::endl;
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Clear File")) {
		if(dataFile.is_open()) {
			dataFile.close();
			dataFile.open(dataFilename);
			dataFile.close();
			dataFile.open(dataFilename, std::ios::app);
		}
		else {
			dataFile.open(dataFilename);
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Close File")) {
		dataFile.close();
	}


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

	std::ifstream file(dataFilename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	fileContents = buffer.str();
	textBuffer = std::vector<char>(fileContents.begin(), fileContents.end());
	textBuffer.push_back('\0');
	
	ImGui::InputTextMultiline("##source", textBuffer.data(), textBuffer.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
	updateBuffer = textBuffer.data();

	ImGui::SeparatorText("Output File");

	// End of Imgui UI
	ImGui::End();
}
