// AdaptiveProcessor.hpp - New adaptive processor
#pragma once

#include "Performance.hpp"
#include <cmath>

namespace nlm {

class AdaptiveProcessor {
public:
    AdaptiveProcessor();
    ~AdaptiveProcessor() = default;
    
    // Disable copying
    AdaptiveProcessor(const AdaptiveProcessor&) = delete;
    AdaptiveProcessor& operator=(const AdaptiveProcessor&) = delete;
    
    // Update configuration based on current metrics
    void updateConfiguration(const PerformanceMetrics& metrics);
    
    // Get optimized configuration
    RuntimePerformanceConfig getOptimalConfiguration(const PerformanceMetrics& metrics) const;
    
    // Determine if optimizations should be enabled
    bool shouldEnableMultithreading(const PerformanceMetrics& metrics) const;
    bool shouldEnableSIMD(const PerformanceMetrics& metrics) const;
    bool shouldEnableEventDriven(const PerformanceMetrics& metrics) const;
    bool shouldEnableSparseConnectivity(const PerformanceMetrics& metrics) const;
    
    // Get adaptive thresholds
    double getWorkloadThreshold() const { return workloadThreshold_; }
    double getMemoryPressureThreshold() const { return memoryPressureThreshold_; }
    
    // Set thresholds
    void setWorkloadThreshold(double threshold) { workloadThreshold_ = threshold; }
    void setMemoryPressureThreshold(double threshold) { memoryPressureThreshold_ = threshold; }
    
    // Get singleton instance
    static AdaptiveProcessor& getInstance() {
        static AdaptiveProcessor instance;
        return instance;
    }
    
private:
    RuntimePerformanceConfig defaultConfig_;
    double workloadThreshold_;
    double memoryPressureThreshold_;
    size_t minThreadsToEnableMT_;
};

} // namespace nlm