#pragma once

/**
 * Performance Monitoring Infrastructure
 * 
 * This module provides comprehensive real-time performance monitoring,
 * profiling, benchmarking, and analysis tools for NLM neural simulations.
 * 
 * Features:
 * - Real-time system metrics (CPU, memory, I/O)
 * - Neural computation profiling
 * - Memory usage tracking and optimization
 * - Network performance monitoring
 * - Simulation progress tracking
 * - Line-by-line and function call profiling
 * - Memory allocation profiling
 * - Spike timing analysis
 * - Plasticity event profiling
 * - Neuromodulation profiling
 * - Standard benchmark suite
 * - Performance comparison tools
 * - Scalability analysis
 * - Resource utilization analysis
 * - Activity pattern analysis
 * - Network topology analysis
 * - Learning curve visualization
 * - Performance trend analysis
 * - Memory pool optimization analysis
 * - SIMD optimization profiling
 * - Parallel processing analysis
 * - Cache optimization analysis
 * - Thread synchronization analysis
 * - Real-time performance dashboards
 * - Historical performance graphs
 * - Network performance visualization
 * - Learning progress tracking
 * - Resource usage monitoring
 * 
 * Integration:
 * - Seamless integration with existing Brain.cpp performance metrics
 * - Python bindings for all monitoring APIs
 * - CLI commands for performance operations
 * - Comprehensive documentation
 */

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <functional>
#include <numeric>

// System performance metrics
struct SystemMetrics {
    double cpuUsage;           // CPU usage percentage
    size_t memoryUsage;        // Memory usage in bytes
    size_t memoryPeak;         // Peak memory usage
    uint64_t ioReads;          // I/O read operations
    uint64_t ioWrites;         // I/O write operations
    double cpuTemperature;     // CPU temperature (if available)
    size_t diskUsage;          // Disk usage in bytes
    size_t networkIn;          // Network input in bytes
    size_t networkOut;         // Network output in bytes
    Timestamp timestamp;       // When metrics were collected
};

// Neural computation metrics
struct NeuralMetrics {
    size_t neuronCount;        // Total number of neurons
    size_t activeNeuronCount;  // Number of active neurons
    size_t firingNeuronCount;  // Number of firing neurons
    size_t totalSpikes;        // Total spikes in simulation
    size_t spikesPerSecond;    // Current spike rate
    double averageFiringRate;  // Average firing rate across all neurons
    float excitationInhibitionRatio; // E/I balance
    std::vector<double> firingRateDistribution; // Histogram of firing rates
    double synapticActivity;   // Average synaptic activity level
    Timestamp timestamp;       // When metrics were collected
};

// Plasticity metrics
struct PlasticityMetrics {
    size_t plasticityEvents;   // Number of plasticity events
    double weightChangeSum;    // Sum of all weight changes
    double weightChangeMax;    // Maximum weight change
    double learningRate;       // Current learning rate
    double homeostaticPlasticity; // Homeostatic plasticity level
    double structuralPlasticityRate; // Synaptogenesis/pruning rate
    Timestamp timestamp;       // When metrics were collected
};

// Memory system metrics
struct MemoryMetrics {
    size_t workingMemoryCapacity;      // Working memory capacity
    size_t workingMemoryActiveItems;   // Active working memory traces
    double workingMemoryUtilization;   // Working memory utilization (0-1)
    size_t episodicMemoryEpisodes;    // Number of episodic memory episodes
    double episodicMemoryUtilization; // Episodic memory utilization
    size_t associativeMemoryCapacity; // Associative memory capacity
    double associativeMemoryUtilization; // Associative memory utilization
    size_t memoryAllocationCount;      // Number of memory allocations
    size_t memoryDeallocationCount;    // Number of memory deallocations
    Timestamp timestamp;               // When metrics were collected
};

// Prediction system metrics
struct PredictionMetrics {
    double predictionAccuracy;       // Prediction accuracy
    double predictionErrorRate;       // Prediction error rate
    double predictionConfidence;      // Average prediction confidence
    double rewardPredictionError;     // Reward prediction error (dopamine)
    double curiosityDrive;            // Curiosity level
    double noveltySensitivity;        // Novelty sensitivity
    Timestamp timestamp;              // When metrics were collected
};

// Cognitive system metrics
struct CognitiveMetrics {
    double attentionLevel;            // Attention level (0-1)
    double planningHorizon;           // Planning horizon in steps
    double conceptFormationRate;       // Rate of new concept formation
    double workingMemoryUpdates;       // Number of working memory updates
    double episodicMemoryIntegrations; // Number of episodic memory integrations
    Timestamp timestamp;              // When metrics were collected
};

// Development system metrics
struct DevelopmentMetrics {
    int developmentalStage;            // Current developmental stage
    double plasticityRate;             // Current plasticity rate
    double synaptogenesisRate;         // Synaptogenesis rate
    double pruningRate;               // Pruning rate
    double maturationLevel;            // Maturation level (0-1)
    Timestamp timestamp;              // When metrics was collected
};

// Neuromodulation metrics
struct NeuromodulationMetrics {
    double dopamineLevel;              // Dopamine level
    double acetylcholineLevel;         // Acetylcholine level
    double norepinephrineLevel;        // Norepinephrine level
    double serotoninLevel;             // Serotonin level
    double noveltyLevel;                // Novelty level
    double curiosityLevel;              // Curiosity level
    double predictionErrorLevel;       // Prediction error level
    Timestamp timestamp;               // When metrics were collected
};

// Complete performance metrics snapshot
struct PerformanceMetrics {
    Timestamp timestamp;               // Simulation timestamp
    SystemMetrics system;              // System metrics
    NeuralMetrics neural;              // Neural metrics
    PlasticityMetrics plasticity;       // Plasticity metrics
    MemoryMetrics memory;              // Memory metrics
    PredictionMetrics prediction;      // Prediction metrics
    CognitiveMetrics cognition;        // Cognitive metrics
    DevelopmentMetrics development;    // Development metrics
    NeuromodulationMetrics neuromodulation; // Neuromodulation metrics
};

// Performance monitoring callback type
using PerformanceCallback = std::function<void(const PerformanceMetrics&)>;

// Time series data structure for performance history
template<typename T>
class TimeSeries {
public:
    TimeSeries(size_t maxSize = 1000) : maxSize_(maxSize) {}
    
    void add(const Timestamp& timestamp, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.emplace_back(timestamp, value);
        if (data_.size() > maxSize_) {
            data_.erase(data_.begin());
        }
    }
    
    std::vector<std::pair<Timestamp, T>> getData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.clear();
    }
    
private:
    std::vector<std::pair<Timestamp, T>> data_;
    size_t maxSize_;
    mutable std::mutex mutex_;
};

// Line profiler
struct ProfileEntry {
    std::string functionName;
    std::string fileName;
    int lineNumber;
    double duration;            // Function execution time
    uint64_t callCount;         // Number of times called
    double cpuTime;             // CPU time spent
    double memoryAllocated;     // Memory allocated by this function
    Timestamp timestamp;        // When function was called
};

// Memory allocation tracker
struct MemoryAllocation {
    void* address;              // Memory address
    size_t size;                // Size in bytes
    std::string allocationSite;  // File and line where allocated
    Timestamp timestamp;        // When allocated
    std::thread::id threadId;    // Thread that allocated
};

// Performance benchmark result
struct BenchmarkResult {
    std::string benchmarkName;   // Name of the benchmark
    double executionTime;        // Total execution time in seconds
    double cpuTime;              // CPU time in seconds
    size_t memoryPeak;           // Peak memory usage in bytes
    size_t iterations;           // Number of iterations
    double avgTimePerIteration;  // Average time per iteration
    std::map<std::string, double> metrics; // Additional metrics
    Timestamp timestamp;         // When benchmark was run
};

// Performance analysis result
struct AnalysisResult {
    std::string analysisName;    // Name of the analysis
    std::string analysisType;    // Type of analysis
    std::string findings;        // Key findings
    std::string recommendations; // Optimization recommendations
    double score;                // Performance score (0-100)
    std::map<std::string, double> metrics; // Analysis metrics
    Timestamp timestamp;         // When analysis was performed
};

#endif // NLM_PERFORMANCE_MONITORING_HPP