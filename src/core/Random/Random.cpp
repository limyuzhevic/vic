#include "Random.hpp"
#include "ErrorHandling.hpp"
#include <stdexcept>
#include <cstring>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) {
    // Validate seed
    if (!Validation::isValidSeed(seed)) {
        ExceptionManager::throwError<RandomError>(
            "Invalid seed for random generator", RandomError::Type::InvalidSeed);
    }
    
    try {
        pImpl = new Impl(seed);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to create random generator with seed " + std::to_string(seed) + ": " + e.what(),
            RandomError::Type::InvalidSeed);
    }
}

RandomGenerator::RandomGenerator() {
    try {
        // Use a deterministic seed from system if available, but with fallback
        std::random_device rd;
        uint64_t seed = rd();
        
        // Additional entropy from system time
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        seed ^= static_cast<uint64_t>(timestamp) ^ (static_cast<uint64_t>(std::rand()) << 32);
        
        pImpl = new Impl(seed);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to create random generator with default seed: " + std::string(e.what()),
            RandomError::Type::InvalidSeed);
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
    // Validate seed
    if (!Validation::isValidSeed(seed)) {
        ExceptionManager::throwError<RandomError>(
            "Invalid seed for random generator: " + std::to_string(seed),
            RandomError::Type::InvalidSeed);
    }
    
    try {
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to seed random generator: " + std::string(e.what()),
            RandomError::Type::InvalidSeed);
    }
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    // Validate parameters
    if (min > max) {
        ExceptionManager::throwError<RandomError>(
            "min must be <= max for uniformInt (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")",
            RandomError::Type::InvalidRange);
    }
    if (min == max) {
        return min;
    }
    
    try {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate uniform integer in range [" + std::to_string(min) + ", " + std::to_string(max) + "]: " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    // Validate parameters
    if (min > max) {
        ExceptionManager::throwError<RandomError>(
            "min must be <= max for uniformInt (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")",
            RandomError::Type::InvalidRange);
    }
    if (min == max) {
        return min;
    }
    
    try {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate uniform integer in range [" + std::to_string(min) + ", " + std::to_string(max) + "]: " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    // Validate parameters
    if (min >= max) {
        ExceptionManager::throwError<RandomError>(
            "min must be < max for uniformReal (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")",
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate uniform real in range [" + std::to_string(min) + ", " + std::to_string(max) + "]: " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    // Validate parameters
    if (min >= max) {
        ExceptionManager::throwError<RandomError>(
            "min must be < max for uniformReal (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")",
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate uniform real in range [" + std::to_string(min) + ", " + std::to_string(max) + "]: " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    // Validate parameters
    if (!Validation::isValidMembranePotential(static_cast<float>(mean))) {
        ExceptionManager::throwError<RandomError>(
            "Invalid mean for normal distribution: " + std::to_string(mean),
            RandomError::Type::InvalidRange);
    }
    if (!Validation::isValidMembranePotential(static_cast<float>(stddev)) || stddev <= 0.0) {
        ExceptionManager::throwError<RandomError>(
            "Invalid standard deviation for normal distribution: " + std::to_string(stddev),
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate normal distribution with mean=" + std::to_string(mean) + 
            ", stddev=" + std::to_string(stddev) + ": " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    // Validate parameters
    if (!Validation::isValidMembranePotential(mean)) {
        ExceptionManager::throwError<RandomError>(
            "Invalid mean for normal distribution: " + std::to_string(mean),
            RandomError::Type::InvalidRange);
    }
    if (!Validation::isValidMembranePotential(stddev) || stddev <= 0.0f) {
        ExceptionManager::throwError<RandomError>(
            "Invalid standard deviation for normal distribution: " + std::to_string(stddev),
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate normal distribution with mean=" + std::to_string(mean) + 
            ", stddev=" + std::to_string(stddev) + ": " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

bool RandomGenerator::bernoulli(double p) {
    // Validate parameter
    if (!Validation::isValidMembranePotential(static_cast<float>(p)) || p < 0.0 || p > 1.0) {
        ExceptionManager::throwError<RandomError>(
            "Invalid probability for bernoulli distribution: " + std::to_string(p) + ". Must be in [0, 1]",
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate bernoulli distribution with p=" + std::to_string(p) + ": " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    // Validate parameter
    if (!Validation::isValidMembranePotential(static_cast<float>(lambda)) || lambda < 0.0) {
        ExceptionManager::throwError<RandomError>(
            "Invalid lambda for poisson distribution: " + std::to_string(lambda) + ". Must be >= 0",
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate poisson distribution with lambda=" + std::to_string(lambda) + ": " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

double RandomGenerator::exponential(double lambda) {
    // Validate parameter
    if (!Validation::isValidMembranePotential(static_cast<float>(lambda)) || lambda <= 0.0) {
        ExceptionManager::throwError<RandomError>(
            "Invalid lambda for exponential distribution: " + std::to_string(lambda) + ". Must be > 0",
            RandomError::Type::InvalidRange);
    }
    
    try {
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to generate exponential distribution with lambda=" + std::to_string(lambda) + ": " + e.what(),
            RandomError::Type::OutOfBounds);
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
        ExceptionManager::throwError<RandomError>(
            "Cannot pick random index from zero items",
            RandomError::Type::InvalidRange);
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    // Validate parameters
    if (!data) {
        ExceptionManager::throwError<MemoryError>(
            "Cannot fill null memory buffer",
            MemoryError::Type::NullPointerDereference);
    }
    if (size == 0) {
        return;  // Nothing to fill
    }
    
    try {
        uint8_t* bytes = static_cast<uint8_t*>(data);
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to fill " + std::to_string(size) + " random bytes: " + e.what(),
            RandomError::Type::OutOfBounds);
    }
}

std::mt19937_64& RandomGenerator::engine() {
    try {
        if (!pImpl) {
            ExceptionManager::throwError<RandomError>(
                "Random generator engine not initialized",
                RandomError::Type::MemoryError);
        }
        return pImpl->gen;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to access random generator engine: " + std::string(e.what()),
            RandomError::Type::MemoryError);
    }
}

const std::mt19937_64& RandomGenerator::engine() const {
    try {
        if (!pImpl) {
            ExceptionManager::throwError<RandomError>(
                "Random generator engine not initialized",
                RandomError::Type::MemoryError);
        }
        return pImpl->gen;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<RandomError>(
            "Failed to access random generator engine: " + std::string(e.what()),
            RandomError::Type::MemoryError);
    }
}

} // namespace nlm
