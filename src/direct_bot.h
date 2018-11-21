#pragma once

#include <constants.h>
#include <engine.h>


class HeurBot
{
  public:
    bool mode;
    bool endgame;
    Engine *engine;
    Game* game;
    Player* me;

    Grid<Ship *> ship_on_tile; // ship on tile next turn
    Grid<Ship *> targeted;     // mining site targeted by

    HeurBot(Engine *engine) : engine(engine)
    {
        game = engine->game;
        me = game->me;

        ship_on_tile = Grid<Ship *>(game->map_width, game->map_height);
        targeted = Grid<Ship *>(game->map_width, game->map_height);
        endgame = false;
        mode = game->num_players == 2 ? MODE_2P : MODE_4P;
    }

    void search();
    std::string get_commands();

    int max_ships();
    int max_dropoffs();
    int enemy_count(Point& n);
    
    void assign_tasks();
    void navigate();
    
    void make_dropoff();
    void spawn_ship();

    Point find_deliver_site(Ship *ship);
    Point find_mining_site(Ship *ship, bool first = true);
    Point find_mining_sector(Ship *ship);

    bool move_ship(Ship *ship, Ship *forcing = nullptr);
    bool move_ship_dir(Ship *ship, int dir);
};
