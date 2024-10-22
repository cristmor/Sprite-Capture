#include "Interface.h"
#include "App.h"
#include "imgui_internal.h"

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
	std::vector<const char*> filenames = AppState::getInstance().filenames();
	std::vector<std::filesystem::path> paths = AppState::getInstance().paths();
	sf::Texture texture = AppState::getInstance().texture();


	imageSelectionUI();
	boxSettingUI();
	boundingBoxUI();
	dataValuesUI();

	ImGui::SeparatorText("Output File");

	// End of Imgui UI
	ImGui::End();
}

void Interface::imageSelectionUI() {
	static auto& filenames = AppState::getInstance().filenames();
	static auto& paths = AppState::getInstance().paths();
	static auto& texture = AppState::getInstance().texture();

	if(!filenames.empty()) {
		if(ImGui::Combo("Selected File", &fileIndex, filenames.data(), filenames.size())) {
			texture.loadFromFile(paths[fileIndex].string());
		}
	}
}

void Interface::boxSettingUI() {
	static auto& data = AppState::getInstance().data();
	static auto& texture = AppState::getInstance().texture();
	static auto& selectRect = AppState::getInstance().selectRect();
	static auto& splitRects = AppState::getInstance().splitRects();
	ImGui::SeparatorText("Box Settings");

	// Size Options
	enum sizeOptions {Size_8, Size_12, Size_16, Size_32, Size_48, Size_64, Size_COUNT};
	static int sizeIndex = Size_8;
	const char* sizeNames[Size_COUNT] = {"8x8 Pixels", "12x12 Pixels", "16x16 Pixels", "32x32 Pixels", "48x48 Pixels", "64x64 Pixels"};
	const char* sizeName = (sizeIndex >=0 && sizeIndex < Size_COUNT) ? sizeNames[sizeIndex]: "Size Unknowm";

	if(ImGui::SliderInt("Frame Size/Sprite Size", &sizeIndex, 0, Size_COUNT - 1, sizeName)) {
		switch (sizeIndex) {
			case Size_8:
				data.size = 8;
				break;
			case Size_12:
				data.size = 12;
				break;
			case Size_16:
				data.size = 16;
				break;
			case Size_32:
				data.size = 32;
				break;
			case Size_48:
				data.size = 48;
				break;
			case Size_64:
				data.size = 64;
				break;
		}
		selectRect.setSize({ static_cast<float>(data.size * SCALE), static_cast<float>(data.size * SCALE) });
	}

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

	ImGui::SliderInt("Set Row", &data.row, 1, maxRow);
	ImGui::SliderInt("Set Column", &data.column, 1, maxColumn);

	// Box Splits
	ImGui::SliderInt("Set Splits", &data.count, 1, 10);

	static bool fullWidth = false;
	static bool fullHeight = false;
	ImGui::Checkbox("Full Width", &fullWidth);
	ImGui::Checkbox("Full Height", &fullHeight);

	// Reset Button
	if(ImGui::Button("Reset")) {
		selectRect.setPosition({ 0,0 });
		selectRect.setSize({ data.size * SCALE, data.size * SCALE});
		data.row = 1;
		data.column = 1;
		fullWidth = false;
		fullHeight = false;
		data.count = 1;
	}

	// Data
	selectRect.setPosition({ (data.size * SCALE) * (column - 1), (data.size * SCALE) * (row - 1) });
	selectRect.setSize({ (data.size * SCALE), (data.size * SCALE)});
	if(fullWidth) {
		selectRect.setSize({ texture.getSize().x * SCALE, selectRect.getSize().y });
	}
	if(fullHeight) {
		selectRect.setSize({ selectRect.getSize().x, texture.getSize().y * SCALE });
	}
	for(int i = 1; i <= data.count;i++) {
		splitRects[i-1].setPosition(selectRect.getPosition());
		splitRects[i-1].setSize({(selectRect.getSize().x/data.count)*i, selectRect.getSize().y});
	}
}

void Interface::boundingBoxUI() {
	static auto& data = AppState::getInstance().data();
	static auto& boundingBox = AppState::getInstance().boundingBox();
	static auto& selectRect = AppState::getInstance().selectRect();
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
	boundingBox.setPosition({selectRect.getPosition().x + selectRect.getSize().x/(2.0f * data.count) + pos[0], selectRect.getPosition().y + selectRect.getSize().y/2.0f + pos[1]});
	boundingBox.setSize({static_cast<float>(boundingSize[0]), static_cast<float>(boundingSize[1])});
}

void Interface::dataValuesUI() {
	static auto& paths = AppState::getInstance().paths();
	static auto& data = AppState::getInstance().data();
	static auto& dataFile = AppState::getInstance().dataFile();
	static auto& dataFilename = AppState::getInstance().dataFilename();
	static auto& boundingBox = AppState::getInstance().boundingBox();
	static auto& selectRect = AppState::getInstance().selectRect();
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
				   << data.size << " "
				   << data.row << " "
				   << data.column << " "
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
				   << data.size << " "
				   << data.row << " "
				   << data.column << " "
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
}

void Interface::readDataUI() {
	static auto& dataFilename = AppState::getInstance().dataFilename();
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
}
