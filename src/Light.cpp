#include "Light.hpp"
#include "Opaque.hpp"
#include "Lines.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

std::vector<Light*> Light::LightGroup;
Light* Light::ActiveSelection;
bool Light::SWITCH_ON = false;
bool Light::SHOW_LINES = false;

Light::Light(sf::Vector2f position)
{
	source.setRadius(10);
	source.setOrigin(source.getRadius(), source.getRadius());
	source.setPosition(position);
	source.setFillColor(sf::Color::Yellow);

	this->update();
}

Light::~Light()
{
}

void Light::draw(sf::RenderWindow& window)
{
	if (SWITCH_ON) {
		for (auto source : sourcePoints)
			this->illuminate(window, source);
	}
	window.draw(source);
}


void Light::update()
{
	sourcePoints.clear();
	sourcePoints.push_back(source.getPosition());
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(0, -source.getRadius()));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(0, +source.getRadius()));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(-source.getRadius(), 0));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(+source.getRadius(), 0));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(-source.getRadius(), -source.getRadius()));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(-source.getRadius(), +source.getRadius()));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(+source.getRadius(), +source.getRadius()));
	sourcePoints.push_back(source.getPosition() + sf::Vector2f(+source.getRadius(), -source.getRadius()));
}

void Light::drawGroup(sf::RenderWindow& window)
{
	for (auto light : LightGroup) {
		light->draw(window);
	}
}

void Light::delGroup()
{
	auto it = LightGroup.begin();
	while (it != LightGroup.end())
		it = LightGroup.erase(it);
}

void Light::setSelectedObject(sf::Vector2f pos)
{
	for (unsigned int i = 0; i < LightGroup.size(); i++) {
		sf::Vector2f p = pos - LightGroup[i]->getPosition();
		if (p.x * p.x + p.y * p.y <= LightGroup[i]->source.getRadius() * LightGroup[i]->source.getRadius()) {
			ActiveSelection = LightGroup[i];
			break;
		}
	}
}

void Light::resetSelection()
{
	ActiveSelection = nullptr;
}

sf::Vector2f Light::getPosition()
{
	return source.getPosition();
}

void Light::mouseMove(sf::Vector2f pos)
{
	this->source.setPosition(pos);
	this->update();
}

bool isRayObstructed(Lines ray, std::vector<int> except)
{
	for (int object = 1; object < Opaque::OpaqueGroup.size(); object++) {
		if (std::find(except.begin(), except.end(), object) != except.end())continue;
		for (int i = 0; i < 4; i++) {
			auto k = intersection(ray, Opaque::OpaqueGroup[object]->sides[i]);
			if (ray.contains(k) && Opaque::OpaqueGroup[object]->sides[i].contains(k))return true;
		}
	}
	return false;
}

bool isRaySelfObstructed(Lines ray, int object, int except)
{
	for (int side = 0; side < 4; side++) {
		if (side == except)continue;
		auto k = intersection(ray, Opaque::OpaqueGroup[object]->sides[side]);
		if (ray.contains(k) && Opaque::OpaqueGroup[object]->sides[side].contains(k)) {
			return true;
		}
	}
	return false;
}

void getRaysSidesFromObject(sf::Vector2f light, int object, std::vector<std::vector<std::vector<sf::Vector2f>>> &rayPoints, std::vector <std::vector<std::vector<bool>>> &visiblity)
{
	Lines rays[4];
	std::vector<int> valid_sides;

	for (int i = 0; i < 4; i++) {
		rays[i] = Lines(light, Opaque::OpaqueGroup[object]->sides[i].midPoint());
	}

	for (int i = 0; i < 4; i++) {
		if (!isRaySelfObstructed(rays[i], object, i))valid_sides.push_back(i);
	}

	for (int i = 0; i < valid_sides.size(); i++) {
		auto side = Opaque::OpaqueGroup[object]->sides[valid_sides[i]];
		rayPoints[object][valid_sides[i]].push_back(side.a);
		rayPoints[object][valid_sides[i]].push_back(side.b);
		visiblity[object][valid_sides[i]].push_back(0);
		visiblity[object][valid_sides[i]].push_back(0);
	}

	if (valid_sides.size() == 2 && valid_sides[0] == 0 && valid_sides[1] == 3) {
		valid_sides[0] = 3; valid_sides[1] = 0;
	}
	else if (valid_sides.size() == 3) {
		if (valid_sides[0] == 0 && valid_sides[1] == 2) {
			valid_sides[0] = 2; valid_sides[1] = 3; valid_sides[2] = 0;
		}
		else if (valid_sides[0] == 0 && valid_sides[2] == 3) {
			valid_sides[0] = 3; valid_sides[1] = 0; valid_sides[2] = 1;
		}
	}
	if (valid_sides.size()) {
		*(visiblity[object][valid_sides[0]].end() - 2) = 1;
		visiblity[object][valid_sides.back()].back() = 1;
	}


}

void removeObstructedRays(sf::Vector2f light, std::vector<std::vector<std::vector<sf::Vector2f>>>& rayPoints, std::vector <std::vector<std::vector<bool>>> &visiblity)
{
	for(int object = 0; object < rayPoints.size(); object++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (auto it = rayPoints[object][i].begin(); it != rayPoints[object][i].end();) {
				Lines ray(light, *it);
				if (isRayObstructed(ray, { object })) {
					it = rayPoints[object][i].erase(it);
					visiblity[object][i].erase(visiblity[object][i].begin() + int(it - rayPoints[object][i].begin()));
				} 
				else it++;
			}
		}
	}
}

void extendLines(sf::Vector2f light, std::vector<std::vector<std::vector<sf::Vector2f>>>& rayPoints, std::vector <std::vector<std::vector<bool>>> &visiblity)
{
	for (int object = 1; object < rayPoints.size(); object++)
	{
		for (int side = 0; side < 4; side++)
		{
			for (int i = 0; i < rayPoints[object][side].size(); i++) 
			{
				if (!visiblity[object][side][i])continue;

				for (int o = 0; o < Opaque::OpaqueGroup.size(); o++)
				{
					if (object == o)continue;
					for (int s = 0; s < 4; s++) {
						auto k = intersection(Lines(light, rayPoints[object][side][i]), Opaque::OpaqueGroup[o]->sides[s]);
						Lines ray(light, k);
						if (!Opaque::OpaqueGroup[o]->sides[s].contains(k) || !ray.contains(rayPoints[object][side][i]))continue;
						if (isRaySelfObstructed(ray, o, s))continue;
						if (isRayObstructed(Lines(light, k), { o, object }))continue;
						rayPoints[o][s].push_back(k);
						visiblity[o][s].push_back(0);
					}
				}
			}
		}
	}
}

void sortPoints(std::vector<std::vector<std::vector<sf::Vector2f>>>& rayPoints)
{
	for (int object = 0; object < rayPoints.size(); object++) 
	{
		for (int side = 0; side < 4; side++)
		{
			sortinLine(rayPoints[object][side], Opaque::OpaqueGroup[object]->sides[side]);
		}
	}
}

void Light::illuminate(sf::RenderWindow& window, sf::Vector2f light)
{
	std::vector<std::vector<std::vector<sf::Vector2f>>> rayPoints(Opaque::OpaqueGroup.size(), std::vector<std::vector<sf::Vector2f>>(4, std::vector<sf::Vector2f>() ));
	std::vector<std::vector<std::vector<bool>>> visiblity(Opaque::OpaqueGroup.size(), std::vector<std::vector<bool>>(4, std::vector<bool>()));

	sf::Vertex line[2];
	line[0].color = sf::Color::Green;
	line[1].color = sf::Color::Green;
	line[0].position = light;

	sf::Vertex triangle[3];
	sf::Color lightColor = sf::Color(200, 200, 200, 50);
	triangle[0].color = lightColor;
	triangle[1].color = lightColor;
	triangle[2].color = lightColor;
	triangle[0].position = light;


	for (int object = 0; object < Opaque::OpaqueGroup.size(); object++) {
		getRaysSidesFromObject(light, object, rayPoints, visiblity);
	}

	removeObstructedRays(light, rayPoints, visiblity);
	extendLines(light, rayPoints, visiblity); 

	sortPoints(rayPoints);

	for (int object = 0; object < Opaque::OpaqueGroup.size(); object++) {
		for (int i = 0; i < 4; i++) {
			for (auto point = 0; point < rayPoints[object][i].size(); point++) {
				if (!SHOW_LINES && point % 2 == 0 && point + 1 < rayPoints[object][i].size()) {
					triangle[1].position = rayPoints[object][i][point];
					triangle[2].position = rayPoints[object][i][point + 1];
					window.draw(triangle, 3, sf::Triangles, sf::BlendAdd);
				}
				if (SHOW_LINES) {
					line[1].position = rayPoints[object][i][point];
					window.draw(line, 2, sf::Lines);
				}
			}
		}
	}
}