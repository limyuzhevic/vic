#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>
#include <map>

namespace nlm {

// ScalingAnalyzer: Performance analysis across different scales
class ScalingAnalyzer {
public:
    ScalingAnalyzer();
    ~ScalingAnalyzer();
    
    // Configuration
    void setEnableScaleProfiling(bool enable) { enableScaleProfiling_ = enable; }
    void setEnableScalingAnalysis(bool enable) { enableScalingAnalysis_ = enable; }
    void setMinScalePoints(size_t min) { minScalePoints_ = min; }
    void setMaxScalePoints(size_t max) { maxScalePoints_ = max; }
    
    // Scale profiling
    void recordScaleMeasurement(size_t scale, const std::string& operation, 
                               double time, double memory,
                               size_t threads = 1, double cpu = 0.0);
    
    // Query results
    double getAverageTimeAtScale(size_t scale, const std::string& operation) const;
    double getAverageMemoryAtScale(size_t scale, const std::string& operation) const;
    
    // Scale listing
    std::vector<size_t> getScales() const;
    std::vector<std::string> getOperations() const;
    
    // Analysis
    std::string analyzeScalingBehavior() const;
    std::string analyzeParallelScaling() const;
    std::string identifyScalingBottlenecks() const;
    std::string analyzeEfficiencyAtDifferentScales() const;
    
    // Statistics
    size_t getTotalMeasurements() const { return totalMeasurements_; }
    std::map<size_t, double> getTimeScalingRatio(const std::string& operation) const;
    std::map<size_t, double> getMemoryScalingRatio(const std::string& operation) const;
    
    // Export
    bool exportScalingProfile(const std::string& filepath) const;
    std::string exportScalingProfileAsString() const;
    
    // Real-time scaling analysis
    std::string getRealTimeScalingStats() const;
    
private:
    struct ScaleMeasurement {
        std::chrono::system_clock::time_point timestamp;
        size_t scale;
        std::string operation;
        double time;
        double memory;
        size_t threads;
        double cpu;
        
        ScaleMeasurement(size_t scale, const std::string& op, double time, double memory,
                        size_t threads, double cpu)
            : scale(scale), operation(op), time(time), memory(memory),
              threads(threads), cpu(cpu) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<ScaleMeasurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    bool enableScaleProfiling_;
    bool enableScalingAnalysis_;
    size_t minScalePoints_;
    size_t maxScalePoints_;
    size_t totalMeasurements_;
    
    // Helper methods
    void cleanupOldMeasurements();
    double calculateSpeedup(size_t scale1, size_t scale2, 
                           const std::string& operation) const;
    double calculateEfficiency(size_t scale, const std::string& operation) const;
    double calculateMemoryEfficiency(size_t scale, const std::string& operation) const;
    std::string getScaleDescription(size_t scale) const;
    bool isScalingOptimal(double speedup, double scale) const;
};

} // namespace nlm
