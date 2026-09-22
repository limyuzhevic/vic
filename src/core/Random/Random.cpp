#include "Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>
#include <cassert>
#include <execution>
#include <optional>
#include <span>
#include <memory>
#include <type_traits>

namespace nlm {

namespace {
    // Helper functions for better performance and validation
    constexpr bool isValidSeed(uint64_t seed) noexcept {
        return seed != 0 && seed != UINT64_MAX;
    }
    
    constexpr bool isValidRange(uint64_t min, uint64_t max) noexcept {
        return min < max && max > 0;
    }
    
    // Optimized uniform distribution function
    template<typename T>
    T optimizedUniform(std::mt19937_64& gen, T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(gen);
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(gen);
        }
    }
    
    // Parallel sum for activity calculation
    template<typename Iterator>
    float parallelSum(Iterator begin, Iterator end) {
        try {
            return std::accumulate(std::execution::par, begin, end, 0.0f);
        } catch (const std::exception&) {
            // Fallback to sequential
            return std::accumulate(begin, end, 0.0f);
        }
    }
}

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    RandomMode mode;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed), mode(RandomMode::Deterministic) {}
    Impl(std::mt19937_64 gen, uint64_t seed) : gen(std::move(gen)), currentSeed(seed), mode(RandomMode::MersenneTwister) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)), mode_(RandomMode::Deterministic) {
    if (!isValidSeed(seed)) {
        throw std::invalid_argument("Invalid seed: seed must not be 0 or UINT64_MAX");
    }
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())), mode_(RandomMode::PseudoRandom) {
}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(other.pImpl), mode_(other.mode_) {
    other.pImpl = nullptr;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        mode_ = other.mode_;
        other.pImpl = nullptr;
        // Notify observers of this generator
        notifyObservers();
    }
    return *this;
}

Result<std::unique_ptr<RandomGenerator>> RandomGenerator::create(uint64_t seed, RandomMode mode) {
    if (!isValidSeed(seed)) {
        return std::unexpected("Invalid seed: seed must not be 0 or UINT64_MAX");
    }
    
    auto generator = std::make_unique<RandomGenerator>(seed);
    generator->setMode(mode);
    
    return generator;
}

Result<std::unique_ptr<RandomGenerator>> RandomGenerator::createFromSeed(const std::string& seedString) {
    try {
        // Simple hash function for string to uint64
        uint64_t seed = 0;
        for (char c : seedString) {
            seed = seed * 31 + static_cast<uint64_t>(c);
        }
        if (seed == 0 || seed == UINT64_MAX) {
            seed = 42; // Default seed
        }
        
        return create(seed);
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Failed to create generator from seed: ") + e.what());
    }
}

void RandomGenerator::seed(uint64_t seed) {
    if (!isValidSeed(seed)) {
        throw std::invalid_argument("Invalid seed: seed must not be 0 or UINT64_MAX");
    }
    
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
    notifyObservers();
}

uint64_t RandomGenerator::getSeed() const noexcept {
    return pImpl->currentSeed;
}

void RandomGenerator::setMode(RandomMode mode) {
    mode_ = mode;
    notifyObservers();
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (!isValidRange(min, max)) {
        throw std::invalid_argument("Invalid range: min must be <= max and both must be > 0");
    }
    return optimizedUniform(pImpl->gen, min, max);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (!isValidRange(min, max)) {
        throw std::invalid_argument("Invalid range: min must be <= max and both must be > 0");
    }
    return optimizedUniform(pImpl->gen, min, max);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("Invalid range: min must be < max");
    }
    return optimizedUniform(pImpl->gen, min, max);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("Invalid range: min must be < max");
    }
    return optimizedUniform(pImpl->gen, min, max);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev < 0.0) {
        throw std::invalid_argument("Standard deviation must be >= 0");
    }
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev < 0.0f) {
        throw std::invalid_argument("Standard deviation must be >= 0");
    }
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Probability must be in [0, 1]");
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("Lambda must be >= 0");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("Lambda must be > 0");
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
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!data || size == 0) {
        return;
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
