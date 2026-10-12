#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include "Profiler.hpp"
#include "../core/Logger.hpp"

namespace nlm {

// Performance metrics for different aspects of NLM
struct MemoryMetrics {
    size_t totalMemoryUsage;
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    size_t allocatedBytes;
    size_t deallocatedBytes;
    size_t currentAllocationCount;
    float fragmentationRatio;
    Timestamp lastCollectionTime;
    uint64_t collectionCount;
};

struct NetworkMetrics {
    size_t totalNeurons;
    size_t totalSynapses;
    float connectionDensity;
    float spikeRate;
    float averageFiringRate;
    size_t activeNeurons;
    float plasticityFactor;
    float neuromodulationLevel;
    std::map<std::string, float> regionMetrics;
};

struct IODMetrics {
    float cpuUtilization;
    float gpuUtilization;
    float memoryBandwidth;
    float diskReadSpeed;
    float diskWriteSpeed;
    float networkInSpeed;
    float networkOutSpeed;
    std::map<std::string, float> componentIO;
};

struct PerformanceProfile {
    Timestamp timestamp;
    MemoryMetrics memory;
    NetworkMetrics network;
    IODMetrics io;
    float totalPerformanceScore;
    std::vector<std::string> performanceIssues;
};

// Component-specific optimization hints
struct OptimizationHint {
    std::string componentName;
    std::string optimizationType; // "memory", "cache", "parallel", "vectorization"
    std::string description;
    double potentialImprovement; // Percentage
    bool isImplemented;
    bool isCritical;
    Timestamp suggestedTime;
};

// Performance bottleneck analysis
struct BottleneckAnalysis {
    std::string bottleneckType; // "memory", "cpu", "io", "network"
    std::string componentName;
    double severity; // 0.0 to 1.0
    double impactOnThroughput; // Percentage
    double estimatedImprovement; // Percentage
    std::vector<std::string> rootCauses;
    std::vector<std::string> suggestedSolutions;
    bool isResolved;
};

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();

    // Initialization and configuration
    bool initialize();
    void shutdown();

    // Performance data collection
    void recordPerformanceSample(const PerformanceProfile& profile);
    void analyzeComponentPerformance(const std::string& componentName);
    void detectMemoryLeaks();
    void analyzeNeuralNetworkTopology();

    // Bottleneck detection and analysis
    void identifyBottlenecks();
    void analyzeSpikePropagationBottlenecks();
    void analyzeMemorySystemBottlenecks();
    void analyzePlasticityComputationBottlenecks();
    void analyzeIntegrationLoopBottlenecks();

    // Optimization analysis
    void generateOptimizationHints();
    void analyzeMemoryAllocationPatterns();
    void analyzeCacheMissPatterns();
    void analyzeParallelizationOpportunities();

    // Performance reports and analysis
    void generatePerformanceReport(const std::string& filename);
    void exportPerformanceData(const std::string& filename);
    void generateOptimizationPlan(const std::string& filename);

    // Query and analysis methods
    const PerformanceProfile& getCurrentProfile() const;
    const std::vector<PerformanceProfile>& getPerformanceHistory() const;
    const std::vector<OptimizationHint>& getOptimizationHints() const;
    const std::vector<BottleneckAnalysis>& getBottleneckAnalysis() const;

    // Configuration and settings
    void setAnalysisInterval(double seconds);
    void setMaxHistoricalData(size_t maxDataPoints);
    void enableMemoryLeakDetection(bool enable);
    void enableBottleneckDetection(bool enable);
    void enableOptimizationSuggestions(bool enable);

    // Statistics
    size_t getSampleCount() const;
    size_t getBottleneckCount() const;
    size_t getOptimizationHintCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm