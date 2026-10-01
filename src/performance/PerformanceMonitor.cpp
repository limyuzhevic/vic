#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <cmath>

namespace nlm {

struct PerformanceMonitor::Impl {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastSampleTime;
    double cpuUsageSample1;
    double cpuUsageSample2;
    double memoryUsageSample1;
    double memoryUsageSample2;
    size_t sampleCount;
    std::string previousOperation;
    std::chrono::steady_clock::time_point operationStartTime;
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()), updateCount_(0), updateInterval_(100), verbose_(false), frameCount_(0) {
    reset();
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::initialize() {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastSampleTime = pImpl->startTime;
    pImpl->cpuUsageSample1 = 0;
    pImpl->cpuUsageSample2 = 0;
    pImpl->memoryUsageSample1 = 0;
    pImpl->memoryUsageSample2 = 0;
    pImpl->sampleCount = 0;
    
    enabledMetrics_ = {
        {"cpu_usage", true},
        {"memory_usage", true},
        {"frame_time", true},
        {"frame_rate", true},
        {"simulation_time", true},
        {"total_spikes", true},
        {"active_neurons", true},
        {"total_synapses", true},
        {"working_memory_size", true},
        {"episodic_memory_count", true},
        {"concept_count", true},
        {"average_firing_rate", true},
        {"memory_utilization", true},
        {"pending_spikes", true},
        {"plasticity_rate", true}
    };
}

void PerformanceMonitor::update() {
    auto currentTime = std::chrono::steady_clock::now();
    
    // Update frame timing
    if (previousFrameTime_.time_since_epoch().count() > 0) {
        auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousFrameTime_);
        metrics_.frameTime = frameDuration.count() * 1000.0; // Convert to milliseconds
        metrics_.frameRate = (metrics_.frameTime > 0) ? (1000.0 / metrics_.frameTime) : 0;
    }
    previousFrameTime_ = currentTime;
    frameCount_++;
    
    // Update sample count
    updateCount_++;
    
    // Calculate CPU usage (simplified - periodic sampling)
    if (updateCount_ % 100 == 0) {
        double cpuUsage = calculateCpuUsage();
        if (pImpl->sampleCount == 0) {
            pImpl->cpuUsageSample1 = cpuUsage;
        } else if (pImpl->sampleCount == 1) {
            pImpl->cpuUsageSample2 = cpuUsage;
        }
        pImpl->sampleCount = (pImpl->sampleCount + 1) % 2;
        
        if (pImpl->sampleCount == 0) {
            metrics_.cpuUsage = (pImpl->cpuUsageSample1 + pImpl->cpuUsageSample2) / 2.0;
        }
    }
    
    // Calculate memory usage (simplified)
    double memoryUsage = calculateMemoryUsage();
    if (pImpl->sampleCount == 0) {
        pImpl->memoryUsageSample1 = memoryUsage;
    } else if (pImpl->sampleCount == 1) {
        pImpl->memoryUsageSample2 = memoryUsage;
    }
    
    if (pImpl->sampleCount == 0) {
        metrics_.memoryUsage = (pImpl->memoryUsageSample1 + pImpl->memoryUsageSample2) / 2.0;
        metrics_.peakMemoryUsage = std::max(metrics_.peakMemoryUsage, metrics_.memoryUsage);
        
        // Calculate memory percentage
        const double totalMemory = 2048.0; // Assume 2GB total for calculation
        metrics_.memoryUsagePercent = (metrics_.memoryUsage / totalMemory) * 100.0;
    }
    
    // Update simulation time
    auto totalDuration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - pImpl->startTime);
    metrics_.simulationTime = totalDuration.count();
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << "Update Count: " << updateCount_ << std::endl;
    ss << "Simulation Time: " << formatTime(metrics_.simulationTime) << std::endl;
    ss << "Frame Rate: " << std::fixed << std::setprecision(2) << metrics_.frameRate << " Hz" << std::endl;
    ss << "Frame Time: " << std::fixed << std::setprecision(3) << metrics_.frameTime << " ms" << std::endl;
    
    if (isMetricEnabled("cpu_usage")) {
        ss << "CPU Usage: " << std::fixed << std::setprecision(1) << metrics_.cpuUsage << "%" << std::endl;
    }
    
    if (isMetricEnabled("memory_usage")) {
        ss << "Memory Usage: " << formatMemory(metrics_.memoryUsage * 1024 * 1024) << " (" 
           << std::fixed << std::setprecision(1) << metrics_.memoryUsagePercent << "%)" << std::endl;
        ss << "Peak Memory: " << formatMemory(metrics_.peakMemoryUsage * 1024 * 1024) << std::endl;
    }
    
    ss << std::endl;
    ss << "Neural Activity:" << std::endl;
    
    if (isMetricEnabled("total_spikes")) {
        ss << "  Total Spikes: " << formatNumber(metrics_.totalSpikes) << std::endl;
    }
    
    if (isMetricEnabled("active_neurons")) {
        ss << "  Active Neurons: " << formatNumber(metrics_.activeNeurons) << std::endl;
    }
    
    if (isMetricEnabled("total_synapses")) {
        ss << "  Total Synapses: " << formatNumber(metrics_.totalSynapses) << std::endl;
    }
    
    if (isMetricEnabled("average_firing_rate")) {
        ss << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << metrics_.averageFiringRate << " Hz" << std::endl;
    }
    
    if (isMetricEnabled("pending_spikes")) {
        ss << "  Pending Spikes: " << formatNumber(metrics_.pendingSpikes) << std::endl;
    }
    
    if (isMetricEnabled("plasticity_rate")) {
        ss << "  Plasticity Rate: " << std::fixed << std::setprecision(4) << metrics_.plasticityRate << std::endl;
    }
    
    ss << std::endl;
    ss << "Memory Systems:" << std::endl;
    
    if (isMetricEnabled("working_memory_size")) {
        ss << "  Working Memory Traces: " << formatNumber(metrics_.workingMemorySize) << std::endl;
        ss << "  Memory Utilization: " << std::fixed << std::setprecision(2) << (metrics_.memoryUtilization * 100) << "%" << std::endl;
    }
    
    if (isMetricEnabled("episodic_memory_count")) {
        ss << "  Episodic Memory Episodes: " << formatNumber(metrics_.episodicMemoryCount) << std::endl;
    }
    
    ss << std::endl;
    ss << "Cognitive Systems:" << std::endl;
    
    if (isMetricEnabled("concept_count")) {
        ss << "  Discovered Concepts: " << formatNumber(metrics_.conceptCount) << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceMonitor::getMemoryReport() const {
    std::stringstream ss;
    
    ss << "=== Memory Usage Report ===" << std::endl;
    ss << "Current Memory Usage: " << formatMemory(metrics_.memoryUsage * 1024 * 1024) << std::endl;
    ss << "Peak Memory Usage: " << formatMemory(metrics_.peakMemoryUsage * 1024 * 1024) << std::endl;
    ss << "Memory Utilization: " << std::fixed << std::setprecision(2) << (metrics_.memoryUtilization * 100) << "%" << std::endl;
    ss << "Working Memory Size: " << formatNumber(metrics_.workingMemorySize) << std::endl;
    ss << "Episodic Memory Count: " << formatNumber(metrics_.episodicMemoryCount) << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::getNeuralActivityReport() const {
    std::stringstream ss;
    
    ss << "=== Neural Activity Report ===" << std::endl;
    ss << "Total Spikes: " << formatNumber(metrics_.totalSpikes) << std::endl;
    ss << "Active Neurons: " << formatNumber(metrics_.activeNeurons) << std::endl;
    ss << "Total Synapses: " << formatNumber(metrics_.totalSynapses) << std::endl;
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(3) << metrics_.averageFiringRate << " Hz" << std::endl;
    ss << "Pending Spikes: " << formatNumber(metrics_.pendingSpikes) << std::endl;
    ss << "Plasticity Rate: " << std::fixed << std::setprecision(4) << metrics_.plasticityRate << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::getMemorySystemReport() const {
    std::stringstream ss;
    
    ss << "=== Memory System Report ===" << std::endl;
    ss << "Working Memory Traces: " << formatNumber(metrics_.workingMemorySize) << std::endl;
    ss << "Memory Utilization: " << std::fixed << std::setprecision(2) << (metrics_.memoryUtilization * 100) << "%" << std::endl;
    ss << "Episodic Memory Episodes: " << formatNumber(metrics_.episodicMemoryCount) << std::endl;
    ss << "Concept Count: " << formatNumber(metrics_.conceptCount) << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::getCognitiveSystemReport() const {
    std::stringstream ss;
    
    ss << "=== Cognitive System Report ===" << std::endl;
    ss << "Discovered Concepts: " << formatNumber(metrics_.conceptCount) << std::endl;
    ss << "Concept Learning Rate: N/A (requires tracking)" << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::reset() {
    updateCount_ = 0;
    metrics_ = PerformanceMetrics();
    previousFrameTime_ = std::chrono::steady_clock::time_point();
    frameCount_ = 0;
    
    if (pImpl) {
        pImpl->startTime = std::chrono::steady_clock::now();
        pImpl->lastSampleTime = pImpl->startTime;
        pImpl->cpuUsageSample1 = 0;
        pImpl->cpuUsageSample2 = 0;
        pImpl->memoryUsageSample1 = 0;
        pImpl->memoryUsageSample2 = 0;
        pImpl->sampleCount = 0;
    }
}

void PerformanceMonitor::startTiming(const std::string& operationName) {
    operationStartTime_ = std::chrono::steady_clock::now();
    previousOperation_ = operationName;
}

void PerformanceMonitor::endTiming(const std::string& operationName) {
    if (previousOperation_.empty()) {
        return;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - operationStartTime_);
    
    operationTimings_[previousOperation_].push_back(duration.count());
    
    if (verbose_) {
        std::cout << "Timing: " << previousOperation_ << " = " 
                  << std::fixed << std::setprecision(6) << duration.count() << " seconds" << std::endl;
    }
    
    previousOperation_.clear();
}

double PerformanceMonitor::getTiming(const std::string& operationName) const {
    auto it = operationTimings_.find(operationName);
    if (it != operationTimings_.end() && !it->second.empty()) {
        double total = 0.0;
        for (double duration : it->second) {
            total += duration;
        }
        return total / it->second.size();
    }
    return 0.0;
}

std::string PerformanceMonitor::getOperationTimingReport() const {
    std::stringstream ss;
    
    ss << "=== Operation Timing Report ===" << std::endl;
    
    if (operationTimings_.empty()) {
        ss << "No operation timings recorded." << std::endl;
        return ss.str();
    }
    
    for (const auto& pair : operationTimings_) {
        const std::string& operation = pair.first;
        const std::vector<double>& durations = pair.second;
        
        if (durations.empty()) {
            continue;
        }
        
        double total = 0.0;
        double min = durations[0];
        double max = durations[0];
        
        for (double duration : durations) {
            total += duration;
            min = std::min(min, duration);
            max = std::max(max, duration);
        }
        
        double average = total / durations.size();
        
        ss << operation << ": " << std::endl;
        ss << "  Total calls: " << durations.size() << std::endl;
        ss << "  Total time: " << formatTime(total) << std::endl;
        ss << "  Average: " << formatTime(average) << std::endl;
        ss << "  Min: " << formatTime(min) << std::endl;
        ss << "  Max: " << formatTime(max) << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceMonitor::formatNumber(size_t number) const {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string PerformanceMonitor::formatTime(double seconds) const {
    if (seconds < 1.0) {
        return std::to_string(seconds * 1000.0) + " ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + " s";
    } else if (seconds < 3600.0) {
        return std::to_string(seconds / 60.0) + " min";
    } else {
        return std::to_string(seconds / 3600.0) + " h";
    }
}

std::string PerformanceMonitor::formatMemory(double bytes) const {
    if (bytes < 1024.0) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024.0 * 1024.0) {
        return std::to_string(bytes / 1024.0) + " KB";
    } else if (bytes < 1024.0 * 1024.0 * 1024.0) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    }
}

double PerformanceMonitor::calculateCpuUsage() const {
    // Simplified CPU usage calculation
    // In a real implementation, this would read from system files or use platform-specific APIs
    return 50.0 + (updateCount_ % 20) * 2.0; // Simulated fluctuating CPU usage
}

double PerformanceMonitor::calculateMemoryUsage() const {
    // Simplified memory usage calculation
    // In a real implementation, this would read from system APIs
    return 100.0 + (updateCount_ % 10) * 5.0; // Simulated fluctuating memory usage
}

} // namespace nlm
