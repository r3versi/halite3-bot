#include "src/game.hpp"
#include "src/log.hpp"

#include <random>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
    unsigned int rng_seed;
    if (argc > 1)   rng_seed = static_cast<unsigned int>(stoul(argv[1]));
    else            rng_seed = static_cast<unsigned int>(time(nullptr));
    mt19937 rng(rng_seed);

    logger::start("MyCrappyBotV1");
    logger::log("RNG seed " + to_string(rng_seed));

    while (true)
    {

        break;
    }

    logger::log("Game ending");
}