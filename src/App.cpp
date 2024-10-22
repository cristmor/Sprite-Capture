#include "App.h"

App::App() {
}

// Public
void App::run() {
	std::shared_ptr<sf::RenderWindow>& window = AppState::getInstance().window();

	sf::Clock deltaClock;
	while(window->isOpen()) {

		// Inputs
		inputs();

		// SFML Interface
		mInterface.update();

		// Render
		windowChange();
		render();
	}
}

// Private

// SFML
void App::inputs() {
	static auto& window = AppState::getInstance().window();
	static auto& dataFile = AppState::getInstance().dataFile();
	sf::Event event;
	while(window->pollEvent(event)) {
		ImGui::SFML::ProcessEvent(*window, event);
		if(event.type == sf::Event::Closed) {
			dataFile.close();
			window->close();
		}
	}
}

void App::windowChange() {
	static auto& window = AppState::getInstance().window();
	static auto& texture = AppState::getInstance().texture();
	static auto& sprite = AppState::getInstance().sprite();
	static auto& pos = mInterface.pos();
	static auto& imguiSize = mInterface.size();
	if((window->getSize().x - imguiSize.x) != texture.getSize().x * SCALE) {
		sf::View view = window->getView();
		sf::Vector2u size = {static_cast<unsigned int>(texture.getSize().x*SCALE + imguiSize.x), static_cast<unsigned int>(texture.getSize().y*SCALE)};

		if(imguiSize.y > size.y) {
			size.y = imguiSize.y;
		}
		
		view.setSize(size.x, size.y);
		view.setCenter(size.x/2.0f, size.y/2.0f);

		window->setView(view);
		window->setSize(size);

		sprite.setPosition(0, 0);
		sprite.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
		pos = {static_cast<float>(window->getSize().x - imguiSize.x), 0};

		//textureOutline.setPosition(0, 0);
		//textureOutline.setSize({static_cast<float>(texture.getSize().x * scale), static_cast<float>(texture.getSize().y * scale)});
	}
}

void App::render() {
	auto& window = AppState::getInstance().window();
	static auto& sprite = AppState::getInstance().sprite();
	static auto& selectRect = AppState::getInstance().selectRect();
	static auto& boundingBox = AppState::getInstance().boundingBox();
	static auto& data = AppState::getInstance().data();
	static auto& splitRects = AppState::getInstance().splitRects();
	window->clear();
	window->draw(sprite);
	//window->draw(textureOutline);
	window->draw(selectRect);
	window->draw(boundingBox);
	if(data.count) {
		for(int i = 0;i < data.count;i++) {
			window->draw(splitRects[i]);
		}
	}
	ImGui::SFML::Render(*window);
	window->display();
}

