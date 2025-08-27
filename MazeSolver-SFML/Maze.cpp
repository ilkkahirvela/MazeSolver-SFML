#include "Maze.hpp"

using namespace std;
using namespace sf;

Maze::Maze(int cols, int rows, int cellSize) 
	: _cols(cols), _rows(rows), _cellSize(cellSize) {}

// Algorithm for generating the maze
void Maze::generate() {

	// Create the main window
    RenderWindow window(VideoMode(Vector2u(_cols * _cellSize, _rows * _cellSize), 32), "Maze skeleton");

    // Base cell shape (color will be changed per cell)
    RectangleShape cellShape({ (float)_cellSize - 2, (float)_cellSize - 2 });
    cellShape.setFillColor(Color::Black);
    cellShape.setOutlineThickness(1.5f);

    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }

        // Clear the window with white background
        window.clear(Color::White);

        // Draw the grid
        for (int y = 0; y < _rows; y++)
        {
            for (int x = 0; x < _cols; x++)
            {
				// Example: skip column 4 and leave a white gap in the maze
                if (x == 4) {
                    continue;
                }

                // Check if cell is on the border
                if (x == 0 || y == 0 || x == _cols - 1 || y == _rows - 1) {
                    cellShape.setOutlineColor(Color::Red); // border color
                }
                else {
                    cellShape.setOutlineColor(Color::Green); // inner cells
                }

                Vector2f position((float)(x * _cellSize + 1), (float)(y * _cellSize + 1));
                cellShape.setPosition(position);
                window.draw(cellShape);
            }
        }

        window.display();
    }

}

// Drawing code
void Maze::draw(RenderWindow& window) {

}
