#include "PentagonDrawer.hpp"
#include <cmath>

const float radius = 80.f;

using namespace std;
using namespace sf;


ConvexShape PentagonDrawer::createInitialPentagon(float cx, float cy, float rotation, vector<Vector2f>& vertices) {
    ConvexShape pentagon;
    pentagon.setPointCount(5);
    float angleOffset = M_PI / 2 + rotation;
    vertices.clear();

    for (int i = 0; i < 5; ++i) {
        float angle = angleOffset + i * 2 * M_PI / 5;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        Vector2f point(x, y);
        pentagon.setPoint(i, point);
        vertices.push_back(point);
    }

    pentagon.setFillColor(Color(160, 160, 160));
    pentagon.setOutlineThickness(2);
    pentagon.setOutlineColor(Color::Black);
    return pentagon;
}

ConvexShape PentagonDrawer::createConnectedPentagon(
    const vector<Vector2f>& base,
    int idx1, int idx2,
    vector<Vector2f>& newVertices
) {
    Vector2f baseA = base[idx1];
    Vector2f baseB = base[idx2];
    Vector2f baseMid = (baseA + baseB) / 2.f;

    vector<Vector2f> dummy;
    createInitialPentagon(0.f, 0.f, M_PI, dummy);

    Vector2f newA = dummy[1];
    Vector2f newB = dummy[0];
    Vector2f newMid = (newA + newB) / 2.f;

    float angleBase = atan2(baseB.y - baseA.y, baseB.x - baseA.x);
    float angleNew = atan2(newB.y - newA.y, newB.x - newA.x);
    float rotation = angleBase - angleNew;

    vector<Vector2f> rotated;
    for (auto pt : dummy) {
        float x = pt.x * cos(rotation) - pt.y * sin(rotation);
        float y = pt.x * sin(rotation) + pt.y * cos(rotation);
        rotated.push_back({x, y});
    }

    Vector2f rotatedMid = (rotated[1] + rotated[0]) / 2.f;
    Vector2f translation = baseMid - rotatedMid;

    newVertices.clear();
    ConvexShape result;
    result.setPointCount(5);
    for (int i = 0; i < 5; ++i) {
        Vector2f point = rotated[i] + translation;
        result.setPoint(i, point);
        newVertices.push_back(point);
    }

    result.setFillColor(Color(160, 160, 160));
    result.setOutlineThickness(2);
    result.setOutlineColor(Color::Black);
    return result;
}

void PentagonDrawer::addConnectedPentagon(
    int baseIndex, int v1, int v2, Font& font,
    vector<ConvexShape>& pentagons,
    vector<vector<Vector2f>>& verticesList,
    vector<Text>& labels
) {
    vector<Vector2f> newVertices;
    ConvexShape pent = createConnectedPentagon(verticesList[baseIndex], v1, v2, newVertices);

    pentagons.push_back(pent);
    verticesList.push_back(newVertices);

    Vector2f center(0.f, 0.f);
    for (const auto& pt : newVertices) center += pt;
    center /= 5.f;

    Text label(to_string(pentagons.size()), font, 24);
    label.setFillColor(Color::Black);
    label.setPosition(center.x - 6, center.y - 12);
    labels.push_back(label);
}
