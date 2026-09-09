#ifndef NLM_PERFORMANCE_MONITER_HPP
#define NLM_PERFORMANCE_MONITER_HPP

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Brain.hpp"

namespace nlm {

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    
    void recordEvent(const std::string& eventName);
    double getEventDuration(const std::string& eventName) const;
    
    // Brain performance metrics
    struct BrainMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikeCount;
        size_t pendingSpikeCount;
        double lastStepDuration;
    };
    
    BrainMetrics getBrainMetrics() const;
    
    // Memory usage
    struct MemoryStats {
        size_t totalMemory;
        size_t usedMemory;
        size_t peakMemory;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Performance profiling
    void enableProfiling(bool enabled);
    bool isProfilingEnabled() const;
    
    // Benchmarking utilities
    double benchmarkStep(std::function<void()> stepFunction);
    double benchmarkSequence(std::function<void()> sequenceFunction, size_t repetitions = 10);
    
    // Configuration
    void setSamplingRate(double rate);  // samples per step
    double getSamplingRate() const;
    
    // Callbacks
    using MetricsCallback = std::function<void(const BrainMetrics&)>;
    void setMetricsCallback(MetricsCallback callback);
    
    // Statistics
    size_t getSampleCount() const;
    double getAverageStepDuration() const;
    double getStandardDeviationStepDuration() const;
    
    // Reset
    void reset();
    
    // Output
    void logMetrics() const;
    void saveMetricsToFile(const std::string& filepath) const;
    
private:
    struct EventRecord {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double duration;
    };
    
    std::unordered_map<std::string, std::vector<EventRecord>> eventHistory_;
    bool monitoringEnabled_;
    bool profilingEnabled_;
    double samplingRate_;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Metrics collection
    std::vector<double> stepDurations_;
    std::vector<BrainMetrics> metricHistory_;
    
    // Callbacks
    MetricsCallback metricsCallback_;
    
    // Memory tracking (placeholder for now)
    size_t peakMemory_;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITER_HPP