#pragma once

#include "grid.h"
#include "player.h"
#include "input.h"
#include "log.h"

#include <string>
#include <iostream>

struct Game
{
    int my_id, num_players;
    int width, height;
    int total_halite;
    
    Grid grid;
    Player players[4];

    Game() {};
    void init();
    void update();
    void ready(const std::string& name);
};
