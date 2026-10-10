#pragma once

#include "BenchmarkRunner.hpp"
#include "PerformanceProfiler.hpp"
#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>
#include <cmath>

namespace nlm {

class PerformanceBenchmarker : public Benchmark {
private:
    std::string benchmarkName;
    BenchmarkCategory category;
    std::unique_ptr<PerformanceProfiler> profiler;
    
    // Benchmark configuration
    size_t neuronCount;
    size_t synapseCount;
    size_t simulationSteps;
    double injectionCurrent;
    double stdpLtpWeight;
    double stdpLtdWeight;
    
    // Benchmark state
    bool isSetup;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::mutex benchmarkMutex;
    
    // Performance data collection
    struct DetailedMetrics {
        double cpuUsage;
        size_t memoryUsage;
        double threadUsage;
        double ioUsage;
        double simulationStepTime;
        size_t spikesGenerated;
        size_t activeNeurons;
        size_t activeSynapses;
        double synapticActivity;
        double networkStability;
        std::vector<double> stepTimes;
        std::vector<size_t> memoryUsageHistory;
    };
    
    DetailedMetrics detailedMetrics;
    
    // Platform-specific performance measurement
    double measureCpuUsage() const;
    size_t measureMemoryUsage() const;
    double measureThreadUsage() const;
    double measureIoUsage() const;
    
public:
    PerformanceBenchmarker(size_t neurons = 1000, 
                           size_t steps = 1000,
                           double current = 30.0,
                           double ltp = 0.01,
                           double ltd = 0.012);
    ~PerformanceBenchmarker() override = default;
    
    // Benchmark interface implementation
    std::string getName() const override;
    BenchmarkCategory getCategory() const override;
    void setup() override;
    BenchmarkMetrics run() override;
    void teardown() override;
    bool validate() const override;
    
    // Configuration
    void setNeuronCount(size_t count);
    void setSimulationSteps(size_t steps);
    void setInjectionCurrent(double current);
    void setStdpParameters(double ltp, double ltd);
    
    size_t getNeuronCount() const;
    size_t getSimulationSteps() const;
    double getInjectionCurrent() const;
    std::pair<double, double> getStdpParameters() const;
    
    // Performance measurement
    double getCpuUsage() const;
    size_t getMemoryUsage() const;
    double getThreadUsage() const;
    double getIoUsage() const;
    
    // Simulation performance
    double getStepsPerSecond() const;
    double getMemoryPerStep() const;
    double getSpikeRate() const;
    double getSimulationEfficiency() const;
    
    // Performance analysis
    double calculatePerformanceScore() const;
    double calculateResourceUtilization() const;
    double calculateSimulationQuality() const;
    bool validatePerformanceTargets() const;
    
    // Data collection
    void recordStepMetrics(double stepTime, size_t neurons, size_t synapses, size_t spikes);
    void recordMemoryUsage(size_t memory);
    void recordCpuUsage(double cpu);
    
    // Advanced benchmarking
    BenchmarkMetrics runWithProfiling() const;
    std::vector<BenchmarkMetrics> runMultipleIterations(size_t iterations) const;
    double calculateStatisticalSignificance(const std::vector<BenchmarkMetrics>& results) const;
    
    // Hardware-specific benchmarking
    void enableHardwareProfiling();
    void disableHardwareProfiling();
    bool isHardwareProfilingEnabled() const;
    
    // Scalability benchmarking
    BenchmarkMetrics runScalabilityTest(size_t neuronMultiplier) const;
    std::vector<BenchmarkMetrics> runScalabilitySuite(const std::vector<size_t>& multipliers) const;
    
    // Performance regression detection
    bool detectRegression(const BenchmarkMetrics& baseline) const;
    double calculatePerformanceImprovement(const BenchmarkMetrics& baseline) const;
    
private:
    void initializeProfiler();
    void cleanupProfiler();
    BenchmarkMetrics convertDetailedMetricsToBenchmark(const DetailedMetrics& detailed) const;
};

} // namespace nlm