#pragma once

#include "location.h"

struct Entity
{
    int owner;
    int id;
    Location loc;

    Entity(int id, int owner, Location loc) : id(id), owner(owner), loc(loc) {}
    Entity(int id, int owner, int x, int y) : owner(owner), id(id), loc(x, y) {}
};

struct Ship : public Entity
{
    int halite;
    Ship(int id, int owner, int halite, int x, int y) : Entity(id, owner, x, y), halite(halite) {}
};

struct Dropoff : public Entity
{
    Dropoff(int id, int owner, int x, int y) : Entity(id, owner, x, y) {}
};