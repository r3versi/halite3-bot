#include <bits/stdc++.h>
#include <src/game.h>
#include <src/engine.h>

using namespace std;
using namespace std::chrono;

#define NOW high_resolution_clock::now()
#define TURNTIME duration_cast<microseconds>(NOW - start).count() / 1000.

static auto start = NOW;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    unsigned int rng_seed;
    if (argc > 1)   rng_seed = static_cast<unsigned int>(stoul(argv[1]));
    else            rng_seed = static_cast<unsigned int>(time(nullptr));
    mt19937 rng(rng_seed);
    std::uniform_int_distribution<int> uid(1,4);
    
    Game game;
    //Engine engine = Engine(&game);

    game.init_input();
    //game.dump();

    cout << "MyCrappyBotV1" << endl;

    Player* me = game.me;
    while (true)
    {
        /*
        cerr << endl
             << endl
             << "Turn" << game.turn << "\n================================================" << endl
             << " @" << TURNTIME << " ms" << endl << flush;
        */
        game.turn_update(start);
        start = NOW;
        /*
        cerr << "game updated"
             << " @" << TURNTIME << " ms" << endl;
        //game.save();
        cerr << "game saved"
             << " @" << TURNTIME << " ms" << endl;
        //game.dump();
        cerr << "game dumped"
             << " @" << TURNTIME << " ms" << endl;
        */
        if (me->halite >= 1000)
        {
            /*
            cerr << "P" << me->id << ": spawning ship"
                 << " @" << TURNTIME << " ms" << endl;
            */
            cout << "g ";
        }
        /*
        cerr << "spawn checked"
             << " @" << TURNTIME << " ms" << endl;
        

        cerr << "Starting ship commands" 
             << " @" << TURNTIME << " ms" << endl;
        */
        //int i = 0;
        for(auto& ship : me->ships)
        {
            //cerr << (i++) << " ";

            if (!ship->active)
                continue;

            int move = uid(rng);
            ship->action = move;
            /*
            cerr << "P" << me->id 
                 << " moving ship " << ship->id << " to " << moves_str[move] 
                 << " @" << TURNTIME << " ms" << endl;
            */
            // command to stdout
            cout << "m " << ship->id << " " << moves_str[move] << " " << flush;
        }
        /*
        cerr << "Ship commands issued"
             << " @" << TURNTIME << " ms" << endl;
        */
        // end line for commands
        cout << endl;

        /*engine.play_turn();
        game.dump(false);

        cerr << "Tiles updated during sim" << endl;
        cerr << "POINT\tGAME\tSIM" << endl;
        for(size_t x = 0; x < game.grid.width; x++)
            for(size_t y = 0; y < game.grid.height; y++)
            {
                Point p = Point(x, y);
                if (game.grid[p]-game.cache_grid[p] != 0)
                    cerr << p << "\t" << game.cache_grid[p] << "\t" << game.grid[p] << endl;
            }
        
        game.load();
        */
    }
}