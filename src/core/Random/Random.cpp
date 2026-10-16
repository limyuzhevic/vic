#include "Random.hpp"
#include "../core/Error/Error.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) {
    if (seed == 0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Invalid random seed", std::to_string(seed));
    }
    
    try {
        pImpl = new Impl(seed);
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed for random generator", "");
    }
}

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
    if (seed == 0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Invalid random seed", std::to_string(seed));
    }
    
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "min must be <= max", std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    
    try {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating uniform integer: " + std::string(e.what()), "");
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "min must be <= max", std::to_string(min) + " > " + std::to_string(max));
    }
    if (min == max) {
        return min;
    }
    
    try {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating uniform integer: " + std::string(e.what()), "");
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "min must be < max", std::to_string(min) + " >= " + std::to_string(max));
    }
    
    try {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating uniform real: " + std::string(e.what()), "");
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "min must be < max", std::to_string(min) + " >= " + std::to_string(max));
    }
    
    try {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating uniform real: " + std::string(e.what()), "");
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev <= 0.0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "standard deviation must be > 0", std::to_string(stddev));
    }
    
    try {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating normal distribution: " + std::string(e.what()), "");
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev <= 0.0f) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "standard deviation must be > 0", std::to_string(stddev));
    }
    
    try {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating normal distribution: " + std::string(e.what()), "");
    }
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "probability must be between 0.0 and 1.0", std::to_string(p));
    }
    
    try {
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating bernoulli distribution: " + std::string(e.what()), "");
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "lambda must be >= 0.0", std::to_string(lambda));
    }
    
    try {
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating poisson distribution: " + std::string(e.what()), "");
    }
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "lambda must be > 0.0", std::to_string(lambda));
    }
    
    try {
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error generating exponential distribution: " + std::string(e.what()), "");
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
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Cannot pick from zero items", std::to_string(n));
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!data) {
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
            "Cannot fill null pointer", "");
    }
    if (size == 0) {
        return; // Nothing to fill
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    try {
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
        }
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error filling bytes: " + std::string(e.what()), "");
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
