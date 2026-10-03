#include "Random.hpp"
#include <stdexcept>

namespace nlm {

// Implementation details for RandomGenerator
// Implementation details for RandomGenerator
struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {}

RandomGenerator::RandomGenerator(nullptr) : pImpl(std::make_unique<Impl>(std::random_device{}())) {}

RandomGenerator::~RandomGenerator(nullptr) = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    if (pImpl) {
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
    }
}

uint64_t RandomGenerator::getSeed(nullptr) const {
    return pImpl ? pImpl->currentSeed : 0;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    
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
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    
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
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("p must be in [0, 1]");
    }
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    if (lambda < 0.0) {
        throw std::invalid_argument("lambda must be >= 0");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be > 0");
    }
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

bool RandomGenerator::randomBool(nullptr) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit(nullptr) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    return uniformInt(0, 1);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!pImpl) {
        throw std::runtime_error("RandomGenerator not initialized");
    }
    if (!data || size == 0) {
        return; // Nothing to do
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
    }
}

std::mt19937_64& RandomGenerator::engine(nullptr) {
    if (!pImpl) {
        static std::mt19937_64 emptyGen;
        return emptyGen;
    }
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine(nullptr) const {
    if (!pImpl) {
        static const std::mt19937_64 emptyGen;
        return emptyGen;
    }
    return pImpl->gen;
}

} // namespace nlm