#include <game.h>

#include <string.h> // memcpy
#include <string>   // string
#include <iostream> // std::cin, cout, cerr, endl

#define NOW std::chrono::high_resolution_clock::now()
#define TURNTIME std::chrono::duration_cast<std::chrono::microseconds>(NOW - start).count() / 1000.

Game::Game() {}

void Game::init_input()
{
    std::string line;
    std::cin >> line;

    std::cin >> num_players >> my_id;
    me = players + my_id;

    for (size_t i = 0; i < num_players; i++)
    {
        int id, x, y;
        std::cin >> id >> x >> y;
        dropoffs[id] = Dropoff(id, id, x, y);
        players[id] = Player(id, x, y, dropoffs + id);
    }

    std::cin >> map_width >> map_height;
    grid = Map(map_width, map_height);
    cache_grid = Map(map_width, map_height);
    size_t halite;
    for (size_t y = 0; y < map_height; y++)
    {
        for (size_t x = 0; x < map_width; x++)
        {
            std::cin >> halite;
            grid.at(x, y) = halite;
            cache_grid.at(x, y) = halite;
        }
    }
}

void Game::turn_update(std::chrono::time_point<std::chrono::high_resolution_clock> start)
{
    //std::cerr << "Init turn update " << TURNTIME << std::endl;
    // ./halite feeds 1 based turn, i like 0 based
    std::cin >> turn;

    //std::cerr << "Read turn " << TURNTIME << std::endl;
    set_ships_dead();
    //std::cerr << "ships set dead " << TURNTIME << std::endl;

    for (size_t i = 0; i < num_players; i++)
    {
        //std::cerr << "init player update " << TURNTIME << std::endl;

        size_t id, n_ships, n_dropoffs, halite;
        std::cin >> id >> n_ships >> n_dropoffs >> halite;
        //std::cerr << "read player " << TURNTIME << std::endl;

        players[id].update(halite);

        for (size_t j = 0; j < n_ships; j++)
        {
            int ship_id, x, y, cargo;
            std::cin >> ship_id >> x >> y >> cargo;

            ships[ship_id].update(ship_id, id, x, y, cargo);
            players[id].ships.put(ships + ship_id);
        }

        for (size_t j = 0; j < n_dropoffs; j++)
        {
            int dropoff_id, x, y;
            std::cin >> dropoff_id >> x >> y;

            dropoffs[dropoff_id + 1].update(x, y);
            players[id].dropoffs[j] = dropoffs + dropoff_id;
        }
        //std::cerr << "updated ships & dropoffs " << TURNTIME << std::endl;
    }

    size_t tiles;
    std::cin >> tiles;

    for (size_t i = 0; i < tiles; i++)
    {
        int x, y, halite;
        std::cin >> x >> y >> halite;
        grid.at(x, y) = halite;
    }
    //std::cerr << "map updated, exiting " << TURNTIME << std::endl;
}

void Game::load()
{
    memcpy(&grid.grid[0][0], &cache_grid.grid[0][0], sizeof(cache_grid.grid[0][0]) * cache_grid.width * cache_grid.height);
    //memcpy(grid.grid,   cache_grid.grid,    sizeof(int) * grid.width * grid.height);
    memcpy(players, cache_players, sizeof players);
    memcpy(ships, cache_ships, sizeof ships);
    memcpy(dropoffs, cache_dropoffs, sizeof dropoffs);
}

void Game::save()
{
    memcpy(&cache_grid.grid[0][0], &grid.grid[0][0], sizeof(grid.grid[0][0]) * grid.width * grid.height);
    //memcpy(cache_grid.grid,     grid.grid,  sizeof(int) * grid.width * grid.height);
    memcpy(cache_players, players, sizeof players);
    memcpy(cache_ships, ships, sizeof ships);
    memcpy(cache_dropoffs, dropoffs, sizeof dropoffs);
}

void Game::dump(bool dump_map)
{
    if (dump_map)
    {
        std::cerr << my_id << " " << num_players << " " << map_width << " " << map_height << " " << turn << std::endl;
        std::cerr << "me: " << me << std::endl;
        std::cerr << grid << std::endl;
    }

    for (size_t i = 0; i < num_players; i++)
    {
        std::cerr << players[i] << std::endl;

        for (auto &ship : players[i].ships)
        {
            std::cerr << *ship << std::endl;
        }

        for (auto &dropoff : players[i].dropoffs)
        {
            std::cerr << *dropoff << std::endl;
        }
    }
}

void Game::set_ships_dead()
{
    for(size_t i = 0; i < num_players; i++)
    {
        for (auto &ship : players[i].ships)
            ship->active = false;
    }
}