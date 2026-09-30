#include "Random.hpp"
#include "../core/Exceptions.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {
    NLM_LOG_INFO("RandomGenerator initialized with seed: " + std::to_string(seed));
}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {
    NLM_LOG_INFO("RandomGenerator initialized with random device");
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
    NLM_LOG_INFO("RandomGenerator reseeded to: " + std::to_string(seed));
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid range: min=" + std::to_string(min) + " must be <= max=" + std::to_string(max),
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_uniform_int");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid range: min=" + std::to_string(min) + " must be <= max=" + std::to_string(max),
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_uniform_int64");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid range: min=" + std::to_string(min) + " must be < max=" + std::to_string(max),
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_uniform_real");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid range: min=" + std::to_string(min) + " must be < max=" + std::to_string(max),
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_uniform_real_float");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev < 0.0) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid standard deviation: " + std::to_string(stddev) + " must be >= 0",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_normal");
    }
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev < 0.0f) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid standard deviation: " + std::to_string(stddev) + " must be >= 0",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_normal_float");
    }
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid probability: " + std::to_string(p) + " must be in [0, 1]",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_bernoulli");
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda < 0.0) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid lambda for Poisson: " + std::to_string(lambda) + " must be >= 0",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_poisson");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0.0) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Invalid lambda for exponential: " + std::to_string(lambda) + " must be > 0",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                         "random_exponential");
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
        NLM_THROW_AND_LOG(NLMMemoryException, 
                         "Cannot pick from zero items",
                         NLMException::Severity::CRITICAL,
                         static_cast<int>(NLMMemoryException::MemoryError::OUT_OF_MEMORY),
                         "random_pick_index");
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (data == nullptr && size > 0) {
        NLM_THROW_AND_LOG(NLMValidationException, 
                         "Cannot fill bytes with null pointer and size > 0",
                         NLMException::Severity::ERROR,
                         static_cast<int>(NLMValidationException::ValidationError::NULL_POINTER),
                         "random_fill_bytes");
    }
    
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
