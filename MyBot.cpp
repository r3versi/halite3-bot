#include <bits/stdc++.h>

#include <src/game.h>
#include <src/engine.h>
#include <src/direct_bot.h>

// #include <src/search.h>

using namespace std;
using namespace std::chrono;

#define NOW high_resolution_clock::now()
#define TURNTIME duration_cast<microseconds>(NOW - START).count() / 1000.
time_point<high_resolution_clock> START = NOW;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    unsigned int rng_seed;
    if (argc > 1)   rng_seed = static_cast<unsigned int>(stoul(argv[1]));
    else            rng_seed = static_cast<unsigned int>(time(nullptr));
    mt19937 rng(rng_seed);
    std::uniform_int_distribution<int> uid(1,4);


    Game game;
    Engine engine = Engine(&game);
    game.init_input();

    //RandomSearch solver = RandomSearch(6, &engine);
    //GASearch solver = GASearch(5, 6, &engine);
    HeurBot solver = HeurBot(&engine);

    std::ofstream err_log("logs/"+to_string(game.my_id)+"_test_out.txt");
    std::cerr.rdbuf(err_log.rdbuf());

    //game.dump();

    cout << "HeurBot v21" << endl;
    while (true)
    {
        game.turn_update();
        game.save();
        START = NOW;

        cerr << endl
             << "@" << TURNTIME << " ms \t" 
             << "TURN " << game.turn
             << endl;

        solver.search();

        cerr << "@" << TURNTIME << " ms \t"
             << "Search terminated" << endl
             << "Commands string= {"
             << solver.get_commands() << "}" << endl;

        cout << solver.get_commands() << endl;
    }

    /*
    while(true)
    {
        game.turn_update();
        game.save();
        start = NOW;

        cerr << "TURN " << game.turn
             << " @" << TURNTIME << " ms" << endl;

        
        Solution best = solver.search(100.);

        cerr << "Search terminated" 
             << " @" << TURNTIME << " ms" << endl;

        for (size_t i = 0; i < best.size(); i++)
        {
            cout << "m " << game.me->ships[i]->id << " " << moves_str[best[i][0]] << " ";

            game.me->ships[i]->action = best[i][0];
        }

        engine.play_turn();

        if (game.me->halite >= 1000 && game.me->ships.size() < 30)
        {
            bool free = true;
            for(auto& ship : game.me->ships)
            {
                if (ship->pos == game.me->spawn)
                {
                    free = false;
                    break;
                }
            }

            if (free)            
                cout << "g ";
        }
        game.load();
        cout << " " << endl;
    }
    */
        /*
    Player *me = game.me;

    while (true)
    {
        cerr << endl
             << endl
             << "Turn" << game.turn << "\n================================================" << endl
             << " @" << TURNTIME << " ms" << endl << flush;
        
        game.turn_update(start);
        start = NOW;
        
        cerr << "game updated"
             << " @" << TURNTIME << " ms" << endl;
        game.save();
        cerr << "game saved"
             << " @" << TURNTIME << " ms" << endl;
        game.dump();
        cerr << "game dumped"
             << " @" << TURNTIME << " ms" << endl;
        
        if (me->halite >= 1000 && me->ships.size() < 30)
        {
        
            cerr << "P" << me->id << ": spawning ship"
                 << " @" << TURNTIME << " ms" << endl;
        
            cout << "g ";
        }
        
        cerr << "spawn checked"
             << " @" << TURNTIME << " ms" << endl;
        

        cerr << "Starting ship commands" 
             << " @" << TURNTIME << " ms" << endl;
        
        //int i = 0;
        for(auto& ship : me->ships)
        {
            //cerr << (i++) << " ";

            if (!ship->active)
                continue;

            int move = uid(rng);
            ship->action = move;
        
            cerr << "P" << me->id 
                 << " moving ship " << ship->id << " to " << moves_str[move] 
                 << " @" << TURNTIME << " ms" << endl;
        
            // command to stdout
            if (ship->halite > 300 || ship->halite == 0)
            {
                ship->action = 1;
                cout << "m " << ship->id << " " << "n "; //moves_str[move] << " " << flush;
            }
            else
            {
                ship->action = 0;
                cout << "m " << ship->id << " " << "o ";
            }
        }
        
        cerr << "Ship commands issued"
             << " @" << TURNTIME << " ms" << endl;
        
        engine.play_turn();
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

        // end line for commands
        cout << endl;
    }
    */
}