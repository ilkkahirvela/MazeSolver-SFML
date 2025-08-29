#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Cell.hpp"

class Maze {
private:
    int _cols, _rows, _cellSize;
    std::vector<std::vector<Cell>> _grid; // 2D grid of cells

public:
    Maze(int cols, int rows, int cellSize);

    void draw();
    Cell& getCell(int x, int y);
};
