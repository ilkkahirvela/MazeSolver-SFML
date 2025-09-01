#include "Maze.hpp"
#include "Cell.hpp"
#include <SFML/Graphics/Color.hpp>
#include <random>
#include <algorithm>
#include <vector>
#include <utility>


// Maze initial contructor
Maze::Maze(int cols, int rows, int cellSize)
    : _cols(cols), _rows(rows), _cellSize(cellSize)
{
    _grid.resize(_rows);

    for (int y = 0; y < _rows; y++) {
        _grid[y].reserve(_cols);
        for (int x = 0; x < _cols; x++) {
            _grid[y].emplace_back(x, y, _cellSize);
            _grid[y][x].setBlocked(true); // default all to blocked
            _grid[y][x].shape.setOutlineColor(sf::Color::Black);
        }
    }
}

void Maze::generate() {
    // --- DFS MAZE (recursive backtracker) on a tile grid ---
    // Uses odd (x,y) as "rooms" and carves walls between them.

    if (_rows >= 3 && _cols >= 3) {
        std::mt19937 rng(std::random_device{}());

        auto inBoundsInner = [&](int x, int y) {
            return x > 0 && x < _cols - 1 && y > 0 && y < _rows - 1;
            };

        // Track carved (open) tiles to avoid relying on Cell internals.
        std::vector<std::vector<uint8_t>> open(_rows, std::vector<uint8_t>(_cols, 0));

        auto carve = [&](int cx, int cy) {
            _grid[cy][cx].setBlocked(false);
            open[cy][cx] = 1;
            };

        // Pick a random odd start (or (1,1) if minimal)
        auto randOdd = [&](int limit) -> int {
            if (limit <= 2) return 1 % limit; // handle tiny grids
            std::uniform_int_distribution<int> d(0, (limit - 3) / 2);
            return 1 + 2 * d(rng);
            };

        int sx = (_cols > 2) ? randOdd(_cols) : 0;
        int sy = (_rows > 2) ? randOdd(_rows) : 0;

        std::vector<std::pair<int, int>> stack;
        stack.emplace_back(sx, sy);
        carve(sx, sy);

        const std::pair<int, int> DIRS[4] = { {2,0},{-2,0},{0,2},{0,-2} };

        while (!stack.empty()) {
            auto [cx, cy] = stack.back();

            // collect uncarved odd neighbors
            std::vector<std::pair<int, int>> nbrs;
            nbrs.reserve(4);
            for (auto [dx, dy] : DIRS) {
                int nx = cx + dx, ny = cy + dy;
                if (inBoundsInner(nx, ny) && !open[ny][nx]) {
                    nbrs.emplace_back(nx, ny);
                }
            }

            if (!nbrs.empty()) {
                std::shuffle(nbrs.begin(), nbrs.end(), rng);
                auto [nx, ny] = nbrs.front();

                // carve the wall between (cx,cy) and (nx,ny)
                int wx = cx + (nx - cx) / 2;
                int wy = cy + (ny - cy) / 2;
                carve(wx, wy);
                carve(nx, ny);

                stack.emplace_back(nx, ny);
            }
            else {
                stack.pop_back();
            }
        }

        // Add an entrance at top and exit at bottom
        // Find an open tile on row 1 and row _rows-2 and punch through the border.
        for (int x = 1; x < _cols - 1; ++x) {
            if (open[1][x]) { _grid[0][x].setBlocked(false); break; }
        }
        for (int x = _cols - 2; x >= 1; --x) {
            if (open[_rows - 2][x]) { _grid[_rows - 1][x].setBlocked(false); break; }
        }
    }
}

// Displaying the maze
void Maze::draw(sf::RenderWindow& window) {
    for (int y = 0; y < _rows; y++) {
        for (int x = 0; x < _cols; x++) {
            _grid[y][x].draw(window);
        }
    }
}

Cell& Maze::getCell(int x, int y) {
    return _grid[y][x];
}
