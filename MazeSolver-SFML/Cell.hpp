#pragma once
#include <SFML/Graphics.hpp>

class Cell {
public:
    // Grid position
    int x, y;

    // State
    bool isBlocked;
    bool visited;

    // Visuals
    sf::RectangleShape shape;

    Cell(int x, int y, int size, bool blocked = false);

    void setBlocked(bool blocked);
    void setVisited(bool visited);

    void draw(sf::RenderTarget& target);

};
