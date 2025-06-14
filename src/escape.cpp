#include <SFML/Graphics.hpp>
#include "model/Pentagono.hpp"
#include "graphics/PentagonDrawer.hpp"
#include "utils/JsonLoader.hpp"

using namespace std;
using namespace sf;

int main() {
    RenderWindow window(VideoMode(1600, 1200), "Pentagon Grid");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("src/resources/roboto.ttf")) return -1;

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

    for (const auto& p : pentagonos) {
        PentagonDrawer::addConnectedPentagon(p.base_index, p.vertice1, p.vertice2, font, pentagons, verticesList, labels);
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color::White);
        for (size_t i = 0; i < pentagons.size(); ++i) {
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
