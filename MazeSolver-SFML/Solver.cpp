#include "Solver.hpp"
#include <algorithm>

static const std::pair<int, int> DIRS4[4] = {
    {1,0}, {-1,0}, {0,1}, {0,-1}
};

Solver::Solver(Maze& maze) : _maze(maze) {}

bool Solver::step() {
    if (_finished) return true;

    auto& grid = _maze.getGrid();
    int rows = _maze.getRows();
    int cols = _maze.getCols();

    // First call → find start/goal + init queue
    if (!_initialized) {
        int startX = -1;
        for (int x = 0; x < cols; ++x) {
            if (!grid[0][x].isBlocked) { startX = x; break; }
        }
        int goalX = -1;
        for (int x = 0; x < cols; ++x) {
            if (!grid[rows - 1][x].isBlocked) { goalX = x; break; }
        }
        if (startX == -1 || goalX == -1) {
            _finished = true;
            return true;
        }

        _start = { startX, 0 };
        _goal = { goalX, rows - 1 };
        _q.push(_start);
        _parent[_start] = { -1,-1 };
        _initialized = true;
    }

    if (_q.empty()) {
        _finished = true;
        return true;
    }

    auto [cx, cy] = _q.front(); _q.pop();

    // Color visited nodes
    grid[cy][cx].shape.setFillColor(sf::Color(100, 150, 255));

    if (cx == _goal.first && cy == _goal.second) {
        _finished = true;
        _found = true;
        return true;
    }

    for (auto [dx, dy] : DIRS4) {
        int nx = cx + dx, ny = cy + dy;
        if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) continue;
        if (grid[ny][nx].isBlocked) continue;

        if (!_parent.count({ nx, ny })) {
            _parent[{nx, ny}] = { cx, cy };
            _q.push({ nx, ny });
        }
    }

    return false;
}

std::vector<Cell*> Solver::getPath() {
    std::vector<Cell*> path;
    if (!_found) return path;

    auto& grid = _maze.getGrid();
    auto cur = _goal;

    while (cur.first != -1) {
        path.push_back(&grid[cur.second][cur.first]);
        cur = _parent[cur];
    }
    std::reverse(path.begin(), path.end());
    return path;
}
