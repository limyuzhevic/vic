#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <random>
#include <chrono>
#include <random_device>

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

void RandomGenerator::seed(uint64_t seed) {
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
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
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

// Additional distributions
uint32_t RandomGenerator::gamma(double shape, double scale) {
    if (shape <= 0.0 || scale <= 0.0) {
        throw std::invalid_argument("Gamma distribution parameters must be positive");
    }
    
    // Marsaglia and Tsang method for Gamma distribution
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / std::sqrt(9.0 * d);
    
    while (true) {
        double x = engine()();
        if (x <= 0.0) continue;  // Reject negative values
        
        double v = 1.0 - c * std::log(x);
        if (v <= 0.0) continue;
        
        double y = engine()();
        if (y <= 1.0 - 0.231 * (v * v) * (v - 4.0) / 3.0) {
            return static_cast<uint32_t>(scale * d * v);
        }
    }
}

uint32_t RandomGenerator::chiSquare(uint32_t degreesOfFreedom) {
    if (degreesOfFreedom == 0) {
        throw std::invalid_argument("Chi-square degrees of freedom must be positive");
    }
    
    double sum = 0.0;
    for (uint32_t i = 0; i < degreesOfFreedom; ++i) {
        double z = normal(0.0, 1.0);
        sum += z * z;
    }
    return static_cast<uint32_t>(std::round(sum));
}

uint32_t RandomGenerator::exponential(float lambda) {
    if (lambda <= 0.0f) {
        throw std::invalid_argument("Exponential lambda must be positive");
    }
    
    std::exponential_distribution<float> dist(lambda);
    return static_cast<uint32_t>(dist(pImpl->gen));
}

float RandomGenerator::gamma(float shape, float scale) {
    if (shape <= 0.0f || scale <= 0.0f) {
        throw std::invalid_argument("Gamma distribution parameters must be positive");
    }
    
    // Use double version for better accuracy
    double result = gamma(static_cast<double>(shape), static_cast<double>(scale));
    return static_cast<float>(result);
}

float RandomGenerator::chiSquare(uint32_t degreesOfFreedom) {
    if (degreesOfFreedom == 0) {
        throw std::invalid_argument("Chi-square degrees of freedom must be positive");
    }
    
    double sum = 0.0;
    for (uint32_t i = 0; i < degreesOfFreedom; ++i) {
        double z = normal(0.0, 1.0);
        sum += z * z;
    }
    return static_cast<float>(sum);
}

float RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("Exponential lambda must be positive");
    }
    
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

// Thread-safe random number generation
uint32_t RandomGenerator::random32() {
    return uniformInt(0, UINT32_MAX);
}

uint64_t RandomGenerator::random64() {
    return uniformInt(0, ULLONG_MAX);
}

// Fisher-Yates shuffle algorithm
void RandomGenerator::shuffle(uint32_t* array, uint64_t n) {
    if (!array || n <= 1) return;
    
    for (uint64_t i = n - 1; i > 0; --i) {
        uint64_t j = uniformInt(0, i);
        std::swap(array[i], array[j]);
    }
}

void RandomGenerator::shuffle(float* array, uint64_t n) {
    if (!array || n <= 1) return;
    
    for (uint64_t i = n - 1; i > 0; --i) {
        uint64_t j = uniformInt(0, i);
        std::swap(array[i], array[j]);
    }
}

// Batch random number generation for performance
void RandomGenerator::fillUniform32(uint32_t* buffer, size_t count, uint32_t min, uint32_t max) {
    if (!buffer || count == 0) return;
    
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    
    std::uniform_int_distribution<uint32_t> dist(min, max);
    for (size_t i = 0; i < count; ++i) {
        buffer[i] = dist(pImpl->gen);
    }
}

void RandomGenerator::fillUniformFloat(float* buffer, size_t count, float min, float max) {
    if (!buffer || count == 0) return;
    
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    
    std::uniform_real_distribution<float> dist(min, max);
    for (size_t i = 0; i < count; ++i) {
        buffer[i] = dist(pImpl->gen);
    }
}

// Improved seed management
void RandomGenerator::seedFromSystem() {
    // Use multiple entropy sources for better seed
    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed[0]; // Use first seed as current seed
}

void RandomGenerator::seedFromTime() {
    auto now = std::chrono::system_clock::now();
    auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    // Mix with some entropy from random device
    std::random_device rd;
    uint64_t seed = static_cast<uint64_t>(timeSinceEpoch) ^ 
                   (static_cast<uint64_t>(rd()) << 32) ^ 
                   (static_cast<uint64_t>(rd()) << 16);
    
    seed(seed);
}
