#include "grid.h"

#include <memory>

Grid::Grid() : width(0), height(0), grid(nullptr) {}

Grid::Grid(int width, int height) : width(width), height(height)
{
    grid = (int **)std::calloc(height, sizeof(int *));
    for (size_t i = 0; i < height; i++)
    {
        grid[i] = (int *)std::calloc(width, sizeof(int));
    }
}

int& Grid::at(const Location& loc) {return grid[loc.x][loc.y];}
int& Grid::at(int x, int y) {return grid[x][y];}
int& Grid::at(const Entity& entity) {return at(entity.loc);}

const int Grid::at(const Location& loc) const {return grid[loc.x][loc.y];}
const int Grid::at(int x, int y) const {return grid[x][y];}
const int Grid::at(const Entity &entity) const { return at(entity.loc); }

int& Grid::operator[](const Location &loc) {return grid[loc.x][loc.y];}
const int Grid::operator[](const Location &loc) const {return grid[loc.x][loc.y];}