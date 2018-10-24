#pragma once

#include <basetypes.h>
#include <game.h>
#include <actions.h>

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
    
    inline bool can_move(Ship* ship);
    inline bool is_full(Ship* ship);

    inline void update_inspiration(Ship* ship);
    inline void try_move(Ship *ship);
    inline void mine(Ship *ship);
};
