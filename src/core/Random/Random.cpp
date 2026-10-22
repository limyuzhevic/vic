#include "Random.hpp"
#include <stdexcept>
#include <chrono>
#include <cstring>
#include <random>
#include <functional>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    bool seededWithRandomDevice;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed), seededWithRandomDevice(false) {}
    Impl() {
        // Use high-resolution time for better default seeding
        auto now = std::chrono::high_resolution_clock::now();
        auto timeSinceEpoch = now.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(timeSinceEpoch);
        
        // Combine time-based seed with random device for better entropy
        uint64_t timeSeed = static_cast<uint64_t>(nanoseconds.count());
        std::seed_seq seedSequence{timeSeed, static_cast<uint64_t>(std::random_device{}())};
        gen.seed(seedSequence);
        currentSeed = timeSeed; // Store time-based seed
        seededWithRandomDevice = true;
    }
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl()) {}

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
        NLM_LOG_WARNING("Seeding random generator with zero - this may produce poor random sequences");
    }
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
    pImpl->seededWithRandomDevice = false;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max: " + std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max: " + std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max: " + std::to_string(min) + " >= " + std::to_string(max));
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max: " + std::to_string(min) + " >= " + std::to_string(max));
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev < 0.0) {
        throw std::invalid_argument("stddev must be >= 0: " + std::to_string(stddev));
    }
    if (stddev == 0.0) {
        return mean;
    }
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev < 0.0f) {
        throw std::invalid_argument("stddev must be >= 0: " + std::to_string(stddev));
    }
    if (stddev == 0.0f) {
        return mean;
    }
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("probability p must be in [0, 1]: " + std::to_string(p));
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0: " + std::to_string(lambda));
    }
    if (lambda > 1000.0) { // Large lambda may cause performance issues
        NLM_LOG_WARNING("Large lambda value (" + std::to_string(lambda) + ") for Poisson distribution - may impact performance");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be > 0: " + std::to_string(lambda));
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
    
    // Use a more efficient approach for large buffers
    uint8_t* bytes = static_cast<uint8_t*>(data);
    size_t remaining = size;
    
    while (remaining > 0) {
        // Generate 4-byte chunks and copy
        uint32_t chunk = uniformInt(0, UINT32_MAX);
        size_t copySize = std::min(remaining, sizeof(chunk));
        
        std::memcpy(bytes, &chunk, copySize);
        bytes += copySize;
        remaining -= copySize;
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

// Additional convenience methods for neural simulations

double RandomGenerator::gaussian(double mean, double stddev) {
    return normal(mean, stddev);
}

float RandomGenerator::gaussianFloat(float mean, float stddev) {
    return normal(mean, stddev);
}

double RandomGenerator::logNormal(double mean, double stddev) {
    if (stddev < 0.0) {
        throw std::invalid_argument("stddev must be >= 0 for log-normal distribution");
    }
    double normalVal = normal(0.0, stddev);
    return std::exp(mean + normalVal);
}

double RandomGenerator::chiSquared(unsigned int degreesOfFreedom) {
    if (degreesOfFreedom == 0) {
        throw std::invalid_argument("Chi-squared requires at least 1 degree of freedom");
    }
    
    double sum = 0.0;
    for (unsigned int i = 0; i < degreesOfFreedom; ++i) {
        double normalVal = normal(0.0, 1.0);
        sum += normalVal * normalVal;
    }
    return sum;
}

uint32_t RandomGenerator::uniformInt(uint32_t max) {
    return uniformInt(0, max);
}

uint64_t RandomGenerator::uniformInt(uint64_t max) {
    return uniformInt(0, max);
}

float RandomGenerator::uniformFloat(float max) {
    return uniformReal(0.0f, max);
}

double RandomGenerator::uniformDouble(double max) {
    return uniformReal(0.0, max);
}

bool RandomGenerator::wasSeededWithRandomDevice() const {
    return pImpl->seededWithRandomDevice;
}

} // namespace nlm
