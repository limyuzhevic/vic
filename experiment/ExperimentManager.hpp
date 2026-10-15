#pragma once

#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

namespace nlm {

/**
 * Experiment configuration template
 */
struct ExperimentConfig {
    std::string name;
    std::string description;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::vector<double>> parameterRanges;
    uint64_t seed = 42;
    size_t maxSteps = 1000;
    bool enabled = true;
    
    ExperimentConfig() {}
    ExperimentConfig(const std::string& name) : name(name) {}
};

/**
 * Experiment management system
 * Coordinates experiment creation, execution, and result management
 */
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Experiment lifecycle management
    bool createExperiment(const std::string& name, const ExperimentConfig& config);
    bool createExperimentFromTemplate(const std::string& name, const std::string& templateName);
    bool runExperiment(const std::string& name, std::shared_ptr<Brain> brain, 
                      std::shared_ptr<Environment> environment, size_t maxSteps = 1000);
    bool runExperiment(const std::string& name);
    
    // Batch experiment operations
    std::vector<std::string> runBatch(const std::vector<std::string>& experimentNames,
                                     std::shared_ptr<Brain> brainTemplate,
                                     std::shared_ptr<Environment> environmentTemplate,
                                     size_t maxSteps = 1000);
    
    // Experiment template management
    bool createTemplate(const std::string& name, const ExperimentConfig& config);
    bool updateTemplate(const std::string& name, const ExperimentConfig& config);
    bool deleteTemplate(const std::string& name);
    const std::map<std::string, ExperimentConfig>& getTemplates() const;
    
    // Experiment result management
    bool storeResult(const std::string& experimentName, std::shared_ptr<Experiment> experiment);
    std::shared_ptr<Experiment> getResult(const std::string& experimentName) const;
    std::vector<std::shared_ptr<Experiment>> getAllResults() const;
    
    // Experiment parameter management
    bool setParameter(const std::string& experimentName, const std::string& key, 
                     const std::string& value);
    std::string getParameter(const std::string& experimentName, const std::string& key) const;
    std::map<std::string, std::string> getExperimentParameters(const std::string& experimentName) const;
    
    // Experiment comparison
    bool compareExperiments(const std::string& exp1Name, const std::string& exp2Name,
                           std::map<std::string, double>& comparisonResults) const;
    
    // Automation and scheduling
    bool scheduleExperiment(const std::string& experimentName, 
                           const std::chrono::system_clock::time_point& scheduledTime);
    bool runScheduledExperiments();
    
    // Configuration management
    bool saveManagerConfig(const std::string& filepath) const;
    bool loadManagerConfig(const std::string& filepath);
    
    // Result analysis and visualization
    std::string generateResultSummary(const std::string& experimentName) const;
    std::string generateComparativeAnalysis(const std::vector<std::string>& experimentNames) const;
    
    // Utility functions
    bool validateExperimentConfig(const ExperimentConfig& config) const;
    std::vector<std::string> getAllExperimentNames() const;
    std::vector<std::string> getAllExperimentNames(bool includeCompleted) const;
    size_t getExperimentCount(bool includeCompleted) const;
    
    // Progress tracking
    float getExperimentProgress(const std::string& experimentName) const;
    std::map<std::string, float> getBatchProgress(const std::vector<std::string>& experimentNames) const;
    
    // Cleanup
    void clearCompletedExperiments(size_t maxKeep = 100);
    void cleanup() { clearCompletedExperiments(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
