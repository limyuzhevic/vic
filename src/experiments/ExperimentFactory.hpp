#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "Experiment.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class ExperimentFactory : public ObjectFactory {
public:
    ExperimentFactory() = default;
    ~ExperimentFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ExperimentFactory
    }
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ExperimentFactory
    }
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ExperimentFactory
    }
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ExperimentFactory
    }
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by ExperimentFactory
    }
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override;
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "ExperimentFactory";
    }
    
    std::string getSupportedType() const override {
        return "Experiment";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("ExperimentFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("ExperimentFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("ExperimentFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<ExperimentFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<ExperimentFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(ExperimentFactory, "default_experiment", "Experiment", true)
    
    // Experiment-specific factory methods
    std::unique_ptr<Experiment> createBasicExperiment();
    std::unique_ptr<Experiment> createPhaseExperiment(int phase);
    std::unique_ptr<Experiment> createAblationExperiment();
    std::unique_ptr<Experiment> createScalingBenchmark();
    std::unique_ptr<Experiment> createCustomExperiment(const std::string& name);
    
    // Object pooling for Experiment objects
    std::unique_ptr<Experiment> acquireExperimentFromPool();
    void releaseExperimentToPool(std::unique_ptr<Experiment> experiment);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<Experiment>> experimentPool_;
};

} // namespace nlm
