// ExperimentManager.hpp - Modular experiment management
#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

namespace nlm {

class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager() = default;
    
    // Register experiment factory functions
    using ExperimentFactory = std::function<std::shared_ptr<Experiment>(const Config&)>;
    
    void registerExperiment(const std::string& name, ExperimentFactory factory);
    
    // Run multiple experiments
    std::vector<std::string> runExperiments(const std::vector<std::string>& names,
                                           std::shared_ptr<Brain> brain,
                                           std::shared_ptr<Environment> environment = nullptr);
    
    // Run all registered experiments
    std::vector<std::string> runAllExperiments(std::shared_ptr<Brain> brain,
                                              std::shared_ptr<Environment> environment = nullptr);
    
    // Get list of registered experiments
    std::vector<std::string> getRegisteredExperimentNames() const;
    
    // Check if experiment is registered
    bool hasExperiment(const std::string& name) const;
    
    // Set experiment configuration
    void setExperimentConfig(const std::string& name, const std::unordered_map<std::string, ConfigValue>& config);
    
    // Get experiment configuration
    std::unordered_map<std::string, ConfigValue> getExperimentConfig(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Setup and teardown
    void initialize();
    void cleanup();
    
    // Get experiment runner
    std::shared_ptr<ExperimentRunner> getRunner() { return runner_; }

private:
    std::shared_ptr<ExperimentRunner> runner_;
    std::unordered_map<std::string, ExperimentFactory> experimentFactories_;
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigValue>> experimentConfigs_;
    
    // Built-in experiment factories
    void registerBuiltInExperiments();
    
    // Default experiment configurations
    void setupDefaultExperimentConfigurations();
};

} // namespace nlm
