#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Cell.hpp"

class Maze {
public:
    Maze(int cols, int rows, int cellSize);

    void generate();
    void draw(sf::RenderWindow& window);
    Cell& getCell(int x, int y);
    std::vector<std::vector<Cell>>& getGrid() { return _grid; }
    int getRows() const { return _rows; }
    int getCols() const { return _cols; }

private:
    int _cols, _rows, _cellSize;
    std::vector<std::vector<Cell>> _grid; // 2D grid of cells
};
