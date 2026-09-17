#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// Experiment factory interface
class IExperimentFactory {
public:
    virtual ~IExperimentFactory() = default;
    virtual std::shared_ptr<Phase6IntegratedExperiment> create(const Phase6Config& config) = 0;
    virtual std::string getName() const = 0;
    virtual bool isAvailable() const = 0;
};

// Default experiment factory implementation
class DefaultExperimentFactory : public IExperimentFactory {
public:
    DefaultExperimentFactory();
    ~DefaultExperimentFactory() override;
    
    std::shared_ptr<Phase6IntegratedExperiment> create(const Phase6Config& config) override;
    std::string getName() const override { return "DefaultPhase6Experiment"; }
    bool isAvailable() const override { return true; }
};

// Experiment factory registry
class ExperimentFactoryRegistry {
public:
    ExperimentFactoryRegistry();
    ~ExperimentFactoryRegistry() = default;
    
    // Disable copying
    ExperimentFactoryRegistry(const ExperimentFactoryRegistry&) = delete;
    ExperimentFactoryRegistry& operator=(const ExperimentFactoryRegistry&) = delete;
    
    // Singleton access
    static ExperimentFactoryRegistry& getInstance();
    
    // Register a factory
    void registerFactory(const std::string& name, std::unique_ptr<IExperimentFactory> factory);
    
    // Create experiment by name
    std::shared_ptr<Phase6IntegratedExperiment> createExperiment(
        const std::string& factoryName, const Phase6Config& config);
    
    // Get all registered factory names
    std::vector<std::string> getRegisteredFactories() const;
    
    // Check if factory is registered
    bool hasFactory(const std::string& name) const;
    
    // Remove factory
    void unregisterFactory(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience factory function
std::shared_ptr<Phase6IntegratedExperiment> createPhase6Experiment(const Phase6Config& config) {
    auto& factoryRegistry = ExperimentFactoryRegistry::getInstance();
    return factoryRegistry.createExperiment("default", config);
}

} // namespace nlm

