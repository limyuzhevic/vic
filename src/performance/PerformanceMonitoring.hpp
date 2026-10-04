// Performance Monitoring and Profiling System
// Clean implementation with proper separation of declarations and implementations

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <cmath>

// Forward declarations
namespace nlm {
    class Brain;
    class Config;
    class Logger;
    class RandomGenerator;
}

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    double simulationTime;
    double realTimeFactor;
    size_t neuronsPerSecond;
    size_t synapsesPerSecond;
    size_t spikesPerSecond;
    size_t synapticEventsPerSecond;
    
    double avgStepTimeMs;
    double minStepTimeMs;
    double maxStepTimeMs;
    double stepTimeStdDev;
    
    size_t neuronMemoryMB;
    size_t synapseMemoryMB;
    size_t eventQueueMemoryMB;
    size_t totalMemoryMB;
    size_t peakMemoryMB;
    
    size_t activeNeurons;
    size_t firingNeurons;
    double avgFiringRateHz;
    double spikeDensity;
    
    double synapticWeightMean;
    double synapticWeightStdDev;
    double excitatoryRatio;
    double connectionDensity;
    
    double cpuUtilizationPercent;
    double gpuUtilizationPercent;
    size_t threadCount;
    
    float taskPerformance;
    float learningAccuracy;
    float memoryRetention;
    
    PerformanceMetrics() : simulationTime(0.0), realTimeFactor(0.0),
                          neuronsPerSecond(0), synapsesPerSecond(0),
                          spikesPerSecond(0), synapticEventsPerSecond(0),
                          avgStepTimeMs(0.0), minStepTimeMs(0.0),
                          maxStepTimeMs(0.0), stepTimeStdDev(0.0),
                          neuronMemoryMB(0), synapseMemoryMB(0),
                          eventQueueMemoryMB(0), totalMemoryMB(0),
                          peakMemoryMB(0), activeNeurons(0),
                          firingNeurons(0), avgFiringRateHz(0.0),
                          spikeDensity(0.0), synapticWeightMean(0.0),
                          synapticWeightStdDev(0.0), excitatoryRatio(0.5),
                          connectionDensity(0.0), cpuUtilizationPercent(0.0),
                          gpuUtilizationPercent(0.0), threadCount(1),
                          taskPerformance(0.0), learningAccuracy(0.0),
                          memoryRetention(0.0) {}
    
    std::string toJSON() const;
};

// Real-time performance monitoring system
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void start();
    void stop();
    void recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons);
    
    PerformanceMetrics getCurrentMetrics(size_t windowSize = 100) const;
    PerformanceMetrics getSnapshot() const;
    bool isStable(size_t windowSize = 100, double threshold = 0.1) const;
    void reset();
    
    const std::vector<double>& getStepTimes() const;
    const std::vector<size_t>& getSpikeCounts() const;
    const std::vector<size_t>& getActiveNeuronCounts() const;
    
    bool isRunning() const;
    uint64_t getCurrentStep() const;
    void setCurrentStep(uint64_t step);
    bool shouldSample(uint64_t currentStep, double intervalSteps);

private:
    bool running_;
    uint64_t currentStep_;
    uint64_t totalSteps_;
    uint64_t totalSpikes_;
    size_t activeNeurons_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCounts_;
    std::vector<size_t> activeNeuronCounts_;
    
    mutable std::mutex mutex_;
};

// Neural activity profiler
class NeuralProfiler {
public:
    NeuralProfiler();
    ~NeuralProfiler();
    
    void startProfiling();
    void stopProfiling();
    void recordNeuralActivity(const Brain* brain, SimulationStep step);
    
    struct NeuralProfile;
    NeuralProfile getProfileData() const;
    
    bool isRunning() const;

private:
    bool running_;
    uint64_t startStep_;
    uint64_t currentStep_;
    
    std::map<uint64_t, std::map<NeuronId, float>> neuronActivities_;
    std::vector<std::tuple<uint64_t, NeuronId, float>> spikeHistory_;
    std::vector<std::pair<uint64_t, size_t>> synapticEvents_;
    
    mutable std::mutex mutex_;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    void startProfiling();
    void stopProfiling();
    void record();
    
    size_t getPeakMemoryMB() const;
    size_t getCurrentMemoryMB() const;
    bool isRunning() const;
    
    struct MemoryProfile;
    MemoryProfile getMemoryProfile() const;

private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    size_t peakMemoryMB_;
    size_t currentMemoryMB_;
    
    std::vector<size_t> samples_;
    
    mutable std::mutex mutex_;
};

// Algorithm profiler
class AlgorithmProfiler {
public:
    AlgorithmProfiler();
    ~AlgorithmProfiler();
    
    void start();
    void stop();
    void recordStep(double stepTimeMs, size_t neuronCount, size_t synapseCount,
                   size_t spikeCount, size_t activeNeurons);
    
    struct AlgorithmProfile;
    AlgorithmProfile getProfile() const;
    
    bool isRunning() const;
    size_t getStepCount() const;

private:
    bool running_;
    size_t stepCount_;
    
    std::vector<std::tuple<uint64_t, double, size_t, size_t, size_t, size_t>> stepData_;
    
    mutable std::mutex mutex_;
};

// Performance benchmarking system
class BenchmarkRunner {
public:
    BenchmarkRunner();
    ~BenchmarkRunner();
    
    void start();
    void stop();
    bool isRunning() const;
    
    std::vector<PerformanceMetrics> runBenchmarks(const std::shared_ptr<Brain>& brain,
                                                  const std::vector<uint64_t>& steps,
                                                  const std::string& outputFile = "benchmark.json");
    
    std::vector<PerformanceMetrics> benchmarkConfigurations(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::vector<uint64_t>& steps,
        const std::string& outputFile = "benchmark_compare.json");
    
    std::vector<PerformanceMetrics> benchmarkMemory(
        const std::vector<std::shared_ptr<Config>>& configurations,
        const std::string& outputFile = "benchmark_memory.json");

private:
    bool running_;
    
    void saveBenchmarkResults(const std::vector<PerformanceMetrics>& results,
                             const std::string& outputFile);
    
    void saveConfigurationComparisonResults(const std::vector<PerformanceMetrics>& results,
                                           const std::string& outputFile);
    
    void saveMemoryBenchmarkResults(const std::vector<PerformanceMetrics>& results,
                                    const std::string& outputFile);
};

// Performance analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    struct AnalysisResults;
    AnalysisResults analyze(const std::vector<PerformanceMetrics>& metrics);
    
    struct NeuralAnalysis;
    NeuralAnalysis analyzeNeural(const std::vector<NeuralProfiler::NeuralProfile>& profiles);
    
    struct MemoryAnalysis;
    MemoryAnalysis analyzeMemory(const std::vector<MemoryProfiler::MemoryProfile>& profiles);
};

// Factory functions
inline std::shared_ptr<PerformanceMonitor> createPerformanceMonitor() {
    return std::make_shared<PerformanceMonitor>();
}

inline std::shared_ptr<NeuralProfiler> createNeuralProfiler() {
    return std::make_shared<NeuralProfiler>();
}

inline std::shared_ptr<MemoryProfiler> createMemoryProfiler() {
    return std::make_shared<MemoryProfiler>();
}

inline std::shared_ptr<BenchmarkRunner> createBenchmarkRunner() {
    return std::make_shared<BenchmarkRunner>();
}

inline std::shared_ptr<PerformanceAnalyzer> createPerformanceAnalyzer() {
    return std::make_shared<PerformanceAnalyzer>();
}

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP