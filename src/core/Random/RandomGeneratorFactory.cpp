#include "RandomGeneratorFactory.hpp"
#include "Random.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <chrono>

namespace nlm {

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::createRandomGenerator(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("RandomGeneratorFactory not initialized, creating basic generator");
        return std::make_unique<RandomGenerator>();
    }
    
    // Get configuration from params or use defaults
    bool deterministic = params.getParam<bool>("deterministic", false);
    uint64_t seed = params.getParam<uint64_t>("seed", 0);
    std::string distribution = params.getParam<std::string>("distribution", "uniform");
    std::string type = params.getParam<std::string>("type", "default");
    
    // Handle different generator creation scenarios based on type
    if (type == "deterministic" || deterministic) {
        if (seed == 0) {
            seed = std::chrono::system_clock::now().time_since_epoch().count();
        }
        return createDeterministicGenerator(seed);
    } else if (type == "secure") {
        return createSecureGenerator();
    } else if (type == "time_based") {
        return createTimeBasedGenerator();
    } else {
        // Default generator
        return createDeterministicGenerator(seed);
    }
}

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::createDeterministicGenerator(uint64_t seed) {
    spdlog::debug("Creating deterministic random generator with seed: {}", seed);
    return std::make_unique<RandomGenerator>(seed);
}

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::createTimeBasedGenerator() {
    spdlog::debug("Creating time-based random generator");
    return std::make_unique<RandomGenerator>();
}

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::createSecureGenerator() {
    spdlog::debug("Creating secure random generator");
    return std::make_unique<RandomGenerator>();
}

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::createGeneratorWithDistribution(const std::string& distributionType) {
    spdlog::debug("Creating random generator with distribution: {}", distributionType);
    return std::make_unique<RandomGenerator>();
}

std::unique_ptr<RandomGenerator> RandomGeneratorFactory::acquireGeneratorFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!generatorPool_.empty()) {
        auto generator = std::move(generatorPool_.back());
        generatorPool_.pop_back();
        spdlog::debug("Acquired random generator from pool");
        return generator;
    }
    return nullptr;
}

void RandomGeneratorFactory::releaseGeneratorToPool(std::unique_ptr<RandomGenerator> generator) {
    if (generator) {
        std::lock_guard<std::mutex> lock(mutex_);
        generatorPool_.push_back(std::move(generator));
        spdlog::debug("Released random generator to pool");
    }
}

} // namespace nlm
