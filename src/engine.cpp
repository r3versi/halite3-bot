#include <engine.h>

#include <cmath>
#include <iostream>

void Engine::play_turn()
{
    //inspire();
    commit_moves();
    mine();
    collisions();
    drop_halite();
}

void Engine::inspire()
{
    for (size_t i = 0; i < game->num_players; i++)
    {
        for (auto &ship : game->players[i].ships)
        {
            if (!ship->active)
                continue;

            update_inspiration(ship);
        }
    }
}

void Engine::commit_moves()
{
    for (size_t i = 0; i < game->num_players; i++)
    {
        if (game->players[i].action)
        {
            // spawn
        }

        for (auto &ship : game->players[i].ships)
        {
            if (ship->action != -1 && ship->active)
                Engine::try_move(ship);
        }
    }
}

void Engine::mine()
{
    for (size_t i = 0; i < game->num_players; i++)
    {
        for (auto &ship : game->players[i].ships)
        {
            if (ship->just_moved || !ship->active)
                continue;

            mine(ship);
        }
    }
}

void Engine::collisions()
{
    Mask taken = Mask(game->map_width, game->map_height);
    Mask dead = Mask(game->map_width, game->map_height);

    for (size_t i = 0; i < game->num_players; i++)
    {
        for (auto &ship : game->players[i].ships)
        {
            if (!ship->active)
                continue;

            if (taken[ship->pos])
                dead[ship->pos] = true;
            taken[ship->pos] = true;
        }
    }

    // set not active colliding ships and drop halite
    for (size_t i = 0; i < game->num_players; i++)
    {
        for (auto &ship : game->players[i].ships)
        {
            if (!ship->active)
                continue;

            if (dead[ship->pos])
            {
                ship->active = false;
                game->grid[ship->pos] += ship->halite;
                ship->halite = 0;
            }
        }
    }

    // give halite dropped on dropoffs to players
    for (size_t i = 0; i < game->num_players; i++)
    {
        for (auto &dropoff : game->players[i].dropoffs)
        {
            if (game->grid[dropoff->pos])
            {
                game->players[i].halite += game->grid[dropoff->pos];
                game->grid[dropoff->pos] = 0;
            }
        }
    }
}

void Engine::drop_halite()
{
    for(size_t i = 0; i < game->num_players; i++)
    {
        for(auto& ship : game->players[i].ships)
        {
            if (ship->halite == 0 || !ship->action)
                continue;

            for(auto& dropoff : game->players[i].dropoffs)
            {
                if (ship->pos == dropoff->pos)
                {
                    game->players[i].halite += ship->halite;
                    ship->halite = 0;
                    break;
                }
            }   
        }
    }
    
}

void Engine::try_move(Ship *ship)
{
    if (ship->action > 4)
    {
        std::cerr << "Action not supported yet" << std::endl;
        return;
    }

    // stay still
    if (ship->action == 0)
        return;

    int cost = game->grid[ship->pos] / MOVE_COST_RATIO;

    if (ship->halite >= cost)
    {
        ship->just_moved = true;

        ship->halite -= cost;

        ship->pos = ship->pos + moves_dir[ship->action];
        game->grid.normalize(ship->pos);
    }
}

bool Engine::can_move(Ship *ship)
{
    return ship->halite >= game->grid[ship->pos] / MOVE_COST_RATIO;
}

bool Engine::is_full(Ship *ship)
{
    return ship->halite >= FULL_SHIP;
}

// assume ship didnt move
void Engine::mine(Ship *ship)
{
    int extracted = std::ceil(game->grid[ship->pos] / (float)EXTRACTION_RATIO);
    int gained = extracted;

    if (extracted == 0 && game->grid[ship->pos])
        extracted = gained = game->grid[ship->pos];

    if (extracted + ship->halite > MAX_CARGO)
        extracted = MAX_CARGO - ship->halite;

    if (ship->inspired)
        gained += gained * INSPIRATION_BONUS;

    if (gained + ship->halite > MAX_CARGO)
        gained = MAX_CARGO - ship->halite;

    ship->halite += gained;
    game->grid[ship->pos] -= extracted;
}

void Engine::update_inspiration(Ship *ship)
{
    int count = 0;
    for (size_t i = 0; i < game->num_players; i++)
    {
        if (i == (unsigned)ship->owner)
            continue;

        for (auto &enemy_ship : game->players[i].ships)
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
