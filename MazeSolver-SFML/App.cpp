#include "Maze.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int main()
{
    Maze maze(9, 9, 40);

    maze.draw();



    return 0;
}
