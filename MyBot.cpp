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
    cout << "MyCrappyBotV1" << endl;

    while (true)
    {
        game.turn_update();
        start = NOW;
        

        
    }
}