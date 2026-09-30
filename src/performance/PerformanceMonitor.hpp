#pragma once

/**
 * PerformanceMonitor.hpp - Real-time performance monitoring and benchmarking
 * 
 * Provides comprehensive performance monitoring for neural simulation.
 * Tracks metrics from memory usage to CPU utilization.
 * 
 * Features:
 * - Real-time metrics collection
 * - Historical event tracking
 * - Benchmarking capabilities
 * - Performance visualization data
 * 
 * Biological motivation:
 * - Like brain monitoring systems that track energy consumption
 * - Neural circuits maintain homeostasis and track their activity
 * - Learning systems monitor their own performance
 */

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include "Types/Types.hpp"
#include "Performance.hpp"

namespace nlm {

// Memory event types
enum class MemoryEventType {
    ALLOCATION,
    DEALLOCATION,
    COMPACTION,
    RECLAIM,
    GROWTH
};

// Neuromodulator types for event tracking
enum class NeuromodulatorType {
    Dopamine,
    Novelty,
    Curiosity,
    Acetylcholine,
    Serotonin
};

// Performance event structures
struct PerformanceEvent {
    std::chrono::steady_clock::time_point timestamp;
    MemoryEventType type;
    std::string details;
    
    PerformanceEvent() : type(MemoryEventType::ALLOCATION) {}
    PerformanceEvent(std::chrono::steady_clock::time_point ts, MemoryEventType t, 
                    const std::string& d = "") : timestamp(ts), type(t), details(d) {}
};

struct CheckpointEvent {
    std::chrono::steady_clock::time_point timestamp;
    size_t neuronsSaved;
    size_t synapsesSaved;
    size_t memorySize;
    
    CheckpointEvent() : neuronsSaved(0), synapsesSaved(0), memorySize(0) {}
    CheckpointEvent(std::chrono::steady_clock::time_point ts, size_t n, size_t s, size_t m)
        : timestamp(ts), neuronsSaved(n), synapsesSaved(s), memorySize(m) {}
};

struct ReplayEvent {
    std::chrono::steady_clock::time_point timestamp;
    size_t episodesReplayed;
    size_t neuronsRestored;
    
    ReplayEvent() : episodesReplayed(0), neuronsRestored(0) {}
    ReplayEvent(std::chrono::steady_clock::time_point ts, size_t e, size_t n)
        : timestamp(ts), episodesReplayed(e), neuronsRestored(n) {}
};

struct DevelopmentEvent {
    std::chrono::steady_clock::time_point timestamp;
    DevelopmentalStage stage;
    size_t neuronsAffected;
    
    DevelopmentEvent() : stage(DevelopmentalStage::Initial), neuronsAffected(0) {}
    DevelopmentEvent(std::chrono::steady_clock::time_point ts, DevelopmentalStage s, size_t n)
        : timestamp(ts), stage(s), neuronsAffected(n) {}
};

struct NeuromodulationEvent {
    std::chrono::steady_clock::time_point timestamp;
    NeuromodulatorType type;
    float level;
    
    NeuromodulationEvent() : type(NeuromodulatorType::Dopamine), level(0.0f) {}
    NeuromodulationEvent(std::chrono::steady_clock::time_point ts, NeuromodulatorType t, float l)
        : timestamp(ts), type(t), level(l) {}
};

struct BenchmarkResult {
    std::string name;
    PerformanceMetrics metrics;
    std::chrono::steady_clock::time_point timestamp;
    
    BenchmarkResult() : name(""), timestamp(std::chrono::steady_clock::now()) {}
    BenchmarkResult(const std::string& n, const PerformanceMetrics& m)
        : name(n), metrics(m), timestamp(std::chrono::steady_clock::now()) {}
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Record simulation step metrics
     */
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount);
    
    /**
     * Record neuron spike event
     */
    void recordSpike(NeuronId neuronId);
    
    /**
     * Record memory system event
     */
    void recordMemoryEvent(MemoryEventType type);
    
    /**
     * Record checkpoint creation
     */
    void recordCheckpoint(size_t neurons, size_t synapses, size_t memorySize);
    
    /**
     * Record replay system activity
     */
    void recordReplay(size_t episodeCount, size_t neuronsRestored);
    
    /**
     * Record development system update
     */
    void recordDevelopmentUpdate(DevelopmentalStage stage, size_t neuronsAffected);
    
    /**
     * Record neuromodulation event
     */
    void recordNeuromodulationEvent(NeuromodulatorType type, float level);
    
    /**
     * Update simulation time
     */
    void recordSimulationTime(double time);
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getMetrics() const;
    
    /**
     * Get memory events history
     */
    std::vector<PerformanceEvent> getMemoryEvents() const;
    
    /**
     * Get checkpoint events history
     */
    std::vector<CheckpointEvent> getCheckpointEvents() const;
    
    /**
     * Get replay events history
     */
    std::vector<ReplayEvent> getReplayEvents() const;
    
    /**
     * Get development events history
     */
    std::vector<DevelopmentEvent> getDevelopmentEvents() const;
    
    /**
     * Get neuromodulation events history
     */
    std::vector<NeuromodulationEvent> getNeuromodulationEvents() const;
    
    /**
     * Reset all metrics
     */
    void reset();
    
    /**
     * Start new performance monitoring session
     */
    void startSession() {
        startTime_ = std::chrono::steady_clock::now();
        metrics_.totalSteps = 0;
        metrics_.totalSpikes = 0;
        metrics_.memoryUsage = 0;
        metrics_.peakMemoryUsage = 0;
        metrics_.simulationTime = 0.0;
    }
    
    /**
     * Check if monitoring is active
     */
    bool isActive() const {
        return std::chrono::steady_clock::now() - startTime_ < std::chrono::minutes(10);
    }
    
private:
    mutable std::mutex mutex_;
    PerformanceMetrics metrics_;
    
    // Historical event tracking
    std::vector<PerformanceEvent> memoryEvents_;
    std::vector<CheckpointEvent> checkpointEvents_;
    std::vector<ReplayEvent> replayEvents_;
    std::vector<DevelopmentEvent> developmentEvents_;
    
    // Spike tracking per neuron
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron_;
    
    // Neuromodulator level history
    std::vector<float> dopamineLevelHistory_;
    std::vector<float> noveltyLevelHistory_;
    std::vector<float> curiosityLevelHistory_;
    
    std::chrono::steady_clock::time_point startTime_;
    
    /**
     * Update memory usage estimate
     */
    void updateMemoryUsage();
    
    /**
     * Update CPU usage estimate
     */
    void updateCpuUsage();
};

class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    // Disable copying
    PerformanceBenchmark(const PerformanceBenchmark&) = delete;
    PerformanceBenchmark& operator=(const PerformanceBenchmark&) = delete;
    
    /**
     * Add a benchmark result
     */
    void addBenchmark(const std::string& name, const PerformanceMetrics& metrics);
    
    /**
     * Get average metrics across all benchmarks
     */
    PerformanceMetrics getAverage() const;
    
    /**
     * Get best performing benchmark
     */
    PerformanceMetrics getBestPerformance() const;
    
    /**
     * Get all benchmark results
     */
    std::vector<BenchmarkResult> getResults() const;
    
    /**
     * Clear all benchmarks
     */
    void clear();
    
    /**
     * Save benchmarks to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Load benchmarks from file
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Export benchmarks to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Compare two benchmarks
     */
    struct BenchmarkComparison {
        std::string benchmark1;
        std::string benchmark2;
        PerformanceMetrics diff;
        std::string interpretation;
    };
    
    BenchmarkComparison compare(const std::string& name1, const std::string& name2) const;
    
private:
    std::vector<BenchmarkResult> results_;
    
    /**
     * Sort results by timestamp
     */
    void sortResults();
    
    /**
     * Calculate performance score (lower is better)
     */
    float calculatePerformanceScore(const PerformanceMetrics& metrics) const;
};

} // namespace nlm
