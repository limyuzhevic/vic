// Experiment framework for NLM - provides reusable test scenarios and flexible execution
#ifndef NLM_EXPERIMENT_FRAMEWORK_HPP
#define NLM_EXPERIMENT_FRAMEWORK_HPP

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Experiment result structure
struct ExperimentResult {
    std::string name;
    std::string description;
    bool success;
    std::string output;
    double executionTime;
    std::vector<std::string> logs;
    
    ExperimentResult(const std::string& n, const std::string& desc)
        : name(n), description(desc), success(false), executionTime(0.0) {}
};

// Base experiment class - all experiments inherit from this
class Experiment {
public:
    virtual ~Experiment() = default;
    
    // Experiment metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Execution method
    virtual ExperimentResult run(std::shared_ptr<Brain> brain) = 0;
    
    // Optional setup method (called before experiment)
    virtual void setup(std::shared_ptr<Config> config) {}
    
    // Optional cleanup method (called after experiment)
    virtual void cleanup() {}
    
    // Check if experiment is compatible with brain configuration
    virtual bool isCompatible(std::shared_ptr<const Config> config) const {
        return true; // Default: compatible with all configurations
    }
};

// Experiment factory for creating experiments
class ExperimentFactory {
public:
    static std::vector<std::unique_ptr<Experiment>> getAllExperiments();
    static std::unique_ptr<Experiment> createExperiment(const std::string& name);
    
private:
    static std::vector<std::unique_ptr<Experiment>> registerExperiments();
};

// Utility functions for experiments
class ExperimentUtils {
public:
    // Format time duration for display
    static std::string formatDuration(double seconds);
    
    // Create default configuration for experiments
    static std::shared_ptr<Config> createDefaultConfig(const std::string& experimentName);
    
    // Run a series of experiments
    static std::vector<ExperimentResult> runExperiments(
        const std::vector<std::unique_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Config> config);
    
    // Save experiment results to file
    static bool saveResults(const std::string& filepath, const std::vector<ExperimentResult>& results);
    
    // Load experiment results from file
    static std::vector<ExperimentResult> loadResults(const std::string& filepath);
};

} // namespace nlm

#endif // NLM_EXPERIMENT_FRAMEWORK_HPP
