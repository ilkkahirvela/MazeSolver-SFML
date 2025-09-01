#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>

using namespace std;

int main()
{
    // Generate the maze
    // Have to use odd numbers for rows and cols
    Maze maze(45, 45, 20);
    maze.generate();

    Solver solver(maze);
    auto path = solver.solve();

    for (auto* cell : path) {
        cell->shape.setFillColor(sf::Color::Green);
    }

    maze.draw();


    return 0;
}
