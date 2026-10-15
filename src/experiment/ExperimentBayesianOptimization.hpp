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

class ExperimentBayesianOptimization {
public:
    ExperimentBayesianOptimization();
    ~ExperimentBayesianOptimization();
    
    // Disable copying, enable moving
    ExperimentBayesianOptimization(const ExperimentBayesianOptimization&) = delete;
    ExperimentBayesianOptimization& operator=(const ExperimentBayesianOptimization&) = delete;
    ExperimentBayesianOptimization(ExperimentBayesianOptimization&&) noexcept;
    ExperimentBayesianOptimization& operator=(ExperimentBayesianOptimization&&) noexcept;
    
    // Optimization management
    bool createBO(const std::string& boName, const BayesianOptimizerConfig& config);
    bool removeBO(const std::string& boName);
    bool updateBO(const std::string& boName, const BayesianOptimizerConfig& config);
    BayesianOptimizerConfig getBOConfig(const std::string& boName) const;
    std::vector<std::string> getAllBONames() const;
    
    // Optimization execution
    bool runBO(const std::string& boName);
    bool runBONow(const std::string& boName);
    bool pauseBO(const std::string& boName);
    bool resumeBO(const std::string& boName);
    bool stopBO(const std::string& boName);
    
    // Optimization monitoring
    double getBOProgress(const std::string& boName) const;
    std::string getBOStatus(const std::string& boName) const;
    std::chrono::system_clock::time_point getBOStartTime(const std::string& boName) const;
    std::chrono::system_clock::time_point getBOEndTime(const std::string& boName) const;
    std::chrono::seconds getBODuration(const std::string& boName) const;
    std::string getBOErrorMessage(const std::string& boName) const;
    
    // Optimization results
    bool storeBOResults(const std::string& boName, const Config& optimalParameters, double objectiveValue);
    std::pair<Config, double> getBOResults(const std::string& boName) const;
    bool exportBOResults(const std::string& boName, const std::string& filepath) const;
    
    // Optimization analysis
    double calculateBOConvergence(const std::string& boName) const;
    std::string analyzeBOPerformance(const std::string& boName) const;
    std::map<std::string, double> getBOAcquisitionValueHistory(const std::string& boName) const;
    
    // Optimization optimization
    bool optimizeBOParameters(const std::string& boName);
    bool autoTuneBOSettings(const std::string& boName);
    bool enableParallelBO(const std::string& boName, bool enable);
    bool setBOIterationLimit(const std::string& boName, size_t maxIterations);
    
    // Optimization cleanup
    bool cleanupBOResults(const std::string& boName);
    bool cleanupOldBOs(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetBO(const std::string& boName);
    bool clearAllBOs();
    std::vector<std::string> getAllBONames() const;
    size_t getBOCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm