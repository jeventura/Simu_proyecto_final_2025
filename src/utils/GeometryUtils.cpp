#include "GeometryUtils.hpp"

bool isPointInsidePolygon(const std::vector<sf::Vector2f>& polygon, const sf::Vector2f& point) {
    int crossings = 0;
    size_t count = polygon.size();

    for (size_t i = 0; i < count; ++i) {
        const sf::Vector2f& a = polygon[i];
        const sf::Vector2f& b = polygon[(i + 1) % count];

        if ((a.y > point.y) != (b.y > point.y)) {
            float t = (point.y - a.y) / (b.y - a.y);
            float x = a.x + t * (b.x - a.x);
            if (point.x < x)
                ++crossings;
        }
    }

    return (crossings % 2) == 1;
}