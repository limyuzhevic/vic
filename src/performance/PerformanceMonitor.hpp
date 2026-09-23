// Performance monitoring and profiling system for NLM
// Provides comprehensive performance metrics and profiling capabilities

#pragma once

#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Timing metrics
    double totalTime;                    // Total execution time (seconds)
    double cpuTime;                      // CPU time (seconds)
    double wallClockTime;                // Wall clock time (seconds)
    std::vector<double> stepTimes;       // Time per step
    
    // Neural metrics
    size_t totalSpikes;                  // Total number of spikes
    size_t totalSynapticOperations;      // Total synaptic operations
    size_t averageActiveNeurons;        // Average active neurons per step
    float averageFiringRate;             // Average firing rate (Hz)
    
    // Memory metrics
    size_t memoryUsage;                  // Current memory usage (bytes)
    size_t peakMemoryUsage;              // Peak memory usage (bytes)
    size_t allocationCount;              // Number of memory allocations
    double averageAllocationSize;        // Average allocation size
    
    // Computational metrics
    size_t floatingPointOperations;      // Number of floating point operations
    size_t memoryAccesses;               // Number of memory accesses
    size_t cacheMisses;                  // Number of cache misses
    
    // System metrics
    double cpuUsage;                     // CPU usage percentage
    double memoryUsagePercentage;        // Memory usage percentage
    size_t activeThreads;                // Number of active threads
    
    // Learning metrics
    float learningRate;                  // Average learning rate
    float plasticity;                    // Average plasticity level
    float predictionAccuracy;            // Prediction accuracy
    
    PerformanceMetrics()
        : totalTime(0.0)
        , cpuTime(0.0)
        , wallClockTime(0.0)
        , totalSpikes(0)
        , totalSynapticOperations(0)
        , averageActiveNeurons(0)
        , averageFiringRate(0.0f)
        , memoryUsage(0)
        , peakMemoryUsage(0)
        , allocationCount(0)
        , averageAllocationSize(0.0)
        , floatingPointOperations(0)
        , memoryAccesses(0)
        , cacheMisses(0)
        , cpuUsage(0.0)
        , memoryUsagePercentage(0.0)
        , activeThreads(0)
        , learningRate(0.0f)
        , plasticity(0.0f)
        , predictionAccuracy(0.0f) {}
};

// Timing utility class
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Performance profiler class
class PerformanceProfiler {
public:
    PerformanceProfiler() : currentStep_(0) {}
    
    // Start profiling a step
    void startStep() {
        std::lock_guard<std::mutex> lock(mutex_);
        stepTimer_.reset();
        stepStartTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // End profiling a step
    void endStep(const PerformanceMetrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        double elapsed = stepTimer_.elapsed();
        stepTimes_.push_back(elapsed);
        
        // Update cumulative metrics
        totalTime_ += elapsed;
        totalSpikes_ += metrics.totalSpikes;
        totalSynapticOperations_ += metrics.totalSynapticOperations;
        
        // Update neural metrics
        if (!stepTimes_.empty()) {
            averageActiveNeurons_ = metrics.averageActiveNeurons;
            averageFiringRate_ = metrics.averageFiringRate;
        }
        
        // Update learning metrics
        learningRate_ = metrics.learningRate;
        plasticity_ = metrics.plasticity;
        predictionAccuracy_ = metrics.predictionAccuracy;
        
        currentStep_++;
        
        NLM_LOG_DEBUG("PerformanceProfiler: Step " + std::to_string(currentStep_) +
                     " completed in " + std::to_string(elapsed) + "s");
    }
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentMetrics_;
    }
    
    // Set current metrics
    void setCurrentMetrics(const PerformanceMetrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_ = metrics;
    }
    
    // Get overall performance summary
    PerformanceMetrics getOverallMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        PerformanceMetrics summary = currentMetrics_;
        
        if (!stepTimes_.empty()) {
            double sum = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0);
            summary.totalTime = sum;
            summary.wallClockTime = totalTime_;
            summary.averageActiveNeurons = totalSpikes_ / stepTimes_.size();
        }
        
        return summary;
    }
    
    // Save metrics to file
    bool saveMetrics(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open metrics file: " + filename);
            return false;
        }
        
        // Write header
        file << "NLM Performance Metrics" << std::endl;
        file << "=" << std::string(50, '=') << std::endl;
        file << "Generated at: " << getCurrentTimestamp() << std::endl;
        file << "Total Steps: " << currentStep_ << std::endl;
        file << std::endl;
        
        // Write overall metrics
        PerformanceMetrics summary = getOverallMetrics();
        file << "Overall Metrics:" << std::endl;
        file << "  Total Time: " << summary.totalTime << " seconds" << std::endl;
        file << "  Average Step Time: " << (summary.totalTime / std::max<size_t>(currentStep_, 1)) << " seconds" << std::endl;
        file << "  Total Spikes: " << summary.totalSpikes << std::endl;
        file << "  Average Firing Rate: " << summary.averageFiringRate << " Hz" << std::endl;
        file << "  Peak Memory Usage: " << summary.peakMemoryUsage << " bytes" << std::endl;
        file << "  CPU Usage: " << summary.cpuUsage << "%" << std::endl;
        file << std::endl;
        
        // Write step-by-step metrics
        file << "Step-by-Step Metrics:" << std::endl;
        file << "Step,Time(sp),Spikes,FiringRate,MemoryUsage" << std::endl;
        
        for (size_t i = 0; i < std::min<size_t>(stepTimes_.size(), 100); ++i) {
            file << i << "," << stepTimes_[i] << ","
                 << (i < spikeCounts_.size() ? spikeCounts_[i] : 0) << ","
                 << (i < firingRates_.size() ? firingRates_[i] : 0.0f) << ","
                 << (i < memoryUsages_.size() ? memoryUsages_[i] : 0) << std::endl;
        }
        
        // Write learning curve
        file << std::endl;
        file << "Learning Curve:" << std::endl;
        file << "Step,LearningRate,Plasticity,PredictionAccuracy" << std::endl;
        
        for (size_t i = 0; i < std::min<size_t>(learningRates_.size(), 100); ++i) {
            file << i << "," << learningRates_[i] << "," 
                 << plasticities_[i] << "," << predictionAccuracies_[i] << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Metrics saved to: " + filename);
        return true;
    }
    
    // Generate performance report
    std::string generatePerformanceReport() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::stringstream report;
        report << "NLM Performance Report" << std::endl;
        report << "=" << std::string(50, '=') << std::endl;
        report << "Generated at: " << getCurrentTimestamp() << std::endl;
        report << "Total Steps: " << currentStep_ << std::endl;
        report << std::endl;
        
        PerformanceMetrics summary = getOverallMetrics();
        
        report << "Performance Summary:" << std::endl;
        report << "  Execution Time: " << summary.totalTime << " seconds" << std::endl;
        report << "  Steps per Second: " << (currentStep_ / std::max(summary.totalTime, 1.0)) << std::endl;
        report << "  Average Step Time: " << (summary.totalTime / std::max<size_t>(currentStep_, 1)) << " seconds" << std::endl;
        report << "  Total Neural Activity: " << summary.totalSpikes << " spikes" << std::endl;
        report << "  Average Firing Rate: " << summary.averageFiringRate << " Hz" << std::endl;
        report << "  Memory Efficiency: " << (summary.totalSpikes / std::max(summary.peakMemoryUsage, 1)) << " spikes/byte" << std::endl;
        report << "  Learning Progress: " << (summary.learningRate > 0.0f ? "Improving" : "Stable") << std::endl;
        
        return report.str();
    }
    
    // Update memory metrics
    void updateMemoryMetrics(size_t currentMemoryUsage) {
        std::lock_guard<std::mutex> lock(mutex_);
        memoryUsages_.push_back(currentMemoryUsage);
        peakMemoryUsage_ = std::max(peakMemoryUsage_, currentMemoryUsage);
        currentMetrics_.memoryUsage = currentMemoryUsage;
        currentMetrics_.peakMemoryUsage = peakMemoryUsage_;
    }
    
    // Update spike count
    void updateSpikeCount(size_t spikeCount) {
        std::lock_guard<std::mutex> lock(mutex_);
        spikeCounts_.push_back(spikeCount);
        currentMetrics_.totalSpikes += spikeCount;
    }
    
    // Update firing rate
    void updateFiringRate(float firingRate) {
        std::lock_guard<std::mutex> lock(mutex_);
        firingRates_.push_back(firingRate);
        currentMetrics_.averageFiringRate = firingRate;
    }
    
    // Update learning metrics
    void updateLearningMetrics(float learningRate, float plasticity, float predictionAccuracy) {
        std::lock_guard<std::mutex> lock(mutex_);
        learningRates_.push_back(learningRate);
        plasticities_.push_back(plasticity);
        predictionAccuracies_.push_back(predictionAccuracy);
        
        currentMetrics_.learningRate = learningRate;
        currentMetrics_.plasticity = plasticity;
        currentMetrics_.predictionAccuracy = predictionAccuracy;
    }
    
    // Update CPU metrics
    void updateCPUMetrics(double cpuUsage, size_t memoryUsage, size_t activeThreads) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_.cpuUsage = cpuUsage;
        currentMetrics_.memoryUsage = memoryUsage;
        currentMetrics_.memoryUsagePercentage = (static_cast<double>(memoryUsage) / getSystemMemorySize()) * 100.0;
        currentMetrics_.activeThreads = activeThreads;
    }
    
    // Reset profiler
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentStep_ = 0;
        stepTimes_.clear();
        spikeCounts_.clear();
        firingRates_.clear();
        memoryUsages_.clear();
        learningRates_.clear();
        plasticities_.clear();
        predictionAccuracies_.clear();
        totalTime_ = 0.0;
        totalSpikes_ = 0;
        totalSynapticOperations_ = 0;
        peakMemoryUsage_ = 0;
        currentMetrics_ = PerformanceMetrics();
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    mutable std::mutex mutex_;
    
    // Step tracking
    size_t currentStep_;
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCounts_;
    std::vector<float> firingRates_;
    std::vector<size_t> memoryUsages_;
    std::vector<float> learningRates_;
    std::vector<float> plasticities_;
    std::vector<float> predictionAccuracies_;
    
    // Cumulative metrics
    double totalTime_;
    size_t totalSpikes_;
    size_t totalSynapticOperations_;
    size_t peakMemoryUsage_;
    
    // Current metrics
    PerformanceMetrics currentMetrics_;
    
    // Timing
    Timer stepTimer_;
    std::chrono::high_resolution_clock::time_point stepStartTime_;
};

// System resource monitor
class SystemResourceMonitor {
public:
    SystemResourceMonitor() : updateInterval_(1.0) {}
    
    // Set update interval
    void setUpdateInterval(double interval) {
        updateInterval_ = interval;
    }
    
    // Get current CPU usage
    double getCPUUsage() {
        return cpuUsage_;
    }
    
    // Get current memory usage
    size_t getMemoryUsage() {
        return memoryUsage_;
    }
    
    // Get system memory size
    size_t getSystemMemorySize() {
        return systemMemorySize_;
    }
    
    // Get active thread count
    size_t getActiveThreadCount() {
        return activeThreadCount_;
    }
    
    // Update resource metrics
    void update() {
        // Update CPU usage (simplified implementation)
        cpuUsage_ = getCurrentCPUUsage();
        
        // Update memory usage (simplified implementation)
        memoryUsage_ = getCurrentMemoryUsage();
        
        // Update thread count (simplified implementation)
        activeThreadCount_ = getCurrentThreadCount();
        
        NLM_LOG_DEBUG("System resources updated: CPU=" + std::to_string(cpuUsage_) + 
                     "%, Memory=" + std::to_string(memoryUsage_) + 
                     " bytes, Threads=" + std::to_string(activeThreadCount_));
    }
    
    // Start monitoring
    void startMonitoring() {
        monitoring_ = true;
        monitorThread_ = std::thread(&SystemResourceMonitor::monitorLoop, this);
        NLM_LOG_INFO("System resource monitoring started");
    }
    
    // Stop monitoring
    void stopMonitoring() {
        monitoring_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
        NLM_LOG_INFO("System resource monitoring stopped");
    }
    
private:
    void monitorLoop() {
        while (monitoring_) {
            update();
            std::this_thread::sleep_for(std::chrono::duration<double>(updateInterval_));
        }
    }
    
    // Simplified implementations (platform-specific)
    double getCurrentCPUUsage() {
        // Simplified implementation - in practice, this would use platform-specific APIs
        return 25.0 + (std::rand() % 50) / 100.0; // Random value between 25% and 75%
    }
    
    size_t getCurrentMemoryUsage() {
        // Simplified implementation - in practice, this would use platform-specific APIs
        return 100 * 1024 * 1024 + (std::rand() % 400 * 1024 * 1024); // Random between 100MB and 500MB
    }
    
    size_t getCurrentThreadCount() {
        // Simplified implementation - in practice, this would use platform-specific APIs
        return 4 + (std::rand() % 8); // Random between 4 and 12 threads
    }
    
    // Get system memory size (simplified)
    size_t getSystemMemorySize() {
        if (systemMemorySize_ == 0) {
            // Try to get actual memory size
            systemMemorySize_ = 8 * 1024 * 1024 * 1024; // Default to 8GB
        }
        return systemMemorySize_;
    }
    
    double updateInterval_;
    double cpuUsage_;
    size_t memoryUsage_;
    size_t activeThreadCount_;
    size_t systemMemorySize_;
    bool monitoring_;
    std::thread monitorThread_;
};

// Performance metrics collector
class PerformanceMetricsCollector {
public:
    PerformanceMetricsCollector() : profiler_(std::make_shared<PerformanceProfiler>()), 
                                     monitor_(std::make_shared<SystemResourceMonitor>()) {}
    
    // Initialize the collector
    void initialize() {
        profiler_->reset();
        monitor_->startMonitoring();
        NLM_LOG_INFO("Performance metrics collector initialized");
    }
    
    // Start collecting metrics for a simulation
    void startCollection() {
        profiler_->reset();
        monitor_->startMonitoring();
    }
    
    // Stop collecting metrics
    void stopCollection() {
        monitor_->stopMonitoring();
    }
    
    // Record performance metrics for a step
    void recordStepMetrics(const PerformanceMetrics& metrics) {
        if (profiler_) {
            profiler_->setCurrentMetrics(metrics);
            profiler_->endStep(metrics);
        }
    }
    
    // Record neural metrics
    void recordNeuralMetrics(size_t spikeCount, float firingRate) {
        if (profiler_) {
            profiler_->updateSpikeCount(spikeCount);
            profiler_->updateFiringRate(firingRate);
        }
    }
    
    // Record memory metrics
    void recordMemoryMetrics(size_t memoryUsage) {
        if (profiler_) {
            profiler_->updateMemoryMetrics(memoryUsage);
        }
    }
    
    // Record learning metrics
    void recordLearningMetrics(float learningRate, float plasticity, float predictionAccuracy) {
        if (profiler_) {
            profiler_->updateLearningMetrics(learningRate, plasticity, predictionAccuracy);
        }
    }
    
    // Record system metrics
    void recordSystemMetrics(double cpuUsage, size_t memoryUsage, size_t activeThreads) {
        if (profiler_) {
            profiler_->updateCPUMetrics(cpuUsage, memoryUsage, activeThreads);
        }
    }
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() {
        if (profiler_) {
            return profiler_->getCurrentMetrics();
        }
        return PerformanceMetrics();
    }
    
    // Get overall performance metrics
    PerformanceMetrics getOverallMetrics() {
        if (profiler_) {
            return profiler_->getOverallMetrics();
        }
        return PerformanceMetrics();
    }
    
    // Generate performance report
    std::string generatePerformanceReport() {
        if (profiler_) {
            return profiler_->generatePerformanceReport();
        }
        return "Performance metrics collector not initialized";
    }
    
    // Save metrics to file
    bool saveMetrics(const std::string& filename) {
        if (profiler_) {
            return profiler_->saveMetrics(filename);
        }
        return false;
    }
    
    // Check if metrics collection is active
    bool isCollecting() const {
        return profiler_ && !profiler_->getCurrentMetrics().totalTime == 0.0;
    }
    
private:
    std::shared_ptr<PerformanceProfiler> profiler_;
    std::shared_ptr<SystemResourceMonitor> monitor_;
};

// Global performance metrics collector instance
inline PerformanceMetricsCollector& getPerformanceMetricsCollector() {
    static PerformanceMetricsCollector collector;
    return collector;
}

// Convenience functions for performance monitoring
inline void startPerformanceMonitoring() {
    getPerformanceMetricsCollector().startCollection();
}

inline void stopPerformanceMonitoring() {
    getPerformanceMetricsCollector().stopCollection();
}

inline void recordPerformanceMetrics(const PerformanceMetrics& metrics) {
    getPerformanceMetricsCollector().recordStepMetrics(metrics);
}

inline void recordNeuralMetrics(size_t spikeCount, float firingRate) {
    getPerformanceMetricsCollector().recordNeuralMetrics(spikeCount, firingRate);
}

inline void recordMemoryMetrics(size_t memoryUsage) {
    getPerformanceMetricsCollector().recordMemoryMetrics(memoryUsage);
}

inline void recordLearningMetrics(float learningRate, float plasticity, float predictionAccuracy) {
    getPerformanceMetricsCollector().recordLearningMetrics(learningRate, plasticity, predictionAccuracy);
}

inline std::string generatePerformanceReport() {
    return getPerformanceMetricsCollector().generatePerformanceReport();
}

inline bool savePerformanceMetrics(const std::string& filename) {
    return getPerformanceMetricsCollector().saveMetrics(filename);
}

} // namespace nlm