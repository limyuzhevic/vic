#pragma once

// Random.cpp - Enhanced implementation with comprehensive error handling

#include "Random.hpp"
#include "Error/NLMError.hpp"
#include "Error/ErrorCategory.hpp"
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <system_error>

namespace nlm {

// Seed validation constants
static constexpr uint64_t MIN_VALID_SEED = 0;
static constexpr uint64_t MAX_VALID_SEED = UINT64_MAX;

// Range validation
static constexpr int32_t MIN_INT32 = std::numeric_limits<int32_t>::min();
static constexpr int32_t MAX_INT32 = std::numeric_limits<int32_t>::max();
static constexpr int64_t MIN_INT64 = std::numeric_limits<int64_t>::min();
static constexpr int64_t MAX_INT64 = std::numeric_limits<int64_t>::max();
static constexpr double MIN_DOUBLE = std::numeric_limits<double>::lowest();
static constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();
static constexpr float MIN_FLOAT = std::numeric_limits<float>::lowest();
static constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

// Secure random device initialization
static uint64_t getSecureSeed() {
    try {
        std::random_device rd;
        uint64_t seed = 0;
        
        // Combine multiple values for better entropy
        for (size_t i = 0; i < sizeof(uint64_t); ++i) {
            seed = (seed << 8) | static_cast<uint8_t>(rd());
        }
        
        return seed;
    } catch (const std::exception& e) {
        throw RandomError("getSecureSeed", "Failed to generate secure seed: " + std::string(e.what()));
    }
}

static bool validateSeed(uint64_t seed) {
    return seed >= MIN_VALID_SEED && seed <= MAX_VALID_SEED;
}

static bool validateIntegerRange(int64_t value, int64_t min, int64_t max) {
    return value >= min && value <= max;
}

static bool validateRealRange(double value, double min, double max) {
    if (std::isnan(value) || std::isinf(value)) {
        return false;
    }
    return value >= min && value <= max;
}

static bool validateRealRange(float value, float min, float max) {
    if (std::isnan(value) || std::isinf(value)) {
        return false;
    }
    return value >= min && value <= max;
}

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
    if (!validateSeed(seed)) {
        throw RandomError("constructor", "Invalid seed value: " + std::to_string(seed));
    }
    spdlog::debug("RandomGenerator created with seed: {}", seed);
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(getSecureSeed())) {
    spdlog::debug("RandomGenerator created with time-based seed");
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
    if (!validateSeed(seed)) {
        throw RandomError("seed", "Invalid seed value: " + std::to_string(seed));
    }
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
    spdlog::debug("RandomGenerator seeded with: {}", seed);
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

namespace nlm {

// Implementation of template methods and convenience functions

// Generate uniform integer in [min, max]
template<typename IntegerType>
IntegerType RandomGenerator::uniformInt(IntegerType min, IntegerType max) {
    static_assert(std::is_integral_v<IntegerType>, "IntegerType must be integral");
    
    if (min > max) {
        throw RandomError("uniformInt", "min must be <= max: " + std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    
    std::uniform_int_distribution<IntegerType> dist(min, max);
    return dist(pImpl->gen);
}

// Generate uniform real in [min, max)
template<typename RealType>
RealType RandomGenerator::uniformReal(RealType min, RealType max) {
    static_assert(std::is_floating_point_v<RealType>, "RealType must be floating point");
    
    if (!validateRealRange(min, MIN_DOUBLE, MAX_DOUBLE) || !validateRealRange(max, MIN_DOUBLE, MAX_DOUBLE)) {
        throw RandomError("uniformReal", "Invalid range values: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
    }
    
    if (min >= max) {
        throw RandomError("uniformReal", "min must be < max: " + std::to_string(min) + " >= " + std::to_string(max));
    }
    
    std::uniform_real_distribution<RealType> dist(min, max);
    return dist(pImpl->gen);
}

// Generate normal distribution
namespace {
    template<typename RealType>
    RealType normal(RealType mean, RealType stddev, std::mt19937_64& gen) {
        static_assert(std::is_floating_point_v<RealType>, "RealType must be floating point");
        
        if (!validateRealRange(mean, MIN_DOUBLE, MAX_DOUBLE) || !validateRealRange(stddev, 0.0, MAX_DOUBLE)) {
            throw RandomError("normal", "Invalid parameters: mean=" + std::to_string(mean) + ", stddev=" + std::to_string(stddev));
        }
        
        if (stddev < 0.0) {
            throw RandomError("normal", "Standard deviation must be non-negative: " + std::to_string(stddev));
        }
        
        std::normal_distribution<RealType> dist(mean, stddev);
        return dist(gen);
    }
}

// Convenience methods
uint32_t RandomGenerator::uniformInt32(uint32_t min, uint32_t max) {
    return uniformInt(min, max);
}

uint64_t RandomGenerator::uniformInt64(uint64_t min, uint64_t max) {
    return uniformInt(min, max);
}

double RandomGenerator::uniformReal64(double min, double max) {
    return uniformReal(min, max);
}

float RandomGenerator::uniformReal32(float min, float max) {
    return uniformReal(min, max);
}

double RandomGenerator::normal64(double mean, double stddev) {
    return normal(mean, stddev, pImpl->gen);
}

float RandomGenerator::normal32(float mean, float stddev) {
    return normal(mean, stddev, pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (!validateRealRange(p, 0.0, 1.0)) {
        throw RandomError("bernoulli", "Probability p must be in [0, 1]: " + std::to_string(p));
    }
    
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (!validateRealRange(lambda, 0.0, MAX_DOUBLE)) {
        throw RandomError("poisson", "Lambda must be non-negative: " + std::to_string(lambda));
    }
    
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (!validateRealRange(lambda, 0.0, MAX_DOUBLE)) {
        throw RandomError("exponential", "Lambda must be non-negative: " + std::to_string(lambda));
    }
    
    std::exponential_distribution<double> dist(lambda);
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
        throw RandomError("pickRandomIndex", "Cannot pick from zero items");
    }
    return uniformInt<uint64_t>(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!data || size == 0) {
        throw RandomError("fillBytes", "Invalid parameters: data=nullptr or size=0");
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm

// Template method implementations
namespace nlm {

// Shuffle container
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::shuffle(container.begin(), container.end(), engine());
}

// Pick random element from container
template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}

} // namespace nlm