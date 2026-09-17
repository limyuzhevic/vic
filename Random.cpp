#include "Random.hpp"
#include <stdexcept>
#include <cstring>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate memory for RandomGenerator");
    }
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate memory for RandomGenerator (random device)");
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
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
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
    try {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate uniform integer in range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "]: " + 
                                 e.what());
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max: " + std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    try {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate uniform integer in range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "]: " + 
                                 e.what());
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max: " + std::to_string(min) + " >= " + std::to_string(max));
    }
    try {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate uniform real in range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "]: " + 
                                 e.what());
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max: " + std::to_string(min) + " >= " + std::to_string(max));
    }
    try {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate uniform real in range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "]: " + 
                                 e.what());
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev < 0.0) {
        throw std::invalid_argument("stddev must be >= 0: " + std::to_string(stddev) + " < 0");
    }
    try {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate normal distribution with mean=" + 
                                 std::to_string(mean) + ", stddev=" + std::to_string(stddev) + ": " + 
                                 e.what());
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev < 0.0f) {
        throw std::invalid_argument("stddev must be >= 0: " + std::to_string(stddev) + " < 0");
    }
    try {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate normal distribution with mean=" + 
                                 std::to_string(mean) + ", stddev=" + std::to_string(stddev) + ": " + 
                                 e.what());
    }
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("p must be in [0, 1]: " + std::to_string(p) + " outside [0, 1]");
    }
    try {
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate Bernoulli distribution with p=" + 
                                 std::to_string(p) + ": " + e.what());
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0: " + std::to_string(lambda) + " < 0");
    }
    try {
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate Poisson distribution with lambda=" + 
                                 std::to_string(lambda) + ": " + e.what());
    }
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be > 0: " + std::to_string(lambda) + " <= 0");
    }
    try {
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to generate exponential distribution with lambda=" + 
                                 std::to_string(lambda) + ": " + e.what());
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
        throw std::invalid_argument("Cannot fill null data pointer");
    }
    if (size == 0) {
        return;
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        try {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to fill bytes at position " + std::to_string(i) + ": " + e.what());
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
