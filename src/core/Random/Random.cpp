#include "Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    uint64_t totalCalls;
    uint64_t cacheHits;
    mutable std::mutex mutex;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed), totalCalls(0), cacheHits(0) {}
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
    pImpl->totalCalls = 0;
    pImpl->cacheHits = 0;
}

uint64_t RandomGenerator::getSeed() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSeed;
}

// Thread-local storage implementation
thread_local RandomGenerator* RandomGenerator::threadLocalGenerator = nullptr;

RandomGenerator& RandomGenerator::getThreadLocalGenerator() {
    if (!threadLocalGenerator) {
        threadLocalGenerator = new RandomGenerator(std::random_device{}());
    }
    return *threadLocalGenerator;
}

void RandomGenerator::setThreadLocalGenerator(RandomGenerator& generator) {
    if (threadLocalGenerator) {
        delete threadLocalGenerator;
    }
    threadLocalGenerator = &generator;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    // Fast path for common cases
    if (min == 0 && max == RAND_MAX) {
        pImpl->cacheHits++;
        return static_cast<uint32_t>(pImpl->gen() & RAND_MAX);
    }
    
    if (min == max) {
        return min;
    }
    
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    
    // Use efficient algorithm for small ranges
    if (max - min < 1024) {
        pImpl->cacheHits++;
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    }
    
    // Use rejection sampling for larger ranges
    return std::uniform_int_distribution<uint32_t>(min, max)(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    if (min == max) {
        return min;
    }
    
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    
    return std::uniform_int_distribution<uint64_t>(min, max)(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    
    // Fast path for common ranges
    if (min == 0.0 && max == 1.0) {
        pImpl->cacheHits++;
        return static_cast<double>(pImpl->gen()) / pImpl->max();
    }
    
    return std::uniform_real_distribution<double>(min, max)(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    
    return std::uniform_real_distribution<float>(min, max)(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    return std::normal_distribution<double>(mean, stddev)(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    return std::normal_distribution<float>(mean, stddev)(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    // Use fast algorithm for common probabilities
    if (p == 0.0) return false;
    if (p == 1.0) return true;
    if (p == 0.5) {
        pImpl->cacheHits++;
        return (pImpl->gen() & 1) != 0;
    }
    
    return std::bernoulli_distribution(p)(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    // Use optimized algorithm for small lambda values
    if (lambda <= 10.0) {
        pImpl->cacheHits++;
        uint32_t k = 0;
        double p = exp(-lambda);
        double sum = p;
        while (sum < 1.0) {
            k++;
            p *= lambda / k;
            sum += p;
        }
        return k;
    }
    
    return std::poisson_distribution<uint32_t>(lambda)(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    return std::exponential_distribution<double>(lambda)(pImpl->gen);
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    return uniformInt(0, 1);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    if (n == 1) {
        pImpl->cacheHits++;
        return 0;
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls++;
    
    if (size == 0) return;
    
    // Optimize for cache line size (typically 64 bytes)
    constexpr size_t cacheLineSize = 64;
    size_t numLines = (size + cacheLineSize - 1) / cacheLineSize;
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    
    // Use bulk random generation when possible
    for (size_t i = 0; i < size; i += cacheLineSize) {
        size_t remaining = std::min(cacheLineSize, size - i);
        size_t wordsToFill = remaining / sizeof(uint64_t);
        
        // Fill with 64-bit chunks for better performance
        uint64_t* chunks = reinterpret_cast<uint64_t*>(bytes + i);
        for (size_t j = 0; j < wordsToFill; ++j) {
            chunks[j] = pImpl->gen();
        }
        
        // Fill remaining bytes
        size_t bytesFilled = wordsToFill * sizeof(uint64_t);
        for (size_t j = 0; j < remaining - bytesFilled; ++j) {
            bytes[i + bytesFilled + j] = static_cast<uint8_t>(pImpl->gen() & 0xFF);
        }
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

RandomGenerator::PerformanceStats RandomGenerator::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    PerformanceStats stats;
    stats.calls = pImpl->totalCalls;
    stats.cacheHits = pImpl->cacheHits;
    return stats;
}

void RandomGenerator::resetPerformanceStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCalls = 0;
    pImpl->cacheHits = 0;
}

void RandomGenerator::seedThreadSafe(uint64_t seed) {
    seed(seed);
}

} // namespace nlm