#pragma once

#include <basetypes.h>
#include <constants.h>

#include <chrono>
class Game
{
  public:
    size_t my_id, num_players, map_width, map_height, turn;
    size_t max_turn;
    Player* me;

    Map grid;
    Player players[4];
    Ship ships[4 * MAX_SHIPS];
    Dropoff dropoffs[4 * MAX_DROPOFFS];

    Map cache_grid;
    Player cache_players[4];
    Ship cache_ships[4 * MAX_SHIPS];
    Dropoff cache_dropoffs[4 * MAX_DROPOFFS];

    // Statistics
    Grid<float> halite_nbhood, turns_to_collect;
    Grid<int> dist_to_dropoff;
    Grid<Dropoff*> nearest_dropoff;

    int total_halite;
    /* 
       8x8 sectors (side of a sector = 4, 5, 6, 7, 8, for map_width = 32, 40, 48, 56, 64)
       maybe try 4x4 sectors too (sides = 8, 10, 12, 14, 16).
       anyway, number of sectors *should* be related to the radius of scan performed by a ship at DirectSearch::find_mining_site
    */
    Sector sectors[NUM_SECTORS];

    Game();

    void init_input();
    void turn_update();

    void run_statistics();
    void update_sectors();

    void load();
    void save();

    void dump(bool dump_map = true);

    void set_ships_dead();
};
