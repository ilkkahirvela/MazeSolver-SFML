#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;

class Maze {
public:
	Maze(int cols, int rows, int cellSize);
	void generate();						// generate the maze
	void draw(RenderWindow& window);	// draws the maze

private:
	int _cols, _rows, _cellSize;

};