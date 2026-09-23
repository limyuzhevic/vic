#pragma once

/**
 * Performance Monitor - Core performance monitoring for neural simulation
 * 
 * Tracks simulation timing, memory usage, CPU metrics, and performance statistics
 * Non-intrusive and configurable to minimize performance impact
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <sys/resource.h>
#include <sys/times.h>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        // Timing metrics
        double wallClockTime = 0.0;      // Real time elapsed (seconds)
        double cpuTime = 0.0;           // CPU time used (seconds)
        double stepTime = 0.0;          // Average time per simulation step (seconds)
        double maxStepTime = 0.0;       // Maximum time for a single step
        double minStepTime = 0.0;       // Minimum time for a single step
        
        // Performance metrics
        double fps = 0.0;               // Frames per second (steps/sec)
        double neuronsPerSecond = 0.0;  // Neuron processing rate
        double synapsesPerSecond = 0.0; // Synapse processing rate
        double spikeRate = 0.0;         // Spike generation rate
        
        // Memory metrics
        size_t peakMemoryUsage = 0;     // Peak memory usage (bytes)
        size_t currentMemoryUsage = 0;  // Current memory usage (bytes)
        size_t allocationCount = 0;     // Number of memory allocations
        size_t deallocationCount = 0;   // Number of memory deallocations
        
        // Resource metrics
        double cpuUsage = 0.0;          // CPU usage percentage
        int numThreads = 0;             // Number of active threads
        double ioThroughput = 0.0;      // I/O throughput (bytes/sec)
        
        // Step statistics
        size_t totalSteps = 0;          // Total simulation steps completed
        size_t successfulSteps = 0;     // Steps without errors
        size_t failedSteps = 0;          // Steps with errors
        
        // Advanced metrics
        double efficiency = 0.0;        // Performance efficiency (0-1)
        double cacheHitRate = 0.0;      // Cache hit rate (0-1)
        double branchPredictorAccuracy = 0.0; // Branch predictor accuracy (0-1)
    };

    struct Configuration {
        bool enabled = true;                    // Enable performance monitoring
        bool trackMemory = true;                // Track memory usage
        bool trackCpu = true;                   // Track CPU usage
        bool trackStepTiming = true;            // Track step timing
        bool trackFiringRate = true;            // Track firing rate
        bool trackNetworkEfficiency = true;     // Track network efficiency
        bool trackResourceUsage = true;         // Track resource usage
        bool trackProgress = true;              // Track progress reporting
        bool exportData = false;                // Export performance data to files
        std::string exportPath = "performance_"; // Export file path prefix
        double alertThreshold = 0.95;           // Alert threshold (performance efficiency)
        bool realTimeReporting = true;          // Enable real-time reporting
        size_t reportInterval = 100;            // Reporting interval (steps)
        bool threadSafe = true;                 // Thread-safe operation
    };

    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;

    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;

    /**
     * Start monitoring
     */
    void start() {
        if (!config_.enabled) return;
        
        startTime_ = std::chrono::high_resolution_clock::now();
        lastReportTime_ = startTime_;
        currentMetrics_ = PerformanceMetrics();
        
        if (config_.trackCpu) {
            // Get initial CPU times
            rusage initialUsage;
            getrusage(RUSAGE_SELF, &initialUsage);
            initialCpuTime_ = initialUsage.ru_utime.tv_sec + initialUsage.ru_stime.tv_sec;
            initialCpuTime_ += (initialUsage.ru_utime.tv_usec + initialUsage.ru_stime.tv_usec) / 1e6;
        }
        
        isRunning_ = true;
    }

    /**
     * Stop monitoring
     */
    void stop() {
        if (!isRunning_) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        currentMetrics_.wallClockTime = duration.count();
        
        if (config_.trackCpu) {
            rusage finalUsage;
            getrusage(RUSAGE_SELF, &finalUsage);
            double finalCpuTime = finalUsage.ru_utime.tv_sec + finalUsage.ru_stime.tv_sec;
            finalCpuTime += (finalUsage.ru_utime.tv_usec + finalUsage.ru_stime.tv_usec) / 1e6;
            currentMetrics_.cpuTime = finalCpuTime - initialCpuTime_;
        }
        
        isRunning_ = false;
    }

    /**
     * Reset all metrics
     */
    void reset() {
        currentMetrics_ = PerformanceMetrics();
        if (config_.trackCpu) {
            rusage initialUsage;
            getrusage(RUSAGE_SELF, &initialUsage);
            initialCpuTime_ = initialUsage.ru_utime.tv_sec + initialUsage.ru_stime.tv_sec;
            initialCpuTime_ += (initialUsage.ru_utime.tv_usec + initialUsage.ru_stime.tv_usec) / 1e6;
        }
        lastReportTime_ = std::chrono::high_resolution_clock::now();
    }

    /**
     * Record a simulation step timing
     */
    void recordStepTime(double stepTime) {
        if (!isRunning_ || !config_.trackStepTiming) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_.totalSteps++;
        
        // Update step timing statistics
        if (currentMetrics_.totalSteps == 1) {
            currentMetrics_.maxStepTime = stepTime;
            currentMetrics_.minStepTime = stepTime;
        } else {
            currentMetrics_.maxStepTime = std::max(currentMetrics_.maxStepTime, stepTime);
            currentMetrics_.minStepTime = std::min(currentMetrics_.minStepTime, stepTime);
        }
        
        currentMetrics_.stepTime = (currentMetrics_.stepTime * (currentMetrics_.totalSteps - 1) + stepTime) / currentMetrics_.totalSteps;
        
        // Calculate FPS
        if (currentMetrics_.wallClockTime > 0) {
            currentMetrics_.fps = currentMetrics_.totalSteps / currentMetrics_.wallClockTime;
        }
        
        // Check for step success/failure (simplified - assume successful if stepTime > 0)
        if (stepTime > 0.0) {
            currentMetrics_.successfulSteps++;
        } else {
            currentMetrics_.failedSteps++;
        }
    }

    /**
     * Record memory usage
     */
    void recordMemoryUsage(size_t memoryBytes) {
        if (!isRunning_ || !config_.trackMemory) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_.currentMemoryUsage = memoryBytes;
        currentMetrics_.peakMemoryUsage = std::max(currentMetrics_.peakMemoryUsage, memoryBytes);
    }

    /**
     * Record allocation/deallocation
     */
    void recordAllocation() {
        if (!isRunning_ || !config_.trackMemory) return;
        currentMetrics_.allocationCount++;
    }

    void recordDeallocation() {
        if (!isRunning_ || !config_.trackMemory) return;
        currentMetrics_.deallocationCount++;
    }

    /**
     * Record firing rate
     */
    void recordFiringRate(double rate) {
        if (!isRunning_ || !config_.trackFiringRate) return;
        currentMetrics_.spikeRate = rate;
    }

    /**
     * Record processing rates
     */
    void recordNeuronsPerSecond(double rate) {
        if (!isRunning_) return;
        currentMetrics_.neuronsPerSecond = rate;
    }

    void recordSynapsesPerSecond(double rate) {
        if (!isRunning_) return;
        currentMetrics_.synapsesPerSecond = rate;
    }

    /**
     * Get current performance metrics
     */
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PerformanceMetrics metrics = currentMetrics_;
        
        // Calculate derived metrics
        if (metrics.wallClockTime > 0) {
            metrics.efficiency = (metrics.neuronsPerSecond > 0) ? 1.0 / metrics.neuronsPerSecond : 0.0;
        }
        
        if (metrics.totalSteps > 0) {
            metrics.cacheHitRate = 0.8; // Placeholder - would need actual cache monitoring
            metrics.branchPredictorAccuracy = 0.95; // Placeholder
        }
        
        return metrics;
    }

    /**
     * Export performance data to file
     */
    bool exportData(const std::string& filename = "") {
        if (!config_.exportData) return false;
        
        std::string filepath = filename.empty() ? config_.exportPath + ".json" : filename;
        
        // TODO: Implement JSON export
        // For now, just create a simple text export
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto metrics = getMetrics();
        file << "Performance Metrics Export" << std::endl;
        file << "=========================" << std::endl;
        file << "Wall Clock Time: " << metrics.wallClockTime << "s" << std::endl;
        file << "CPU Time: " << metrics.cpuTime << "s" << std::endl;
        file << "Steps: " << metrics.totalSteps << std::endl;
        file << "Successful Steps: " << metrics.successfulSteps << std::endl;
        file << "Failed Steps: " << metrics.failedSteps << std::endl;
        file << "Peak Memory: " << metrics.peakMemoryUsage << " bytes" << std::endl;
        file << "Average Step Time: " << metrics.stepTime << "s" << std::endl;
        file << "Max Step Time: " << metrics.maxStepTime << "s" << std::endl;
        file << "Min Step Time: " << metrics.minStepTime << "s" << std::endl;
        file << "FPS: " << metrics.fps << std::endl;
        file << "Firing Rate: " << metrics.spikeRate << " spikes/sec" << std::endl;
        file << "Neurons/sec: " << metrics.neuronsPerSecond << std::endl;
        file << "Synapses/sec: " << metrics.synapsesPerSecond << std::endl;
        file << "Memory Allocations: " << metrics.allocationCount << std::endl;
        file << "Memory Deallocations: " << metrics.deallocationCount << std::endl;
        
        file.close();
        return true;
    }

    /**
     * Set configuration
     */
    void setConfiguration(const Configuration& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
    }

    /**
     * Get configuration
     */
    Configuration getConfiguration() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }

private:
    mutable std::mutex mutex_;
    Configuration config_;
    PerformanceMetrics currentMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastReportTime_;
    bool isRunning_ = false;
    double initialCpuTime_ = 0.0;
};

} // namespace nlm
