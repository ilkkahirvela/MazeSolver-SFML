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
    const float pathStepMs = 0.1f;
    std::size_t pathIndex = 0;
    auto path = std::shared_ptr<const std::vector<Cell*>>(nullptr);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // grab path once solver finished
        if (!path && solver.isFinished() && solver.foundPath()) {
            path = solver.getPath();
            pathIndex = 0;
            clock.restart();
        }

        // animate path coloring
        if (path && pathIndex < path->size()) {
            if (clock.getElapsedTime().asMilliseconds() >= pathStepMs) {
                (*path)[pathIndex]->shape.setFillColor(sf::Color::Green);
                ++pathIndex;
                clock.restart();
            }
        }

        window.clear(sf::Color::White);
        maze.draw(window);
        window.display();
    }

    solver.join(); // clean shutdown
    return 0;
}