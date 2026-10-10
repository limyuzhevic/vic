#include "Random.hpp"
#include "../ErrorHandling.hpp"
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(std::make_unique<Impl>(std::random_device{}())) {}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
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
    try {
        if (min > max) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "min must be <= max in uniformInt");
        }
        if (min == max) {
            return min;
        }
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate uniform integer");
    }
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    try {
        if (min > max) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "min must be <= max in uniformInt");
        }
        if (min == max) {
            return min;
        }
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate uniform integer");
    }
}

double RandomGenerator::uniformReal(double min, double max) {
    try {
        if (min >= max) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "min must be < max in uniformReal");
        }
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate uniform real");
    }
}

float RandomGenerator::uniformReal(float min, float max) {
    try {
        if (min >= max) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "min must be < max in uniformReal");
        }
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate uniform real");
    }
}

double RandomGenerator::normal(double mean, double stddev) {
    try {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate normal distribution");
    }
}

float RandomGenerator::normal(float mean, float stddev) {
    try {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate normal distribution");
    }
}

bool RandomGenerator::bernoulli(double p) {
    try {
        if (p < 0.0 || p > 1.0) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "Probability p must be in [0, 1] for bernoulli");
        }
        std::bernoulli_distribution dist(p);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate bernoulli distribution");
    }
}

uint32_t RandomGenerator::poisson(double lambda) {
    try {
        if (lambda < 0.0) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "Lambda must be >= 0 for poisson distribution");
        }
        std::poisson_distribution<uint32_t> dist(lambda);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate poisson distribution");
    }
}

double RandomGenerator::exponential(double lambda) {
    try {
        if (lambda <= 0.0) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "Lambda must be > 0 for exponential distribution");
        }
        std::exponential_distribution<double> dist(lambda);
        return dist(pImpl->gen);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate exponential distribution");
    }
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    try {
        if (n == 0) {
            NLM_THROW_ERROR(ErrorCode::Component_InvalidParameter,
                           "Cannot pick from zero items");
        }
        return uniformInt(0, n - 1);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to pick random index");
    }
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    try {
        uint8_t* bytes = static_cast<uint8_t*>(data);
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
        }
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to fill bytes with random data");
    }
}

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->gen;
}

} // namespace nlm
