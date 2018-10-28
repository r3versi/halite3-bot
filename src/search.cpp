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
    ship_on_tile[0].reset();
    ship_on_tile[1].reset();
    targeted.reset();

    assign_tasks();
    navigate();

    if (engine->game->me->halite >= 1000 && 
        engine->game->me->ships.size() < 30 &&
        ship_on_tile[1][engine->game->me->spawn] == nullptr &&
        engine->game->max_turn - engine->game->turn > 100
        )
    {
        engine->game->me->action = true;
    }
    else
    {
        std::cerr << "Cannot spawn new ship: "
                  << ship_on_tile[1][engine->game->me->spawn] << "on tile" << std::endl
                  << engine->game->me->halite << " halite" << std::endl
                  << engine->game->me->ships.size() << " ships" << std::endl;

        engine->game->me->action = false;
    }
    return Solution();
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

Point DirectSearch::find_deliver_site(Ship *ship)
{
    /*
    int min_dist = 100;
    for(auto& dropoff : engine->game->me->dropoffs)
    {
        int dist = engine->game->grid.dist(ship->pos, dropoff->pos);
        if (dist < min_dist)
            ship->target = dropoff->pos;
    }
    */
    if (!ship->active)
        std::cerr << "Ship not active!" << std::endl;

    return engine->game->me->spawn;
}

Point DirectSearch::find_mining_site(Ship *ship)
{
    float best_score = 0.;
    Point best = ship->pos;
    std::cerr << *ship << std::endl;
    int radius = 4;
    for(int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            Point p = ship->pos + Point(dx, dy);
            engine->game->grid.normalize(p);
            int mining_turns = std::max(0, 2*radius - engine->game->grid.dist(ship->pos, p));
            float score = static_cast<float>(engine->game->grid[p]) / 4.f
            * (1.f - std::pow(.75f, mining_turns)) / .25f;

            score /= 1. + engine->game->grid.dist(ship->pos, p);
            if (score > best_score)
            {
                best_score = score;
                best = p;
            }
            std::cerr << p << " " << (int)score << "\t";
        }
        std::cerr << std::endl;
    }
    /*
    Ship *other = targeted[best];
    if (other != nullptr) 
    {
        if (engine->game->grid.dist(other->pos, best) > engine->game->grid.dist(ship->pos, best))
        {
            targeted[best] = ship;
            other->target = find_mining_site(other);
        }      
    }
    */
    targeted[best] = ship;
    return best;
}

Point DirectSearch::find_mining_zone(Ship *ship)
{
    //Point dir = ship->pos - engine->game->me->spawn;
    Point neighbours[5];
    for(auto i : {0,1,2,3,4})
    {
        neighbours[i] = ship->pos + moves_dir[i];
        engine->game->grid.normalize(neighbours[i]);
    }

    std::sort(std::begin(neighbours), std::end(neighbours),
              [this](Point &a, Point &b) {
                  return engine->game->grid[a] > engine->game->grid[b];
              });

    return neighbours[0];
}

void DirectSearch::assign_tasks()
{
    int remaining_turns = engine->game->max_turn - engine->game->turn;
    endgame = remaining_turns < (int)engine->game->map_width;

    for (auto &ship : engine->game->me->ships)
    {
        if (!ship->active)
            continue;

        std::cerr << *ship;
        Point deliver_site = find_deliver_site(ship);

        if (ship->halite >= 900 
            || (ship->task == DELIVER && ship->halite > 0) 
            || (endgame && engine->game->grid.dist(ship->pos, deliver_site) >= remaining_turns-5)
            )
        {
            ship->task = DELIVER;
            ship->target = deliver_site;
        }
        /*
        else if (engine->game->halite_nbhood[ship->pos] >= 6250)
        {
            ship->task = MINE;
            ship->target = find_mining_site(ship);
        }
        */
        else
        {
            ship->task = GOTO;
            ship->target = find_mining_site(ship);
            targeted[ship->target] = ship;
        }

        std::cerr << "{Task: " << ship->task << ", Target " << ship->target << "}" << std::endl;
    }
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

        ship_on_tile[0][ship->pos] = ship;
        if (!engine->can_move(ship) || ship->target == ship->pos)
        {
            ship_on_tile[1][ship->pos] = ship;
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
                ship_on_tile[1][engine->game->me->spawn] = nullptr;
            std::cerr << "SUCCESS " << ship->id << " to " << moves_str[ship->action] << std::endl;
        }
        else
            std::cerr << "FAIL " << ship->id << std::endl;
    }
}

bool DirectSearch::next_turn_free(Point &p)
{
    return (ship_on_tile[1][p] == nullptr);
}

Ship *DirectSearch::next_turn(Point &p)
{
    return ship_on_tile[1][p];
}

bool DirectSearch::move_ship_dir(Ship *ship, int dir)
{
    Point n = ship->pos + moves_dir[dir];
    engine->game->grid.normalize(n);
    Ship *other_ship = next_turn(n);
    if (other_ship == nullptr)
    {
        ship_on_tile[1][n] = ship;
        ship->just_moved = true;
        ship->action = dir;
        return true;
    }
    return false;
}

// returns true if found successful move (i.e. no collision)
bool DirectSearch::move_ship(Ship *ship)
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
            return engine->game->grid.dist(ship->target, na) < engine->game->grid.dist(ship->target, nb);
        });

        for (auto i : dirs)
        {
            if (move_ship_dir(ship, i))
                return true;
        }
        
        return false;
        /*
        Point dir0 = ship->target - ship->pos, dir1 = -dir0;
        std::cerr << dir0 << " " << dir1 << std::endl;
        engine->game->grid.normalize(dir0);
        engine->game->grid.normalize(dir1);
        std::cerr << dir0 << " " << dir1 << std::endl;
        std::cerr << dir0.length() << " " << dir1.length() << std::endl;
        Point dir = dir0.length() <= dir1.length() ? ship->target - ship->pos : ship->pos - ship->target;
        
        if (dir.x < 0)
        {
            if (move_ship_dir(ship, 4))
                return true;
        }
        else if (dir.x > 0)
        {
            // e
            if (move_ship_dir(ship, 2))
                return true;
        }

        if (dir.y < 0)
        {
            //n
            if (move_ship_dir(ship, 1))
                return true;
        }
        else if (dir.y > 0)
        {
            //s
            if (move_ship_dir(ship, 3))
                return true;
        }

        // try other directions
        for (auto &i : {1, 2, 3, 4, 0})
        {
            if (move_ship_dir(ship, i))
                return true;
        }
        */
    }
    else
    {
        // can stay still?
        if (move_ship_dir(ship, 0))
            return true;
    }

    return false;
}