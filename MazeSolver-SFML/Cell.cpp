#include "Cell.hpp"
#include <vector>

Cell::Cell(int x, int y, int size, bool blocked)
    : x(x), y(y), isBlocked(blocked), visited(false)
{
    shape.setSize(sf::Vector2f((float)size - 2, (float)size - 2));
    shape.setPosition(sf::Vector2f((float)(x * size + 1), (float)(y * size + 1)));
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color(200, 200, 200)); // light gray grid
    setBlocked(blocked);
}

void Cell::setBlocked(bool blocked) {
    isBlocked = blocked;
    if (isBlocked) {
        shape.setFillColor(sf::Color::Black);
    }
    else {
        shape.setFillColor(sf::Color::White);
        shape.setOutlineColor(sf::Color::White);
        // shape.setOutlineColor(sf::Color(200, 200, 200));
    }
}

void Cell::setVisited(bool v) {
    visited = v;
    if (visited) {
        shape.setFillColor(sf::Color::Yellow); // highlight visited for debug
    }
    else {
        setBlocked(isBlocked); // reset to normal appearance
    }
}

void Cell::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
