#pragma once

#include "location.h"
#include "entity.h"

#include <memory>

struct Grid 
{
    int** grid;
    int width, height;

    Grid();
    Grid(int width, int height);

    int& at(const Location& loc);
    int& at(int x, int y);
    int& at(const Entity& entity);

    const int at(const Location& loc) const;
    const int at(int x, int y) const;
    const int at(const Entity& entity) const;
    
    int& operator[](const Location& loc);
    const int operator[](const Location &loc) const;
};