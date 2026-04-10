#pragma once
#include <SFML/Graphics.hpp>

/**
 * @class Cell
 * @brief Represents a single grid cell in the maze.
 *
 * Each cell stores its grid position, state (blocked or open, visited or not),
 * and a rectangle shape for rendering with SFML.
 */
class Cell {
public:
    /// Grid coordinates
    int x, y;

    /// Whether this cell is blocked (wall) or open (path)
    bool isBlocked;

    /// SFML shape used to draw this cell
    sf::RectangleShape shape;

    /**
     * @brief Construct a cell at the given grid position.
     * @param x Column index in the maze grid.
     * @param y Row index in the maze grid.
     * @param size Pixel size of the cell.
     * @param blocked Whether the cell should be initially blocked.
     */
    Cell(int x, int y, int size, bool blocked = false);

    /**
     * @brief Set whether the cell is blocked.
     * @param blocked True if cell is a wall, false if it is open.
     */
    void setBlocked(bool blocked);

    /**
     * @brief Draw the cell on the given render target.
     * @param target Reference to an SFML render target (e.g., window or texture).
     */
    void draw(sf::RenderTarget& target);
};
