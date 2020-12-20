#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

class Lines {
public:
	sf::Vector2f a, b;

	Lines();
	Lines(sf::Vector2f, sf::Vector2f);
	~Lines();

	float m();
	float c();
	float x(float y);
	float y(float x);
	float getValue(sf::Vector2f);
	bool isParallelToXaxis();
	bool isParallelToYaxis();
	bool intersects(Lines);
	bool contains(sf::Vector2f);
	sf::Vector2f midPoint();
	double magnitude();

	friend sf::Vector2f intersection(Lines, Lines);
	friend float angle(Lines, Lines);
	friend bool areParallel(Lines, Lines);
	friend std::ostream& operator<<(std::ostream&, Lines);
	friend bool operator==(Lines, Lines);
	friend void sortinLine(std::vector<sf::Vector2f>& points, Lines line);
};

sf::Vector2f intersection(Lines, Lines);
bool areParallel(Lines, Lines);
float Linesangle(Lines, Lines);
Lines Perpendicular(Lines, sf::Vector2f);
std::ostream& operator<<(std::ostream&, Lines);
sf::Vector2f normaliseVec(sf::Vector2f);
bool operator==(Lines, Lines);
void sortinLine(std::vector<sf::Vector2f>& points, Lines line);