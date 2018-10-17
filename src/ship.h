#pragma once

#include "location.h"

struct Ship
{
    int id;
    int owner;
    int halite;
    Location loc;

    Ship(int id, int owner, int halite, int x, int y) : id(id), owner(owner), halite(halite), loc(x, y) {}  
};