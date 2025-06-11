#ifndef CELL_H
#define CELL_H

#include <SFML/Graphics.hpp>

using namespace sf;

class Cell {
public:
    Cell(Vector2f center, float radius, int row, int col);

    void draw(RenderWindow& window) const;
    bool contains(Vector2f point) const;

    int getRow() const;
    int getCol() const;

private:
    ConvexShape shape;
    float radius;
    int row, col;
};

#endif
