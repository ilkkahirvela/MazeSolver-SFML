#pragma once
#include "Maze.hpp"
#include <queue>
#include <map>
#include <utility>
#include <vector>

class Solver {
public:
    Solver(Maze& maze);
    
    // Returns the path as a vector of Cell pointers
    std::vector<Cell*> solve();

private:
    Maze& _maze;
};
