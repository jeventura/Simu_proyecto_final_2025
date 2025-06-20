#include <iostream>
#include <SFML/Graphics.hpp>
#include "model/PentagonType.hpp"
#include "model/Pentagono.hpp"
#include "graphics/PentagonDrawer.hpp"
#include "utils/JsonLoader.hpp"
#include "utils/GeometryUtils.hpp"
#include "utils/DialogBox.hpp"

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
            return Color::Green;
        default:
            return Color(160, 160, 160);
    }
}


int main() {
    RenderWindow window(VideoMode(1600, 1200), "Pentagon Grid");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("src/resources/roboto.ttf")) return 1;

    DialogBox dialog(font, 600, 120, window);

    int selectedIndex = 0;

    map<Pentagono, bool> switches;

    vector<ConvexShape> pentagons;
    vector<vector<Vector2f>> verticesList;
    vector<Text> labels;

    vector<Vector2f> v1;
    auto p1 = PentagonDrawer::createInitialPentagon(800.f, 500.f, 0.f, v1);
    pentagons.push_back(p1);
    verticesList.push_back(v1);

    vector<Pentagono> pentagonos;
    int steps;
    if(!loadDataFromJson("src/resources/map_creation.json", pentagonos, steps)) return 1;

    Vector2f center(0.f, 0.f);
    for (auto& p : v1) center += p;
    center /= 5.f;
    Text label("1", font, 24), stepsTitle("Pasos restantes: " + to_string(steps), font, 24);
    FloatRect bounds = stepsTitle.getLocalBounds();

    label.setFillColor(Color::Black);
    label.setPosition(center.x - 6, center.y - 12);
    labels.push_back(label);

    stepsTitle.setFillColor(Color::Black);
    stepsTitle.setOrigin(bounds.left + bounds.width, bounds.top);
    stepsTitle.setPosition(window.getSize().x - 10, 10);

    int startType = returnPentagonTypeAsInt(PentagonType::START);
    int endType = returnPentagonTypeAsInt(PentagonType::FINISH);
    int switchType = returnPentagonTypeAsInt(PentagonType::SWITCH);
    int wallType = returnPentagonTypeAsInt(PentagonType::WALL);
    int electricWallType = returnPentagonTypeAsInt(PentagonType::ELECTRIC_WALL);
    for (int i = 0; i < pentagonos.size(); i++) {
        const Pentagono& p = pentagonos[i];

        // Encontramos el pentagono inicial y lo coloreamos
        if(p.type == startType) {
            selectedIndex = i;
            pentagons[i].setFillColor(i == selectedIndex ? Color::Yellow : Color(160, 160, 160));
        } else if (p.type == switchType) {
            switches[p] = false;
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

            // Enter o Espacio
            if (event.type == Event::KeyPressed &&
            (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Space)) {
                if (dialog.isVisible()) {
                    dialog.dismiss();
                }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                // Clic en botón
                if (dialog.isButtonClicked(mousePos)) {
                        dialog.dismiss();
                        return 1;
                    }

                if(!dialog.isVisible()) {
                    for (size_t i = 0; i < verticesList.size(); ++i) {
                        if (isPointInsidePolygon(verticesList[i], mousePos)) {

                            bool isWall = pentagonos[i].type == wallType;
                            bool isSwitch = pentagonos[i].type == switchType;
                            bool areSwitchesOpen, isElectricWall;

                            if(isSwitch) {
                                switches[pentagonos[i]] = !switches[pentagonos[i]];
                                areSwitchesOpen = all_of(
                                    switches.begin(),
                                    switches.end(),
                                    [](const pair<Pentagono, bool>& pair) {
                                        return pair.second;
                                    }
                                );
                            }

                            isElectricWall = pentagonos[i].type == electricWallType && !areSwitchesOpen;

                            // Solo permitir cambio si i es adyacente a selectedIndex o es el mismo pentágono y si no es un wall o electric wall
                            if (
                                (i == selectedIndex || 
                                find(adjacencyList[selectedIndex].begin(), adjacencyList[selectedIndex].end(), i) != adjacencyList[selectedIndex].end()) &&
                                !isWall && !isElectricWall
                            ) {
                                selectedIndex = i;
                                steps--;
                                if(pentagonos[selectedIndex].type == endType && steps >= 0) {
                                    dialog.setText("Ganaste!");
                                    dialog.show();
                                } else if(pentagonos[selectedIndex].type != endType && steps <= 0) {
                                    dialog.setText("Te quedaste sin movimiento!");
                                    dialog.show();
                                }
                                cout << "Steps: " << steps << endl;
                                break;
                            }
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

        dialog.draw(window);

        // Actualziar contador de pasos
        stepsTitle.setString("Pasos restantes: " + to_string(steps));
        window.draw(stepsTitle);
        window.display();
    }

    return 0;
}
