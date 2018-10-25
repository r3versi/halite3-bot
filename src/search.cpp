#include <search.h>

#include <iostream>
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()
#define TURNTIME std::chrono::duration_cast<std::chrono::microseconds>(NOW - start).count() / 1000.

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
