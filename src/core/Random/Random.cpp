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

void RandomGenerator::seed(uint64_t seed) {
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

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
    try {
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException seedEx(ErrorCode::InvalidArgument, std::string("Failed to seed random generator: ") + e.what(), context);
        nlm::ErrorHandling::handleException(seedEx);
        throw seedEx;
    }
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (min > max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "min must be <= max", ErrorCategory::System);
        }
        if (min == max) {
            return min;
        }
        std::uniform_int_distribution<uint32_t> dist(min, max);
        uint32_t result = dist(pImpl->gen);
        
        // Validate result is within expected range
        if (result < min || result > max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Random integer result out of range", ErrorCategory::System);
        }
        
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Uniform int error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (min > max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "min must be <= max", ErrorCategory::System);
        }
        if (min == max) {
            return min;
        }
        std::uniform_int_distribution<uint64_t> dist(min, max);
        uint64_t result = dist(pImpl->gen);
        
        // Validate result is within expected range
        if (result < min || result > max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Random integer result out of range", ErrorCategory::System);
        }
        
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Uniform int error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (min >= max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "min must be < max", ErrorCategory::System);
        }
        std::uniform_real_distribution<double> dist(min, max);
        double result = dist(pImpl->gen);
        
        // Validate result is within expected range
        if (result < min || result >= max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Random real result out of range", ErrorCategory::System);
        }
        
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Uniform real error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (min >= max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "min must be < max", ErrorCategory::System);
        }
        std::uniform_real_distribution<float> dist(min, max);
        float result = dist(pImpl->gen);
        
        // Validate result is within expected range
        if (result < min || result >= max) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Random real result out of range", ErrorCategory::System);
        }
        
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Uniform real error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate parameters for normal distribution
        if (std::isnan(mean) || std::isinf(mean)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid mean parameter for normal distribution", ErrorCategory::System);
        }
        if (std::isnan(stddev) || std::isinf(stddev) || stddev < 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid standard deviation parameter for normal distribution", ErrorCategory::System);
        }
        
        std::normal_distribution<double> dist(mean, stddev);
        double result = dist(pImpl->gen);
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Normal distribution error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate parameters for normal distribution
        if (std::isnan(mean) || std::isinf(mean)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid mean parameter for normal distribution", ErrorCategory::System);
        }
        if (std::isnan(stddev) || std::isinf(stddev) || stddev < 0.0f) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid standard deviation parameter for normal distribution", ErrorCategory::System);
        }
        
        std::normal_distribution<float> dist(mean, stddev);
        float result = dist(pImpl->gen);
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Normal distribution error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

bool RandomGenerator::bernoulli(double p) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate probability parameter
        if (p < 0.0 || p > 1.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Probability p must be in range [0, 1]", ErrorCategory::System);
        }
        
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Bernoulli error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate lambda parameter for Poisson distribution
        if (lambda < 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Poisson lambda must be non-negative", ErrorCategory::System);
        }
        
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Poisson error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

double RandomGenerator::exponential(double lambda) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate lambda parameter for exponential distribution
        if (lambda < 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Exponential lambda must be non-negative", ErrorCategory::System);
        }
        
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Exponential error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (n == 0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Cannot pick from zero items", ErrorCategory::System);
        }
        return uniformInt(0, n - 1);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Pick random index error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (!data && size > 0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Data pointer is null but size is positive", ErrorCategory::System);
        }
        
        if (size > 0) {
            uint8_t* bytes = static_cast<uint8_t*>(data);
            for (size_t i = 0; i < size; ++i) {
                bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
            }
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Fill bytes error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
