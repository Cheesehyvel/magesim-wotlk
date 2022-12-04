#include "common.h"

#include <random>

thread_local std::mt19937 g_rng(std::random_device{}());

double critRatingToChance(double rating)
{
    return rating / 45.91;
}

double hitRatingToChance(double rating)
{
    return rating / 26.232;
}

double hasteRatingToHaste(double rating)
{
    return rating / 32.79;
}

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
