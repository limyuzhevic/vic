#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <functional>
#include <thread>
#include <mutex>
#include <limits>
#include <cmath>
#include <string>

namespace nlm {

// Memory Analysis Data Structures

struct MemoryAllocation {
    uint64_t timestamp;
    size_t size;
    std::string type;
    std::string source;
    SimulationStep step;
};

struct MemoryProfile {
    std::string name;
    size_t allocationCount;
    double averageAllocationTime;
    size_t peakMemoryUsage;
    double memoryGrowthRate;
    std::vector<MemoryAllocation> allocationHistory;
    double fragmentationLevel;
    size_t liveAllocs;
};

struct LeakAnalysis {
    bool leakDetected;
    double leakRate;
    std::string growthTrend;
    std::vector<uint64_t> leakedObjects;
    std::string suggestedCleanup;
};

struct FragmentationAnalysis {
    double fragmentationLevel;
    bool needsDefragmentation;
    size_t wastedMemory;
    double optimizationPotential;
    std::vector<size_t> holeSizes;
};

struct UsagePattern {
    SimulationStep peakUsageTime;
    double periodicity;
    double predictiveCompressionRatio;
    double compressionEfficiency;
    std::vector<double> usageHistory;
};

struct MemoryStats {
    size_t totalMemoryUsage;
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    size_t associativeMemoryUsage;
    double memoryEfficiency;
    double utilizationRatio;
    size_t capacity;
};

struct ConsolidationStats {
    double peakConsolidation;
    SimulationStep timeToPeak;
    double halfLife;
    double finalConsolidation;
};

struct ReplayStats {
    double effectiveness;
    double patternPreservation;
    double integrationScore;
    size_t replayCount;
};

struct BenchmarkResult {
    double allocationsPerSecond;
    double averageAllocationTime;
    double peakThroughput;
    double efficiencyScore;
};

struct MemoryPrediction {
    size_t predictedMemoryUsage;
    double confidence;
    size_t uncertainty;
    std::vector<size_t> predictedTrends;
};

struct OptimizationRecommendation {
    std::string area;
    std::string suggestion;
    double potentialImprovement;
    std::string implementationComplexity;
};

class MemoryAnalysisTools {
public:
    MemoryAnalysisTools();
    ~MemoryAnalysisTools();

    // Core analysis methods
    MemoryStats analyzeWorkingMemory(const NeuralWorkingMemory* workingMemory) const;
    MemoryStats analyzeEpisodicMemory(const NeuralEpisodicMemory* episodicMemory) const;
    ConsolidationStats analyzeConsolidation(const std::vector<double>& consolidationMetrics) const;
    ReplayStats analyzeReplay(const std::vector<const EpisodicMemoryItem*>& episodes) const;

    // Memory profiling
    MemoryProfile profileMemoryAllocation(const std::string& name,
                                          const std::function<void()>& allocationFunc) const;

    // Memory leak detection
    LeakAnalysis detectMemoryLeaks(const std::vector<uint64_t>& createdObjects,
                                  const std::vector<uint64_t>& deletedObjects) const;

    // Memory fragmentation analysis
    FragmentationAnalysis analyzeFragmentation(const NeuralWorkingMemory* workingMemory) const;

    // Memory optimization
    UsagePattern analyzeUsagePatterns(const NeuralWorkingMemory* workingMemory,
                                      const NeuralEpisodicMemory* episodicMemory) const;
    std::vector<OptimizationRecommendation> generateOptimizationRecommendations() const;

    // Memory prediction
    MemoryPrediction predictMemoryUsage(size_t neurons, size_t regions,
                                        float connectionProbability) const;

    // Configuration analysis
    MemoryStats analyzeMemoryUsage(const Config& config) const;

    // Benchmarking
    BenchmarkResult benchmarkMemoryAllocation(const Config& config) const;

    // Export and reporting
    void exportAnalysis(const std::string& filepath) const;
    std::string generateOptimizationReport() const;

    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isMonitoring() const { return isMonitoring_; }

    // Integration with NLM system
    void integrateWithBrain(Brain* brain);
    void updateAnalysisData();

private:
    // Memory profiling implementation
    MemoryProfile performMemoryProfile(const std::string& name,
                                       const std::function<void()>& allocationFunc) const;

    // Memory leak detection implementation
    LeakAnalysis performLeakAnalysis(const std::vector<uint64_t>& createdObjects,
                                    const std::vector<uint64_t>& deletedObjects) const;

    // Memory fragmentation analysis implementation
    FragmentationAnalysis performFragmentationAnalysis(const NeuralWorkingMemory* workingMemory) const;

    // Memory optimization implementation
    UsagePattern performUsagePatternAnalysis(const NeuralWorkingMemory* workingMemory,
                                            const NeuralEpisodicMemory* episodicMemory) const;
    std::vector<OptimizationRecommendation> performOptimizationRecommendations() const;

    // Memory prediction implementation
    MemoryPrediction performMemoryPrediction(size_t neurons, size_t regions,
                                            float connectionProbability) const;

    // Configuration analysis implementation
    MemoryStats performConfigAnalysis(const Config& config) const;

    // Benchmarking implementation
    BenchmarkResult performBenchmark(const Config& config) const;

    // Export implementation
    void performExport(const std::string& filepath) const;
    std::string performReportGeneration() const;

    // Real-time monitoring implementation
    void runMonitoringLoop();
    void collectMemoryMetrics();

    // Helper methods
    double calculateUtilizationRatio(size_t current, size_t capacity) const;
    double calculateFragmentation(const std::vector<size_t>& holeSizes) const;
    double calculateLeakRate(size_t created, size_t deleted, size_t total) const;
    std::string determineGrowthTrend(const std::vector<size_t>& values) const;

    // Data structures for analysis
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Real-time monitoring state
    bool isMonitoring_;
    std::thread monitoringThread_;
    mutable std::mutex mutex_;

    // Integration state
    Brain* brain_;
    SimulationStep currentStep_;
    std::chrono::steady_clock::time_point startTime_;
};