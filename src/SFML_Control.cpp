#include "SFML_Control.hpp"
#include "Opaque.hpp"
#include "Light.hpp"
#include <iostream>

void SFML_Control::InitWindow()
{
	window.create(sf::VideoMode(1200, 700), "Laser");
	window.setFramerateLimit(60);
}

SFML_Control::SFML_Control()
{
	this->InitWindow();
	window.setFramerateLimit(480);
	
	Opaque::OpaqueGroup.push_back(new Opaque(sf::Vector2f(window.getSize()) * 0.5f, sf::Vector2f(window.getSize())));
	for (int i = 0; i < 6; i++) {
		Opaque::OpaqueGroup[0]->quad[i].color = sf::Color::Black;
	}
}

SFML_Control::~SFML_Control()
{
	Opaque::delGroup();
	Light::delGroup();
}

bool SFML_Control::isRunning()
{
	return this->window.isOpen();
}

void SFML_Control::update()
{
	this->pollEvents();
}

void SFML_Control::pollEvents()
{
	sf::Event event;
	sf::Vector2f mousePos = this->getMousePosition();
	while (this->window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			this->window.close();

		if (event.type == sf::Event::MouseButtonPressed) {
			Light::setSelectedObject(mousePos);
			Opaque::setSelectedObject(mousePos);
		}
		else if (event.type == sf::Event::MouseButtonReleased) {
			Light::resetSelection();
			Opaque::resetSelection();
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Space) {
				pauseSystem = !pauseSystem;
				std::cout << (pauseSystem ? "" : "Un") << "Paused ! \n";
			}
			else if (event.key.code == sf::Keyboard::I) {
				Light::SWITCH_ON = !Light::SWITCH_ON;
				std::cout << "LIGHTS " << (Light::SWITCH_ON ? "ON" : "OFF") << "! \n";
			}
			else if (event.key.code == sf::Keyboard::V) {
				Light::SHOW_LINES = !Light::SHOW_LINES;
				std::cout << "SHOWING " << (Light::SHOW_LINES ? "LINES" : "LIGHT") << "! \n";
			}
			else if (event.key.code == sf::Keyboard::L) {
				Light::LightGroup.push_back(new Light(mousePos));
			}
			else if (event.key.code == sf::Keyboard::R) {
				Opaque::OpaqueGroup.push_back(new Opaque(mousePos, { 100, 100 }));
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (Light::ActiveSelection != nullptr)Light::ActiveSelection->mouseMove(this->getMousePosition());
		else if (Opaque::ActiveSelection != nullptr)Opaque::ActiveSelection->mouseMove(this->getMousePosition());
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		if (Opaque::ActiveSelection != nullptr)Opaque::ActiveSelection->rotate();
	}
}

void SFML_Control::render()
{
	if (!pauseSystem) {
		this->window.clear();

		Opaque::drawGroup(this->window);
		Light::drawGroup(this->window);

		this->window.display();
	}
}

sf::Vector2f SFML_Control::getMousePosition()
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
}