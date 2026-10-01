#include "Random.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    std::mutex mutex;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
    
    // Seed from system randomness sources for better entropy
    static uint64_t seedFromSystem() {
        uint64_t seed;
        #ifdef __linux__
        std::ifstream urandom("/dev/urandom");
        if (urandom.read(reinterpret_cast<char*>(&seed), sizeof(seed)).good()) {
            return seed;
        }
        #endif
        // Fallback to random_device
        static std::random_device rd;
        return static_cast<uint64_t>(rd()) << 32 | static_cast<uint64_t>(rd());
    }
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl(Impl::seedFromSystem())) {}

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
}

uint64_t RandomGenerator::getSeed() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
    
    // Use direct bit generation for better performance
    // Generate 64-bit chunks and split into bytes
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t remaining = size;
    uint8_t* current = bytes;
    
    while (remaining >= sizeof(uint64_t)) {
        uint64_t chunk = pImpl->gen();
        std::memcpy(current, &chunk, sizeof(uint64_t));
        current += sizeof(uint64_t);
        remaining -= sizeof(uint64_t);
    }
    
    // Handle remaining bytes
    if (remaining > 0) {
        uint64_t chunk = pImpl->gen();
        std::memcpy(current, &chunk, remaining);
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
