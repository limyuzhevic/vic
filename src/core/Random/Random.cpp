#include "Random.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {}

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

// Validation helper functions
namespace {
    bool validateSeedValue(uint64_t seed) {
        // Seed values should be reasonable (not zero or extremely large)
        // Zero seed can lead to predictable sequences
        // Extremely large values might cause overflow in some engines
        const uint64_t MIN_SEED = 1;
        const uint64_t MAX_SEED = std::numeric_limits<uint64_t>::max();
        return seed >= MIN_SEED && seed <= MAX_SEED;
    }
    
    bool validateInt64Range(int64_t min, int64_t max) {
        // For 64-bit integer distributions, ensure range is valid
        return min <= max;
    }
    
    bool validateRealRange(double min, double max) {
        // Real ranges should be finite and ordered
        return min < max && !std::isinf(min) && !std::isinf(max) && 
               !std::isnan(min) && !std::isnan(max);
    }
    
    bool validateFloatRange(float min, float max) {
        // Float ranges should be finite and ordered
        return min < max && !std::isinf(min) && !std::isinf(max) && 
               !std::isnan(min) && !std::isnan(max);
    }
    
    bool validateNormalStddev(double stddev) {
        // Standard deviation should be positive
        return stddev > 0.0 && !std::isinf(stddev) && !std::isnan(stddev);
    }
    
    bool validateNormalStddev(float stddev) {
        // Standard deviation should be positive
        return stddev > 0.0f && !std::isinf(stddev) && !std::isnan(stddev);
    }
    
    bool validateBernoulliProbability(double p) {
        // Bernoulli probability should be between 0 and 1
        return p >= 0.0 && p <= 1.0 && !std::isinf(p) && !std::isnan(p);
    }
    
    bool validatePoissonLambda(double lambda) {
        // Poisson rate parameter should be positive
        return lambda > 0.0 && !std::isinf(lambda) && !std::isnan(lambda);
    }
    
    bool validateExponentialLambda(double lambda) {
        // Exponential rate parameter should be positive
        return lambda > 0.0 && !std::isinf(lambda) && !std::isnan(lambda);
    }
    
    bool validateContainerSize(size_t n) {
        // Container size should not be zero for pick operations
        return n > 0;
    }
} // namespace anonymous

void RandomGenerator::seed(uint64_t seed) {
    // Validate seed before applying
    if (validateSeedValue(seed)) {
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
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
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

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

double RandomGenerator::uniformReal(double min, double max) {
    // Validate range parameters
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    // Validate range parameters
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
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
