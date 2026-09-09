#include "Random.hpp"
#include <stdexcept>

namespace nlm {

// Implementation of deterministic random number generator
// Provides reproducible randomness for scientific simulations

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    // Initialize with given seed for reproducible results
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

// Create deterministic random generator with explicit seed
// Ensures reproducible experiments across runs
RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
}

// Create with time-based seed for non-deterministic use
// Use only for applications requiring different results each run
RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {
}

RandomGenerator::~RandomGenerator() = default;

// Move constructor - transfers ownership of implementation
RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator - transfers ownership
RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Set seed for reproducible random sequence
// Resets the internal generator state
void RandomGenerator::seed(uint64_t seed) {
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

// Get current seed
uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

// Generate uniform integer in inclusive range [min, max]
// Validates input parameters to prevent errors
uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

// Generate uniform uint64_t in inclusive range [min, max]
uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

// Generate uniform float in half-open range [min, max)
double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

// Generate uniform float in half-open range [min, max)
float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

// Generate normal distribution
// Mean and standard deviation must be non-negative
// For normal distribution, stddev >= 0 is required

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

// Bernoulli distribution for coin flips
bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

// Poisson distribution
uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0) {
        throw std::invalid_argument("lambda must be non-negative");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

// Exponential distribution
double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0) {
        throw std::invalid_argument("lambda must be positive");
    }
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

// Random boolean with equal probability
bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

// Random bit (0 or 1)
uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

// Pick random element from container
// Ensures container is not empty before selection
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

// Pick random index from 0 to n-1
uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(0, n - 1);
}

// Fill buffer with random bytes
void RandomGenerator::fillBytes(void* data, size_t size) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
    }
}

// Get reference to underlying engine for advanced usage
std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

// Template implementations
// Shuffle container elements using Fisher-Yates algorithm
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
