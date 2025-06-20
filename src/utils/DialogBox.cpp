#include "DialogBox.hpp"

DialogBox::DialogBox(Font& font, float width, float height, RenderWindow& window) {
    float x = (window.getSize().x - width) / 2.f;
    float y = (window.getSize().y - height) / 2.f;

    background.setSize({width, height});
    background.setFillColor(Color(0, 0, 0, 180));
    background.setPosition(x, y);

    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(Color::White);
    text.setPosition(x + 20, y + 10);

    button.setSize({120, 40});
    button.setFillColor(Color(100, 100, 255));
    button.setPosition(x + width - 140, y + height - 50);

    FloatRect btnBounds = buttonText.getLocalBounds();
    buttonText.setFont(font);
    buttonText.setCharacterSize(18);
    buttonText.setFillColor(Color::White);
    buttonText.setString("Aceptar");
    buttonText.setPosition(
        button.getPosition().x + (button.getSize().x - btnBounds.width) / 2.f - btnBounds.left,
        button.getPosition().y + (button.getSize().y - btnBounds.height) / 2.f - btnBounds.top
    );
}

void DialogBox::setText(const string& line) {
    text.setString(line);
}

void DialogBox::show() {
    visible = true;
}

void DialogBox::dismiss() {
    visible = false;
}

bool DialogBox::isVisible() const {
    return visible;
}

bool DialogBox::isButtonClicked(Vector2f mousePos) const {
    return visible && button.getGlobalBounds().contains(mousePos);
}

void DialogBox::draw(RenderWindow& window) {
    if (!visible) return;
    window.draw(background);
    window.draw(text);
    window.draw(button);
    window.draw(buttonText);
}