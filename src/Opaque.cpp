#include "Opaque.hpp"

#include <cmath>
#include <iostream>

std::vector<Opaque*> Opaque::OpaqueGroup;
Opaque* Opaque::ActiveSelection = nullptr;
int Opaque::selectedPoint = 0;

Opaque::Opaque(sf::Vector2f center, sf::Vector2f size)
{
	quad = sf::VertexArray(sf::TriangleFan, 6);

	quad[0].color = sf::Color::Red;
	quad[1].color = sf::Color::Red;
	quad[2].color = sf::Color::Red;
	quad[3].color = sf::Color::Red;
	quad[4].color = sf::Color::Red;

	quad[0].position = center;
	quad[1].position = center - size * 0.5f;
	quad[2].position = { center.x + size.x * 0.5f, center.y - size.y * 0.5f };
	quad[3].position = center + size * 0.5f;
	quad[4].position = { center.x - size.x * 0.5f, center.y + size.y * 0.5f };
	quad[5] = quad[1];

	this->rotateAngle = 0.025f;
	this->rotation = sf::Transform::Identity;

	this->update();
}

Opaque::~Opaque()
{
}

void Opaque::draw(sf::RenderWindow& window)
{
	window.draw(this->quad);
}

void Opaque::drawGroup(sf::RenderWindow& window)
{
	for (auto object : OpaqueGroup)
		object->draw(window);
}

void Opaque::delGroup()
{
	auto it = OpaqueGroup.begin();
	while (it != OpaqueGroup.end())
		it = OpaqueGroup.erase(it);
}

void Opaque::setSelectedObject(sf::Vector2f position)
{
	sf::CircleShape corner(10);
	corner.setOrigin(10, 10);
	for (unsigned int i = 1; i < OpaqueGroup.size(); i++) {
		for (int j = 0; j < 5; j++) {
			corner.setPosition(OpaqueGroup[i]->quad[j].position);
			if (corner.getGlobalBounds().contains(position)) {
				ActiveSelection = OpaqueGroup[i];
				selectedPoint = j;
				break;
			}
		}
	}

	if(ActiveSelection != nullptr)std::cout << selectedPoint << "\n";
}

void Opaque::resetSelection()
{
	if(ActiveSelection != nullptr)ActiveSelection->rotateAngle = 0.025f;
	ActiveSelection = nullptr;
	selectedPoint = 0;
}

sf::Vector2f Opaque::getPosition()
{
	return this->quad[0].position;
}

void Opaque::mouseMove(sf::Vector2f pos)
{
	if (selectedPoint == 0) {
		for (int i = 1; i < 6; i++) {
			this->quad[i].position += pos - this->quad[0].position;
		}
		this->quad[0].position = pos;
	}
	else {
		if (selectedPoint == 1)this->quad[5].position = pos;
		this->quad[selectedPoint].position = pos;
	}
	this->update();
}

void Opaque::rotate()
{
	this->rotation = sf::Transform::Identity;
	this->rotation.rotate(this->rotateAngle, this->quad[0].position);
	
	for (int i = 1; i < 6; i++) {
		this->quad[i].position = this->rotation.transformPoint(quad[i].position);
	}
	this->rotateAngle += 0.005f;
	this->update();
	
}

void Opaque::update()
{
	for (int i = 0; i < 4; i++) {
		sides[i] = Lines(quad[i + 1].position, quad[i + 2].position);
	}
}