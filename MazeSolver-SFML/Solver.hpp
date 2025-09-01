#pragma once
#include "Maze.hpp"
#include <queue>
#include <map>
#include <utility>
#include <vector>

class Solver {
public:
    Solver(Maze& maze);

    // Run one BFS step
    bool step();

    // Once BFS finishes, reconstruct path
    std::vector<Cell*> getPath();

    bool isFinished() const { return _finished; }
    bool foundPath() const { return _found; }

private:
    Maze& _maze;
    std::queue<std::pair<int, int>> _q;
    std::map<std::pair<int, int>, std::pair<int, int>> _parent;

    std::pair<int, int> _start;
    std::pair<int, int> _goal;

    bool _initialized = false;
    bool _finished = false;
    bool _found = false;
};
