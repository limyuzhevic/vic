#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "Random.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class RandomGeneratorFactory : public ObjectFactory {
public:
    RandomGeneratorFactory() = default;
    ~RandomGeneratorFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by RandomGeneratorFactory
    }
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override;
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by RandomGeneratorFactory
    }
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by RandomGeneratorFactory
    }
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by RandomGeneratorFactory
    }
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by RandomGeneratorFactory
    }
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "RandomGeneratorFactory";
    }
    
    std::string getSupportedType() const override {
        return "RandomGenerator";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("RandomGeneratorFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("RandomGeneratorFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("RandomGeneratorFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<RandomGeneratorFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<RandomGeneratorFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(RandomGeneratorFactory, "default_random", "RandomGenerator", true)
    
    // RandomGenerator-specific factory methods
    std::unique_ptr<RandomGenerator> createDeterministicGenerator(uint64_t seed);
    std::unique_ptr<RandomGenerator> createTimeBasedGenerator();
    std::unique_ptr<RandomGenerator> createSecureGenerator();
    std::unique_ptr<RandomGenerator> createGeneratorWithDistribution(const std::string& distributionType);
    
    // Object pooling for RandomGenerator objects
    std::unique_ptr<RandomGenerator> acquireGeneratorFromPool();
    void releaseGeneratorToPool(std::unique_ptr<RandomGenerator> generator);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<RandomGenerator>> generatorPool_;
};

} // namespace nlm
