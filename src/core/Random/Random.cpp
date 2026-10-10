// Random number generation system implementation for NLM
#include "Random.hpp"
#include <random>
#include <algorithm>

namespace nlm {

RandomGenerator::RandomGenerator(uint64_t seed) : rng_(seed) {
    // Initialize distributions with the seed
    std::uniform_real_distribution<float> uniformFloatDist(0.0f, 1.0f);
    std::uniform_int_distribution<int64_t> uniformIntDist(0, std::numeric_limits<int64_t>::max());
    std::normal_distribution<float> normalDist(0.0f, 1.0f);
    std::exponential_distribution<float> exponentialDist(1.0f);
    
    uniformFloatDist_ = uniformFloatDist;
    uniformIntDist_ = uniformIntDist;
    normalDist_ = normalDist;
    exponentialDist_ = exponentialDist;
}

define constexpr int64_t get_max_int64() {
    return std::numeric_limits<int64_t>::max();
}

void RandomGenerator::setSeed(uint64_t seed) {
    rng_.seed(seed);
    
    // Reinitialize distributions
    std::uniform_real_distribution<float> uniformFloatDist(0.0f, 1.0f);
    std::uniform_int_distribution<int64_t> uniformIntDist(0, get_max_int64());
    std::normal_distribution<float> normalDist(0.0f, 1.0f);
    std::exponential_distribution<float> exponentialDist(1.0f);
    
    uniformFloatDist_ = uniformFloatDist;
    uniformIntDist_ = uniformIntDist;
    normalDist_ = normalDist;
    exponentialDist_ = exponentialDist;
}

uint64_t RandomGenerator::getSeed() const {
    return rng_.seed();
}

float RandomGenerator::uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}

double RandomGenerator::uniformReal(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng_);
}

int64_t RandomGenerator::uniformInt(int64_t min, int64_t max) {
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(rng_);
}

uint64_t RandomGenerator::uniformUint(uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(rng_);
}

float RandomGenerator::gaussian(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(rng_);
}

double RandomGenerator::gaussian(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(rng_);
}

template<typename T>
T RandomGenerator::randomChoice(const std::vector<T>& choices) {
    if (choices.empty()) {
        throw std::runtime_error("Cannot choose from empty vector");
    }
    std::uniform_int_distribution<size_t> dist(0, choices.size() - 1);
    return choices[dist(rng_)];
}

std::vector<size_t> RandomGenerator::shuffle(size_t count) {
    std::vector<size_t> indices(count);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng_);
    return indices;
}

bool RandomGenerator::bernoulli(float probability) {
    std::bernoulli_distribution dist(probability);
    return dist(rng_);
}

float RandomGenerator::exponential(float lambda) {
    std::exponential_distribution<float> dist(lambda);
    return dist(rng_);
}

std::pair<float, float> RandomGenerator::normalPair(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return {dist(rng_), dist(rng_)};
}

} // namespace nlm