#pragma once

#include <SFML/Graphics.hpp>
#include "Lines.hpp"

class Opaque {
private:
	float rotateAngle;
	sf::Transform rotation;

public:
	Opaque(sf::Vector2f center, sf::Vector2f size);
	~Opaque();

	void draw(sf::RenderWindow&);
	void mouseMove(sf::Vector2f);
	void rotate();
	void update();
	static void drawGroup(sf::RenderWindow&);
	static void delGroup();
	static void setSelectedObject(sf::Vector2f);
	static void resetSelection();

	sf::Vector2f getPosition();

	static std::vector<Opaque*> OpaqueGroup;
	static Opaque* ActiveSelection;
	static int selectedPoint;

	sf::VertexArray quad;
	Lines sides[4];
};
