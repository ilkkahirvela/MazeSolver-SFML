#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

int main() {
    ///// const int cols = 55, rows = 55, cellSize = 12;
    const int cols = 555, rows = 311, cellSize = 3; // 57x more cells

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(cols * cellSize, rows * cellSize)),
        "Maze Solver"
    );

    Maze maze(cols, rows, cellSize);
    maze.generate();

    Solver solver(maze);
    solver.start();

    // --- STATIC LAYER: maze walls ---
    sf::RenderTexture staticLayer({ cols * cellSize, rows * cellSize });
    staticLayer.clear(sf::Color::White);
    maze.draw(staticLayer);
    staticLayer.display();
    sf::Sprite mazeBackground(staticLayer.getTexture());

    // --- DYNAMIC LAYER: visited + path cells ---
    sf::RenderTexture dynamicLayer({ cols * cellSize, rows * cellSize });
    dynamicLayer.clear(sf::Color::Transparent);
    dynamicLayer.display();
    sf::Sprite dynamicOverlay(dynamicLayer.getTexture());

    // animation timing
    sf::Clock clock;
    ///// const int stepMs = 30;
    const int stepMs = 0.5;
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

        if (phase == Phase::Visiting) {
            auto currentVisited = solver.getVisited();

            if (visitedIndex < currentVisited.size()) {
                if (clock.getElapsedTime().asMilliseconds() >= stepMs) {
                    Cell* c = currentVisited[visitedIndex];
                    c->shape.setFillColor(sf::Color(150, 200, 255));
                    c->shape.setOutlineColor(sf::Color(150, 200, 255));

                    // Draw into dynamic layer (persistent)
                    dynamicLayer.draw(c->shape);
                    dynamicLayer.display();

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

                    // Draw into dynamic layer (persistent)
                    dynamicLayer.draw(c->shape);
                    dynamicLayer.display();

                    ++pathIndex;
                    clock.restart();
                }
            }
        }

        // Compose final frame
        window.clear();
        window.draw(mazeBackground);   // static maze walls
        window.draw(dynamicOverlay);   // all visited + path drawn incrementally
        window.display();
    }

    solver.join();
    return 0;
}
