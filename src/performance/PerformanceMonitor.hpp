#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Performance monitoring and profiling system
 * 
 * Provides real-time monitoring, profiling, and performance analysis
 * for NLM neural simulations. Integrates with existing Phase 5 performance
 * optimizations while adding comprehensive monitoring capabilities.
 */

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        uint64_t currentStep = 0;
        double currentTime = 0.0;
        uint64_t totalNeurons = 0;
        uint64_t totalSynapses = 0;
        uint64_t activeNeurons = 0;
        uint64_t firingNeurons = 0;
        double stepsPerSecond = 0.0;
        double spikesPerStep = 0.0;
        double efficiencyRatio = 0.0;
        double memoryUsageMB = 0.0;
        uint64_t allocationsPerSecond = 0;
        double peakMemoryMB = 0.0;
        double cpuUtilization = 0.0;
        uint64_t threadCount = 0;
        uint64_t spikeEventsProcessed = 0;
        uint64_t synapticUpdates = 0;
        double avgStepTime = 0.0;
        double minStepTime = 0.0;
        double maxStepTime = 0.0;
        uint64_t stepsInLastSecond = 0;
        uint64_t spikesInLastSecond = 0;
    };

    enum class ProfilingLevel {
        OFF,
        BASIC,
        STANDARD,
        ADVANCED,
        COMPREHENSIVE
    };

    enum class AlertLevel {
        OK,
        WARNING,
        ERROR,
        CRITICAL
    };

    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }

    void initialize() {
        // Initialize performance monitoring
        startMonitoringThread();
    }

    void configure(size_t sampleRateHz,
                   size_t minSamples,
                   bool enableCPUProfiling,
                   bool enableMemoryProfiling,
                   bool enableTimingProfiling) {
        sampleRate_ = sampleRateHz;
        minSamples_ = minSamples;
        enableCPUProfiling_ = enableCPUProfiling;
        enableMemoryProfiling_ = enableMemoryProfiling;
        enableTimingProfiling_ = enableTimingProfiling;
    }

    void setProfilingLevel(ProfilingLevel level) {
        profilingLevel_ = level;
    }

    void start() {
        monitoringActive_ = true;
        monitoringThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    }

    void stop() {
        monitoringActive_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }

    void recordStep(uint64_t step, double time) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        metrics_.currentStep = step;
        metrics_.currentTime = time;
        
        // Update timing statistics
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - lastStepTime_);
        lastStepTime_ = now;
        
        stepTimes_.push_back(duration.count());
        if (stepTimes_.size() > maxStepSamples_) {
            stepTimes_.pop_front();
        }
        
        // Calculate average step time
        if (!stepTimes_.empty()) {
            double sum = 0.0;
            for (double t : stepTimes_) {
                sum += t;
            }
            metrics_.avgStepTime = sum / stepTimes_.size();
            
            if (metrics_.minStepTime == 0.0 || metrics_.avgStepTime < metrics_.minStepTime) {
                metrics_.minStepTime = metrics_.avgStepTime;
            }
            if (metrics_.avgStepTime > metrics_.maxStepTime) {
                metrics_.maxStepTime = metrics_.avgStepTime;
            }
        }
        
        // Update steps per second
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSecondTime_);
        
        if (elapsed.count() >= 1) {
            metrics_.stepsPerSecond = static_cast<double>(stepsInSecond_) / elapsed.count();
            metrics_.stepsInLastSecond = 0;
            lastSecondTime_ = currentTime;
        }
        
        metrics_.stepsInLastSecond++;
    }

    void recordSpike() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.spikeEventsProcessed++;
        metrics_.spikesInLastSecond++;
        
        // Update spikes per step
        if (metrics_.currentStep > 0) {
            metrics_.spikesPerStep = static_cast<double>(metrics_.spikeEventsProcessed) / metrics_.currentStep;
        }
    }

    void recordSynapticUpdate() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.synapticUpdates++;
    }

    void updateMemoryUsage(double memoryMB) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.memoryUsageMB = memoryMB;
        if (memoryMB > metrics_.peakMemoryMB) {
            metrics_.peakMemoryMB = memoryMB;
        }
    }

    void updateCPUUsage(double cpuPercent) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.cpuUtilization = cpuPercent;
    }

    const PerformanceMetrics& getMetrics() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        return metrics_;
    }

    void generateReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "NLM Performance Report" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        
        const auto& metrics = getMetrics();
        
        file << "Simulation Metrics:" << std::endl;
        file << "  Current Step: " << metrics.currentStep << std::endl;
        file << "  Current Time: " << std::fixed << std::setprecision(3) << metrics.currentTime << "s" << std::endl;
        file << "  Total Neurons: " << metrics.totalNeurons << std::endl;
        file << "  Total Synapses: " << metrics.totalSynapses << std::endl;
        file << "  Active Neurons: " << metrics.activeNeurons << std::endl;
        file << "  Firing Neurons: " << metrics.firingNeurons << std::endl;
        file << std::endl;
        
        file << "Performance Metrics:" << std::endl;
        file << "  Steps per second: " << std::fixed << std::setprecision(1) << metrics.stepsPerSecond << std::endl;
        file << "  Spikes per step: " << std::fixed << std::setprecision(2) << metrics.spikesPerStep << std::endl;
        file << "  Efficiency ratio: " << std::fixed << std::setprecision(3) << metrics.efficiencyRatio << std::endl;
        file << "  Average step time: " << std::fixed << std::setprecision(3) << metrics.avgStepTime << "s" << std::endl;
        file << "  Min step time: " << std::fixed << std::setprecision(3) << metrics.minStepTime << "s" << std::endl;
        file << "  Max step time: " << std::fixed << std::setprecision(3) << metrics.maxStepTime << "s" << std::endl;
        file << std::endl;
        
        file << "Memory Usage:" << std::endl;
        file << "  Current usage: " << std::fixed << std::setprecision(2) << metrics.memoryUsageMB << "MB" << std::endl;
        file << "  Peak usage: " << std::fixed << std::setprecision(2) << metrics.peakMemoryMB << "MB" << std::endl;
        file << "  Allocations per second: " << metrics.allocationsPerSecond << std::endl;
        file << std::endl;
        
        file << "System Resources:" << std::endl;
        file << "  CPU utilization: " << std::fixed << std::setprecision(1) << metrics.cpuUtilization << "%" << std::endl;
        file << "  Thread count: " << metrics.threadCount << std::endl;
        file << std::endl;
        
        file << "Event Processing:" << std::endl;
        file << "  Spike events processed: " << metrics.spikeEventsProcessed << std::endl;
        file << "  Synaptic updates: " << metrics.synapticUpdates << std::endl;
        file << std::endl;
        
        file.close();
    }

    void saveData(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return;
        }
        
        // Serialize metrics to binary format
        const auto& metrics = getMetrics();
        
        file.write(reinterpret_cast<const char*>(&metrics.currentStep), sizeof(metrics.currentStep));
        file.write(reinterpret_cast<const char*>(&metrics.currentTime), sizeof(metrics.currentTime));
        file.write(reinterpret_cast<const char*>(&metrics.totalNeurons), sizeof(metrics.totalNeurons));
        file.write(reinterpret_cast<const char*>(&metrics.totalSynapses), sizeof(metrics.totalSynapses));
        file.write(reinterpret_cast<const char*>(&metrics.activeNeurons), sizeof(metrics.activeNeurons));
        file.write(reinterpret_cast<const char*>(&metrics.firingNeurons), sizeof(metrics.firingNeurons));
        file.write(reinterpret_cast<const char*>(&metrics.stepsPerSecond), sizeof(metrics.stepsPerSecond));
        file.write(reinterpret_cast<const char*>(&metrics.spikesPerStep), sizeof(metrics.spikesPerStep));
        file.write(reinterpret_cast<const char*>(&metrics.efficiencyRatio), sizeof(metrics.efficiencyRatio));
        file.write(reinterpret_cast<const char*>(&metrics.memoryUsageMB), sizeof(metrics.memoryUsageMB));
        file.write(reinterpret_cast<const char*>(&metrics.allocationsPerSecond), sizeof(metrics.allocationsPerSecond));
        file.write(reinterpret_cast<const char*>(&metrics.peakMemoryMB), sizeof(metrics.peakMemoryMB));
        file.write(reinterpret_cast<const char*>(&metrics.cpuUtilization), sizeof(metrics.cpuUtilization));
        file.write(reinterpret_cast<const char*>(&metrics.threadCount), sizeof(metrics.threadCount));
        file.write(reinterpret_cast<const char*>(&metrics.spikeEventsProcessed), sizeof(metrics.spikeEventsProcessed));
        file.write(reinterpret_cast<const char*>(&metrics.synapticUpdates), sizeof(metrics.synapticUpdates));
        file.write(reinterpret_cast<const char*>(&metrics.avgStepTime), sizeof(metrics.avgStepTime));
        file.write(reinterpret_cast<const char*>(&metrics.minStepTime), sizeof(metrics.minStepTime));
        file.write(reinterpret_cast<const char*>(&metrics.maxStepTime), sizeof(metrics.maxStepTime));
        file.write(reinterpret_cast<const char*>(&metrics.stepsInLastSecond), sizeof(metrics.stepsInLastSecond));
        file.write(reinterpret_cast<const char*>(&metrics.spikesInLastSecond), sizeof(metrics.spikesInLastSecond));
        
        file.close();
    }

    void setTotalNeurons(uint64_t count) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.totalNeurons = count;
    }

    void setTotalSynapses(uint64_t count) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.totalSynapses = count;
        if (metrics_.totalNeurons > 0) {
            metrics_.efficiencyRatio = static_cast<double>(metrics_.totalSynapses) / metrics_.totalNeurons;
        }
    }

    void setActiveNeuronCount(uint64_t count) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.activeNeurons = count;
    }

    void setFiringNeuronCount(uint64_t count) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.firingNeurons = count;
    }

    void setAllocationsPerSecond(uint64_t allocations) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.allocationsPerSecond = allocations;
    }

    void setThreadCount(uint64_t threads) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.threadCount = threads;
    }

private:
    PerformanceMonitor()
        : monitoringActive_(false)
        , profilingLevel_(ProfilingLevel::OFF)
        , sampleRate_(1000)
        , minSamples_(100)
        , enableCPUProfiling_(false)
        , enableMemoryProfiling_(false)
        , enableTimingProfiling_(false)
        , stepsInSecond_(0)
        , lastSecondTime_(std::chrono::high_resolution_clock::now())
        , maxStepSamples_(10000)
    {
    }

    ~PerformanceMonitor() {
        stop();
    }

    void startMonitoringThread() {
        monitoringThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    }

    void monitoringLoop() {
        auto lastSampleTime = std::chrono::high_resolution_clock::now();
        
        while (monitoringActive_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSampleTime);
            
            if (elapsed.count() >= 1) {
                // Update performance metrics
                updateSecondMetrics();
                
                lastSampleTime = currentTime;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void updateSecondMetrics() {
        // Calculate second-based metrics
        double cpuUsage = calculateCPUUsage();
        double memoryUsage = calculateMemoryUsage();
        
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        // Update metrics
        if (profilingLevel_ >= ProfilingLevel::BASIC) {
            // Update basic metrics
        }
        
        if (profilingLevel_ >= ProfilingLevel::ADVANCED) {
            // Update advanced metrics
        }
        
        if (profilingLevel_ >= ProfilingLevel::COMPREHENSIVE) {
            // Update comprehensive metrics
        }
    }

    double calculateCPUUsage() {
        // Calculate CPU usage based on system information
        // This is a simplified implementation
        return 0.0;
    }

    double calculateMemoryUsage() {
        // Calculate memory usage based on system information
        // This is a simplified implementation
        return 0.0;
    }

private:
    std::mutex metricsMutex_;
    PerformanceMetrics metrics_;
    bool monitoringActive_;
    ProfilingLevel profilingLevel_;
    size_t sampleRate_;
    size_t minSamples_;
    bool enableCPUProfiling_;
    bool enableMemoryProfiling_;
    bool enableTimingProfiling_;
    std::thread monitoringThread_;
    
    // Statistics for real-time calculations
    uint64_t stepsInSecond_;
    std::chrono::high_resolution_clock::time_point lastSecondTime_;
    std::deque<double> stepTimes_;
    size_t maxStepSamples_;
    
    std::chrono::high_resolution_clock::time_point lastStepTime_;
};

} // namespace nlm
