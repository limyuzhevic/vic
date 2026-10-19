#pragma once

#include <memory>
#include <string>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"

namespace nlm {

class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double timeSeconds;
        size_t neuronsProcessed;
        float efficiency; // neurons per second
        size_t memoryUsage; // KB
        std::string optimizationLevel;
    };
    
    BenchmarkResult benchmarkMemoryPool(const std::shared_ptr<Config>& config, int iterations = 100);
    BenchmarkResult benchmarkSIMD(const std::shared_ptr<Config>& config, int size = 10000);
    BenchmarkResult benchmarkParallel(const std::shared_ptr<Config>& config, int regions = 4);
    BenchmarkResult benchmarkCheckpoint(const std::shared_ptr<Config>& config, int steps = 500);
    BenchmarkResult benchmarkOverall(const std::shared_ptr<Config>& config);
};

class Phase5Experiment : public Experiment {
public:
    Phase5Experiment();
    ~Phase5Experiment();
    
    bool initialize(const std::shared_ptr<Config>& config) override;
    Phase5Results run(Brain* brain, size_t trials, size_t stepsPerTrial) override;
    void printResults(const Phase5Results& results) const override;
    
    // Phase 5 specific methods
    double runMemoryPoolOptimization(Brain* brain, size_t iterations);
    double runSIMDOptimization(const std::vector<float>& data);
    double runParallelOptimization(Brain* brain, size_t regions);
    double runCheckpointOptimization(Brain* brain, size_t steps);
    
    // Configuration management
    std::shared_ptr<Config> createOptimizedConfig(bool enableMemoryPools = true,
                                                 bool enableSIMD = true,
                                                 bool enableParallel = true,
                                                 bool enableCheckpoints = true,
                                                 const std::string& optimizationLevel = "advanced");
    
    // Performance analysis
    double calculateMemoryEfficiency(size_t neuronsProcessed, double timeSeconds);
    double calculateProcessingThroughput(size_t neuronsProcessed, double timeSeconds);
    double calculateSpeedupFactor(double baselineTime, double optimizedTime);
    
    // Configuration validation
    bool validatePerformanceConfig(const Config& config) const;
    void printConfigurationReport(const Config& config) const;
    
    // Get experiment name
    std::string getName() const override { return "Phase 5: Advanced Performance Optimizations"; }
    
private:
    BenchmarkResult benchmarkNeuralDynamics(const std::shared_ptr<Config>& config, int iterations);
    BenchmarkResult benchmarkMemoryAllocation(const std::shared_ptr<Config>& config, int size);
    BenchmarkResult benchmarkVectorOperations(const std::shared_ptr<Config>& config, int size);
    BenchmarkResult benchmarkThreadPool(const std::shared_ptr<Config>& config, int tasks);
    BenchmarkResult benchmarkFileIO(const std::shared_ptr<Config>& config, const std::string& filename);
    
    class PerformanceMonitor {
    public:
        PerformanceMonitor();
        ~PerformanceMonitor();
        
        void startMeasurement(const std::string& operation);
        void stopMeasurement(const std::string& operation);
        double getMeasurement(const std::string& operation) const;
        std::vector<std::pair<std::string, double>> getAllMeasurements() const;
        void reset();
        
        void enableMemoryTracking() { trackMemory_ = true; }
        void enableTimeTracking() { trackTime_ = true; }
        
        size_t getMaxMemoryUsage() const { return maxMemoryUsage_; }
        double getTotalTime() const { return totalTime_; }
        
    private:
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes_;
        std::unordered_map<std::string, double> durations_;
        bool trackTime_;
        bool trackMemory_;
        size_t maxMemoryUsage_;
        double totalTime_;
    };
    
    PerformanceMonitor performanceMonitor_;
    std::unique_ptr<BenchmarkResult> lastBenchmark_;
};

} // namespace nlm
