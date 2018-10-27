#pragma once

#include <basetypes.h>
#include <game.h>

class Engine
{
public:
    Game* game;

    Engine(Game* game) : game(game) {}


    void play_turn();
    void inspire();
    void commit_moves();
    void mine();
    void collisions();
    void drop_halite();
    
    bool can_move(Ship* ship);
    bool is_full(Ship* ship);

    void update_inspiration(Ship* ship);
    void try_move(Ship *ship);
    void mine(Ship *ship);
};
