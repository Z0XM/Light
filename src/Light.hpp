#pragma once

#include <SFML/Graphics.hpp>

class Light {
	sf::CircleShape source;
	std::vector<sf::Vector2f> sourcePoints;

public:
	Light(sf::Vector2f position);
	~Light();

	void draw(sf::RenderWindow&);
	void mouseMove(sf::Vector2f);
	void update();
	void illuminate(sf::RenderWindow&, sf::Vector2f);
	static void drawGroup(sf::RenderWindow&);
	static void delGroup();
	static void setSelectedObject(sf::Vector2f);
	static void resetSelection();

	sf::Vector2f getPosition();

	static std::vector<Light*> LightGroup;
	static Light* ActiveSelection;
	static bool SWITCH_ON;
	static bool SHOW_LINES;
};