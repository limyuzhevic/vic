#include "Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace nlm {

struct RandomGenerator::Impl {
    uint64_t seed;
    std::mt19937_64 engine;
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>()) {
    pImpl->seed = seed;
    pImpl->engine.seed(seed);
}

RandomGenerator::RandomGenerator() : pImpl(std::make_unique<Impl>()) {
    // Use time and process info for non-deterministic seeding
    std::random_device rd;
    uint64_t seed = rd() ^ (std::chrono::steady_clock::now().time_since_epoch().count() << 32);
    pImpl->seed = seed;
    pImpl->engine.seed(seed);
}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&&) noexcept = default;

RandomGenerator& RandomGenerator::operator=(RandomGenerator&&) noexcept = default;

void RandomGenerator::seed(uint64_t seed) {
    pImpl->seed = seed;
    pImpl->engine.seed(seed);
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->seed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->engine);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min > max) std::swap(min, max);
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->engine);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min > max) std::swap(min, max);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->engine);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->engine);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Bernoulli probability must be in [0,1]");
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->engine);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("Poisson lambda must be non-negative");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->engine);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("Exponential lambda must be positive");
    }
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->engine);
}

bool RandomGenerator::randomBool() {
    return bernoulli();
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->engine;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->engine;
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; i += sizeof(uint64_t)) {
        uint64_t randomValue = uniformInt();
        size_t bytesToCopy = std::min(sizeof(uint64_t), size - i);
        std::memcpy(bytes + i, &randomValue, bytesToCopy);
    }
}

} // namespace nlm