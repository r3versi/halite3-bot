#include <bits/stdc++.h>
#include <game.h>

using namespace std;
using namespace std::chrono;

#define NOW high_resolution_clock::now()
#define TURNTIME duration_cast<microseconds>(NOW - start).count() / 1000.

static auto start = NOW;

int main(int argc, char *argv[])
{
    unsigned int rng_seed;
    if (argc > 1)   rng_seed = static_cast<unsigned int>(stoul(argv[1]));
    else            rng_seed = static_cast<unsigned int>(time(nullptr));
    mt19937 rng(rng_seed);

    Game game;
    game.init_input();
    game.dump();

    cout << "MyCrappyBotV1" << endl;

    Player* me = game.me;
    while (true)
    {
        game.turn_update();
        game.dump();
        start = NOW;
        
        
        if (me->halite >= 1000)
            cout << "s ";
        for(size_t i = 0; i < me->n_ships; i++)
        {
            cout << me->ships[i] << " n";
        }
        cout << endl;
        
    }
}