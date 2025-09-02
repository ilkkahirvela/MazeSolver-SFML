#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

int main() {
    const int cols = 75, rows = 75, cellSize = 12;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(cols * cellSize, rows * cellSize)),
        "Maze Solver"
    );

    Maze maze(cols, rows, cellSize);
    maze.generate();

    Solver solver(maze);
    solver.start();

    // animation timing
    sf::Clock clock;
    const int stepMs = 0.0001f; // ms per animation step
    std::size_t visitedIndex = 0;
    std::size_t pathIndex = 0;

    std::vector<Cell*> visitedSnapshot;
    std::shared_ptr<const std::vector<Cell*>> pathPtr;

    enum class Phase { Visiting, Path };
    Phase phase = Phase::Visiting;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (phase == Phase::Visiting) {
            // grab visited snapshot from solver
            auto currentVisited = solver.getVisited();

            if (visitedIndex < currentVisited.size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    currentVisited[visitedIndex]->shape.setFillColor(sf::Color(150, 200, 255)); // light blue
                    currentVisited[visitedIndex]->shape.setOutlineColor(sf::Color(150, 200, 255));
                    ++visitedIndex;
                    clock.restart();
                }
            }
            else if (solver.isFinished() && solver.foundPath()) {
                // all visited done, move to path phase
                pathPtr = solver.getPath();
                pathIndex = 0;
                phase = Phase::Path;
                clock.restart();
            }
        }
        else if (phase == Phase::Path && pathPtr) {
            if (pathIndex < pathPtr->size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    (*pathPtr)[pathIndex]->shape.setFillColor(sf::Color::Red);
                    (*pathPtr)[pathIndex]->shape.setOutlineColor(sf::Color::Red);
                    ++pathIndex;
                    clock.restart();
                }
            }
        }

        window.clear(sf::Color::White);
        maze.draw(window);
        window.display();
    }

    solver.join();
    return 0;
}
