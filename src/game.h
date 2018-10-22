#pragma once

#include <basetypes.h>
#include <string.h> // memcpy
#include <string> // string
#include <iostream> // std::cin, cout, cerr, endl

class Game
{
  public:
    size_t my_id, num_players, map_width, map_height, turn;

    Map grid;
    Player players[4];
    Ship ships[4 * MAX_SHIPS];
    Dropoff dropoffs[4 * MAX_DROPOFFS];

    Map cache_grid;
    Player cache_players[4];
    Ship cache_ships[4 * MAX_SHIPS];
    Dropoff cache_dropoffs[4 * MAX_DROPOFFS];

    Game() {}

    void load()
    {
        grid = cache_grid;
        memcpy(players, cache_players, sizeof players);
        memcpy(ships, cache_ships, sizeof ships);
        memcpy(dropoffs, cache_dropoffs, sizeof dropoffs);
    }

    void save()
    {
        cache_grid = grid;
        memcpy(cache_players, players, sizeof players);
        memcpy(cache_ships, ships, sizeof ships);
        memcpy(cache_dropoffs, dropoffs, sizeof dropoffs);
    }

    void init_input()
    {
        string line;
        std::cin >> line;

        std::cin >> my_id >> num_players;

        for (size_t i = 0; i < num_players; i++)
        {
            int id, x, y;
            std::cin >> id >> x >> y;
            players[id] = Player(id, x, y);
            dropoffs[id] = Dropoff(id, id, x, y);
        }

        std::cin >> map_width >> map_height;
        grid = Map(map_width, map_height);

        int halite;
        for (size_t y = 0; y < map_height; y++)
        {
            for (size_t x = 0; x < map_width; x++)
            {
                std::cin >> halite;
                grid.at(x, y) = halite;
            }
        }
    }

    inline void set_ships_dead()
    {
        for (auto &ship : ships)
            ship.active = false;
    }

    void turn_update()
    {
        // ./halite feeds 1 based turn, i like 0 based
        std::cin >> turn;
        --turn;

        set_ships_dead();

        for (size_t i = 0; i < num_players; i++)
        {
            int id, n_ships, n_dropoffs, halite;
            std::cin >> id >> n_ships >> n_dropoffs >> halite;

            players[id].update(n_ships, n_dropoffs, halite);

            for (size_t j = 0; j < n_ships; j++)
            {
                int ship_id, x, y, cargo;
                std::cin >> ship_id, x, y, cargo;

                ships[ship_id].update(x, y, cargo);
                players[id].ships[j] = ship_id;
            }

            for (size_t j = 0; j < n_dropoffs; j++)
            {
                int dropoff_id, x, y;
                std::cin >> dropoff_id >> x >> y;

                dropoffs[dropoff_id + 1].update(x, y);
                players[id].dropoffs[j] = dropoff_id;
            }
        }

        int tiles;
        std::cin >> tiles;

        for (size_t i = 0; i < tiles; i++)
        {
            int x, y, halite;
            std::cin >> x >> y >> halite;
            grid.at(x, y) = halite;
        }
    }
};
