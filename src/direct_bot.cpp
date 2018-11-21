#include <direct_bot.h>

#include <iostream>
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()
#define TURNTIME std::chrono::duration_cast<std::chrono::microseconds>(NOW - START).count() / 1000.

void HeurBot::search()
{
    ship_on_tile.reset();
    targeted.reset();

    assign_tasks();
    std::cerr << "@" << TURNTIME << " ms \t"
              << "Tasks assigned" << std::endl;
    navigate();
    std::cerr << "@" << TURNTIME << " ms \t"
              << "Navigation completed" << std::endl;

    make_dropoff();
    spawn_ship();
    std::cerr << "@" << TURNTIME << " ms \t"
              << "Dropoffs and spawns decided" << std::endl;
}

void HeurBot::make_dropoff()
{
    if (game->me->dropoffs.size() < max_dropoffs() &&
        game->max_turn - game->turn > 150)
    {
        Ship *best = nullptr;
        int max_dist = 0;
        for (auto &ship : game->me->ships)
        {
            if (ship->halite + game->me->halite + game->grid[ship->pos] >= 4000 &&
                game->dist_to_dropoff[ship->pos] > max_dist)
            {
                max_dist = game->dist_to_dropoff[ship->pos];
                best = ship;
            }
        }

        if (best != nullptr && max_dist >= 25)
        {
            game->me->halite -= (4000 - best->halite - game->grid[best->pos]);
            best->action = 5;
        }
    }
}
void HeurBot::spawn_ship()
{
    /*
    if (me->halite >= 1000 && ship_on_tile[game->me->spawn] == nullptr && game->turn <= game->max_turn/2)
        me->action = true;
    else
        me->action = false;
    */
    if (mode == MODE_2P)
    {
        Player* opponent = &game->players[(me->id + 1) % 2];
        if (me->halite >= 1000 && 
            game->max_turn - game->turn > 100 && 
            ship_on_tile[game->me->spawn] == nullptr && 
            me->ships.size() - opponent->ships.size() < 10)
        {
            me->action = true;
        }
        
        else
        {
            me->action = false;
        }
        
    }
    else
    {
        if (game->me->halite >= 1000 &&
            game->me->ships.size() < max_ships() &&
            ship_on_tile[game->me->spawn] == nullptr &&
            game->max_turn - game->turn > 100)
        {
            game->me->action = true;
        }
        else
            game->me->action = false;
    }
}

int HeurBot::max_dropoffs()
{
    int map_id = (game->map_width - 32) / 8;
    if (mode == MODE_2P)
        return MAX_DROPOFFS_2P[map_id];
    else
        return MAX_DROPOFFS_4P[map_id];
}

int HeurBot::max_ships()
{
    // <100 per tile = min, > 200 per tile = max
    // # ships = MIN + (MAX - MIN)* (min(200, max(100, tot/(size*size))) - 100)/100
    int map_id = (game->map_width - 32) / 8;
    unsigned int ships = 0;
    if (mode == MODE_2P)
    {
        ships = MIN_SHIPS_2P[map_id] + (MAX_SHIPS_2P[map_id] - MIN_SHIPS_2P[map_id]) * (std::min(UPPER_BOUND_2P[map_id], std::max(LOWER_BOUND_2P[map_id], (float)game->total_halite / (game->map_width * game->map_width))) - 100.f) / 100.f;
    }
    else
    {
        ships = MIN_SHIPS_4P[map_id] + (MAX_SHIPS_4P[map_id] - MIN_SHIPS_4P[map_id]) * (std::min(UPPER_BOUND_4P[map_id], std::max(LOWER_BOUND_4P[map_id], (float)game->total_halite / (game->map_width * game->map_width))) - 100.f) / 100.f;
    }
    return ships;
}

std::string HeurBot::get_commands()
{
    std::string commands = "";

    if (game->me->action)
        commands += "g ";

    for (auto &ship : game->me->ships)
        commands += ship->get_command();

    return commands;
}

void HeurBot::assign_tasks()
{
    int remaining_turns = game->max_turn - game->turn;
    endgame = remaining_turns < (int)game->map_width;

    for (auto &ship : game->me->ships)
    {
        if (!ship->active)
            continue;

        Point deliver_site = find_deliver_site(ship);

        if (ship->halite >= 900 || (ship->task == DELIVER && ship->halite > 0) || (endgame && game->grid.dist(ship->pos, deliver_site) >= remaining_turns - 5))
        {
            ship->task = DELIVER;
            ship->target = deliver_site;
        }
        else
        {
            ship->task = TRAVEL;
            ship->target = find_mining_site(ship, true);
        }

        //std::cerr << *ship << " {Task: " << ship->task << ", Target " << ship->target << "}" << std::endl;
    }

    std::cerr << "RECOMPUTE TASKS" << std::endl;
    for (auto &ship : game->me->ships)
    {
        if (!ship->active || ship->task != TRAVEL)
            continue;

        ship->target = find_mining_site(ship, false);
        //std::cerr << *ship << " {Task: " << ship->task << ", Target " << ship->target << "}" << std::endl;
    }
}

Point HeurBot::find_deliver_site(Ship *ship)
{
    if (!ship->active)
        std::cerr << "Ship not active!" << std::endl;

    return game->nearest_dropoff[ship->pos]->pos;
}

Point HeurBot::find_mining_site(Ship *ship, bool first)
{
    float best_score = 0.;
    Point best = ship->pos;
    //std::cerr << *ship << std::endl;
    int radius = std::min(game->map_width / 2, (game->max_turn - game->turn)/2);

    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            Point p = ship->pos + Point(dx, dy);
            game->grid.normalize(p);

            if (!first && targeted[p] != nullptr && targeted[p] != ship)
                continue;

            int mining_turns = std::max(0, 2 * radius - game->grid.dist(ship->pos, p));
            float score = static_cast<float>(game->grid[p]) / 4.f * (1.f - std::pow(.75f, mining_turns)) / .25f * (1.f + 2.f * game->inspired[ship->owner][p]);

            score = std::min(MAX_CARGO - ship->halite, static_cast<int>(score));

            score /= 1. + game->grid.dist(ship->pos, p);
            if (score > best_score)
            {
                best_score = score;
                best = p;
            }
            //std::cerr << p << " " << (int)score << "\t";
        }
        //std::cerr << std::endl;
    }

    if (first)
    {
        Ship *other = targeted[best];
        if (other != nullptr)
        {
            if (game->grid.dist(ship->pos, best) < game->grid.dist(other->pos, best))
                targeted[best] = ship;
        }
        else
            targeted[best] = ship;
    }

    return best;
}

Point HeurBot::find_mining_sector(Ship *ship)
{
    /*
    std::sort(std::begin(game->sectors), std::end(game->sectors),
              [this, ship](Sector &a, Sector &b) {
                  return a.halite / (game->dist_to_dropoff[a.centroid]) > b.halite / (game->dist_to_dropoff[b.centroid]);
              });
    */
    int best_score = 0;
    Point best = ship->pos;
    for (auto &sector : game->sectors)
    {
        int score = sector.halite / (game->dist_to_dropoff[sector.centroid] + game->grid.dist(sector.centroid, ship->pos));
        if (score > best_score)
        {
            best_score = score;
            best = sector.centroid;
        }
    }
    return best;
}

void HeurBot::navigate()
{
    std::sort(game->me->ships.begin(), game->me->ships.end(),
              [](Ship *a, Ship *b) {
                  if (a->task != b->task)
                      return a->task < b->task;

                  return a->halite > b->halite;
              });

    for (auto &ship : game->me->ships)
    {
        if (!ship->active)
            continue;

        if (!engine->can_move(ship))
        {
            ship_on_tile[ship->pos] = ship;
            ship->action = 0;
            ship->just_moved = true;
            continue;
        }

        ship->just_moved = false;
    }

    for (auto &ship : game->me->ships)
    {
        if (ship->just_moved || !ship->active)
            continue;

        if (move_ship(ship))
        {
            if (endgame)
            {
                for (auto &dropoff : game->me->dropoffs)
                    ship_on_tile[dropoff->pos] = nullptr;
            }
            //std::cerr << "SUCCESS " << ship->id << " to " << moves_str[ship->action] << std::endl;
        }
        else
            std::cerr << "FAIL " << ship->id << std::endl;
    }
}

int HeurBot::enemy_count(Point& n)
{
    int count = 0;
    for(int i = 0; i < game->num_players; i++)
    {
        if (i == game->my_id)
            continue;
        
        count += game->ships_around[i][n];
    }
    return count;    
}

bool HeurBot::move_ship_dir(Ship *ship, int dir)
{
    Point n = ship->pos + moves_dir[dir];
    game->grid.normalize(n);

    // my ship plans to move there next turn
    Ship *other_ship = ship_on_tile[n];
    if (other_ship != nullptr)
        return false;

    if (ship->task == DELIVER && game->dist_to_dropoff[ship->pos] > 2)
    {
        if (!game->unsafe[ship->owner][n])
        {
            ship_on_tile[n] = ship;
            ship->just_moved = true;
            ship->action = dir;
            return true;
        }
    }
    else if (mode == MODE_4P)
    {
        // any ship currently on this spot
        if (!game->unsafe[ship->owner][n] ||
            (game->unsafe[ship->owner][n] && game->ships_around[me->id][n] > enemy_count(n)))
        {
            ship_on_tile[n] = ship;
            ship->just_moved = true;
            ship->action = dir;
            return true;
        }
    }
    else
    {
        Ship *anyone = game->ships_grid[n];
        if (anyone == nullptr ||
            (anyone != nullptr && (anyone->owner == me->id || game->ships_around[me->id][n] > enemy_count(n))))
        {
            ship_on_tile[n] = ship;
            ship->just_moved = true;
            ship->action = dir;
            return true;
        }
    }

    return false;
}

// returns true if found successful move (i.e. no collision)
bool HeurBot::move_ship(Ship *ship, Ship *forcing)
{
    if (TURNTIME >= 1900.)
        return true;

    if (ship->target == ship->pos)
    {
        if (move_ship_dir(ship, 0))
            return true;
    }

    if (engine->can_move(ship))
    {
        //std::cerr<< *ship << " wants to go to " << ship->target << " from " << ship->pos << std::endl;
        int dirs[5] = {0, 1, 2, 3, 4};

        std::sort(std::begin(dirs), std::end(dirs), [this, ship](int &a, int &b) {
            Point na = ship->pos + moves_dir[a], nb = ship->pos + moves_dir[b];
            game->grid.normalize(na);
            game->grid.normalize(nb);
            int da = game->grid.dist(ship->target, na);
            int db = game->grid.dist(ship->target, nb);

            // priority: 1- distance, 2- cost
            if (da != db)
                return da < db;
            else
                return game->grid[na] < game->grid[nb];
        });

        for (auto i : dirs)
        {
            if (move_ship_dir(ship, i))
                return true;
        }

        for (auto dir : dirs)
        {
            Point n = ship->pos + moves_dir[dir];
            game->grid.normalize(n);

            Ship *other_ship = ship_on_tile[n];
            if (other_ship == nullptr)
            {
                ship_on_tile[n] = ship;
                ship->just_moved = true;
                ship->action = dir;
                return true;
            }
            else
            {
                if (other_ship != forcing)
                {
                    ship_on_tile[n] = ship;
                    ship->just_moved = true;
                    ship->action = dir;
                    other_ship->just_moved = false;

                    if (move_ship(other_ship, ship))
                    {
                        return true;
                    }

                    // if can't force move, reset previous situation!
                    ship_on_tile[n] = other_ship;
                    other_ship->just_moved = true;
                    ship->just_moved = false;
                }
            }
        }
    }
    else
    {
        // can stay still?
        if (move_ship_dir(ship, 0))
            return true;

        else
        {
            std::cerr << "FAIL - unmovable - " << *ship << std::endl;

            Ship *other_ship = ship_on_tile[ship->pos];

            ship_on_tile[ship->pos] = ship;
            ship->just_moved = true;
            ship->action = 0;
            other_ship->just_moved = false;

            if (forcing != other_ship && move_ship(other_ship, ship))
            {
                return true;
            }
        }
    }

    return false;
}