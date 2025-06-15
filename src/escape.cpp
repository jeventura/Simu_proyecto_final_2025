#include <iostream>
#include <SFML/Graphics.hpp>
#include "model/PentagonType.hpp"
#include "model/Pentagono.hpp"
#include "graphics/PentagonDrawer.hpp"
#include "utils/JsonLoader.hpp"
#include "utils/GeometryUtils.hpp"

using namespace std;
using namespace sf;

Color getColorByType(PentagonType type) {
    switch(type) {
        case PentagonType::START:
            return Color::Green;
        case PentagonType::FINISH:
            return Color::Blue;
        case PentagonType::WALL:
            return Color::Black;
        case PentagonType::ELECTRIC_WALL:
            return Color::Red;
        case PentagonType::FREE:
            return Color(160, 160, 160);
        case PentagonType::SWITCH:
            return Color::Yellow;
        default:
            return Color(160, 160, 160);
    }
}


int main() {
    RenderWindow window(VideoMode(1600, 1200), "Pentagon Grid");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("src/resources/roboto.ttf")) return 1;

    int selectedIndex = 0;

    vector<ConvexShape> pentagons;
    vector<vector<Vector2f>> verticesList;
    vector<Text> labels;

    vector<Vector2f> v1;
    auto p1 = PentagonDrawer::createInitialPentagon(800.f, 500.f, 0.f, v1);
    pentagons.push_back(p1);
    verticesList.push_back(v1);

    Vector2f center(0.f, 0.f);
    for (auto& p : v1) center += p;
    center /= 5.f;
    Text label("1", font, 24);
    label.setFillColor(Color::Black);
    label.setPosition(center.x - 6, center.y - 12);
    labels.push_back(label);

    vector<Pentagono> pentagonos;
    if (!loadPentagonosFromJson("src/resources/map_creation.json", pentagonos)) return 1;

    int type = returnPentagonTypeAsInt(PentagonType::START);
    for (int i = 0; i < pentagonos.size(); i++) {
        const Pentagono& p = pentagonos[i];

        // Encontramos el pentagono inicial y lo coloreamos
        if(p.type == type) {
            selectedIndex = i;
            pentagons[i].setFillColor(i == selectedIndex ? Color::Yellow : Color(160, 160, 160));
        }
        PentagonDrawer::addConnectedPentagon(p.base_index, p.vertice1, p.vertice2, font, pentagons, verticesList, labels);
    }

    int totalPentagonos = pentagons.size();
    vector<vector<int>> adjacencyList(totalPentagonos);

    int currentIndex = 1; // el primer pentágono es el 0
    for (const auto& p : pentagonos) {
        int base = p.base_index;
        int connected = currentIndex++;

        // Asegúrate de no acceder fuera del rango
        if (base < adjacencyList.size() && connected < adjacencyList.size()) {
            adjacencyList[base].push_back(connected);
            adjacencyList[connected].push_back(base);
        }
    }

    for (size_t i = 0; i < verticesList.size(); ++i) {
        for (size_t j = i + 1; j < verticesList.size(); ++j) {
            if (countSharedVertices(verticesList[i], verticesList[j]) >= 2) {
                // Verifica si ya están conectados
                if (find(adjacencyList[i].begin(), adjacencyList[i].end(), j) == adjacencyList[i].end()) {
                    adjacencyList[i].push_back(j);
                    adjacencyList[j].push_back(i);
                }
            }
        }
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                for (size_t i = 0; i < verticesList.size(); ++i) {
                    if (isPointInsidePolygon(verticesList[i], mousePos)) {
                        // Solo permitir cambio si i es adyacente a selectedIndex o es el mismo pentágono y si no es un wall o electric wall
                        if (
                            (i == selectedIndex || 
                            find(adjacencyList[selectedIndex].begin(), adjacencyList[selectedIndex].end(), i) != adjacencyList[selectedIndex].end()) &&
                            pentagonos[i].type != returnPentagonTypeAsInt(PentagonType::WALL)
                        ) {
                            selectedIndex = i;
                            break;
                        }
                    }
                }

                // Actualizar colores
                for (size_t i = 0; i < pentagons.size(); ++i) {
                    pentagons[i].setFillColor(i == selectedIndex ? Color::Yellow : Color(160, 160, 160));
                }
            }


        }

        window.clear(Color::White);
        for (size_t i = 0; i < pentagons.size(); ++i) {
            if(i != selectedIndex) {
                PentagonType type = static_cast<PentagonType>(pentagonos[i].type);
                Color color = getColorByType(type);
                pentagons[i].setFillColor(color);
            }
        
            window.draw(pentagons[i]);
            window.draw(labels[i]);

            for (int j = 0; j < 5; ++j) {
                Vector2f v = verticesList[i][j];
                Text vLabel(to_string(j), font, 14);
                vLabel.setFillColor(Color::Red);
                vLabel.setPosition(v.x - 5, v.y - 5);
                window.draw(vLabel);
            }
        }

        window.display();
    }

    return 0;
}
