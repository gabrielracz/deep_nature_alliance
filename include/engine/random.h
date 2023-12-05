#ifndef RANDOMGEN_H
#define RANDOMGEN_H
#pragma once
#include <algorithm>
#include <random>
#include <chrono>
#include <glm/glm.hpp>
#include "glm/gtc/random.hpp"

class RandGenerator {
public:
	RandGenerator();
	RandGenerator(int seed);
	int randint(int min, int max);
	float randfloat(float min, float max);
	void setrange(float min, float max);
	void setrange(int min, int max);
	int randint();
	float randfloat();
    int randsign();
	float squaredDistance(const glm::vec3& p1, const glm::vec3& p2);
	float squaredDistanceXZ(const glm::vec3& p1, const glm::vec3& p2);
	std::vector<glm::vec3> generateUniqueRandomPoints(int num_points, float min_dist, float range, bool xz = true);
private:
	std::random_device rd;
	std::default_random_engine eng;
	std::uniform_real_distribution<float> float_distr;
	std::uniform_int_distribution<int> int_distr;
};


template <typename IntegerT>
IntegerT random( IntegerT min, IntegerT max )
{
  static std::ranlux48 rng( std::chrono::system_clock().now().time_since_epoch().count() );
  return std::uniform_real_distribution <IntegerT> ( min, max )( rng );
}
#endif
