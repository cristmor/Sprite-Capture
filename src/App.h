#pragma once

// Project
#include "AppState.h"
#include "Interface.h"

class Interface;

class AppState;

class App {
public:
	App();

	void run();
private:
	void imguiUI();

	void inputs();
	void windowChange();
	void render();

	Interface mInterface{};
};
