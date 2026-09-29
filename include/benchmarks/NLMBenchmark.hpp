#pragma once

#include <string>
#include <vector>
#include <memory>
#include "brain/Brain.hpp"

namespace nlm {
namespace benchmarks {

// Benchmark results structures
struct BrainBenchmarkResult {
    uint64_t steps = 0;
    double wallClockTime = 0.0;
    double stepsPerSecond = 0.0;
    
    // Brain statistics
    size_t initialNeuronCount = 0;
    size_t initialSynapseCount = 0;
    size_t initialSpikeCount = 0;
    size_t finalNeuronCount = 0;
    size_t finalSynapseCount = 0;
    size_t finalSpikeCount = 0;
    uint64_t spikesGenerated = 0;
    
    // Performance metrics
    size_t firingNeurons = 0;
    size_t memoryTraces = 0;
    size_t memoryEpisodes = 0;
    double averageFiringRate = 0.0;
    double averageMemoryTraces = 0.0;
    double averageMemoryEpisodes = 0.0;
};

struct NeuralBenchmarkResult {
    uint64_t steps = 0;
    double wallClockTime = 0.0;
    double stepsPerSecond = 0.0;
    
    // Neural performance
    uint64_t totalSpikes = 0;
    size_t firingNeurons = 0;
    double spikesPerSecond = 0.0;
    double averageFiringRate = 0.0;
    double plasticityRate = 0.0;
    
    // Weight statistics
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
};

struct DevelopmentBenchmarkResult {
    uint64_t steps = 0;
    double wallClockTime = 0.0;
    double stepsPerSecond = 0.0;
    
    // Development metrics
    DevelopmentalStage initialStage = DevelopmentalStage::Initial;
    DevelopmentalStage finalStage = DevelopmentalStage::Initial;
    uint32_t stageChanges = 0;
    double developmentEfficiency = 0.0;
};

struct ResourceBenchmarkResult {
    uint64_t steps = 0;
    double wallClockTime = 0.0;
    double stepsPerSecond = 0.0;
    
    // Resource efficiency
    size_t initialMemoryUsage = 0;
    size_t finalMemoryUsage = 0;
    double memoryUsagePerNeuron = 0.0;
    double memoryUsagePerSynapse = 0.0;
    double memoryEfficiency = 0.0;
};

struct ScalabilityMetrics {
    std::vector<double> neuronScaling;
    std::vector<double> regionScaling;
    std::vector<double> connectionScaling;
    std::vector<double> memoryScaling;
};

struct ScaleTestConfig {
    std::vector<size_t> neuronCounts;
    std::vector<size_t> regionCounts;
    float connectionProbability = 0.1f;
    uint64_t simulationSteps = 1000;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
};

// Main benchmark suite
class NLMBenchmark {
public:
    NLMBenchmark();
    ~NLMBenchmark();
    
    void initialize();
    void shutdown();
    
    // Individual benchmark functions
    BrainBenchmarkResult benchmarkBrainPerformance(std::shared_ptr<Brain> brain, uint64_t steps);
    NeuralBenchmarkResult benchmarkNeuralPerformance(std::shared_ptr<Brain> brain, uint64_t steps);
    DevelopmentBenchmarkResult benchmarkDevelopmentPerformance(std::shared_ptr<Brain> brain, uint64_t steps);
    ResourceBenchmarkResult benchmarkResourceEfficiency(std::shared_ptr<Brain> brain, uint64_t steps);
    ScalabilityMetrics benchmarkScalability(const ScaleTestConfig& config);
    
    // Comprehensive benchmark suite
    void runAllBenchmarks();
    
    // Results
    BrainBenchmarkResult getBrainBenchmark() const { return brainBenchmark; }
    NeuralBenchmarkResult getNeuralBenchmark() const { return neuralBenchmark; }
    DevelopmentBenchmarkResult getDevelopmentBenchmark() const { return developmentBenchmark; }
    ResourceBenchmarkResult getResourceBenchmark() const { return resourceBenchmark; }
    ScalabilityMetrics getScalabilityBenchmark() const { return scalabilityBenchmark; }
    
    double getWallClockTime() const { return wallClockDuration; }
    double getCPUTime() const { return cpuDuration; }
    
    void printBenchmarkSummary();
    void printDetailedResults();
    
private:
    // Helper functions
    double calculatePlasticityRate(const std::vector<float>& initialWeights, 
                                  const std::vector<float>& finalWeights, 
                                  double elapsedTime);
    
    std::vector<double> benchmarkNeuronScaling(const ScaleTestConfig& config);
    std::vector<double> benchmarkRegionScaling(const ScaleTestConfig& config);
    std::vector<double> benchmarkConnectionScaling(const ScaleTestConfig& config);
    std::vector<double> benchmarkMemoryScaling(const ScaleTestConfig& config);
    
    // Benchmark results
    BrainBenchmarkResult brainBenchmark;
    NeuralBenchmarkResult neuralBenchmark;
    DevelopmentBenchmarkResult developmentBenchmark;
    ResourceBenchmarkResult resourceBenchmark;
    ScalabilityMetrics scalabilityBenchmark;
    
    // Timing
    double wallClockDuration = 0.0;
    double cpuDuration = 0.0;
    std::chrono::high_resolution_clock::time_point startWallClock;
    std::chrono::steady_clock::time_point startCpuClock;
};

// Standalone function
void runNLMBenchmarks();

} // namespace benchmarks
} // namespace nlm