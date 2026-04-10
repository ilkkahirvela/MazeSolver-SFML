#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Cell.hpp"

/**
 * @class Maze
 * @brief Represents a 2D maze grid and handles generation and rendering.
 *
 * The Maze consists of a grid of Cell objects. A recursive backtracker
 * (depth-first search) algorithm is used to generate the maze structure.
 */
class Maze {
public:
    /**
     * @brief Construct a maze with given dimensions.
     * @param cols Number of columns (width in cells).
     * @param rows Number of rows (height in cells).
     * @param cellSize Pixel size of each cell.
     */
    Maze(int cols, int rows, int cellSize);

    /**
     * @brief Reset all cells back to blocked (wall) state.
     *
     * Call before generate() to produce a fresh maze in the same grid.
     */
    void reset();

    /**
     * @brief Generate a randomized maze using recursive backtracking.
     *
     * Carves passages between cells starting from a random odd coordinate.
     * Ensures there is one entrance at the top and one exit at the bottom.
     */
    void generate();

    /**
     * @brief Draw the entire maze grid to the given render target.
     * @param target SFML render target (e.g., window or render texture).
     */
    void draw(sf::RenderTarget& target);

    /**
     * @brief Access a specific cell by coordinates.
     * @param x Column index.
     * @param y Row index.
     * @return Reference to the requested Cell.
     */
    Cell& getCell(int x, int y);

    /**
     * @brief Access the underlying grid of cells.
     * @return Reference to 2D vector of cells.
     */
    std::vector<std::vector<Cell>>& getGrid() { return _grid; }

    /**
     * @brief Get the number of rows in the maze.
     */
    int getRows() const { return _rows; }

    /**
     * @brief Get the number of columns in the maze.
     */
    int getCols() const { return _cols; }

private:
    int _cols;      ///< Number of columns
    int _rows;      ///< Number of rows
    int _cellSize;  ///< Pixel size of each cell
    std::vector<std::vector<Cell>> _grid; ///< 2D grid of maze cells
};
