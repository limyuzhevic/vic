#pragma once

#include <chrono>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <atomic>
#include <mutex>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double wallClockTime;           // Total wall clock time in seconds
        double cpuTime;                 // CPU time in seconds
        size_t totalSteps;              // Number of simulation steps
        size_t totalSpikes;             // Total spikes generated
        size_t totalNeurons;            // Total neurons in simulation
        size_t totalSynapses;           // Total synapses in simulation
        double avgFiringRate;           // Average firing rate (Hz)
        double memoryUsageMB;           // Memory usage in MB
        std::map<std::string, double> customMetrics; // Custom metric name -> value
        
        PerformanceMetrics() : wallClockTime(0.0), cpuTime(0.0), totalSteps(0),
                               totalSpikes(0), totalNeurons(0), totalSynapses(0),
                               avgFiringRate(0.0), memoryUsageMB(0.0) {}
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start timing
    void startTimer();
    
    // Stop timing and collect metrics
    PerformanceMetrics stopTimer();
    
    // Record a specific metric
    void recordMetric(const std::string& name, double value);
    
    // Get current metrics
    PerformanceMetrics getMetrics() const;
    
    // Reset metrics
    void reset();
    
    // Generate performance report
    std::string generateReport() const;
    
    // Check if performance monitoring is enabled
    bool isEnabled() const;
    
    // Enable/disable performance monitoring
    void setEnabled(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
