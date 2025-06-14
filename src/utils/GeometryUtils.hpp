#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

bool isPointInsidePolygon(const std::vector<sf::Vector2f>& polygon, const sf::Vector2f& point);