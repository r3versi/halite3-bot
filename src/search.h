#pragma once

#include <game.h>
#include <container.h>
#include <time.h>
#include <engine.h>

static unsigned int seed = time(NULL); //time(NULL);
inline int fastrand()
{
    seed = (214013 * seed + 2531011);
    return (seed >> 16) & 0x7FFF;
}
inline int rnd(int a) { return fastrand() % a; }

const int MAX_MOVES = 5;
const int moves[5] = {0, 1, 2, 3, 4};
const char moves_str[5] = {'o','n','e','s','w'};
const Point moves_dir[5] = {Point(0,0), Point(0, -1), Point(1, 0), Point(0, 1), Point(-1, 0)};


class Clusterizer;
class Generator;
class Search;


class RandomSearch {
public:
    size_t depth;
    Generator generator;
    Game* game;

    RandomSearch(Game* game, size_t depth)
    {
        this->game = game;
        this->depth = depth;

        generator = Generator();
    }

    Solution search(int max_iter)
    {
        Solution best = generator.get_rnd();
        float best_score = evaluate(best);

        int i = 0;
        while(i < max_iter)
        {
            Solution sol = generator.get_rnd();
            float score = evaluate(sol);

            if (score >= best_score)
            {
                best_score = score;
                best = sol;
            }
        }

        return best;
    }
};

    float evaluate(Solution& sol)
    {
        float score = 0.;
        game->save();
        play(sol);

        score += game->me->halite;
        
        for(size_t i = 0; i < game->me->n_ships; i++)
        {
            Ship &my_ship = game->ships[game->me->ships[i]];
            
            if (!my_ship.active)
            {
                score += - 1000;
                continue;
            }
            
            if (my_ship.halite > 900)
            {
                // distance to dropoff
                score -= game->grid->dist(my_ship, game->me->spawn);
            }

            score += .75 * my_ship.halite;

        }
        
        game->load();

    }
