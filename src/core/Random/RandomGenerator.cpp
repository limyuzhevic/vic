#include "RandomGenerator.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <vector>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 generator;
    uint64_t seed;
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl) {
    pImpl->seed = seed;
    pImpl->generator.seed(seed);
}

RandomGenerator::RandomGenerator() : RandomGenerator(std::chrono::steady_clock::now().time_since_epoch().count()) {
}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    pImpl->seed = seed;
    pImpl->generator.seed(seed);
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->seed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->generator);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->generator);
}

double RandomGenerator::uniformReal(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->generator);
}

float RandomGenerator::uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->generator);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->generator);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->generator);
}

bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->generator);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->generator);
}

double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->generator);
}

bool RandomGenerator::randomBool() {
    return uniformReal(0.0, 1.0) > 0.5;
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->generator;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->generator;
}

} // namespace nlm
