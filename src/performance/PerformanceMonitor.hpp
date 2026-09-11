#pragma once

#include "../core/Types/Types.hpp"
#include "../core/MemoryPool.hpp"
#include "../brain/Brain.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <numeric>
#include <algorithm>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Performance metrics collection
struct PerformanceCounters {
    // Basic counters
    std::atomic<size_t> totalSpikes{0};
    std::atomic<size_t> activeNeurons{0};
    std::atomic<size_t> firingNeurons{0};
    std::atomic<size_t> totalSteps{0};
    
    // Memory statistics
    std::atomic<size_t> memoryUsage{0};
    std::atomic<size_t> peakMemoryUsage{0};
    
    // Timing statistics
    std::chrono::high_resolution_clock::time_point simulationStart;
    double totalSimulationTime{0.0};
    double peakStepTime{0.0};
    double averageStepTime{0.0};
    
    // SIMD statistics
    std::atomic<size_t> simdOperationsExecuted{0};
    std::atomic<size_t> parallelOperationsExecuted{0};
    
    // Cache statistics
    std::atomic<size_t> cacheHits{0};
    std::atomic<size_t> cacheMisses{0};
    
    PerformanceCounters() : simulationStart(std::chrono::high_resolution_clock::now()) {}
};

// Thread-local performance data
struct ThreadLocalData {
    size_t localSpikes{0};
    size_t localActiveNeurons{0};
    std::chrono::high_resolution_clock::time_point lastStepTime;
};

// Performance statistics
struct SIMDPerformanceStats {
    size_t simdInstructionsExecuted{0};
    size_t parallelOperationsExecuted{0};
};

// Real-time performance monitoring
class PerformanceMonitor {
private:
    PerformanceCounters counters;
    mutable std::mutex countersMutex;
    
    // Track per-step metrics
    std::vector<double> stepDurations;
    std::vector<size_t> neuronCounts;
    std::vector<size_t> spikeCounts;
    static constexpr size_t MAX_HISTORY = 1000;
    
    // Thread-local performance data
    std::vector<ThreadLocalData> threadLocalData;
    
    // SIMD performance statistics
    SIMDPerformanceStats simdStats;
    
    // Optimization flags
    bool enableSIMD{true};
    bool enableParallel{true};
    bool enableMemoryPools{true};
    
    // Optimization thresholds
    double simdThreshold{0.001};  // Minimum operation size for SIMD
    size_t parallelThreshold{1000};  // Minimum work for parallel execution
    
    // Memory pool allocators
    MemoryPool<Neuron> neuronPool;
    MemoryPool<Synapse> synapsePool;
    std::vector<float> floatPool;
    
    // Allocation statistics
    std::atomic<size_t> totalAllocations{0};
    std::atomic<size_t> totalDeallocations{0};
    
public:
    PerformanceMonitor()
        : stepDurations(MAX_HISTORY), neuronCounts(MAX_HISTORY), spikeCounts(MAX_HISTORY) {
        unsigned int cpuCores = std::thread::hardware_concurrency();
        threadLocalData.resize(cpuCores);
        threadLocalData[0].lastStepTime = std::chrono::high_resolution_clock::now();
        
        // Pre-allocate memory pools
        if (enableMemoryPools) {
            neuronPool.reserve(10000);
            synapsePool.reserve(100000);
            floatPool.reserve(1000000);
        }
    }
    
    ~PerformanceMonitor() {
        // Cleanup
    }
    
    // Update counters with thread-local aggregation
    void recordStep(const Brain& brain, double stepDuration) {
        // Update timing
        std::lock_guard<std::mutex> lock(countersMutex);
        
        size_t currentStep = counters.totalSteps.fetch_add(1);
        
        // Record step duration (sliding window)
        stepDurations[currentStep % MAX_HISTORY] = stepDuration;
        
        // Get brain statistics
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t spikeCount = brain.getTotalSpikeCount();
        
        neuronCounts[currentStep % MAX_HISTORY] = neuronCount;
        spikeCounts[currentStep % MAX_HISTORY] = spikeCount;
        
        // Update performance counters
        counters.totalSpikes.fetch_add(spikeCount);
        counters.activeNeurons.fetch_add(brain.getActiveNeuronCount());
        counters.firingNeurons.fetch_add(brain.getFiringNeuronCount());
        
        // Update timing statistics
        counters.totalSimulationTime += stepDuration;
        if (stepDuration > counters.peakStepTime) {
            counters.peakStepTime = stepDuration;
        }
        
        // Calculate average step time (using valid entries)
        size_t validEntries = 0;
        double sum = 0.0;
        for (size_t i = 0; i < MAX_HISTORY; ++i) {
            if (stepDurations[i] > 0.0) {
                sum += stepDurations[i];
                ++validEntries;
            }
        }
        if (validEntries > 0) {
            counters.averageStepTime = sum / validEntries;
        }
        
        // Update SIMD stats if available
        if (enableSIMD) {
            counters.simdOperationsExecuted.fetch_add(simdStats.simdInstructionsExecuted);
            counters.parallelOperationsExecuted.fetch_add(simdStats.parallelOperationsExecuted);
        }
    }
    
    // Thread-local data management
    ThreadLocalData& getThreadLocalData() {
        thread_local ThreadLocalData tld;
        return tld;
    }
    
    // SIMD performance monitoring
    void updateSIMDStats(const SIMDPerformanceStats& stats) {
        std::lock_guard<std::mutex> lock(countersMutex);
        simdStats = stats;
    }
    
    // Performance statistics
    struct BrainPerformanceStats {
        double stepsPerSecond;
        double averageStepTime;
        double peakStepTime;
        double totalSimulationTime;
        
        // Neuron statistics
        double averageFiringRate;
        double averageActiveNeuronRatio;
        
        // Memory statistics
        size_t peakMemoryUsage;
        size_t currentMemoryUsage;
        
        // SIMD statistics
        size_t simdOperations;
        size_t parallelOperations;
        
        // Cache statistics
        double cacheHitRate;
        
        // Optimization effectiveness
        double optimizationScore;  // Composite metric of all optimizations
    };
    
    BrainPerformanceStats getPerformanceStats() const {
        std::lock_guard<std::mutex> lock(countersMutex);
        
        BrainPerformanceStats stats;
        
        // Basic timing
        if (counters.totalSteps > 0) {
            stats.stepsPerSecond = counters.totalSteps.load() / counters.totalSimulationTime;
        } else {
            stats.stepsPerSecond = 0.0;
        }
        
        stats.averageStepTime = counters.averageStepTime;
        stats.peakStepTime = counters.peakStepTime;
        stats.totalSimulationTime = counters.totalSimulationTime;
        
        // Calculate averages (using valid history entries)
        size_t validEntries = 0;
        double sumNeurons = 0.0;
        double sumSpikes = 0.0;
        for (size_t i = 0; i < MAX_HISTORY; ++i) {
            if (neuronCounts[i] > 0) {
                sumNeurons += neuronCounts[i];
                sumSpikes += spikeCounts[i];
                ++validEntries;
            }
        }
        
        if (validEntries > 0) {
            stats.averageFiringRate = (sumSpikes / validEntries) / (sumNeurons / validEntries);
            stats.averageActiveNeuronRatio = (sumNeurons / validEntries) / counters.totalSteps.load();
        } else {
            stats.averageFiringRate = 0.0;
            stats.averageActiveNeuronRatio = 0.0;
        }
        
        // Memory stats
        stats.peakMemoryUsage = counters.peakMemoryUsage.load();
        stats.currentMemoryUsage = counters.memoryUsage.load();
        
        // SIMD stats
        stats.simdOperations = counters.simdOperationsExecuted.load();
        stats.parallelOperations = counters.parallelOperationsExecuted.load();
        
        // Cache stats
        size_t totalAccesses = counters.cacheHits.load() + counters.cacheMisses.load();
        if (totalAccesses > 0) {
            stats.cacheHitRate = counters.cacheHits.load() / static_cast<double>(totalAccesses);
        } else {
            stats.cacheHitRate = 1.0;  // Perfect cache if no misses
        }
        
        // Calculate optimization score
        double simdScore = std::min(1.0, static_cast<double>(stats.simdOperations) / 1000.0);
        double parallelScore = std::min(1.0, static_cast<double>(stats.parallelOperations) / 1000.0);
        double memoryScore = std::min(1.0, static_cast<double>(stats.peakMemoryUsage) / 1000000.0);
        double cacheScore = stats.cacheHitRate;
        
        stats.optimizationScore = (simdScore + parallelScore + memoryScore + cacheScore) / 4.0;
        
        return stats;
    }
    
    // Configuration
    void configureOptimization(bool enableSIMD, bool enableParallel, bool enableMemoryPools) {
        this->enableSIMD = enableSIMD;
        this->enableParallel = enableParallel;
        this->enableMemoryPools = enableMemoryPools;
    }
    
    void setThresholds(double simdThreshold, size_t parallelThreshold) {
        this->simdThreshold = simdThreshold;
        this->parallelThreshold = parallelThreshold;
    }
    
    // Check if optimization should be applied
    bool shouldUseSIMD(size_t operationSize) const {
        return enableSIMD && operationSize >= simdThreshold;
    }
    
    bool shouldUseParallel(size_t operationSize) const {
        return enableParallel && operationSize >= parallelThreshold;
    }
    
    // Performance regression detection
    bool detectRegression(const BrainPerformanceStats& current, 
                         const BrainPerformanceStats& baseline,
                         double threshold = 0.1) const {
        // Compare key metrics against baseline
        double regression = 0.0;
        
        // Step time regression
        if (baseline.averageStepTime > 0.0) {
            regression += std::abs(current.averageStepTime - baseline.averageStepTime) / 
                         baseline.averageStepTime;
        }
        
        // Firing rate regression
        if (baseline.averageFiringRate > 0.0) {
            regression += std::abs(current.averageFiringRate - baseline.averageFiringRate) / 
                         baseline.averageFiringRate;
        }
        
        // SIMD usage regression
        if (baseline.simdOperations > 0) {
            regression += std::abs(static_cast<double>(current.simdOperations - baseline.simdOperations)) / 
                         static_cast<double>(baseline.simdOperations);
        }
        
        return (regression / 3.0) > threshold;
    }
    
    // Memory usage tracking
    void updateMemoryUsage(size_t newUsage) {
        std::lock_guard<std::mutex> lock(countersMutex);
        if (newUsage > counters.peakMemoryUsage) {
            counters.peakMemoryUsage = newUsage;
        }
        counters.memoryUsage = newUsage;
    }
    
    // Cache statistics
    void recordCacheAccess(bool hit) {
        if (hit) {
            counters.cacheHits.fetch_add(1);
        } else {
            counters.cacheMisses.fetch_add(1);
        }
    }
    
    // Update thread-local data
    void updateThreadLocalData() {
        thread_local ThreadLocalData& tld = getThreadLocalData();
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - tld.lastStepTime).total_seconds();
        
        // Update counters based on thread-local activity
        counters.activeNeurons.fetch_add(tld.localActiveNeurons);
        counters.totalSpikes.fetch_add(tld.localSpikes);
        
        tld.lastStepTime = now;
        tld.localSpikes = 0;
        tld.localActiveNeurons = 0;
    }
    
    // Performance analysis
    std::string analyzeBottlenecks() const {
        std::lock_guard<std::mutex> lock(countersMutex);
        
        std::string analysis;
        
        // Check for potential bottlenecks
        double stepTimeRatio = counters.averageStepTime / counters.peakStepTime;
        if (stepTimeRatio < 0.5) {
            analysis += "WARNING: High step time variance may indicate processing bottlenecks.\n";
        }
        
        double cacheEfficiency = counters.cacheHits.load() / 
                                std::max(1.0, static_cast<double>(counters.cacheHits.load() + 
                                                                counters.cacheMisses.load()));
        if (cacheEfficiency < 0.9) {
            analysis += "WARNING: Low cache efficiency may indicate memory bandwidth bottlenecks.\n";
        }
        
        double simdEfficiency = simdStats.simdInstructionsExecuted / 
                               std::max(1.0, static_cast<double>(counters.totalSteps.load()));
        if (simdEfficiency < 0.1) {
            analysis += "WARNING: Low SIMD utilization may indicate suboptimal vectorization.\n";
        }
        
        if (analysis.empty()) {
            analysis = "Performance analysis complete. No significant bottlenecks detected.\n";
        }
        
        return analysis;
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::lock_guard<std::mutex> lock(countersMutex);
        
        auto stats = getPerformanceStats();
        
        std::stringstream ss;
        ss << "=== NLM Performance Report ===\n\n";
        ss << "Simulation Summary:\n";
        ss << "  Total Steps: " << counters.totalSteps.load() << "\n";
        ss << "  Total Simulation Time: " << std::fixed << std::setprecision(3) 
           << counters.totalSimulationTime << " seconds\n";
        ss << "  Steps per Second: " << std::fixed << std::setprecision(1) 
           << stats.stepsPerSecond << "\n\n";
        
        ss << "Performance Metrics:\n";
        ss << "  Average Step Time: " << std::fixed << std::setprecision(6) 
           << stats.averageStepTime << " seconds\n";
        ss << "  Peak Step Time: " << std::fixed << std::setprecision(6) 
           << stats.peakStepTime << " seconds\n";
        ss << "  Optimization Score: " << std::fixed << std::setprecision(3) 
           << stats.optimizationScore << "\n\n";
        
        ss << "Neuron Statistics:\n";
        ss << "  Average Firing Rate: " << std::fixed << std::setprecision(3) 
           << stats.averageFiringRate << " Hz\n";
        ss << "  Average Active Neuron Ratio: " << std::fixed << std::setprecision(3) 
           << stats.averageActiveNeuronRatio << "\n\n";
        
        ss << "Memory Statistics:\n";
        ss << "  Current Memory Usage: " << stats.currentMemoryUsage << " bytes\n";
        ss << "  Peak Memory Usage: " << stats.peakMemoryUsage << " bytes\n\n";
        
        ss << "Optimization Utilization:\n";
        ss << "  SIMD Operations: " << stats.simdOperations << "\n";
        ss << "  Parallel Operations: " << stats.parallelOperations << "\n";
        ss << "  Cache Hit Rate: " << std::fixed << std::setprecision(3) 
           << stats.cacheHitRate << "\n\n";
        
        ss << analyzeBottlenecks() << "\n";
        ss << "Performance Report End ===\n";
        
        return ss.str();
    }
};