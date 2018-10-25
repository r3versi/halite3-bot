#pragma once

#include <basetypes.h>
#include <constants.h>

#include <chrono>
class Game
{
  public:
    size_t my_id, num_players, map_width, map_height, turn;

    Player* me;

    Map grid;
    Player players[4];
    Ship ships[4 * MAX_SHIPS];
    Dropoff dropoffs[4 * MAX_DROPOFFS];

    Map cache_grid;
    Player cache_players[4];
    Ship cache_ships[4 * MAX_SHIPS];
    Dropoff cache_dropoffs[4 * MAX_DROPOFFS];
    Game();

    void init_input();
    void turn_update();

    void load();
    void save();

    void dump(bool dump_map = true);

    void set_ships_dead();
};
