#include <SFML/Graphics.hpp>
#include "grid/Grid.h"

using namespace sf;

int main() {
    RenderWindow window(VideoMode(800, 600), "Escape the Grid");
    window.setFramerateLimit(60);

    Grid grid(10, 10, 30.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(
                    {event.mouseButton.x, event.mouseButton.y});
                grid.handleClick(mousePos);
            }
        }

        window.clear(Color::Black);
        grid.draw(window);
        window.display();
    }

    return 0;
}
