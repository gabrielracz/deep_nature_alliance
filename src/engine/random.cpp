#include "random.h"

RandGenerator::RandGenerator() {
	eng = std::default_random_engine(rd());
}

RandGenerator::RandGenerator(int seed) {
	eng = std::default_random_engine(rd());
    eng.seed(seed);
}

void RandGenerator::setrange(float min, float max) {
	float_distr = std::uniform_real_distribution<float>(min, max);
}

void RandGenerator::setrange(int min, int max) {
	int_distr = std::uniform_int_distribution<int>(min, max);
}

int RandGenerator::randint() {
	return int_distr(eng);
}

float RandGenerator::randfloat() {
	return float_distr(eng);
}

int RandGenerator::randint(int min, int max) {
	if (min > max) {
		int tmp = min;
		min = max;
		max = tmp;
	}
    setrange(min, max);
	return int_distr(eng);
}

float RandGenerator::randfloat(float min, float max) {
	if (min > max) {
		float tmp = min;
		min = max;
		max = tmp;
	}
    setrange(min, max);
	return float_distr(eng);
}

int RandGenerator::randsign() {
    return rand() % 2 == 0 ? 1 : -1;
}
