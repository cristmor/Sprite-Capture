#pragma once

// Standard
#include <iostream>

// Deps
#include "imgui-SFML.h"
#include "imgui.h"

// Project
#include "AppState.h"

class Interface {
public:
	Interface();
	~Interface();

	void update();

	ImVec2& pos() { return mPos; }
	ImVec2& size() { return mSize; }
private:
	sf::Clock mClock;
	int fileIndex = 0;

	bool mCustomMode = false;

	ImVec2 mPos;
	ImVec2 mSize;
};
