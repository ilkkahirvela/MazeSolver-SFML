/**
 * @mainpage Maze Solver Visualization
 *
 * @section intro_sec Introduction
 * This project demonstrates maze generation and solving with an animated
 * visualization built using C++ and the SFML graphics library.
 *
 * @section algo_sec Algorithms
 * - Maze generation: Recursive Backtracking (Depth-First Search)
 * - Maze solving: Breadth-First Search (BFS)
 *
 * @section viz_sec Visualization
 * - Maze walls are drawn once into a static layer.
 * - Visited cells are shown in light blue as the solver explores.
 * - The final shortest path is shown in red once discovered.
 *
 * @section deps_sec Dependencies
 * - C++17 or newer
 * - SFML 3.0+ (graphics, window, system)
 *
 * @section usage_sec Usage
 * Build and run the program. An SFML window opens to show the maze generation
 * and solving process step by step.
 */

/**
 * @file app.cpp
 * @brief Maze generation and solver visualization using SFML.
 *
 * This program generates a maze, solves it using BFS in a separate thread,
 * and visualizes the solving process in real time.
 *
 * Features:
 * - Static rendering of maze walls (drawn once into a render texture).
 * - Dynamic rendering of visited cells (light blue) and final path (red).
 * - Step-by-step animated visualization.
 */

#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

 /**
  * @brief Main entry point of the application.
  *
  * Initializes a window, generates a maze, starts the solver in a separate
  * thread, and animates the solving process using SFML graphics.
  *
  * @return int Exit status code.
  */
int main() {
    /// Maze grid size and cell dimensions
    const int cols = 555, rows = 311, cellSize = 3;

    /// Create the SFML render window
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(cols * cellSize, rows * cellSize)),
        "Maze Solver"
    );

    /// Initialize and generate the maze
    Maze maze(cols, rows, cellSize);
    maze.generate();

    /// Create solver and start it in a background thread
    Solver solver(maze);
    solver.start();

    // ---------------------------------------------------------------------
    // STATIC LAYER: maze walls (drawn once into a render texture)
    // ---------------------------------------------------------------------
    sf::RenderTexture staticLayer({ cols * cellSize, rows * cellSize });
    staticLayer.clear(sf::Color::White);
    maze.draw(staticLayer);        ///< Draw entire maze once
    staticLayer.display();
    sf::Sprite mazeBackground(staticLayer.getTexture());

    // ---------------------------------------------------------------------
    // DYNAMIC LAYER: visited + path cells (incrementally updated)
    // ---------------------------------------------------------------------
    sf::RenderTexture dynamicLayer({ cols * cellSize, rows * cellSize });
    dynamicLayer.clear(sf::Color::Transparent);
    dynamicLayer.display();
    sf::Sprite dynamicOverlay(dynamicLayer.getTexture());

    // ---------------------------------------------------------------------
    // Animation control variables
    // ---------------------------------------------------------------------
    sf::Clock clock;                  ///< Timer for controlling animation speed
    const float stepMs = 0.5f;         ///< Delay between animation steps (ms)
    std::size_t visitedIndex = 0;     ///< Index of current visited cell
    std::size_t pathIndex = 0;        ///< Index of current path cell

    std::shared_ptr<const std::vector<Cell*>> pathPtr; ///< Shared pointer to solution path

    /// Solver phase (Visiting cells or showing final Path)
    enum class Phase { Visiting, Path };
    Phase phase = Phase::Visiting;

    // ---------------------------------------------------------------------
    // Main render loop
    // ---------------------------------------------------------------------
    while (window.isOpen()) {
        // Handle window events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // -------------------------------
        // Phase 1: Animate visited cells
        // -------------------------------
        if (phase == Phase::Visiting) {
            auto currentVisited = solver.getVisited();

            if (visitedIndex < currentVisited.size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    Cell* c = currentVisited[visitedIndex];
                    c->shape.setFillColor(sf::Color(150, 200, 255));
                    c->shape.setOutlineColor(sf::Color(150, 200, 255));

                    // Draw updated cell into dynamic layer (persistent)
                    dynamicLayer.draw(c->shape);
                    dynamicLayer.display();

                    ++visitedIndex;
                    clock.restart();
                }
            }
            else if (solver.isFinished() && solver.foundPath()) {
                // Switch to path animation phase
                pathPtr = solver.getPath();
                pathIndex = 0;
                phase = Phase::Path;
                clock.restart();
            }
        }
        // -------------------------------
        // Phase 2: Animate solution path
        // -------------------------------
        else if (phase == Phase::Path && pathPtr) {
            if (pathIndex < pathPtr->size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    Cell* c = (*pathPtr)[pathIndex];
                    c->shape.setFillColor(sf::Color::Red);
                    c->shape.setOutlineColor(sf::Color::Red);

                    // Draw updated cell into dynamic layer (persistent)
                    dynamicLayer.draw(c->shape);
                    dynamicLayer.display();

                    ++pathIndex;
                    clock.restart();
                }
            }
        }

        // -------------------------------
        // Compose final frame
        // -------------------------------
        window.clear();
        window.draw(mazeBackground);   ///< Static maze walls
        window.draw(dynamicOverlay);   ///< Incrementally drawn visited + path cells
        window.display();
    }

    /// Ensure solver thread is joined before exiting
    solver.join();
    return 0;
}
