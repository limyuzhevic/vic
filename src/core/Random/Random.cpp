#include "Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <random>
#include <cstring>
#include <numeric>
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {
        // Validate seed to ensure it's not zero for mt19937_64
        if (seed == 0) {
            throw std::invalid_argument("Random seed cannot be zero for mt19937_64");
        }
    }
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
    // Seed validation is done in Impl constructor
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {
    // Initialize with random device for non-deterministic seeding
    // This provides good entropy for non-reproducible simulations
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
    if (seed == 0) {
        throw std::invalid_argument("Random seed cannot be zero for mt19937_64");
    }
    
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    
    // Use thread-local distribution for better performance
    thread_local std::uniform_int_distribution<uint32_t> dist;
    dist.param(std::uniform_int_distribution<uint32_t>::param_type(min, max));
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    
    // Use thread-local distribution for better performance
    thread_local std::uniform_int_distribution<uint64_t> dist;
    dist.param(std::uniform_int_distribution<uint64_t>::param_type(min, max));
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::uniform_real_distribution<double> dist;
    dist.param(std::uniform_real_distribution<double>::param_type(min, max));
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::uniform_real_distribution<float> dist;
    dist.param(std::uniform_real_distribution<float>::param_type(min, max));
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev < 0.0) {
        throw std::invalid_argument("standard deviation must be >= 0");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::normal_distribution<double> dist;
    dist.param(std::normal_distribution<double>::param_type(mean, stddev));
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev < 0.0f) {
        throw std::invalid_argument("standard deviation must be >= 0");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::normal_distribution<float> dist;
    dist.param(std::normal_distribution<float>::param_type(mean, stddev));
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("probability must be in [0, 1]");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::bernoulli_distribution dist;
    dist.p(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::poisson_distribution<uint32_t> dist;
    dist.lambda(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0");
    }
    
    // Use thread-local distribution for better performance
    thread_local std::exponential_distribution<double> dist;
    dist.lambda(lambda);
    return dist(pImpl->gen);
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!data && size > 0) {
        throw std::invalid_argument("Cannot fill null pointer with size > 0");
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    
    // Use random device for better randomness in byte generation
    std::random_device rd;
    std::uniform_int_distribution<uint8_t> byteDist(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = byteDist(rd);
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
