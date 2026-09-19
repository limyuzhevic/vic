#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <memory>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace nlm {

class BrainProfiler; // Forward declaration

class DebugTools {
public:
    /**
     * Neural network visualization helper data
     */
    struct NetworkSnapshot {
        std::chrono::steady_clock::time_point timestamp;
        std::vector<double> neuronActivity;
        std::vector<double> firingRates;
        std::vector<std::vector<double>> connectivityMatrix;
        std::vector<size_t> activeNeurons;
        std::vector<std::string> regionNames;
        
        NetworkSnapshot(size_t neuronCount)
            : neuronActivity(neuronCount, 0.0), firingRates(neuronCount, 0.0),
              connectivityMatrix(neuronCount, std::vector<double>(neuronCount, 0.0)) {}
    };

    /**
     * Memory leak information
     */
    struct MemoryLeakInfo {
        std::string context;
        size_t allocatedBytes;
        std::chrono::steady_clock::time_point firstAllocation;
        std::chrono::steady_clock::time_point lastAllocation;
        size_t allocationCount;
        bool isActive;  // Still being allocated
        
        MemoryLeakInfo(const std::string& ctx, size_t bytes)
            : context(ctx), allocatedBytes(bytes), allocationCount(1),
              isActive(true) {
            auto now = std::chrono::steady_clock::now();
            firstAllocation = now;
            lastAllocation = now;
        }
    };

    /**
     * Simulation step analysis result
     */
    struct StepAnalysis {
        size_t stepNumber;
        double simulationTime;
        double cpuUsage;           // percentage
        double memoryUsage;        // percentage
        size_t neuronsProcessed;
        size_t spikesProcessed;
        size_t newConnections;
        size_t brokenConnections;
        double plasticityUpdateTime;
        double memoryUpdateTime;
        double attentionUpdateTime;
        std::vector<size_t> spikeRates;
        std::vector<double> neuronActivations;
        
        StepAnalysis(size_t step, double simTime)
            : stepNumber(step), simulationTime(simTime),
              cpuUsage(0.0), memoryUsage(0.0), neuronsProcessed(0),
              spikesProcessed(0), newConnections(0), brokenConnections(0),
              plasticityUpdateTime(0.0), memoryUpdateTime(0.0),
              attentionUpdateTime(0.0) {}
    };

    /**
     * Performance bottleneck information
     */
    struct PerformanceBottleneck {
        enum class BottleneckType {
            HighMemoryUsage,
            SlowStepProcessing,
            ExcessiveSpikeRate,
            LowFiringRate,
            MemoryLeak,
            ConnectivityOverload,
            PlasticitySlow,
            NetworkStability
        };
        
        BottleneckType type;
        std::string description;
        double severity;              // 0.0 - 1.0, where 1.0 is critical
        std::chrono::steady_clock::time_point detectedTime;
        std::string suggestedFix;
        
        PerformanceBottleneck(BottleneckType t, const std::string& desc, double sev)
            : type(t), description(desc), severity(sev),
              detectedTime(std::chrono::steady_clock::now()) {
            suggestedFix = "Investigate and optimize";
        }
    };

    /**
     * State validation result
     */
    struct StateValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::steady_clock::time_point validationTime;
        
        StateValidationResult() : isValid(true), validationTime(std::chrono::steady_clock::now()) {}
    };

    /**
     * DebugTools constructor
     */
    DebugTools();

    /**
     * DebugTools destructor
     */
    ~DebugTools();

    /**
     * Set profiler instance for performance analysis
     */
    void setProfiler(std::shared_ptr<BrainProfiler> profiler) {
        profiler_ = profiler;
    }

    /**
     * Validate brain state for consistency
     */
    StateValidationResult validateState(const class Brain& brain) const;

    /**
     * Check for memory leaks
     */
    std::vector<MemoryLeakInfo> checkMemoryLeaks() const;

    /**
     * Analyze performance for bottlenecks
     */
    std::vector<PerformanceBottleneck> analyzePerformance() const;

    /**
     * Analyze a specific simulation step
     */
    StepAnalysis analyzeStep(size_t step) const;

    /**
     * Get system health metrics
     */
    struct SystemHealth {
        double cpuUsage;              // percentage
        double memoryUsage;           // percentage
        double stabilityScore;        // 0-1
        bool hasMemoryLeaks;          // true if leaks detected
        bool hasPerformanceIssues;    // true if bottlenecks detected
        std::string healthStatus;     // "healthy", "warning", "critical"
        std::vector<std::string> issues;  // list of issues
        
        SystemHealth()
            : cpuUsage(0.0), memoryUsage(0.0), stabilityScore(1.0),
              hasMemoryLeaks(false), hasPerformanceIssues(false),
              healthStatus("healthy") {}
    };

    SystemHealth getSystemHealth() const;

    /**
     * Take a snapshot for visualization
     */
    NetworkSnapshot takeNetworkSnapshot(const class Brain& brain) const;

    /**
     * Get neural network visualization data
     */
    std::string getVisualizationData(const NetworkSnapshot& snapshot) const;

    /**
     * Check neural connectivity
     */
    struct ConnectivityResult {
        bool isConnected;             // overall connectivity
        size_t totalConnections;      // total synapse count
        double averageWeight;         // average synaptic weight
        size_t isolatedNeurons;       // neurons with no connections
        std::vector<size_t> highDegreeNeurons;  // neurons with > 10 connections
        
        ConnectivityResult()
            : isConnected(true), totalConnections(0), averageWeight(0.0),
              isolatedNeurons(0) {}
    };

    ConnectivityResult checkConnectivity(const class Brain& brain) const;

    /**
     * Analyze neuron firing patterns
     */
    struct FiringPatternAnalysis {
        double meanFiringRate;         // average firing rate across all neurons
        double firingRateStdDev;       // standard deviation
        double meanActivation;        // average neural activation
        size_t maxFiringNeuron;        // neuron with highest firing rate
        double maxFiringRate;          // maximum firing rate
        std::vector<size_t> silentNeurons;  // neurons with zero firing
        std::vector<size_t> hyperactiveNeurons;  // neurons with > 100 Hz firing
        
        FiringPatternAnalysis()
            : meanFiringRate(0.0), firingRateStdDev(0.0), meanActivation(0.0),
              maxFiringNeuron(0), maxFiringRate(0.0) {}
    };

    FiringPatternAnalysis analyzeFiringPatterns(const class Brain& brain) const;

    /**
     * Validate neuromodulation signals
     */
    struct NeuromodulationValidation {
        bool dopamineValid;           // dopamine level in expected range
        bool curiosityValid;          // curiosity level in expected range
        bool noveltyValid;             // novelty detection working
        bool predictionErrorValid;    // prediction error signal working
        std::vector<std::string> validationErrors;
        
        NeuromodulationValidation()
            : dopamineValid(true), curiosityValid(true), noveltyValid(true),
              predictionErrorValid(true) {}
    };

    NeuromodulationValidation validateNeuromodulation(const class Brain& brain) const;

    /**
     * Get recent analysis history
     */
    std::vector<StepAnalysis> getStepAnalysisHistory(size_t maxHistory = 100) const;

    /**
     * Get performance bottleneck history
     */
    std::vector<PerformanceBottleneck> getPerformanceHistory(size_t maxHistory = 100) const;

    /**
     * Get memory leak history
     */
    std::vector<MemoryLeakInfo> getMemoryLeakHistory(size_t maxHistory = 100) const;

    /**
     * Generate debugging report
     */
    std::string generateDebugReport() const;

    /**
     * Export debug data to file
     */
    bool exportDebugData(const std::string& filepath) const;

    /**
     * Import debug data from file
     */
    bool importDebugData(const std::string& filepath);

    /**
     * Clear all debug history
     */
    void clearDebugHistory();

    /**
     * Get debug tools version
     */
    std::string getVersion() const { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
