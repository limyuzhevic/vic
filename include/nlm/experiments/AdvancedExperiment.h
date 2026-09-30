#pragma once

#include "../agent/AgentBody.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../core/Config/Config.hpp"
#include "ExperimentConfiguration.h"
#include "ExperimentResults.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>

namespace nlm {

/**
 * Advanced experiment base class providing comprehensive functionality
 * for neural network experiments with configuration management,
 * results collection, and advanced analysis capabilities.
 */
class AdvancedExperiment {
public:
    // Experiment lifecycle
    virtual ~AdvancedExperiment() = default;
    
    // Initialization and configuration
    virtual bool initialize() = 0;
    virtual void setup() = 0;
    virtual void run() = 0;
    virtual void analyzeResults() = 0;
    virtual void cleanup() = 0;
    
    // Getters and setters
    virtual const std::string& getName() const = 0;
    virtual void setName(const std::string& name) = 0;
    
    virtual std::string getDescription() const = 0;
    virtual void setDescription(const std::string& desc) = 0;
    
    virtual uint64_t getSeed() const = 0;
    virtual void setSeed(uint64_t seed) = 0;
    
    virtual double getTimeLimit() const = 0;
    virtual void setTimeLimit(double limit) = 0;
    
    virtual size_t getNumTrials() const = 0;
    virtual void setNumTrials(size_t trials) = 0;
    
    // Configuration management
    ExperimentConfiguration config;
    
    // Progress tracking
    virtual double getProgress() const = 0;
    virtual std::string getStatus() const = 0;
    
    // Results management
    virtual const ExperimentResults& getResults() const = 0;
    virtual void recordResults(const ExperimentResults& results) = 0;
    
    // Brain and environment setup
    virtual std::shared_ptr<Brain> createBrain() = 0;
    virtual std::shared_ptr<Environment> createEnvironment() = 0;
    
    // Experiment execution
    virtual bool runExperiment(std::shared_ptr<Brain> brain, 
                               std::shared_ptr<Environment> environment) = 0;
    
    // Error handling
    virtual bool hasError() const = 0;
    virtual const std::string& getError() const = 0;
    virtual void clearError() = 0;
    
    // Progress reporting
    virtual void reportProgress(double progress, const std::string& message) = 0;
    
    // Thread safety
    virtual void lock() = 0;
    virtual void unlock() = 0;
    
    // Save/Load
    virtual bool saveResults(const std::string& filepath) = 0;
    virtual bool loadResults(const std::string& filepath) = 0;
    
protected:
    // Protected constructor for subclass implementations
    AdvancedExperiment() = default;
    
    // Helper methods for common functionality
    bool validateParameters() const;
    void log(const std::string& message) const;
    std::string generateExperimentId() const;
    
    // Time tracking
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    // Error handling
    std::string errorMessage;
    bool errorFlag;
    
    // Progress tracking
    std::atomic<double> progress;
    std::string statusMessage;
    mutable std::mutex progressMutex;
};

} // namespace nlm