#include "Cell.h"
#include <cmath>

using namespace std;
using namespace sf;

Cell::Cell(Vector2f center, float radius, int row, int col)
    : row(row), col(col), radius(radius) {
    shape.setPointCount(5);
    for (int i = 0; i < 5; ++i) {
        float angle = i * 2 * 3.14159265f / 5 - 3.14159265f / 2;
        shape.setPoint(i, Vector2f(cos(angle) * radius, sin(angle) * radius));
    }
    shape.setPosition(center);
    shape.setFillColor(Color::White);
    shape.setOutlineColor(Color::Black);
    shape.setOutlineThickness(1.f);
}

void Cell::draw(RenderWindow& window) const {
    window.draw(shape);
}

bool Cell::contains(Vector2f point) const {
    if (!shape.getGlobalBounds().contains(point)) return false;

    Transform trans = shape.getTransform();
    Vector2f localPoint = trans.getInverse().transformPoint(point);

    int intersections = 0;
    const int n = 5;
    for (int i = 0; i < n; ++i) {
        Vector2f p1 = shape.getPoint(i);
        Vector2f p2 = shape.getPoint((i + 1) % n);
        if (((p1.y > localPoint.y) != (p2.y > localPoint.y)) &&
            (localPoint.x < (p2.x - p1.x) * (localPoint.y - p1.y) / (p2.y - p1.y) + p1.x)) {
            intersections++;
        }
    }
    return (intersections % 2) == 1;
}

int Cell::getRow() const {
    return row;
}

int Cell::getCol() const {
    return col;
}

