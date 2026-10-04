#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include "FunctionCallProfiler.hpp"
#include "AlgorithmProfiler.hpp"
#include "BottleneckDetector.hpp"
#include "DebuggingSystem.hpp"

namespace nlm {

// PerformanceProfiler: Core performance profiling system
class PerformanceProfiler {
public:
    PerformanceProfiler();
    explicit PerformanceProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~PerformanceProfiler();
    
    // Configuration
    void setProfilingInterval(double interval) { profilingInterval_ = interval; }
    void setAutoDetectBottlenecks(bool enable) { autoDetectBottlenecks_ = enable; }
    void setMinSamples(size_t minSamples) { minSamples_ = minSamples; }
    void setEnableFunctionProfiling(bool enable) { enableFunctionProfiling_ = enable; }
    void setEnableAlgorithmProfiling(bool enable) { enableAlgorithmProfiling_ = enable; }
    
    // Integration with other profilers
    void setFunctionCallProfiler(std::shared_ptr<FunctionCallProfiler> profiler) { 
        functionCallProfiler_ = profiler; 
    }
    void setAlgorithmProfiler(std::shared_ptr<AlgorithmProfiler> profiler) { 
        algorithmProfiler_ = profiler; 
    }
    void setBottleneckDetector(std::shared_ptr<BottleneckDetector> detector) { 
        bottleneckDetector_ = detector; 
    }
    
    // Performance measurement
    PerformanceMeasurement startMeasurement(const std::string& operation, const std::string& context = "");
    void stopMeasurement(PerformanceMeasurement& measurement);
    void recordPerformance(const std::string& operation, double time, const std::string& context = "");
    
    // Function call profiling
    void recordFunctionCall(const std::string& functionName, double time, 
                           const std::string& context = "");
    
    // Algorithm profiling
    void recordAlgorithmOperation(const std::string& algorithmName, 
                                 const std::string& operation, double time,
                                 const std::string& parameters = "");
    
    // Query results
    double getAverageTime(const std::string& operation) const;
    double getMinTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    double getStandardDeviation(const std::string& operation) const;
    
    // Operation listing
    std::vector<std::string> getOperations() const;
    std::vector<std::string> getFunctionCalls() const;
    std::vector<std::string> getAlgorithmOperations() const;
    size_t getSampleCount(const std::string& operation) const;
    
    // Analysis
    std::string analyzeBottlenecks() const;
    std::string analyzeFunctionLevelBottlenecks() const;
    std::string analyzeAlgorithmLevelBottlenecks() const;
    std::string analyzeTrends() const;
    std::string analyzeCorrelations(const std::string& operation1, 
                                  const std::string& operation2) const;
    
    // Resource monitoring
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        size_t memoryAllocations;
        size_t threadCount;
        std::chrono::system_clock::time_point timestamp;
    };
    
    void recordResourceUsage(double cpu, double memory, size_t allocations, size_t threads);
    std::vector<ResourceUsage> getResourceHistory() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { 
        debuggingSystem_ = debuggingSystem; 
    }
    
    // Export
    bool exportProfile(const std::string& filepath) const;
    std::string exportProfileAsString() const;
    
    // Statistics
    size_t getTotalMeasurements() const { return totalMeasurements_; }
    size_t getTotalFunctionCalls() const;
    size_t getTotalAlgorithmOperations() const;
    double getTotalTime() const { return totalTime_; }
    
    // Combined analysis
    std::string getComprehensivePerformanceReport() const;
    std::string getOptimizationRecommendations() const;
    
private:
    struct Measurement {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        double time;
        std::string context;
    };
    
    std::vector<Measurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    std::shared_ptr<FunctionCallProfiler> functionCallProfiler_;
    std::shared_ptr<AlgorithmProfiler> algorithmProfiler_;
    std::shared_ptr<BottleneckDetector> bottleneckDetector_;
    
    double profilingInterval_;
    bool autoDetectBottlenecks_;
    bool enableFunctionProfiling_;
    bool enableAlgorithmProfiling_;
    size_t minSamples_;
    
    std::vector<ResourceUsage> resourceHistory_;
    mutable std::mutex resourceMutex_;
    
    size_t totalMeasurements_;
    double totalTime_;
    
    // Helper methods
    void updateBottleneckDetection(const std::string& operation, double time);
    bool isBottleneck(const std::string& operation, double avgTime) const;
    double calculateStandardDeviation(const std::vector<double>& values) const;
    std::string generateDetailedReport() const;
};

} // namespace nlm
