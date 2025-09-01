#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    int cols = 75;
    int rows = 75;
    int cellSize = 12;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(cols * cellSize, rows * cellSize)), "Maze Solver");
    window.setFramerateLimit(300);
    int stepsPerFrame = 3;

    Maze maze(cols, rows, cellSize);
    maze.generate();
    
    Solver solver(maze);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            // Request for closing the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Run BFS animation
        if (!solver.isFinished()) {
            for (int i = 0; i < stepsPerFrame && !solver.isFinished(); ++i) {
                solver.step();
            }
        }
        else if (solver.foundPath()) {
            // Once BFS is done, color the path green
            for (auto* cell : solver.getPath()) {
                cell->shape.setFillColor(sf::Color::Green);
            }
        }

        window.clear(sf::Color::White);
        maze.draw(window);   // <- draw maze and solver’s progress
        window.display();
    }
}
