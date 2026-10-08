#include "Random.hpp"
#include <chrono>
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 engine;
    uint64_t seed;
    
    Impl() : engine(), seed(0) {
        seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        engine.seed(seed);
    }
    
    explicit Impl(uint64_t s) : engine(), seed(s) {
        engine.seed(s);
    }
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(std::make_unique<Impl>()) {}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    pImpl->seed = seed;
    pImpl->engine.seed(seed);
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->seed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->engine);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::uniformReal(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->engine);
}

float RandomGenerator::uniformReal(float min, float max) {
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
    std::bernoulli_distribution dist(p);
    return dist(pImpl->engine);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->engine);
}

double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->engine);
}

bool RandomGenerator::randomBool() {
    return uniformInt(0, 1) == 1;
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    auto* bytes = static_cast<unsigned char*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<unsigned char>(uniformInt(0, 255));
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->engine;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->engine;
}

} // namespace nlm
