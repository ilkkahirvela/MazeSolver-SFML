#include "Solver.hpp"
#include <queue>
#include <unordered_map>
#include <algorithm>

/// Directions for BFS (right, left, down, up).
static const std::pair<int, int> DIRS4[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 16);
    }
};

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

    // --- Find start and goal cells (top row open, bottom row open) ---
    int startX = -1, goalX = -1;
    for (int x = 0; x < cols; ++x) if (!grid[0][x].isBlocked) { startX = x; break; }
    for (int x = 0; x < cols; ++x) if (!grid[rows - 1][x].isBlocked) { goalX = x; break; }
    if (startX == -1 || goalX == -1) { _finished = true; return; }

    const std::pair<int, int> start{ startX, 0 };
    const std::pair<int, int> goal{ goalX, rows - 1 };

    std::queue<std::pair<int, int>> q; ///< BFS queue.
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> parent; ///< Parent mapping for path reconstruction.

    q.push(start);
    parent[start] = { -1,-1 };

    bool found = false;

    // --- BFS loop ---
    while (!q.empty()) {
        auto [cx, cy] = q.front(); q.pop();

        // Record visited cell in a thread-safe way
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _visited->push_back(&grid[cy][cx]);
        }

        // Goal reached
        if (cx == goal.first && cy == goal.second) {
            found = true;
            break;
        }

        // Explore neighbors
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

    // --- Build path if found ---
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
    // Return nullptr if no path is available yet
    return std::static_pointer_cast<const std::vector<Cell*>>(_path);
}

std::vector<Cell*> Solver::getVisited() const {
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_visited) return {};
    return *_visited; // Copy snapshot for thread safety
}
