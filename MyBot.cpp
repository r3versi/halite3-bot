#include <bits/stdc++.h>
#include <src/game.h>


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
        game.save();
        start = NOW;

        cerr << "TURN UPDATED " << game.turn << " @" << TURNTIME << endl;
        game.dump(false);
        
        
        if (me->halite >= 1000)
            cout << "g ";
        
        for(auto& ship : me->ships)
        {
            cout << "m " << ship->id << " n";
        }
        cout << endl;
    }
}