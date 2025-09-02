#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

int main() {
    const int cols = 75, rows = 75, cellSize = 6;

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
    const int stepMs = 1.f; // smaller -> faster animation
    std::size_t visitedIndex = 0;
    std::size_t pathIndex = 0;

    std::vector<Cell*> visitedSnapshot;
    std::shared_ptr<const std::vector<Cell*>> pathPtr;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // pull a fresh snapshot of visited cells (copy) each frame
        auto currentVisited = solver.getVisited();

        // animate newly discovered visited cells, one (or more) per tick
        if (visitedIndex < currentVisited.size()) {
            if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                // color a small batch per tick if you want faster coloring:
                int batch = 1; // increase to color more per frame
                for (int b = 0; b < batch && visitedIndex < currentVisited.size(); ++b, ++visitedIndex) {
                    currentVisited[visitedIndex]->shape.setFillColor(sf::Color(150, 200, 255)); // light blue
                    currentVisited[visitedIndex]->shape.setOutlineColor(sf::Color(150, 200, 255)); // light blue
                }
                clock.restart();
            }
        }

        // once solver finished and path exists, start animating path (overwrites visited color)
        if (solver.isFinished() && solver.foundPath() && !pathPtr) {
            pathPtr = solver.getPath();
            pathIndex = 0;
            clock.restart();
        }

        if (pathPtr && pathIndex < pathPtr->size()) {
            if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                // color path cells gradually
                (*pathPtr)[pathIndex]->shape.setFillColor(sf::Color::Red);
                (*pathPtr)[pathIndex]->shape.setOutlineColor(sf::Color::Red);
                ++pathIndex;
                clock.restart();
            }
        }

        window.clear(sf::Color::White);
        maze.draw(window);
        window.display();
    }

    solver.join();
    return 0;
}