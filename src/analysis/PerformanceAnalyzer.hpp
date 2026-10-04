#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

class PerformanceAnalyzer {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double gpuUsage;
        double networkBandwidth;
        size_t activeThreadCount;
        size_t queueSize;
        double processingLatency;
        double throughput;
        double bottleneckScore;
    };
    
    struct ResourceUsage {
        double cpuUtilization;
        size_t memoryFootprint;
        size_t allocatedMemory;
        std::chrono::milliseconds cpuTime;
        std::chrono::milliseconds wallClockTime;
    };
    
    struct BottleneckAnalysis {
        std::string bottleneckType;
        double severity;
        std::string location;
        std::vector<std::string> suggestions;
        std::chrono::milliseconds estimatedFixTime;
    };
    
    static std::unique_ptr<PerformanceAnalyzer> create() {
        return std::make_unique<PerformanceAnalyzer>();
    }
    
    // Performance monitoring
    PerformanceMetrics collectPerformanceMetrics() const;
    ResourceUsage measureResourceUsage() const;
    
    // Bottleneck analysis
    std::vector<BottleneckAnalysis> analyzeBottlenecks(const Brain& brain) const;
    std::vector<BottleneckAnalysis> analyzeMemoryUsage(const Brain& brain) const;
    std::vector<BottleneckAnalysis> analyzeComputation(const Brain& brain) const;
    
    // Performance profiling
    std::vector<double> profileSimulationSteps(size_t numSteps) const;
    double profileMemoryAllocation(size_t allocationSize) const;
    std::chrono::milliseconds profileComputation(size_t operations) const;
    
    // Performance optimization
    std::vector<std::string> getOptimizationSuggestions(const PerformanceMetrics& metrics) const;
    bool applyPerformanceOptimization(const std::string& optimization) const;
    
    // Performance analysis export
    bool exportPerformanceMetrics(const std::string& filepath, const PerformanceMetrics& metrics) const;
    bool exportBottleneckAnalysis(const std::string& filepath, 
                                  const std::vector<BottleneckAnalysis>& bottlenecks) const;
    bool exportResourceUsage(const std::string& filepath, const ResourceUsage& usage) const;
    
private:
    PerformanceAnalyzer() = default;
    
    // Helper methods for performance analysis
    double calculateCPUUsage() const;
    double calculateMemoryUsage() const;
    double calculateGPUUsage() const;
    double calculateNetworkBandwidth() const;
    std::chrono::milliseconds calculateProcessingLatency() const;
    double calculateThroughput(size_t operations, 
                               const std::chrono::milliseconds& duration) const;
    double calculateBottleneckScore(const std::vector<BottleneckAnalysis>& bottlenecks) const;
};

} // namespace nlm