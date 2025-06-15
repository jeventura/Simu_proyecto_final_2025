#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
using namespace sf;

bool isPointInsidePolygon(const vector<Vector2f>& polygon, const Vector2f& point);
bool arePointsClose(Vector2f a, Vector2f b);
int countSharedVertices(const vector<Vector2f>& a, const vector<Vector2f>& b);