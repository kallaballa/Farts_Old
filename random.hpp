#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include "defines.hpp"
#include <random>

std::random_device randomDev;
std::mt19937 rng(randomDev());
std::uniform_int_distribution<> sampleDist(0, SAMPLE_MAX);
std::uniform_real_distribution<> weightDist(0, 1);

floating_t randomWeight() {
	return weightDist(rng);
}

sample_t randomSample() {
	return sampleDist(rng);
}

#endif /* RANDOM_HPP_ */
