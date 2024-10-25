#pragma once

// Standard
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>
#include <cstring>

// Deps
#include <SFML/Graphics.hpp>

const float WINDOW_SIZE_X = 400;
const float WINDOW_SIZE_Y = 400;
const std::string WINDOW_TITLE = "Sprite Capture";
const float SCALE = 3.0f;

struct Data { 
	std::string texturePath;
	std::string textureTag;
	std::string animationTag;
	int count;
	int speed;
	int sizeX;
	int sizeY;
	int row;
	int column;
	bool isCustom;
};

class AppState {
public:
	AppState();
	~AppState();

	static AppState& getInstance() {
		static AppState instance;
		return instance;
	}

	AppState(const AppState&) = delete;
	AppState& operator =(const AppState&) = delete;

	std::shared_ptr<sf::RenderWindow>& window() { return mWindow; }
	sf::Texture& texture() { return mTexture; }
	Data& data() { return mData; }
	sf::RectangleShape& selectRect() { return mSelectRect; }
	std::vector<sf::RectangleShape>& splitRects() { return mSplitRects; }
	sf::RectangleShape& boundingBox() { return mBoundingBox; }
	std::ofstream& dataFile() { return mDataFile; }
	std::string& dataFilename() { return mDataFilename; }
	sf::Sprite& sprite() { return mSprite; }

	const std::vector<std::filesystem::path> paths() { return mPaths; }
	const std::vector<const char*>& filenames() const { return mFilenamesChar; }

private:
	// Window
	std::shared_ptr<sf::RenderWindow> mWindow;

	// Files for Selection Menu
	std::string mAssetPath = "/home/cristmor/dev/cpp/SpriteCapture/deps/assets/sprites/";
	std::vector<std::filesystem::path> mPaths;
	std::vector<std::string> mFilenames;
	std::vector<const char*> mFilenamesChar;

	// Output Data File
	std::string mDataFilename = "data.txt";
	std::ofstream mDataFile;
	Data mData;

	// Rectangles
	sf::RectangleShape mSelectRect;
	std::vector<sf::RectangleShape> mSplitRects;
	sf::RectangleShape mBoundingBox;

	// Textre and Sprite
	sf::Texture mTexture;
	sf::Sprite mSprite;
};
