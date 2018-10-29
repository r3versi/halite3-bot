#include <game.h>

#include <string.h> // memcpy
#include <string>   // string
#include <iostream> // std::cin, cout, cerr, endl
#include <cmath>

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

    max_turn = 400 + static_cast<size_t>(100.f / 32.f * (static_cast<float>(map_width) - 32.f));

    grid = Map(map_width, map_height);
    cache_grid = Map(map_width, map_height);

    halite_nbhood = Grid<float>(map_width, map_height);
    turns_to_collect = Grid<float>(map_width, map_height);
    dist_to_dropoff = Grid<int>(map_width, map_height);
    nearest_dropoff = Grid<Dropoff*>(map_width, map_height);
    total_halite = 0;

    size_t halite;
    for (size_t y = 0; y < map_height; y++)
    {
        for (size_t x = 0; x < map_width; x++)
        {
            std::cin >> halite;
            grid.at(x, y) = halite;
            cache_grid.at(x, y) = halite;
            total_halite += halite;
        }
    }
}

void Game::turn_update()
{
    // ./halite feeds 1 based turn, i like 0 based
    std::cin >> turn;

    set_ships_dead();
    
    for (size_t i = 0; i < num_players; i++)
    {
    
        size_t id, n_ships, n_dropoffs, halite;
        std::cin >> id >> n_ships >> n_dropoffs >> halite;
    
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

            dropoffs[dropoff_id + num_players].update(x, y);
            players[id].dropoffs.put(dropoffs + dropoff_id + num_players);
        }
    }

    size_t tiles;
    std::cin >> tiles;

    for (size_t i = 0; i < tiles; i++)
    {
        int x, y, halite;
        std::cin >> x >> y >> halite;
        
        total_halite -= (grid.at(x,y) - halite);
        grid.at(x, y) = halite;
    }

    run_statistics();
}

void Game::run_statistics()
{    
    for(size_t x = 0; x < map_width; x++)
    {
        for(size_t y = 0; y < map_height; y++)
        {   
            Point p = Point(x,y);
            // Neighbourhood
            halite_nbhood[p] = 0.;

            for(int dx = -2; dx <= 2; dx++)
            {
                for(int dy = -2; dy <= 2; dy++)
                {
                    Point trgt = p + Point(dx,dy);
                    grid.normalize(trgt);
                    halite_nbhood[p] += grid[trgt];
                }   
            }

            int min_dist = map_width*2;
            Dropoff* best = nullptr;
            for(auto& dropoff : me->dropoffs)
            {
                int dist = grid.dist(p, dropoff->pos);
                if (dist <= min_dist)
                {
                    best = dropoff;
                    min_dist = dist;
                }
            }
            
            nearest_dropoff[p] = best;
            dist_to_dropoff[p] = min_dist;
            
            // And now i really don't remember how i computed this. i had to add a comment, my bad. to be reviewed soon.
            turns_to_collect[p] = static_cast<int>(.5f + log(200.f / std::max(200, grid[p])) / log(.75f));
        }
    }

    update_sectors();
}

void Game::update_sectors()
{
    int side = map_width / SECTOR_ROW;
    for(size_t i = 0; i < NUM_SECTORS; i++)
    {
        int x0 = (i%side)*side, y0 = (i/side)*side;
        int sum_x = 0, sum_y = 0, sum_halite = 0;
        for(int x = x0; x < x0 + side; x++)
        {
            for(int y = y0; y < y0 + side; y++)
            {
                int m = grid.at(x, y);
                sum_halite += m;
                sum_x = x*m;
                sum_y = y*m;
            }
        }
        
        sectors[i].centroid = Point(sum_x/sum_halite, sum_y/sum_halite);
        sectors[i].halite = sum_halite;
    }
    
}

void Game::load()
{
    grid = cache_grid;
    memcpy(players, cache_players, sizeof players);
    memcpy(ships, cache_ships, sizeof ships);
    memcpy(dropoffs, cache_dropoffs, sizeof dropoffs);
}

void Game::save()
{
    cache_grid = grid;
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