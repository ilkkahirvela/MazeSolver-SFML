#include "Maze.hpp"
#include "Cell.hpp"
#include <SFML/Graphics/Color.hpp>
#include <random>
#include <algorithm>
#include <vector>
#include <utility>

Maze::Maze(int cols, int rows, int cellSize)
    : _cols(cols), _rows(rows), _cellSize(cellSize)
{
    _grid.resize(_rows);

    for (int y = 0; y < _rows; y++) {
        _grid[y].reserve(_cols);
        for (int x = 0; x < _cols; x++) {
            _grid[y].emplace_back(x, y, _cellSize);
            _grid[y][x].setBlocked(true); // default: all walls
            _grid[y][x].shape.setOutlineColor(sf::Color::Black);
        }
    }
}

void Maze::reset() {
    for (int y = 0; y < _rows; y++) {
        for (int x = 0; x < _cols; x++) {
            _grid[y][x].setBlocked(true);
            _grid[y][x].shape.setOutlineColor(sf::Color::Black);
        }
    }
}

void Maze::generate() {
    // --- Recursive Backtracker (Depth-First Search) algorithm ---
    // Operates on odd coordinates as "rooms" and carves walls between them.

    if (_rows >= 3 && _cols >= 3) {
        std::mt19937 rng(std::random_device{}());

        // Check if a cell is inside the maze bounds (excluding borders)
        auto inBoundsInner = [&](int x, int y) {
            return x > 0 && x < _cols - 1 && y > 0 && y < _rows - 1;
            };

        // Mark a cell as carved (open)
        std::vector<std::vector<uint8_t>> open(_rows, std::vector<uint8_t>(_cols, 0));
        auto carve = [&](int cx, int cy) {
            _grid[cy][cx].setBlocked(false);
            open[cy][cx] = 1;
            };

        // Choose a random odd coordinate (ensures corridors between walls)
        auto randOdd = [&](int limit) -> int {
            if (limit <= 2) return 1 % limit; // handle tiny grids safely
            std::uniform_int_distribution<int> d(0, (limit - 3) / 2);
            return 1 + 2 * d(rng);
            };

        // Random starting cell
        int sx = (_cols > 2) ? randOdd(_cols) : 0;
        int sy = (_rows > 2) ? randOdd(_rows) : 0;

        std::vector<std::pair<int, int>> stack;
        stack.emplace_back(sx, sy);
        carve(sx, sy);

        const std::pair<int, int> DIRS[4] = { {2,0},{-2,0},{0,2},{0,-2} };

        // DFS loop
        while (!stack.empty()) {
            auto [cx, cy] = stack.back();

            // Collect uncarved neighbors
            std::vector<std::pair<int, int>> nbrs;
            nbrs.reserve(4);
            for (auto [dx, dy] : DIRS) {
                int nx = cx + dx, ny = cy + dy;
                if (inBoundsInner(nx, ny) && !open[ny][nx]) {
                    nbrs.emplace_back(nx, ny);
                }
            }

            if (!nbrs.empty()) {
                // Pick random neighbor and carve path
                std::shuffle(nbrs.begin(), nbrs.end(), rng);
                auto [nx, ny] = nbrs.front();

                // Carve wall between current cell and neighbor
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

        // Create entrance (top) and exit (bottom)
        for (int x = 1; x < _cols - 1; ++x) {
            if (open[1][x]) { _grid[0][x].setBlocked(false); break; }
        }
        for (int x = _cols - 2; x >= 1; --x) {
            if (open[_rows - 2][x]) { _grid[_rows - 1][x].setBlocked(false); break; }
        }
    }
}

void Maze::draw(sf::RenderTarget& target) {
    for (int y = 0; y < _rows; y++) {
        for (int x = 0; x < _cols; x++) {
            _grid[y][x].draw(target);
        }
    }
}

Cell& Maze::getCell(int x, int y) {
    return _grid[y][x];
}
