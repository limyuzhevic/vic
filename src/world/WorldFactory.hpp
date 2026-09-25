#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "World.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class WorldFactory : public ObjectFactory {
public:
    WorldFactory() = default;
    ~WorldFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by WorldFactory
    }
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by WorldFactory
    }
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by WorldFactory
    }
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by WorldFactory
    }
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override;
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by WorldFactory
    }
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "WorldFactory";
    }
    
    std::string getSupportedType() const override {
        return "World";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("WorldFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("WorldFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("WorldFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<WorldFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<WorldFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(WorldFactory, "default_world", "World", true)
    
    // World-specific factory methods
    std::unique_ptr<World> createSimpleWorld();
    std::unique_ptr<World> createDynamicWorld();
    std::unique_ptr<World> createSimulationWorld(const std::string& simulationType);
    std::unique_ptr<World> createEnvironmentWorld(const std::string& environmentType);
    std::unique_ptr<World> createCustomWorld(const std::unordered_map<std::string, std::any>& customParams);
    
    // Object pooling for World objects
    std::unique_ptr<World> acquireWorldFromPool();
    void releaseWorldToPool(std::unique_ptr<World> world);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<World>> worldPool_;
};

} // namespace nlm
