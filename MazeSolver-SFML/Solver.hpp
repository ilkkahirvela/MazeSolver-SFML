#pragma once
#include "Maze.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

class Solver {
public:
    explicit Solver(Maze& maze);

    void start();                // run BFS in a background thread
    void join();                 // clean up
    bool isFinished() const { return _finished.load(); }
    bool foundPath()   const { return _found.load(); }

    // returns shared path
    std::shared_ptr<const std::vector<Cell*>> getPath() const;

private:
    void run();                  // BFS implementation

    Maze& _maze;
    std::thread _thread;
    std::atomic<bool> _finished{ false };
    std::atomic<bool> _found{ false };

    mutable std::mutex _mtx;
    std::shared_ptr<std::vector<Cell*>> _path; // set once on success
};
