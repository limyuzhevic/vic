// NLM Batch Processing System
// Handles batch execution of multiple experiments with different configurations

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include "src/core/Config/Config.hpp"

namespace nlm {

// Batch experiment configuration
struct BatchExperiment {
    std::string name;
    std::shared_ptr<Config> config;
    std::string description;
    int priority;
    bool enabled;
    std::map<std::string, std::string> parameters;
};

// Batch processing results
struct BatchResult {
    std::string experimentName;
    bool success;
    double executionTime;
    std::string outputFile;
    std::string errorMessage;
    std::map<std::string, double> metrics;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::vector<std::string> logMessages;
};

// Batch Processor class
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Add experiment to batch
    void addExperiment(const std::string& name, 
                      const std::shared_ptr<Config>& config,
                      const std::string& description = "",
                      int priority = 0);
    
    // Add experiment from config file
    bool addExperimentFromFile(const std::string& name,
                              const std::string& configFile,
                              const std::string& description = "",
                              int priority = 0);
    
    // Remove experiment
    bool removeExperiment(const std::string& name);
    
    // Clear all experiments
    void clearExperiments();
    
    // Get all experiment names
    std::vector<std::string> getExperimentNames() const;
    
    // Get experiment by name
    std::shared_ptr<BatchExperiment> getExperiment(const std::string& name) const;
    
    // Set experiment enabled state
    void setExperimentEnabled(const std::string& name, bool enabled);
    
    // Set experiment parameters
    void setExperimentParameter(const std::string& name,
                               const std::string& key,
                               const std::string& value);
    
    // Get experiment parameter
    std::string getExperimentParameter(const std::string& name,
                                       const std::string& key) const;
    
    // Generate variations of an experiment
    std::vector<std::shared_ptr<BatchExperiment>> generateVariations(
        const std::string& baseName,
        const std::map<std::string, std::vector<std::string>>& parameterSets);
    
    // Run single experiment
    BatchResult runExperiment(const std::string& name);
    
    // Run batch of experiments
    std::vector<BatchResult> runBatch(const std::vector<std::string>& names);
    
    // Run all enabled experiments
    std::vector<BatchResult> runAllEnabled();
    
    // Save batch configuration
    bool saveConfiguration(const std::string& filepath) const;
    
    // Load batch configuration
    bool loadConfiguration(const std::string& filepath);
    
    // Get batch results
    const std::vector<BatchResult>& getResults() const;
    
    // Clear results
    void clearResults();
    
    // Get summary statistics
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
