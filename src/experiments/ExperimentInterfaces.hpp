#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Experiment validation interface
class IExperimentValidator {
public:
    virtual ~IExperimentValidator() = default;
    
    // Validate experiment configuration
    virtual bool validateConfiguration(const std::string& configName) const = 0;
    
    // Validate experiment setup
    virtual bool validateSetup(const std::string& configName) const = 0;
    
    // Get validation errors
    virtual std::vector<std::string> getValidationErrors() const = 0;
    
    // Get validation warnings
    virtual std::vector<std::string> getValidationWarnings() const = 0;
};

// Experiment runner interface
class IExperimentRunner {
public:
    virtual ~IExperimentRunner() = default;
    
    // Run experiment
    virtual bool run(const std::string& configName) = 0;
    
    // Get experiment results
    virtual std::vector<double> getResults() const = 0;
    
    // Get experiment metrics
    virtual std::vector<std::string> getMetrics() const = 0;
    
    // Get experiment status
    virtual std::string getStatus() const = 0;
    
    // Check if experiment is complete
    virtual bool isComplete() const = 0;
    
    // Stop experiment
    virtual void stop() = 0;
};

// Experiment validation result structure
struct ExperimentValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string configName;
    
    ExperimentValidationResult() : isValid(false) {}
    
    bool operator==(const ExperimentValidationResult& other) const {
        return isValid == other.isValid && errors == other.errors && 
               warnings == other.warnings && configName == other.configName;
    }
};

// Experiment result structure
struct ExperimentResult {
    std::string experimentName;
    std::string configName;
    bool success;
    std::vector<double> metrics;
    std::string status;
    uint64_t stepsCompleted;
    uint64_t totalSteps;
    double executionTime;
    
    ExperimentResult() : success(false), stepsCompleted(0), totalSteps(0), executionTime(0.0) {}
};

} // namespace nlm
