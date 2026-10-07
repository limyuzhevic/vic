#pragma once

#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

// ExperimentResult: Stores the results of an experiment
struct ExperimentResult {
    std::string name;
    bool success;
    std::chrono::duration<double> executionTime;
    std::string output;
    std::map<std::string, double> metrics;
};

// CustomExperimentFunction: Type for user-defined experiment functions
using CustomExperimentFunction = std::function<bool(std::shared_ptr<Brain>, std::shared_ptr<Environment>, const Config&)>;

// ExperimentRunner: Manages and runs different test scenarios
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Disable copying, enable moving
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;
    ExperimentRunner(ExperimentRunner&&) noexcept;
    ExperimentRunner& operator=(ExperimentRunner&&) noexcept;
    
    // Core experiment methods
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain, const Config& config);
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain, const Config& config);
    bool runStdpVerification(std::shared_ptr<Brain> brain, const Config& config);
    
    // Hook for user-defined experiments
    bool runCustomExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                           const Config& config, CustomExperimentFunction experimentFunc);
    
    // Experiment management
    void registerExperiment(const std::string& name, CustomExperimentFunction func);
    bool runExperiment(const std::string& name, std::shared_ptr<Brain> brain, 
                      std::shared_ptr<Environment> environment, const Config& config);
    
    // Experiment utilities
    void resetBrain(std::shared_ptr<Brain> brain) const;
    void logExperimentResult(const std::string& name, const ExperimentResult& result) const;
    
    // Configuration validation for experiments
    static bool validateExperimentConfig(const Config& config);
    
    // Get experiment information
    std::string getExperimentInfo(const std::string& name) const;
    
    // Progress reporting
    void setProgressCallback(std::function<void(int, int, const std::string&)> callback);
    
    // Timing utilities
    double getElapsedTime(std::chrono::steady_clock::time_point start) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void logProgress(int current, int total, const std::string& message) const;
};

} // namespace nlm