#pragma once

#include "entity.h"
#include "location.h"

struct Player
{
    int id;
    int halite;    

    Location spawn;
    Ship ships[];

    Player() {}
    Player(int id, int x, int y) : id(id), spawn(x,y) {}
};
