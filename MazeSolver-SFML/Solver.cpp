#include "Solver.hpp"
#include <queue>
#include <map>
#include <algorithm>

static const std::pair<int, int> DIRS4[4] = { {1,0},{-1,0},{0,1},{0,-1} };

Solver::Solver(Maze& maze)
    : _maze(maze),
    _path(nullptr),
    _visited(std::make_shared<std::vector<Cell*>>())
{
}

void Solver::start() {
    _thread = std::thread([this] { run(); });
}

void Solver::join() {
    if (_thread.joinable()) _thread.join();
}

void Solver::run() {
    auto& grid = _maze.getGrid();
    const int rows = _maze.getRows();
    const int cols = _maze.getCols();

    // find start/goal on top/bottom rows
    int startX = -1, goalX = -1;
    for (int x = 0; x < cols; ++x) if (!grid[0][x].isBlocked) { startX = x; break; }
    for (int x = 0; x < cols; ++x) if (!grid[rows - 1][x].isBlocked) { goalX = x; break; }
    if (startX == -1 || goalX == -1) { _finished = true; return; }

    const std::pair<int, int> start{ startX, 0 };
    const std::pair<int, int> goal{ goalX, rows - 1 };

    std::queue<std::pair<int, int>> q;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    q.push(start);
    parent[start] = { -1,-1 };

    bool found = false;

    while (!q.empty()) {
        auto [cx, cy] = q.front(); q.pop();

        // record visited (thread-safe)
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _visited->push_back(&grid[cy][cx]);
        }

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

    // build path if found
    if (found) {
        std::vector<Cell*> pathVec;
        auto cur = goal;
        while (cur.first != -1) {
            pathVec.push_back(&grid[cur.second][cur.first]);
            cur = parent[cur];
        }
        std::reverse(pathVec.begin(), pathVec.end());

        std::lock_guard<std::mutex> lock(_mtx);
        _path = std::make_shared<std::vector<Cell*>>(std::move(pathVec));
        _found = true;
    }

    _finished = true;
}

std::shared_ptr<const std::vector<Cell*>> Solver::getPath() const {
    std::lock_guard<std::mutex> lock(_mtx);
    // allow returning nullptr if no path found / not ready
    return std::static_pointer_cast<const std::vector<Cell*>>(_path);
}

std::vector<Cell*> Solver::getVisited() const {
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_visited) return {};
    return *_visited; // copy snapshot
}
