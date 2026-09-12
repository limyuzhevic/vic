// Advanced NLM Performance Benchmark Framework
// Advanced benchmarking system with detailed performance analysis
// Features: Memory tracking, spike analysis, network topology, multi-format output

#pragma once

#include "Config.hpp"
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace nlm {

// Advanced benchmark metrics collection
struct AdvancedBenchmarkResult {
    // Core performance metrics
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;              // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    
    // Advanced performance metrics
    double memoryUsageMB;          // Memory usage in MB
    size_t workingMemoryTraces;    // Active working memory traces
    size_t episodicMemoryEpisodes; // Stored episodes
    float averageFiringRate;       // Average firing rate across neurons
    float maxFiringRate;          // Maximum firing rate
    float minFiringRate;          // Minimum firing rate
    
    // Spike analysis
    size_t totalSpikeCount;        // Total number of spikes
    double spikeFrequency;         // Spikes per second
    double spikeCoefficientOfVaration; // Variability in spike timing
    
    // Network topology metrics
    float averageConnectionStrength; // Average synaptic strength
    float connectionDensity;        // Synapses per possible connections
    size_t regionCount;             // Number of brain regions
    float averageRegionActivity;    // Average activity across regions
    
    // Time-based metrics
    double cpuTime;                // CPU time used
    double wallClockTime;          // Wall clock time
    
    // Step-level metrics
    std::vector<double> stepTimes;       // Time per step
    std::vector<size_t> spikeCountsPerStep; // Spikes per step
    std::vector<float> firingRatesPerStep; // Firing rate per step
};

// Memory usage tracker
class MemoryTracker {
public:
    struct MemorySample {
        double timestamp;
        size_t memoryBytes;
        std::string description;
    };
    
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    
    void recordMemoryUsage(size_t bytes, const std::string& description = "");
    void startTracking();
    void stopTracking();
    double getPeakMemoryMB() const;
    std::vector<MemorySample> getMemoryHistory() const;
    
private:
    std::vector<MemorySample> memoryHistory;
    double startTime = 0.0;
    size_t peakMemoryBytes = 0;
};

// Spike pattern analyzer
class SpikeAnalyzer {
public:
    struct SpikePattern {
        size_t neuronId;
        double firstSpikeTime;
        double lastSpikeTime;
        std::vector<double> spikeTimes;
        double instantaneousFrequency;
        double refractoryPeriod;
    };
    
    SpikeAnalyzer() = default;
    ~SpikeAnalyzer() = default;
    
    void recordSpike(size_t neuronId, double spikeTime);
    void startRecording();
    void stopRecording();
    std::vector<SpikePattern> getSpikePatterns() const;
    double calculateFiringRate(size_t neuronId) const;
    double calculateCoefficientOfVariation(size_t neuronId) const;
    
private:
    std::unordered_map<size_t, SpikePattern> neuronSpikes;
    std::unordered_map<size_t, std::vector<double>> neuronTimes;
    double recordingStartTime = 0.0;
};

// Network topology analyzer
class NetworkTopologyAnalyzer {
public:
    struct NetworkMetrics {
        float averageDegree;             // Average number of connections per neuron
        float clusteringCoefficient;     // Local clustering
        float pathLength;                // Average shortest path length
        float modularity;                // Network modularity
        float synchronizability;         // Network synchronization
        size_t communityCount;           // Number of functional communities
    };
    
    NetworkTopologyAnalyzer() = default;
    ~NetworkTopologyAnalyzer() = default;
    
    void analyzeBrain(const std::shared_ptr<Brain>& brain);
    NetworkMetrics getMetrics() const;
    std::unordered_map<size_t, size_t> getCommunityAssignments() const;
    
private:
    NetworkMetrics metrics;
    std::unordered_map<size_t, size_t> neuronCommunities;
};

// Advanced benchmark runner
class AdvancedBenchmark {
public:
    AdvancedBenchmark() = default;
    ~AdvancedBenchmark() = default;
    
    // Run comprehensive benchmark
    AdvancedBenchmarkResult runBenchmark(
        const std::shared_ptr<Config>& config,
        size_t neuronCount,
        size_t steps,
        const std::string& benchmarkName = "",
        bool trackMemory = true,
        bool analyzeSpikes = true,
        bool analyzeTopology = true
    );
    
    // Run benchmark with multiple configurations
    std::vector<AdvancedBenchmarkResult> runBenchmarkSuite(
        const std::vector<std::pair<size_t, size_t>>& configurations
    );
    
    // Export results
    bool exportResults(const std::string& filepath, 
                      const std::vector<AdvancedBenchmarkResult>& results,
                      const std::string& format = "json");
    
    // Print detailed results
    void printResults(const std::vector<AdvancedBenchmarkResult>& results) const;
    
private:
    MemoryTracker memoryTracker;
    SpikeAnalyzer spikeAnalyzer;
    NetworkTopologyAnalyzer topologyAnalyzer;
};

// Helper functions for advanced benchmarking
namespace BenchmarkUtils {
    double getCurrentTime();
    size_t getCurrentMemoryUsageMB();
    void setupBenchmarkConfig(std::shared_ptr<Config>& config, 
                            size_t neuronCount,
                            const std::string& benchmarkType);
    
    // Output formatting
    void printAdvancedResults(const AdvancedBenchmarkResult& result);
    std::string formatTime(double seconds);
    std::string formatMemory(size_t bytes);
}

} // namespace nlm
