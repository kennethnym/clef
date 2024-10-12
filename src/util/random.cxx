#include "random.hxx"
#include <random>

static std::random_device dev;
static std::mt19937 rng(dev());

std::mt19937 Clef::get_rng() { return rng; }
