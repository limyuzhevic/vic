#include "Random.hpp"
#include <stdexcept>
#include <chrono>
#include <random>
#include <algorithm>
#include <array>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    std::mt19937_64 seedGen;  // For generating random seeds
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed), seedGen(std::random_device{}()) {}
    
    Impl() : gen(std::random_device{}()), currentSeed(gen()), seedGen(std::random_device{}()) {}
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

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

// Advanced seeding methods
void RandomGenerator::seedFromSystem() {
    // Seed from system time, hardware entropy, and process info
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    std::array<uint64_t, 4> seedData {
        static_cast<uint64_t>(millis),
        static_cast<uint64_t>(std::random_device{}()),
        pImpl->currentSeed ^ 0xDEADBEEF,
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&seedData))
    };
    
    uint64_t combinedSeed = 0;
    for (uint64_t seed : seedData) {
        combinedSeed ^= seed;
        combinedSeed = combinedSeed * 0x9e3779b97f4a7c15ULL + 0xbf58476d1ce4e5b0ULL;
    }
    
    seed(combinedSeed);
}

void RandomGenerator::seedFromString(const std::string& str) {
    if (str.empty()) {
        seedFromSystem();
        return;
    }
    
    // Simple deterministic hash of string
    uint64_t hash = 0xcbf29ce484222325ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 0x100000001b3ULL;
    }
    
    // Add some entropy from process and time
    auto now = std::chrono::high_resolution_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    hash ^= (millis & 0xFFFFFFFF);
    hash ^= (millis >> 32) & 0xFFFFFFFF;
    
    seed(hash);
}

// Enhanced distribution methods
float RandomGenerator::triangle(float a, float b, float c) {
    // Triangle distribution
    float u1 = uniformReal(0.0f, 1.0f);
    float u2 = uniformReal(0.0f, 1.0f);
    
    if ((a + b) < c) {
        // Mode is at c
        return a + (c - a) * std::sqrt(u1);
    } else {
        // Mode is at b
        return a + (b - a) * u1 + (c - b) * u2;
    }
}

float RandomGenerator::beta(float alpha, float beta) {
    // Simple beta distribution using rejection sampling
    float x, y;
    float alphaMinusOne = alpha - 1.0f;
    float betaMinusOne = beta - 1.0f;
    
    while (true) {
        x = uniformReal(0.0f, 1.0f);
        y = uniformReal(0.0f, 1.0f);
        
        float xPow = std::pow(x, alphaMinusOne);
        float yPow = std::pow(1.0f - x, betaMinusOne);
        
        if (y <= xPow * yPow) {
            return x;
        }
    }
}

float RandomGenerator::gamma(float shape, float scale) {
    // Marsaglia and Tsang's method for gamma distribution
    if (shape <= 0 || scale <= 0) {
        throw std::invalid_argument("Shape and scale must be positive");
    }
    
    if (shape == 1.0f) {
        // Exponential distribution
        return uniformReal(0.0f, 1.0f) * scale;
    }
    
    float d = shape - 1.0f / 3.0f;
    float c = 1.0f / std::sqrt(9.0f * d);
    
    while (true) {
        float x = normal(0.0f, 1.0f);
        float v = std::pow(1.0f + c * x, 3.0f);
        
        if (v > 0) {
            float u = uniformReal(0.0f, 1.0f);
            if (std::log(u) < 0.5 * x + d * (1.0f - v + std::log(v))) {
                return scale * v;
            }
        }
    }
}

bool RandomGenerator::binomial(int n, float p) {
    if (n < 0) {
        throw std::invalid_argument("n must be non-negative");
    }
    if (p < 0 || p > 1) {
        throw std::invalid_argument("p must be in [0, 1]");
    }
    
    // Use optimized implementation based on n and p
    if (n > 1000 || p < 0.1f || p > 0.9f) {
        // Normal approximation for large n or extreme probabilities
        float mean = static_cast<float>(n) * p;
        float stddev = std::sqrt(static_cast<float>(n) * p * (1.0f - p));
        float z = normal(0.0f, 1.0f);
        int result = static_cast<int>(mean + z * stddev + 0.5f);
        return std::clamp(result, 0, n);
    } else {
        // Direct summation for small n and moderate probabilities
        int count = 0;
        for (int i = 0; i < n; ++i) {
            if (bernoulli(p)) {
                ++count;
            }
        }
        return count;
    }
}

void RandomGenerator::shuffle(std::vector<Neuron*>& neurons) {
    std::shuffle(neurons.begin(), neurons.end(), engine());
}

void RandomGenerator::shuffle(std::vector<Synapse*>& synapses) {
    std::shuffle(synapses.begin(), synapses.end(), engine());
}

} // namespace nlm
