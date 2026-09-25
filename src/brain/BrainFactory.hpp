#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "Brain.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class BrainFactory : public ObjectFactory {
public:
    BrainFactory() = default;
    ~BrainFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by BrainFactory
    }
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by BrainFactory
    }
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by BrainFactory
    }
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override;
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by BrainFactory
    }
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by BrainFactory
    }
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "BrainFactory";
    }
    
    std::string getSupportedType() const override {
        return "Brain";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("BrainFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("BrainFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("BrainFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<BrainFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<BrainFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(BrainFactory, "default_brain", "Brain", true)
    
    // Brain-specific factory methods
    std::unique_ptr<Brain> createBasicBrain();
    std::unique_ptr<Brain> createStructuredBrain(const std::vector<std::string>& regions);
    std::unique_ptr<Brain> createConnectedBrain(const std::vector<std::pair<std::string, std::string>>& connections);
    std::unique_ptr<Brain> createPlasticBrain(const std::string& plasticityType);
    std::unique_ptr<Brain> createAdaptiveBrain(const std::string& adaptationStrategy);
    
    // Object pooling for Brain objects
    std::unique_ptr<Brain> acquireBrainFromPool();
    void releaseBrainToPool(std::unique_ptr<Brain> brain);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<Brain>> brainPool_;
};

} // namespace nlm
