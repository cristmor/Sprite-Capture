#include "AppState.h"

AppState::AppState() {
	mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), WINDOW_TITLE);

	for(const auto& entry: std::filesystem::recursive_directory_iterator(mAssetPath)) {
		if(entry.is_regular_file()) {
			mPaths.push_back(entry.path());
		}
	}
	std::sort(mPaths.begin(), mPaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) { return a.filename().string() < b.filename().string(); });
	mFilenames.reserve(mPaths.size());
	mFilenamesChar.reserve(mPaths.size());
	for(const auto& path : mPaths) {
		mFilenames.push_back(path.filename().string());
		mFilenamesChar.push_back(mFilenames.back().c_str());
	}

	mTexture.loadFromFile(mPaths[0].string());
	mSprite = sf::Sprite(mTexture);
	mSprite.setScale(SCALE, SCALE);

	mSelectRect.setFillColor(sf::Color(0, 0, 0, 0));
	mSelectRect.setOutlineColor(sf::Color(255, 255, 255, 255));
	mSelectRect.setOutlineThickness(2.0f);
	mSelectRect.setPosition(0, 0);
	mSelectRect.setSize({ 8 * SCALE, 8 * SCALE});

	//textureOutline.setFillColor(sf::Color(0, 0, 0, 0));
	//textureOutline.setOutlineColor(sf::Color(255, 255, 255, 255));
	//textureOutline.setOutlineThickness(2.0f);
	for(int i = 0;i < 10;i++) {
		sf::RectangleShape newRect;
		newRect.setFillColor(sf::Color(0, 0, 0, 0));
		newRect.setOutlineColor(sf::Color(255, 255, 255, 255));
		newRect.setOutlineThickness(2.0f);
		mSplitRects.push_back(newRect);
	}

	mBoundingBox.setFillColor(sf::Color(0, 0, 0, 0));
	mBoundingBox.setOutlineColor(sf::Color(0, 0, 255, 255));
	mBoundingBox.setOutlineThickness(2.0f);

	mData.texturePath = mPaths[0].string();
	mData.textureTag = "textureTag";
	mData.animationTag = "animationTag";
	mData.count = 0;
	mData.speed = 0;
	mData.size = 8 * SCALE;
	mData.row = 1;
}

AppState::~AppState() {
	mWindow->close();
}
