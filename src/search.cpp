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
    ship_on_tile[0].reset();
    ship_on_tile[1].reset();

    assign_tasks();
    navigate();

    if (engine->game->me->halite >= 1000 && 
        engine->game->me->ships.size() < 30 &&
        ship_on_tile[1][engine->game->me->spawn] == nullptr)
        engine->game->me->action = true;
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
    {
        commands += "g ";
    }

    for(auto& ship : engine->game->me->ships)
    {
        commands += ship->get_command();
    }

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
    return engine->game->me->spawn;
}

Point DirectSearch::find_mining_site(Ship *ship)
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

    for (auto &ship : engine->game->me->ships)
    {
        if (!ship->active)
            continue;

        std::cerr << *ship;

        if (ship->halite >= 900 || (ship->task == DELIVER && ship->halite > 0))
        {
            ship->task = DELIVER;
            ship->target = find_deliver_site(ship);
        }
        else if (engine->game->halite_nbhood[ship->pos] >= 6250)
        {
            ship->task = MINE;
            if (engine->game->grid[ship->pos] > 200)
                ship->target = ship->pos;
            else
                ship->target = find_mining_site(ship);
        }
        else
        {
            ship->task = GOTO;
            ship->target = find_mining_site(ship);
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
        if (!engine->can_move(ship))
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
            std::cerr << "SUCCESS " << ship->id << " to " << moves_str[ship->action] << std::endl;
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
    if (engine->can_move(ship))
    {
        Point dir = (ship->target - ship->pos);

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

        if (move_ship_dir(ship, 0))
            return true;

        // try other directions
        for (auto &i : {1, 2, 3, 4})
        {
            if (move_ship_dir(ship, i))
                return true;
        }
    }
    else
    {
        // can stay still?
        if (move_ship_dir(ship, 0))
            return true;
    }

    return false;
}