#include "Lines.hpp"
#include <cmath>
#include <iostream>

Lines::Lines() 
{
}

Lines::Lines(sf::Vector2f a, sf::Vector2f b)
{
	this->a = a;
	this->b = b;
}

Lines::~Lines()
{
}

float Lines::m()
{
	return (b.y - a.y) / (b.x - a.x);
}

float Lines::c()
{
	if (isParallelToXaxis())return a.y;
	if (isParallelToYaxis())return a.x;
	return a.y - a.x * ((b.y - a.y) / (b.x - a.x));
}

float Lines::x(float y)
{
	if (isParallelToYaxis() || isParallelToXaxis())return a.x;
	return (y - c()) / m();
}

float Lines::y(float x)
{
	if (isParallelToYaxis() || isParallelToXaxis())return a.y;
	return m() * x + c();
}

float Lines::getValue(sf::Vector2f point)
{
	if (isParallelToXaxis())return point.y - a.y;
	if (isParallelToYaxis())return point.x - a.x;
	return point.y - m() * point.x - c();
}

sf::Vector2f intersection(Lines l, Lines m)
{
	if (l.isParallelToXaxis()) {
		return { m.x(l.c()), l.c() };
	}
	else if (l.isParallelToYaxis()) {
		return { l.c(), m.y(l.c()) };
	}
	else {
		if (m.isParallelToXaxis()) {
			return { l.x(m.c()), m.c() };
		}
		else if (m.isParallelToYaxis()) {
			return { m.c(), l.y(m.c()) };
		}
		else {
			float x = (l.c() - m.c()) / (m.m() - l.m());
			return{ x, l.y(x) };
		}
	}
}

sf::Vector2f Lines::midPoint()
{
	return (a + b) * 0.5f;
}

double Lines::magnitude()
{
	return std::sqrt((b.y - a.y) * (b.y - a.y) + (b.x - a.x) * (b.x - a.x));
}

bool Lines::contains(sf::Vector2f c)
{
	sf::Vector2f ac = a - c, bc = b - c;
	if ((ac.x > 0 && bc.x > 0) || (ac.x < 0 && bc.x < 0) || (ac.y > 0 && bc.y > 0) || (ac.y < 0 && bc.y < 0))return false;
	return true;
}

bool Lines::intersects(Lines l)
{
	if (m() == l.m())return false;

	return l.contains(intersection(*this, l)) && this->contains(intersection(*this, l));
}

bool areParallel(Lines l, Lines n)
{
	return l.m() == n.m();
}

float angle(Lines l, Lines m)
{
	return std::abs(atan2f((l.m() - m.m()), 1 + l.m() * m.m()));
}

std::ostream& operator<<(std::ostream& out, Lines l)
{
	out << "y = " << l.m() << "x + " << l.c() << "\n";
	return out;
}

sf::Vector2f normaliseVec(sf::Vector2f p)
{
	float mag = std::sqrt(p.x * p.x + p.y * p.y);
	return { p.x / mag, p.y / mag };
}

bool Lines::isParallelToXaxis()
{
	return a.y == b.y;
}

bool Lines::isParallelToYaxis()
{
	return a.x == b.x;
}

bool operator==(Lines l, Lines m)
{
	return (l.a == m.a && l.b == m.b) || (l.a == m.b && l.b == m.a);
}

void sortinLine(std::vector<sf::Vector2f>& points, Lines line)
{
	std::vector<int> distances;
	for (int i = 0; i < points.size(); i++)
	{
		distances.push_back(Lines(line.a, points[i]).magnitude());
	}


	for (int i = 0; i < distances.size(); i++)
	{
		for (int j = 0; j < distances.size() - i - 1; j++)
		{
			if (distances[j] > distances[j + 1])
			{
				int temp = distances[j];
				distances[j] = distances[j + 1];
				distances[j + 1] = temp;
				sf::Vector2f t = points[j];
				points[j] = points[j + 1];
				points[j + 1] = t;
			}
		}
	}
}