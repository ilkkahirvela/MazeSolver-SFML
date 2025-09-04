#pragma once
#include "Maze.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

/**
 * @class Solver
 * @brief Solves a generated maze using Breadth-First Search (BFS).
 *
 * The solver runs BFS in a background thread. During execution it records
 * both visited cells (for visualization) and the final shortest path
 * (if found). Thread-safety is ensured with mutexes and atomics.
 */
class Solver {
public:
    /**
     * @brief Construct a solver for a given maze.
     * @param maze Reference to the Maze object to be solved.
     */
    explicit Solver(Maze& maze);

    /**
     * @brief Start the BFS algorithm in a background thread.
     */
    void start();

    /**
     * @brief Wait for the solver thread to finish (clean up).
     */
    void join();

    /**
     * @brief Check whether the solver has finished running.
     * @return true if finished, false otherwise.
     */
    bool isFinished() const { return _finished.load(); }

    /**
     * @brief Check whether a valid path from entrance to exit was found.
     * @return true if a path was found, false otherwise.
     */
    bool foundPath() const { return _found.load(); }

    /**
     * @brief Get the final solution path.
     *
     * Returns a shared pointer to the path as a vector of Cell pointers.
     * If the path is not yet available, returns nullptr.
     *
     * @return Shared pointer to const path vector.
     */
    std::shared_ptr<const std::vector<Cell*>> getPath() const;

    /**
     * @brief Get the list of visited cells.
     *
     * Returns a copy of the visited cells vector, so the caller can safely
     * iterate without risk of data races.
     *
     * @return Vector of visited cell pointers.
     */
    std::vector<Cell*> getVisited() const;

private:
    /**
     * @brief Run the BFS algorithm (executed in a background thread).
     */
    void run();

    Maze& _maze;   ///< Reference to the maze being solved.
    std::thread _thread; ///< Worker thread for running BFS.
    std::atomic<bool> _finished{ false }; ///< Whether solver has completed.
    std::atomic<bool> _found{ false };    ///< Whether a path was found.

    mutable std::mutex _mtx; ///< Mutex for thread-safe access to shared data.
    std::shared_ptr<std::vector<Cell*>> _path;    ///< Solution path (if found).
    std::shared_ptr<std::vector<Cell*>> _visited; ///< List of visited cells.
};
