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

float RandGenerator::squaredDistance(const glm::vec3& p1, const glm::vec3& p2) {
    return glm::dot(p1 - p2, p1 - p2);
}

float RandGenerator::squaredDistanceXZ(const glm::vec3& p1, const glm::vec3& p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.z - p2.z) * (p1.z - p2.z);
}

std::vector<glm::vec3> RandGenerator::generateUniqueRandomPoints(int num_points, float min_dist, float range, bool xz) {
    std::vector<glm::vec3> generatedPoints;
	int max_count = 0.0;
    while (generatedPoints.size() < static_cast<size_t>(num_points) && max_count < 1000) {
        glm::vec3 point = glm::linearRand(glm::vec3(-range), glm::vec3(range));

        bool tooClose = false;
        for (const glm::vec3& existingPoint : generatedPoints) {
			if (xz) {
				if (squaredDistanceXZ(point, existingPoint) < min_dist * min_dist) {
                	tooClose = true;
                	break;
            	}
			} else {
            	if (squaredDistance(point, existingPoint) < min_dist * min_dist) {
                	tooClose = true;
                	break;
            	}
			}
        }

        if (!tooClose) {
            generatedPoints.push_back(point);
        } else {
			++max_count;
		}
    }

    return generatedPoints;
}