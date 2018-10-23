#pragma once

#include <container.h>
#include <game.h>

#include <cmath>

class Engine
{
public:
    Game* game;

    Engine(Game* game) : game(game) {}

    void play(ShipCluster& ships, Solution& sol);

    void inspire();
    void apply(ShipCluster& ships, ShipClusterMoves& ship_moves);
    void mine();
    void collisions();
    
    
    inline bool can_move(Ship* ship);
    inline bool is_full(Ship* ship);

    inline void mine(Ship *ship);
    inline void update_inspiration(Ship* ship);

    inline void try_move(Ship *ship, Point &dir);
};

void Engine::play(ShipCluster &ships, Solution &sol)
{
    game->save();
    // sol.size = #turns
    for(size_t i = 0; i < sol.size; i++)
    {
        apply(ships, sol[i]);
        mine();
        collisions();
        inspire();
    }
}

inline bool Engine::can_move(Ship* ship)
{
    return ship->halite >= game->grid[ship->pos] / MOVE_COST_RATIO;
}

inline bool Engine::is_full(Ship* ship)
{
    return ship->halite >= FULL_SHIP;
}

// assume ship didnt move
inline void Engine::mine(Ship* ship)
{
    int extracted = std::ceil(game->grid[ship->pos].halite / (float)EXTRACTION_RATIO);
    int gained = extracted;

    if (extracted == 0 && game->grid[ship->pos].halite)
        extracted = gained = game->grid[ship->pos].halite;
    
    if (extracted + ship->halite > MAX_CARGO)
        extracted = MAX_CARGO - ship->halite;

    if (ship->inspired)
        gained += gained * INSPIRATION_BONUS;
    
    if (gained + ship->halite > MAX_CARGO)
        gained = MAX_CARGO - ship->halite;

    ship->halite += gained;
    game->grid[ship->pos].halite -= extracted;
}

inline void Engine::update_inspiration(Ship* ship)
{
    int count = 0;
    for(size_t i = 0; i < game->num_players; i++)
    {
        if (i == ship->owner)
            continue;

        for(auto& enemy_ship : game->players[i].ships)
        {
            if (game->grid.dist(ship->pos, enemy_ship->pos) <= INSPIRATION_RADIUS)
                ++count;
            if (count >= 2)
                break;
        }

        if (count >= 2)
            break; 
    }

    ship->inspired = (count >= 2);
}

void Engine::apply(ShipCluster& ships, ShipClusterMoves& ship_moves)
{
    for(size_t i = 0; i < ship_moves.size; i++)
    {
        if (ships[i])
        {

        }
    }
    
}