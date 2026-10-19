#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Phase 6 integration test result
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
};

/**
 * Configuration for Phase 6 integration experiment
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
};

struct Phase6BenchmarkResult {
    // Performance metrics
    double totalWallClockTime;
    double simulationTime;
    double checkpointTime;
    double loadTime;
    
    // Computational metrics
    size_t totalSpikesGenerated;
    size_t totalSynapticChanges;
    size_t totalMemoryOperations;
    size_t totalNeuromodulationOperations;
    
    // Memory usage
    size_t peakMemoryUsage;
    
    // Throughput metrics
    double stepsPerSecond;
    double spikesPerSecond;
    double synapticChangesPerSecond;
    
    Phase6BenchmarkResult()
        : totalWallClockTime(0.0)
        , simulationTime(0.0)
        , checkpointTime(0.0)
        , loadTime(0.0)
        , totalSpikesGenerated(0)
        , totalSynapticChanges(0)
        , totalMemoryOperations(0)
        , totalNeuromodulationOperations(0)
        , peakMemoryUsage(0)
        , stepsPerSecond(0.0)
        , spikesPerSecond(0.0)
        , synapticChangesPerSecond(0.0) {}
};

/**
 * Configuration for Phase 6 benchmarking
 */
struct Phase6BenchmarkConfig {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6BenchmarkConfig()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_bench.bin") {}
};

/**
 * Phase 6 benchmark runner
 */
class Phase6BenchmarkRunner {
public:
    Phase6BenchmarkRunner();
    ~Phase6BenchmarkRunner();
    
    /**
     * Run performance benchmarks
     */
    Phase6BenchmarkResult runBenchmarks(const Phase6BenchmarkConfig& config);
    
    /**
     * Run benchmark with multiple configurations
     */
    std::vector<Phase6BenchmarkResult> runBenchmarkSuite(const std::vector<Phase6BenchmarkConfig>& configs);
    
    /**
     * Print benchmark results
     */
    void printBenchmarkResults(const Phase6BenchmarkResult& result, const std::string& description);
    
    /**
     * Print benchmark suite results
     */
    void printBenchmarkSuiteResults(const std::vector<Phase6BenchmarkResult>& results);
};

} // namespace nlm
