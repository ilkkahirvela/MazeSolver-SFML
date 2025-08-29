#include "Maze.hpp"
#include "Cell.hpp"
#include <SFML/Graphics/Color.hpp>

Maze::Maze(int cols, int rows, int cellSize)
    : _cols(cols), _rows(rows), _cellSize(cellSize)
{
    _grid.resize(_rows);

    for (int y = 0; y < _rows; y++) {
        _grid[y].reserve(_cols);

        for (int x = 0; x < _cols; x++) {
            _grid[y].emplace_back(x, y, _cellSize);

            // Example: Color pattern based on position
            if ((x + y) % 2 == 0)
                _grid[y][x].setBlocked(false); // white cell
            else
                _grid[y][x].setBlocked(true);  // black cell

            // Or add more colors
            if ((x + y) % 5 == 0)
                _grid[y][x].shape.setFillColor(sf::Color::Green);
            else if ((x + y) % 7 == 0)
                _grid[y][x].shape.setFillColor(sf::Color::Blue);
        }
    }
}

void Maze::draw() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(_cols * _cellSize, _rows * _cellSize)), "Maze Skeleton");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            // Request for closing the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::White);

        for (int y = 0; y < _rows; y++) {
            for (int x = 0; x < _cols; x++) {
                _grid[y][x].draw(window);
            }
        }

        window.display();

    }
}


Cell& Maze::getCell(int x, int y) {
    return _grid[y][x];
}
