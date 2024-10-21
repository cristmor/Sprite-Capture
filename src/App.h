#pragma once

#include "common.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include <filesystem>


// Change Naming
const float windowSizeX = 400;
const float windowSizeY = 400;
const std::string windowTitle = "Sprite Capture";
const float scale = 3.0f;

struct Output {
	std::string texturePath;
	std::string textureTag;
	std::string animationTag;
	int count;
	int speed;
	int size;
	int row;
	int column;
};

class App {
public:
	App();
	void run();
private:
	void imguiUI();

	void inputs();
	void windowChange();
	void render();

	std::vector<std::filesystem::path> paths;
	std::vector<std::string> filenames;
	std::vector<const char*> filenamesChar;
	int fileIndex = 0;

	std::string assetPath = "/home/cristmor/dev/cpp/SpriteCapture/deps/assets/sprites/";
	std::string outputFilename = "Output.txt";
	std::ofstream outputFile;

	std::shared_ptr<sf::RenderWindow> window;
	sf::Texture texture;
	std::shared_ptr<sf::Sprite> sprite;

	sf::RectangleShape rect;
	sf::RectangleShape textureOutline;
	std::vector<sf::RectangleShape> rects;
	sf::RectangleShape boundingBox;
	int splits = 1;


	Output output;

	ImVec2 imguiPos;
	ImVec2 imguiSize;
};
