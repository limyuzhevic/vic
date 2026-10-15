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

class ExperimentParameterSweep {
public:
    ExperimentParameterSweep();
    ~ExperimentParameterSweep();
    
    // Disable copying, enable moving
    ExperimentParameterSweep(const ExperimentParameterSweep&) = delete;
    ExperimentParameterSweep& operator=(const ExperimentParameterSweep&) = delete;
    ExperimentParameterSweep(ExperimentParameterSweep&&) noexcept;
    ExperimentParameterSweep& operator=(ExperimentParameterSweep&&) noexcept;
    
    // Sweep management
    bool createSweep(const std::string& sweepName, const ParameterSweepConfig& config);
    bool removeSweep(const std::string& sweepName);
    bool updateSweep(const std::string& sweepName, const ParameterSweepConfig& config);
    ParameterSweepConfig getSweepConfig(const std::string& sweepName) const;
    std::vector<std::string> getAllSweepNames() const;
    
    // Sweep execution
    bool runSweep(const std::string& sweepName);
    bool runSweepNow(const std::string& sweepName);
    bool pauseSweep(const std::string& sweepName);
    bool resumeSweep(const std::string& sweepName);
    bool stopSweep(const std::string& sweepName);
    
    // Sweep monitoring
    double getSweepProgress(const std::string& sweepName) const;
    std::string getSweepStatus(const std::string& sweepName) const;
    std::chrono::system_clock::time_point getSweepStartTime(const std::string& sweepName) const;
    std::chrono::system_clock::time_point getSweepEndTime(const std::string& sweepName) const;
    std::chrono::seconds getSweepDuration(const std::string& sweepName) const;
    std::string getSweepErrorMessage(const std::string& sweepName) const;
    
    // Sweep results
    bool storeSweepResults(const std::string& sweepName, const std::vector<ExperimentConfig>& results);
    std::vector<ExperimentConfig> getSweepResults(const std::string& sweepName) const;
    bool exportSweepResults(const std::string& sweepName, const std::string& filepath) const;
    
    // Sweep analysis
    double calculateSweepEfficiency(const std::string& sweepName) const;
    std::string analyzeSweepCoverage(const std::string& sweepName) const;
    std::map<std::string, double> calculateParameterImpact(const std::string& sweepName, 
                                                          const std::string& parameterName) const;
    
    // Sweep optimization
    bool optimizeSweepParameters(const std::string& sweepName);
    bool autoAdjustSweepResolution(const std::string& sweepName);
    bool enableSmartSampling(const std::string& sweepName, bool enable);
    bool setSweepConfidenceLevel(const std::string& sweepName, double confidenceLevel);
    
    // Sweep cleanup
    bool cleanupSweepResults(const std::string& sweepName);
    bool cleanupOldSweeps(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetSweep(const std::string& sweepName);
    bool clearAllSweeps();
    std::vector<std::string> getAllSweepNames() const;
    size_t getSweepCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm