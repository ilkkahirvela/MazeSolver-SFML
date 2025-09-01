#include "Solver.hpp"
#include <queue>
#include <map>
#include <utility>
#include <algorithm>

Solver::Solver(Maze& maze) : _maze(maze){}

// BFS directions (up, down, left, right)
static const std::pair<int, int> DIRS4[4] = {
    {1,0}, {-1,0}, {0,1}, {0,-1}
};

std::vector<Cell*> Solver::solve() {
    auto& grid = _maze.getGrid();   // You’ll need Maze::getGrid()
    int rows = _maze.getRows();
    int cols = _maze.getCols();

    std::queue<std::pair<int, int>> q;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    // Find entrance (top row open cell)
    int startX = -1;
    for (int x = 0; x < cols; ++x) {
        if (!grid[0][x].isBlocked) { startX = x; break; }
    }
    if (startX == -1) return {};

    // Find exit (bottom row open cell)
    int goalX = -1;
    for (int x = 0; x < cols; ++x) {
        if (!grid[rows - 1][x].isBlocked) { goalX = x; break; }
    }
    if (goalX == -1) return {};

    std::pair<int, int> start = { startX, 0 };
    std::pair<int, int> goal = { goalX, rows - 1 };

    q.push(start);
    parent[start] = { -1, -1 };

    bool found = false;

    while (!q.empty()) {
        auto [cx, cy] = q.front(); q.pop();

        if (cx == goal.first && cy == goal.second) {
            found = true;
            break;
        }

        for (auto [dx, dy] : DIRS4) {
            int nx = cx + dx, ny = cy + dy;
            if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) continue;
            if (grid[ny][nx].isBlocked) continue;

            if (!parent.count({ nx, ny })) {
                parent[{nx, ny}] = { cx, cy };
                q.push({ nx, ny });
            }
        }
    }

    std::vector<Cell*> path;
    if (found) {
        auto cur = goal;
        while (cur.first != -1) {
            path.push_back(&grid[cur.second][cur.first]);
            cur = parent[cur];
        }
        std::reverse(path.begin(), path.end());
    }
    return path;
}
