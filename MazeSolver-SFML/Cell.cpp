#include "Cell.hpp"
#include <vector>

Cell::Cell(int x, int y, int size, bool blocked)
    : x(x), y(y), isBlocked(blocked)
{
    // At small cell sizes drop the gap and outline - shape would be 0x0 otherwise
    float inset = (size >= 4) ? 1.f : 0.f;
    shape.setSize(sf::Vector2f(size - 2 * inset, size - 2 * inset));
    shape.setPosition(sf::Vector2f(x * size + inset, y * size + inset));
    shape.setOutlineThickness(inset);
    shape.setOutlineColor(sf::Color(200, 200, 200)); // light gray grid
    setBlocked(blocked);
}

void Cell::setBlocked(bool blocked) {
    isBlocked = blocked;
    if (isBlocked) {
        shape.setFillColor(sf::Color::Black); ///< Walls are black
    }
    else {
        shape.setFillColor(sf::Color::White); ///< Open cells are white
        shape.setOutlineColor(sf::Color::White);
        // shape.setOutlineColor(sf::Color(200, 200, 200)); // alternative grid outline
    }
}

void Cell::draw(sf::RenderTarget& target) {
    target.draw(shape);
}
