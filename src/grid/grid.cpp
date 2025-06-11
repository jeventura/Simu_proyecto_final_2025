#include "Grid.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;

Grid::Grid(int rows, int cols, float radius)
    : rows(rows), cols(cols), radius(radius) {
    float width = 2 * radius * cos(3.14159265f / 5); // ancho pentágono
    float height = radius + radius * sin(3.14159265f / 5); // alto pentágono

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float x = c * width;
            float y = r * height * 1.0f;

            // Ajuste final, sumar radius para centrar pentágono
            x += radius;
            y += radius;

            cells.emplace_back(Vector2f(x, y), radius, r, c);
        }
    }
}

void Grid::draw(RenderWindow& window) const {
    for (const auto& cell : cells)
        cell.draw(window);
}

bool Grid::handleClick(Vector2f mousePos) const {
    for (const auto& cell : cells) {
        if (cell.contains(mousePos)) {
            cout << "Clicked cell at row " << cell.getRow()
                      << ", col " << cell.getCol() << std::endl;
            return true;
        }
    }
    return false;
}
