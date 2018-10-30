#pragma once

#include <constants.h>
#include <engine.h>

using ShipSolution  = Container<MAX_DEPTH,          int >;
using GroupSolution = Container<MAX_CLUSTER_SIZE,   ShipSolution >;

static unsigned int seed = 42;
inline int fastrand()
{
    seed = (214013 * seed + 2531011);
    return (seed >> 16) & 0x7FFF;
}
inline int rnd(int a) { return fastrand() % a; }

struct Solution {
    float _score;
    GroupSolution data;

    inline void clear() { data.clear(); }
    inline size_t size() const { return data.size(); }
    inline float score() { return _score; }
    inline void put(ShipSolution item) { data.put(item); }
    inline ShipSolution *begin() { return data.begin(); }
    inline ShipSolution *end() { return data.end(); }
    inline ShipSolution &operator[](size_t idx) { return data[idx]; }
    inline const ShipSolution &operator[](size_t idx) const { return data[idx]; }
};

class Search {
public:
    size_t depth;
    Engine *engine;

    Search(size_t depth, Engine *engine) : depth(depth), engine(engine) {}

    virtual Solution search(float time_limit) = 0;
    Solution get_rnd();

    void apply_actions(Solution &sol, int turn);
    float evaluate(Solution &sol);
};

class GASearch : public Search {
public:
    size_t pop_size;
    Solution* pop;

    GASearch(size_t pop_size, size_t depth, Engine *engine) : Search(depth, engine), pop_size(pop_size) 
    { pop = new Solution[pop_size]; }
    ~GASearch() {delete[] pop;}
    Solution search(float time_limit);

    void sort();
    void mutate(Solution& sol);
    void cross(Solution& mum, Solution& dad, Solution& child0, Solution& child1);
};


class RandomSearch : public Search{
public:

    RandomSearch(size_t depth, Engine *engine) : Search(depth, engine) {}

    Solution search(float time_limit);
};

const int NONE = -1;
const int DELIVER = 0;
const int GOTO = 1;
const int MINE = 2;

class DirectSearch : public Search {
public:

    bool endgame;
    Grid<Ship *> ship_on_tile[2];
    Grid<Ship *> targeted;
    DirectSearch(size_t depth, Engine* engine) : Search(depth, engine) 
    {
        ship_on_tile[0] = Grid<Ship *>(engine->game->map_width, engine->game->map_height);
        ship_on_tile[1] = Grid<Ship *>(engine->game->map_width, engine->game->map_height);
        targeted = Grid<Ship *>(engine->game->map_width, engine->game->map_height);
        endgame = false;
    }

    Solution search(float time_limit);

    std::string get_commands();

    void assign_tasks();
    void navigate();

    void find_target(Ship& ship);
    
    bool move_ship(Ship* ship);

    Point find_deliver_site(Ship* ship);
    Point find_mining_site(Ship* ship, bool first = true);
    Point find_mining_sector(Ship *ship);

    bool next_turn_free(Point &p);
    Ship* next_turn(Point &p);
    bool  move_ship_dir(Ship *ship, int dir);
};
