#include "Random.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
    if (seed == 0) {
        NLM_LOG_WARN("Initializing RandomGenerator with seed 0 (may affect randomness)");
    }
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {
    try {
        uint64_t autoSeed = std::random_device{}();
        if (autoSeed == 0) {
            NLM_LOG_WARN("std::random_device returned 0, using fallback seed");
            pImpl->gen.seed(123456789);
            pImpl->currentSeed = 123456789;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize random device: " + std::string(e.what();
        pImpl->gen.seed(42);  // Fallback to deterministic seed
        pImpl->currentSeed = 42;
    }
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
    try {
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
        NLM_LOG_DEBUG("Random generator seeded with: " + std::to_string(seed));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to seed random generator: " + std::string(e.what();
    }
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
    try {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate uniform int: " + std::string(e.what();
        return min; // Fallback
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    try {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate uniform int64: " + std::string(e.what();
        return min; // Fallback
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    try {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate uniform real: " + std::string(e.what();
        return (min + max) / 2.0; // Fallback to midpoint
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    try {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate uniform real32: " + std::string(e.what();
        return (min + max) / 2.0f; // Fallback to midpoint
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    try {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate normal distribution: " + std::string(e.what();
        return mean; // Fallback
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    try {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate normal distribution float: " + std::string(e.what();
        return mean; // Fallback
    }
}

bool RandomGenerator::bernoulli(double p) {
    try {
        // Validate probability
        if (p < 0.0 || p > 1.0) {
            NLM_LOG_WARN("Bernoulli probability " + std::to_string(p) + " out of range [0,1], clamping");
            p = std::max(0.0, std::min(1.0, p));
        }
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate Bernoulli distribution: " + std::string(e.what();
        return p >= 0.5; // Fallback based on probability
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    try {
        if (lambda < 0.0) {
            NLM_LOG_WARN("Poisson lambda " + std::to_string(lambda) + " negative, using absolute value");
            lambda = std::abs(lambda);
        }
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate Poisson distribution: " + std::string(e.what();
        return 0; // Fallback
    }
}

double RandomGenerator::exponential(double lambda) {
    try {
        if (lambda <= 0.0) {
            NLM_LOG_WARN("Exponential lambda " + std::to_string(lambda) + " <= 0, using default value");
            lambda = 1.0;
        }
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate exponential distribution: " + std::string(e.what();
        return 1.0; // Fallback
    }
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
    if (!data) {
        NLM_LOG_ERROR("Cannot fill null bytes");
        return;
    }
    if (size == 0) {
        NLM_LOG_WARN("Attempting to fill zero bytes");
        return;
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    try {
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to fill bytes: " + std::string(e.what();
        // Try to fill with a simple pattern as last resort
        uint8_t pattern = 0xAA;
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = pattern++;
        }
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
