#pragma once

#include "player.h"
#include "input.h"

struct Game
{
    Game()
    {
        // Ignore first line containing strings
        input::get_string();

        int my_id, num_players;
        input::get_sstream() >> my_id >> num_players;
    }

};
