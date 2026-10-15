#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentAutomation.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class ExperimentHyperparameterOptimization {
public:
    ExperimentHyperparameterOptimization();
    ~ExperimentHyperparameterOptimization();
    
    // Disable copying, enable moving
    ExperimentHyperparameterOptimization(const ExperimentHyperparameterOptimization&) = delete;
    ExperimentHyperparameterOptimization& operator=(const ExperimentHyperparameterOptimization&) = delete;
    ExperimentHyperparameterOptimization(ExperimentHyperparameterOptimization&&) noexcept;
    ExperimentHyperparameterOptimization& operator=(ExperimentHyperparameterOptimization&&) noexcept;
    
    // Optimization management
    bool createOptimizer(const std::string& optimizerName, const HyperparameterOptimizerConfig& config);
    bool removeOptimizer(const std::string& optimizerName);
    bool updateOptimizer(const std::string& optimizerName, const HyperparameterOptimizerConfig& config);
    HyperparameterOptimizerConfig getOptimizerConfig(const std::string& optimizerName) const;
    std::vector<std::string> getAllOptimizerNames() const;
    
    // Optimization execution
    bool runOptimizer(const std::string& optimizerName);
    bool runOptimizerNow(const std::string& optimizerName);
    bool pauseOptimizer(const std::string& optimizerName);
    bool resumeOptimizer(const std::string& optimizerName);
    bool stopOptimizer(const std::string& optimizerName);
    
    // Optimization monitoring
    double getOptimizerProgress(const std::string& optimizerName) const;
    std::string getOptimizerStatus(const std::string& optimizerName) const;
    std::chrono::system_clock::time_point getOptimizerStartTime(const std::string& optimizerName) const;
    std::chrono::system_clock::time_point getOptimizerEndTime(const std::string& optimizerName) const;
    std::chrono::seconds getOptimizerDuration(const std::string& optimizerName) const;
    std::string getOptimizerErrorMessage(const std::string& optimizerName) const;
    
    // Optimization results
    bool storeOptimizerResults(const std::string& optimizerName, const Config& optimalParameters);
    Config getOptimizerResults(const std::string& optimizerName) const;
    bool exportOptimizerResults(const std::string& optimizerName, const std::string& filepath) const;
    
    // Optimization analysis
    double calculateOptimizerConvergence(const std::string& optimizerName) const;
    std::string analyzeOptimizerPerformance(const std::string& optimizerName) const;
    std::map<std::string, double> getParameterImportance(const std::string& optimizerName) const;
    
    // Optimization optimization
    bool optimizeOptimizerParameters(const std::string& optimizerName);
    bool autoTuneOptimizerSettings(const std::string& optimizerName);
    bool enableParallelOptimization(const std::string& optimizerName, bool enable);
    bool setOptimizerTimeout(const std::string& optimizerName, const std::chrono::seconds& timeout);
    
    // Optimization cleanup
    bool cleanupOptimizerResults(const std::string& optimizerName);
    bool cleanupOldOptimizers(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetOptimizer(const std::string& optimizerName);
    bool clearAllOptimizers();
    std::vector<std::string> getAllOptimizerNames() const;
    size_t getOptimizerCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm