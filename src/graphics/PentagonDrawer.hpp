#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

class PentagonDrawer {
public:
    static ConvexShape createInitialPentagon(float cx, float cy, float rotation, vector<Vector2f>& vertices);
    static ConvexShape createConnectedPentagon(
        const vector<Vector2f>& baseVertices,
        int v1, int v2,
        vector<Vector2f>& newVertices
    );

    static void addConnectedPentagon(
        int baseIndex,
        int v1, int v2,
        Font& font,
        vector<ConvexShape>& pentagons,
        vector<vector<Vector2f>>& verticesList,
        vector<Text>& labels
    );
};
