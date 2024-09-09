#pragma once

#include "common.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"


const float windowSizeX = 400;
const float windowSizeY = 400;
const std::string windowTitle = "Sprite Capture";
const float scale = 3.0f;

struct Output {
	std::string texturePath;
	std::string textureTag;
	std::string animationTag;
	int frameCount;
	int frameTime;
	int size;
	int row;
};

class App {
public:
	App();
	void run();
private:
	void loadTexture();
	void displayInfo();
	void calculator();
	void boxSettings();
	void setBoundingBox();
	void outputValues();
	void viewOutputFile();
	void dropTarget(const std::string& targetName, int& targetValue);
	void dragPayload(const std::string& targetName, int& payloadValue);

	void inputs();
	void windowChange();
	void render();

	std::vector<std::string> fileList;
	std::vector<const char*> fileListChar;
	std::vector<std::string> filepathList;
	std::string assetPath = "/home/cristmor/dev/cpp/SpriteCapture/deps/assets/sprites/";
	std::string outputFilename = "Output.txt";
	std::ofstream outputFile;
	int indexFile = 0;

	std::shared_ptr<sf::RenderWindow> window;
	sf::Texture texture;
	std::shared_ptr<sf::Sprite> sprite;

	sf::CircleShape p1;
	sf::CircleShape p2;
	sf::RectangleShape rect;
	sf::RectangleShape textureOutline;
	std::vector<sf::RectangleShape> rects;
	sf::RectangleShape boundingBox;
	sf::Vector2i mousePosition;
	float radius = 5.0f;
	int splits = 0;

	bool p1Active = false;
	bool p2Active= false;
	bool rActive = false;
	bool p1Grab = false;
	bool p2Grab = false;
	bool p1Lock = false;
	bool p2Lock = false;

	Output output;

	ImVec2 imguiPos;
	ImVec2 imguiSize;
};
