#include <random_search.h>

#include <iostream>
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()
#define TURNTIME std::chrono::duration_cast<std::chrono::microseconds>(NOW - START).count() / 1000.

Solution GASearch::search(float time_limit)
{
    START = NOW;
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
    START = NOW;
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

    for (int i = 0; i < engine->game->me->ships.size(); i++)
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
