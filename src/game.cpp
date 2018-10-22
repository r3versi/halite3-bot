#include "game.h"

#include <iostream>
#include <memory>


void Game::init()
{
    // Ignore first line containing strings
    input::get_string();

    input::get_sstream() >> my_id >> num_players;

    // Shipyards
    for (size_t i = 0; i < num_players; i++)
    {
        int id, x, y;
        input::get_sstream() >> id >> x >> y;
    }

    input::get_sstream() >> width >> height;

    grid = Grid(width, height);
    total_halite = 0;

    std::stringstream line;
    int value;
    for (size_t y = 0; y < height; y++)
    {
        line = input::get_sstream();
        for (size_t x = 0; x < width; x++)
        {
            line >> value;
            grid.at(x, y) = value;
            total_halite += value;
        }
    }
}
void Game::update() 
{
    int turn; // 1 based
    input::get_sstream() >> turn;

    for(size_t i = 0; i < num_players; i++)
    {
        int id, n_ships, n_dropoffs, halite;
        input::get_sstream() >> id >> n_ships >> n_dropoffs >> halite;

        for(size_t j = 0; j < n_ships; j++)
        {
            int ship_id, x, y, cargo;
            input::get_sstream() >> ship_id >> x >> y >> cargo;
        }

        for (size_t j = 0; j < n_dropoffs; j++)
        {
            int dropoff_id, x, y;
            input::get_sstream() >> dropoff_id >> x >> y;
        }
    }

    int tiles_updated;
    input::get_sstream() >> tiles_updated;

    for(size_t i = 0; i < tiles_updated; i++)
    {
        int x, y, halite;
        input::get_sstream() >> x >> y >> halite;
        grid.at(x,y) = halite;
    }    
}

void Game::ready(const std::string& name)
{
    std::cout << name << std::endl;
}