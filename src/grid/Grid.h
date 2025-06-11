#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "../cell/Cell.h"

using namespace std;
using namespace sf;

class Grid {
public:
    Grid(int rows, int cols, float radius);

    void draw(RenderWindow& window) const;
    bool handleClick(Vector2f mousePos) const;

private:
    vector<Cell> cells;
    int rows, cols;
    float radius;
};

#endif
