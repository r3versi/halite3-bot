#include <search.h>

#include <iostream>
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()
#define TURNTIME std::chrono::duration_cast<std::chrono::microseconds>(NOW - start).count() / 1000.
Solution GASearch::search(float time_limit)
{
    auto start = NOW;
    if (engine->game->me->ships.size() == 0)
        return get_rnd();

    for (size_t i = 0; i < pop_size; i++)
    {
        pop[i] = get_rnd();
        pop[i]._score = evaluate(pop[i]);
    }

    sort();

    while (TURNTIME < time_limit)
    {

        Solution &dad = pop[rnd(pop_size)];
        Solution &mum = pop[rnd(pop_size)];

        Solution child0, child1;
        if (rnd(100) < 30)
            cross(mum, dad, child0, child1);

        if (rnd(100) < 15)
            mutate(child0);

        if (rnd(100) < 15)
            mutate(child1);

        child0._score = evaluate(child0);
        child1._score = evaluate(child1);

        int offset = 0;
        if (child0._score > pop[0]._score)
        {
            offset = 1;
            pop[0] = child0;
        }
        if (child1._score > pop[offset]._score)
            pop[offset] = child1;

        sort();
        pop[0] = get_rnd();
    }
    return pop[pop_size - 1];
}

void GASearch::mutate(Solution &sol)
{
    size_t ship_id = rnd(engine->game->me->ships.size());
    size_t turn = rnd(depth);

    sol[ship_id][turn] = rnd(5);
}

void GASearch::cross(Solution &mum, Solution &dad, Solution &child0, Solution &child1)
{
    // swap whole ship genome between mum and dad
    size_t ship_id = rnd(engine->game->me->ships.size());

    child0 = mum;
    child1 = dad;

    child0[ship_id] = dad[ship_id];
    child1[ship_id] = mum[ship_id];
}

void GASearch::sort()
{
    std::sort(pop, pop + pop_size, [](Solution &a, Solution &b) { return a._score < b._score; });
}

Solution RandomSearch::search(float time_limit)
{
    size_t count = 0;
    auto start = NOW;
    engine->game->save();

    std::cerr << "Starting search" << std::endl;
    
    Solution best = get_rnd();
    float best_score = evaluate(best);
    
    while (TURNTIME < time_limit)
    {
        ++count;
        Solution sol = get_rnd();
        float score = evaluate(sol);

        if (score > best_score)
        {
            //std::cerr << "New best " << score << std::endl;
            best = sol;
            best_score = score;
        }
    }
    std::cerr << count << std::endl;
    return best;
}

Solution Search::get_rnd()
{
    Solution sol = Solution();

    for (size_t i = 0; i < engine->game->me->ships.size(); i++)
    {
        ShipSolution ship_sol = ShipSolution();

        for (size_t j = 0; j < depth; j++)
        {
            ship_sol.put(rnd(5));
        }
        sol.put(ship_sol);
    }

    return sol;
}

void Search::apply_actions(Solution &sol, int turn)
{
    for (size_t i = 0; i < sol.size(); i++)
    {
        engine->game->me->ships[i]->action = sol[i][turn];
    }
}

float Search::evaluate(Solution &sol)
{
    Game *game = engine->game;
    
    for (size_t i = 0; i < depth; i++)
    {
        apply_actions(sol, i);
        engine->play_turn();
    }

    float score = game->me->halite;

    for (auto &ship : game->me->ships)
    {
        if (!ship->active)
        {
            score += -1000;
            continue;
        }

        score += .75 * std::min(ship->halite, 930);

        if (ship->halite > 500)
        {
            score +=  - 10 * game->grid.dist(ship->pos, game->me->spawn);
        }
        
        else
        {
            score += 5 * game->grid.dist(ship->pos, game->me->spawn);
        }
        
    }

    game->load();
    return score;
}


Solution DirectSearch::search(float time_limit)
{
    std::cerr << "Time limit " << time_limit << std::endl;
    ship_on_tile.reset();
    targeted.reset();

    assign_tasks();
    navigate();

    if (engine->game->me->dropoffs.size() < max_dropoffs() &&
        engine->game->max_turn - engine->game->turn > 100)
    {
        Ship* best = nullptr;
        int max_dist = 0;
        for(auto& ship : engine->game->me->ships)
        {
            if (ship->halite + engine->game->me->halite + engine->game->grid[ship->pos] >= 4000 &&
                engine->game->dist_to_dropoff[ship->pos] > max_dist)
            {
                max_dist = engine->game->dist_to_dropoff[ship->pos];
                best = ship;
            }
        }
        
        if (best != nullptr && max_dist >= 25)
        {
            engine->game->me->halite -= (4000 - best->halite - engine->game->grid[best->pos]) ;
            best->action = 5;
        }
    }

    if (engine->game->me->halite >= 1000 && 
        engine->game->me->ships.size() < max_ships() &&
        ship_on_tile[engine->game->me->spawn] == nullptr &&
        engine->game->max_turn - engine->game->turn > 100
        )
    {
        engine->game->me->action = true;
    }
    else
        engine->game->me->action = false;
    return Solution();
}

unsigned int DirectSearch::max_dropoffs()
{
    int map_id = (engine->game->map_width - 32) / 8;
    if (mode == MODE_2P)
        return MAX_DROPOFFS_2P[map_id];
    else
        return MAX_DROPOFFS_4P[map_id];
}

unsigned int DirectSearch::max_ships()
{
    // <100 per tile = min, > 200 per tile = max
    // # ships = MIN + (MAX - MIN)* (min(200, max(100, tot/(size*size))) - 100)/100
    int map_id = (engine->game->map_width - 32) / 8;
    unsigned int ships = 0;
    if (mode == MODE_2P)
    {
        ships = MIN_SHIPS_2P[map_id] + (MAX_SHIPS_2P[map_id] - MIN_SHIPS_2P[map_id]) * (std::min(UPPER_BOUND_2P[map_id], std::max(LOWER_BOUND_2P[map_id], (float)engine->game->total_halite / (engine->game->map_width * engine->game->map_width)))-100.f)/100.f;
    }
    else
    {
        ships = MIN_SHIPS_4P[map_id] + (MAX_SHIPS_4P[map_id] - MIN_SHIPS_4P[map_id]) * (std::min(UPPER_BOUND_4P[map_id], std::max(LOWER_BOUND_4P[map_id], (float)engine->game->total_halite / (engine->game->map_width * engine->game->map_width))) - 100.f) / 100.f;
    }
    return ships;
}

std::string DirectSearch::get_commands()
{
    std::string commands = "";
    
    if (engine->game->me->action)
        commands += "g ";

    for(auto& ship : engine->game->me->ships)
        commands += ship->get_command();

    return commands;
}

void DirectSearch::assign_tasks()
{
    int remaining_turns = engine->game->max_turn - engine->game->turn;
    endgame = remaining_turns < (int)engine->game->map_width;

    for (auto &ship : engine->game->me->ships)
    {
        if (!ship->active)
            continue;

        Point deliver_site = find_deliver_site(ship);

        if (ship->halite >= 900 || (ship->task == DELIVER && ship->halite > 0) || (endgame && engine->game->grid.dist(ship->pos, deliver_site) >= remaining_turns - 5))
        {
            ship->task = DELIVER;
            ship->target = deliver_site;
        }
        else
        {
            ship->task = GOTO;
            ship->target = find_mining_site(ship, true);
        }

        //std::cerr << *ship << " {Task: " << ship->task << ", Target " << ship->target << "}" << std::endl;
    }

    std::cerr << "RECOMPUTE TASKS" << std::endl;
    for (auto &ship : engine->game->me->ships)
    {
        if (!ship->active || ship->task != GOTO)
            continue;
        
        ship->target = find_mining_site(ship, false);
        //std::cerr << *ship << " {Task: " << ship->task << ", Target " << ship->target << "}" << std::endl;
    }
}

Point DirectSearch::find_deliver_site(Ship *ship)
{
    if (!ship->active)
        std::cerr << "Ship not active!" << std::endl;

    return engine->game->nearest_dropoff[ship->pos]->pos;
}

Point DirectSearch::find_mining_site(Ship *ship, bool first)
{
    float best_score = 0.;
    Point best = ship->pos;
    //std::cerr << *ship << std::endl;
    int radius = engine->game->map_width/2;
    for(int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            Point p = ship->pos + Point(dx, dy);
            engine->game->grid.normalize(p);

            if (!first && targeted[p] != nullptr && targeted[p] != ship)
                continue;

            int mining_turns = std::max(0, 2 * radius - engine->game->grid.dist(ship->pos, p));
            float score = static_cast<float>(engine->game->grid[p]) / 4.f
            * (1.f - std::pow(.75f, mining_turns)) / .25f;
            
            score = std::min(MAX_CARGO - ship->halite, static_cast<int>(score));

            score /= 1. + engine->game->grid.dist(ship->pos, p);
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
            if (engine->game->grid.dist(ship->pos, best) < engine->game->grid.dist(other->pos, best))
                targeted[best] = ship;
        }
        else
            targeted[best] = ship;
    }

    return best;
}

Point DirectSearch::find_mining_sector(Ship *ship)
{
    /*
    std::sort(std::begin(engine->game->sectors), std::end(engine->game->sectors),
              [this, ship](Sector &a, Sector &b) {
                  return a.halite / (engine->game->dist_to_dropoff[a.centroid]) > b.halite / (engine->game->dist_to_dropoff[b.centroid]);
              });
    */
    int best_score = 0;
    Point best = ship->pos;
    for (auto& sector : engine->game->sectors)
    {
        int score = sector.halite / (engine->game->dist_to_dropoff[sector.centroid] + engine->game->grid.dist(sector.centroid, ship->pos));
        if (score > best_score)
        {
            best_score = score;
            best = sector.centroid;
            
        }
    }
    return best;
}


void DirectSearch::navigate()
{
    std::sort(engine->game->me->ships.begin(), engine->game->me->ships.end(),
              [](Ship *a, Ship *b) {
                  if (a->task != b->task)
                      return a->task < b->task;

                  return a->halite > b->halite;
              });

    for (auto &ship : engine->game->me->ships)
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

    for (auto &ship : engine->game->me->ships)
    {
        if (ship->just_moved || !ship->active)
            continue;

        if (move_ship(ship))
        {
            if (endgame)
            {
                for(auto& dropoff : engine->game->me->dropoffs)
                    ship_on_tile[dropoff->pos] = nullptr;
            }
            //std::cerr << "SUCCESS " << ship->id << " to " << moves_str[ship->action] << std::endl;
        }
        else
            std::cerr << "FAIL " << ship->id << std::endl;
        
    }
}

bool DirectSearch::move_ship_dir(Ship *ship, int dir)
{
    Point n = ship->pos + moves_dir[dir];
    engine->game->grid.normalize(n);

    // my ship plans to move there next turn
    Ship *other_ship = ship_on_tile[n];
    if (other_ship != nullptr)
        return false;

    if (ship->task == DELIVER)
    {
        if (!engine->game->unsafe[ship->owner][n])
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
        Ship* anyone = engine->game->ships_grid[n];
        if (anyone == nullptr || (anyone != nullptr && anyone->owner == engine->game->my_id))
        {
            ship_on_tile[n] = ship;
            ship->just_moved = true;
            ship->action = dir;
            return true;
        }
    }
    else
    {
        ship_on_tile[n] = ship;
        ship->just_moved = true;
        ship->action = dir;
        return true;
    }

    return false;
}

// returns true if found successful move (i.e. no collision)
bool DirectSearch::move_ship(Ship *ship, Ship *forcing)
{
    if (ship->target == ship->pos)
    {
        if (move_ship_dir(ship, 0))
            return true;
    }

    if (engine->can_move(ship))
    {
        //std::cerr<< *ship << " wants to go to " << ship->target << " from " << ship->pos << std::endl;
        int dirs[5] = {0, 1, 2 , 3, 4};

        std::sort(std::begin(dirs), std::end(dirs), [this, ship](int &a, int &b) {
            Point na = ship->pos + moves_dir[a], nb = ship->pos + moves_dir[b];
            engine->game->grid.normalize(na);
            engine->game->grid.normalize(nb);
            int da = engine->game->grid.dist(ship->target, na);
            int db = engine->game->grid.dist(ship->target, nb);
            
            // priority: 1- distance, 2- cost
            if (da != db)   return da < db;
            else            return engine->game->grid[na] < engine->game->grid[nb];
        });

        for (auto i : dirs)
        {
            if (move_ship_dir(ship, i))
                return true;
        }

        for (auto dir : dirs)
        {
            Point n = ship->pos + moves_dir[dir];
            engine->game->grid.normalize(n);

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
            
            Ship* other_ship = ship_on_tile[ship->pos];

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