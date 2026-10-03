#include "Random.hpp"
#include <stdexcept>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    ErrorHandler errorHandler;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {
        validateSeed(seed);
    }
    
    void validateSeed(uint64_t seed) {
        NLM_VALIDATE(seed != 0, "Random seed cannot be zero");
        NLM_VALIDATE(seed != UINT64_MAX, "Random seed cannot be UINT64_MAX (causes modulo issues)");
    }
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
    try {
        pImpl->validateSeed(seed);
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to seed random generator: " + std::string(e.what()), "Random"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
    }
}

uint64_t RandomGenerator::getSeed() const {
    try {
        return pImpl->currentSeed;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return 0;
    }
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    try {
        NLM_VALIDATE(min <= max, "min must be <= max");
        
        if (min == max) {
            return min;
        }
        
        pImpl->errorHandler.clear();
        std::uniform_int_distribution<uint32_t> dist(min, max);
        uint32_t result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return min; // Return min on error for backward compatibility
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return min;
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    try {
        NLM_VALIDATE(min <= max, "min must be <= max");
        
        if (min == max) {
            return min;
        }
        
        pImpl->errorHandler.clear();
        std::uniform_int_distribution<uint64_t> dist(min, max);
        uint64_t result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return min;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return min;
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    try {
        NLM_VALIDATE(min < max, "min must be < max");
        
        pImpl->errorHandler.clear();
        std::uniform_real_distribution<double> dist(min, max);
        double result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return min;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return min;
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    try {
        NLM_VALIDATE(min < max, "min must be < max");
        
        pImpl->errorHandler.clear();
        std::uniform_real_distribution<float> dist(min, max);
        float result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return min;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return min;
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    try {
        NLM_VALIDATE(stddev >= 0.0, "stddev must be >= 0");
        
        pImpl->errorHandler.clear();
        std::normal_distribution<double> dist(mean, stddev);
        double result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return mean;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return mean;
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    try {
        NLM_VALIDATE(stddev >= 0.0f, "stddev must be >= 0");
        
        pImpl->errorHandler.clear();
        std::normal_distribution<float> dist(mean, stddev);
        float result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return mean;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return mean;
    }
}

bool RandomGenerator::bernoulli(double p) {
    try {
        NLM_VALIDATE(p >= 0.0 && p <= 1.0, "p must be in range [0, 1]");
        
        pImpl->errorHandler.clear();
        std::bernoulli_distribution dist(p);
        bool result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return false;
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    try {
        NLM_VALIDATE(lambda >= 0.0, "lambda must be >= 0");
        
        pImpl->errorHandler.clear();
        std::poisson_distribution<uint32_t> dist(lambda);
        uint32_t result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return 0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return 0;
    }
}

double RandomGenerator::exponential(double lambda) {
    try {
        NLM_VALIDATE(lambda > 0.0, "lambda must be > 0");
        
        pImpl->errorHandler.clear();
        std::exponential_distribution<double> dist(lambda);
        double result = dist(pImpl->gen);
        
        NLM_THROW_ON_ERROR();
        return result;
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return 0.0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return 0.0;
    }
}

bool RandomGenerator::randomBool() {
    try {
        return bernoulli(0.5);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return false;
    }
}

uint32_t RandomGenerator::randomBit() {
    try {
        return uniformInt(0, 1);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return 0;
    }
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    try {
        NLM_VALIDATE(n > 0, "Cannot pick from zero items");
        
        return uniformInt(0, n - 1);
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(e.what(), "Random"));
        return 0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
        return 0;
    }
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    try {
        NLM_VALIDATE(data != nullptr, "Data pointer cannot be null");
        NLM_VALIDATE(size > 0, "Size must be positive");
        
        uint8_t* bytes = static_cast<uint8_t*>(data);
        pImpl->errorHandler.clear();
        
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
            if (pImpl->errorHandler.hasErrors()) {
                break;
            }
        }
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to fill bytes: " + std::string(e.what()), "Random"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Random"));
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
