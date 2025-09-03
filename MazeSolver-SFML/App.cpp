#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

int main() {
    const int cols = 111, rows = 111, cellSize = 8;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(cols * cellSize, rows * cellSize)),
        "Maze Solver"
    );

    Maze maze(cols, rows, cellSize);
    maze.generate();

    Solver solver(maze);
    solver.start();

    // --- STATIC LAYER: draw maze once into a texture ---
    sf::RenderTexture staticLayer({ cols * cellSize, rows * cellSize });
    staticLayer.clear(sf::Color::White);
    maze.draw(staticLayer);   // draw entire maze once
    staticLayer.display();
    sf::Sprite mazeBackground(staticLayer.getTexture());

    // animation timing
    sf::Clock clock;
    const int stepMs = 1; // ms per animation step
    std::size_t visitedIndex = 0;
    std::size_t pathIndex = 0;

    std::shared_ptr<const std::vector<Cell*>> pathPtr;

    enum class Phase { Visiting, Path };
    Phase phase = Phase::Visiting;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Always draw the background first
        window.clear();
        window.draw(mazeBackground);

        if (phase == Phase::Visiting) {
            auto currentVisited = solver.getVisited();

            if (visitedIndex < currentVisited.size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    Cell* c = currentVisited[visitedIndex];
                    c->shape.setFillColor(sf::Color(150, 200, 255));
                    c->shape.setOutlineColor(sf::Color(150, 200, 255));

                    ++visitedIndex;
                    clock.restart();
                }
            }
            else if (solver.isFinished() && solver.foundPath()) {
                pathPtr = solver.getPath();
                pathIndex = 0;
                phase = Phase::Path;
                clock.restart();
            }
        }
        else if (phase == Phase::Path && pathPtr) {
            if (pathIndex < pathPtr->size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    Cell* c = (*pathPtr)[pathIndex];
                    c->shape.setFillColor(sf::Color::Red);
                    c->shape.setOutlineColor(sf::Color::Red);

                    ++pathIndex;
                    clock.restart();
                }
            }
        }

        // Draw dynamic cells
        for (std::size_t i = 0; i < visitedIndex; i++)
            window.draw(solver.getVisited()[i]->shape);

        if (pathPtr) {
            for (std::size_t i = 0; i < pathIndex; i++)
                window.draw((*pathPtr)[i]->shape);
        }

        window.display();
    }

    solver.join();
    return 0;
}
