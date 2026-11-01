#pragma once

#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Experiment result data structure
struct ExperimentResult {
    std::string name;
    std::string description;
    double executionTime;
    bool success;
    std::string output;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    ExperimentResult() : executionTime(0.0), success(false) {}
    ExperimentResult(const std::string& n, const std::string& d) 
        : name(n), description(d), executionTime(0.0), success(false) {}
};

// Experiment interface
class IExperiment {
public:
    virtual ~IExperiment() = default;
    
    // Get experiment metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getCategory() const = 0;
    
    // Experiment execution
    virtual ExperimentResult run() = 0;
    
    // Pre-run setup
    virtual bool setup(const Config& config) { return true; }
    
    // Post-run cleanup
    virtual void cleanup() {}
    
    // Check if experiment should run
    virtual bool shouldRun(const Config& config) const = 0;
    
    // Get default configuration for this experiment
    virtual Config getDefaultConfig() const { return Config(); }
};

// Concrete experiment implementations
class BasicConnectivityExperiment : public IExperiment {
public:
    std::string getName() const override { return "Basic Connectivity"; }
    std::string getDescription() const override { return "Tests basic neural connectivity and spike propagation"; }
    std::string getCategory() const override { return "Core Functionality"; }
    
    ExperimentResult run() override;
    bool shouldRun(const Config& config) const override;
};

class PlasticityLearningExperiment : public IExperiment {
public:
    std::string getName() const override { return "Plasticity Learning"; }
    std::string getDescription() const override { return "Tests STDP and Hebbian learning through repeated input patterns"; }
    std::string getCategory() const override { return "Plasticity"; }
    
    ExperimentResult run() override;
    bool shouldRun(const Config& config) const override;
};

class StdpVerificationExperiment : public IExperiment {
public:
    std::string getName() const override { return "STDP Verification"; }
    std::string getDescription() const override { return "Verifies STDP rule with correlated pre-post activity"; }
    std::string getCategory() const override { return "Plasticity"; }
    
    ExperimentResult run() override;
    bool shouldRun(const Config& config) const override;
};

class WorkingMemoryExperiment : public IExperiment {
public:
    std::string getName() const override { return "Working Memory"; }
    std::string getDescription() const override { return "Tests working memory trace storage and decay dynamics"; }
    std::string getCategory() const override { return "Memory"; }
    
    ExperimentResult run() override;
    bool shouldRun(const Config& config) const override;
};

class ConceptFormationExperiment : public IExperiment {
public:
    std::string getName() const override { return "Concept Formation"; }
    std::string getDescription() const override { return "Tests concept formation with magnitude-aware similarity metrics"; }
    std::string getCategory() const override { return "Cognition"; }
    
    ExperimentResult run() override;
    bool shouldRun(const Config& config) const override;
};

// Experiment Manager class
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Register an experiment
    void registerExperiment(std::unique_ptr<IExperiment> experiment);
    
    // Unregister an experiment by name
    void unregisterExperiment(const std::string& name);
    
    // Get all registered experiments
    std::vector<std::string> getExperimentNames() const;
    
    // Get experiment by name
    IExperiment* getExperiment(const std::string& name);
    
    // Run a specific experiment
    ExperimentResult runExperiment(const std::string& name, const Config& config);
    
    // Run all experiments
    std::vector<ExperimentResult> runAllExperiments(const Config& config);
    
    // Run experiments by category
    std::vector<ExperimentResult> runExperimentsByCategory(const std::string& category, const Config& config);
    
    // Get default configuration for all experiments
    Config getDefaultConfiguration() const;
    
    // Save experiment results to file
    bool saveResults(const std::vector<ExperimentResult>& results, const std::string& filepath) const;
    
    // Load experiment results from file
    std::vector<ExperimentResult> loadResults(const std::string& filepath) const;
    
    // Clear all results
    void clearResults();
    
    // Get all results
    const std::vector<ExperimentResult>& getAllResults() const { return results; }
    
    // Compare results between experiments
    std::string compareResults(const std::vector<ExperimentResult>& results) const;
    
    // Get summary of all results
    std::string getSummary() const;
    
    // Reset to initial state (clear results, reload default experiments)
    void reset();
    
    // Get current configuration
    const Config& getConfig() const { return currentConfig; }
    
    // Set configuration
    void setConfig(const Config& config) { currentConfig = config; }
    
private:
    // Internal data structures
    std::unordered_map<std::string, std::unique_ptr<IExperiment>> experiments;
    std::vector<ExperimentResult> results;
    Config currentConfig;
    
    // Prevent copying
    ExperimentManager(const ExperimentManager&) = delete;
    ExperimentManager& operator=(const ExperimentManager&) = delete;
    
    // Initialize default experiments
    void initializeDefaultExperiments();
    
    // Helper functions
    std::string formatTime(double seconds) const;
    std::string formatResults(const ExperimentResult& result) const;
};

} // namespace nlm
