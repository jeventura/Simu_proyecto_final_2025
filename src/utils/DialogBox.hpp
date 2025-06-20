#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

class DialogBox {
    public:
        DialogBox(Font& font, float width, float height, RenderWindow& window);

        void setText(const string& line);
        void show();
        void dismiss();
        void draw(RenderWindow& window);
        bool isVisible() const;
        bool isButtonClicked(Vector2f mousePos) const;
    
    private:
        RectangleShape background, button;
        Text text, buttonText;
        bool visible = false;
};