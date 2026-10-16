#pragma once

#include "Profiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace nlm {

// Timer class for high-resolution profiling
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::string name_;
    std::string componentName_;
    bool isActive_ = false;
    
public:
    Timer(const std::string& name, const std::string& component = "")
        : name_(name), componentName_(component), isActive_(false) {}
    
    void start() {
        if (!isActive_) {
            startTime_ = std::chrono::high_resolution_clock::now();
            isActive_ = true;
        }
    }
    
    void stop() {
        if (isActive_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
            
            // Record the timing
            std::lock_guard<std::mutex> lock(timingMutex_);
            auto key = componentName_.empty() ? name_ : componentName_ + "::" + name_;
            
            if (componentTimings_.find(key) == componentTimings_.end()) {
                componentTimings_[key] = ComponentTiming(key);
            }
            
            auto& timing = componentTimings_[key];
            timing.totalDuration += duration;
            timing.minDuration = std::min(timing.minDuration, duration);
            timing.maxDuration = std::max(timing.maxDuration, duration);
            timing.lastDuration = duration;
            timing.callCount++;
            
            isActive_ = false;
        }
    }
    
    bool isActive() const { return isActive_; }
    
    std::chrono::nanoseconds getElapsedTime() const {
        if (!isActive_) return std::chrono::nanoseconds::zero();
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime_);
    }
};

// Profiler implementation
Profiler::Profiler() : isRunning_(false), samplingIntervalMs_(1000) {
    // Initialize timers
    systemTimer_ = std::make_unique<Timer>("System", "System");
    neuralTimer_ = std::make_unique<Timer>("NeuralNetwork", "NeuralNetwork");
    memoryTimer_ = std::make_unique<Timer>("Memory", "MemorySystem");
    configTimer_ = std::make_unique<Timer>("Config", "ConfigSystem");
    buildTimer_ = std::make_unique<Timer>("Build", "BuildSystem");
}

Profiler::~Profiler() {
    stop();
}

void Profiler::start() {
    if (!isRunning_) {
        isRunning_ = true;
        systemTimer_->start();
        
        // Start system monitoring thread
        systemMonitoringThread_ = std::thread(&Profiler::monitorSystem, this);
        
        NLM_LOG_INFO("Profiler started");
    }
}

void Profiler::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        if (systemMonitoringThread_.joinable()) {
            systemMonitoringThread_.join();
        }
        
        systemTimer_->stop();
        
        // Calculate final statistics
        for (auto& [name, timing] : componentTimings_) {
            timing.averageDuration = (timing.callCount > 0) 
                ? (timing.totalDuration.count() / timing.callCount) / 1000000.0  // Convert to ms
                : 0.0;
        }
        
        NLM_LOG_INFO("Profiler stopped");
    }
}

void Profiler::startComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    // Start the appropriate timer based on component type
    if (componentName.find("Neural") != std::string::npos || 
        componentName.find("Neuron") != std::string::npos ||
        componentName.find("Synapse") != std::string::npos) {
        neuralTimer_->start();
    } else if (componentName.find("Memory") != std::string::npos ||
               componentName.find("Allocation") != std::string::npos) {
        memoryTimer_->start();
    } else if (componentName.find("Config") != std::string::npos) {
        configTimer_->start();
    } else if (componentName.find("Build") != std::string::npos) {
        buildTimer_->start();
    } else {
        systemTimer_->start();
    }
}

void Profiler::endComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    if (componentName.find("Neural") != std::string::npos || 
        componentName.find("Neuron") != std::string::npos ||
        componentName.find("Synapse") != std::string::npos) {
        neuralTimer_->stop();
    } else if (componentName.find("Memory") != std::string::npos ||
               componentName.find("Allocation") != std::string::npos) {
        memoryTimer_->stop();
    } else if (componentName.find("Config") != std::string::npos) {
        configTimer_->stop();
    } else if (componentName.find("Build") != std::string::npos) {
        buildTimer_->stop();
    } else {
        systemTimer_->stop();
    }
}

void Profiler::recordMemoryAllocation(const std::string& component, size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    if (memoryAllocations_.find(component) == memoryAllocations_.end()) {
        memoryAllocations_[component] = MemoryAllocation();
    }
    
    auto& allocation = memoryAllocations_[component];
    allocation.allocationSize += size;
    allocation.maxAllocated = std::max(allocation.maxAllocated, allocation.maxAllocated + size);
    allocation.allocationCount++;
}

void Profiler::recordMemoryDeallocation(const std::string& component, size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    if (memoryAllocations_.find(component) == memoryAllocations_.end()) {
        memoryAllocations_[component] = MemoryAllocation();
    }
    
    auto& allocation = memoryAllocations_[component];
    allocation.deallocationSize += size;
    allocation.deallocationCount++;
}

std::unordered_map<std::string, ComponentTiming> Profiler::getComponentTimings() const {
    std::lock_guard<std::mutex> lock(timingMutex_);
    return componentTimings_;
}

std::unordered_map<std::string, MemoryAllocation> Profiler::getMemoryAllocations() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return memoryAllocations_;
}

PerformanceMetrics Profiler::getSystemMetrics() {
    std::lock_guard<std::mutex> lock(systemMutex_);
    return currentSystemMetrics_;
}

void Profiler::setSamplingInterval(size_t intervalMs) {
    samplingIntervalMs_ = intervalMs;
}

void Profiler::monitorSystem() {
    while (isRunning_) {
        // Update system metrics
        updateSystemMetrics();
        
        // Calculate component timing statistics
        calculateComponentStatistics();
        
        // Sleep for the sampling interval
        std::this_thread::sleep_for(std::chrono::milliseconds(samplingIntervalMs_));
    }
}

void Profiler::updateSystemMetrics() {
    // Simulate system metrics for demonstration
    currentSystemMetrics_.cpuUsage = calculateCpuUsage();
    currentSystemMetrics_.memoryUsage = calculateMemoryUsage();
    currentSystemMetrics_.processMemory = calculateProcessMemory();
    currentSystemMetrics_.diskUsage = calculateDiskUsage();
    currentSystemMetrics_.networkUsage = calculateNetworkUsage();
    currentSystemMetrics_.temperature = calculateCpuTemperature();
    
    currentSystemMetrics_.timestamp = std::chrono::system_clock::now();
    
    // Record metrics history
    metricsHistory_.push_back(currentSystemMetrics_);
    
    // Keep only last 1000 entries
    if (metricsHistory_.size() > 1000) {
        metricsHistory_.erase(metricsHistory_.begin());
    }
}

double Profiler::calculateCpuUsage() const {
    // Simulate CPU usage calculation
    // In real implementation, would read from /proc/stat or use platform-specific APIs
    static double currentCpuUsage = 0.0;
    
    // Simulate some CPU usage variation
    currentCpuUsage = 20.0 + (rand() % 60); // 20-80%
    
    return std::min(100.0, currentCpuUsage);
}

double Profiler::calculateMemoryUsage() const {
    // Simulate memory usage calculation
    static double currentMemoryUsage = 0.0;
    
    // Simulate some memory usage variation
    currentMemoryUsage = 30.0 + (rand() % 50); // 30-80%
    
    return std::min(100.0, currentMemoryUsage);
}

uint64_t Profiler::calculateProcessMemory() const {
    // Simulate process memory calculation
    return 100 * 1024 * 1024 + (rand() % 400 * 1024 * 1024); // 100MB - 500MB
}

double Profiler::calculateDiskUsage() const {
    // Simulate disk usage calculation
    return 60.0 + (rand() % 30); // 60-90%
}

double Profiler::calculateNetworkUsage() const {
    // Simulate network usage calculation
    return (rand() % 1000) / 10.0; // 0-100 Mbps
}

double Profiler::calculateCpuTemperature() const {
    // Simulate CPU temperature
    return 45.0 + (rand() % 30); // 45-75 Celsius
}

void Profiler::calculateComponentStatistics() {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    for (auto& [name, timing] : componentTimings_) {
        if (timing.callCount > 0) {
            // Calculate average
            timing.averageDuration = (timing.totalDuration.count() / timing.callCount) / 1000000.0; // ms
            
            // Calculate percentiles
            calculatePercentiles(timing);
        }
    }
}

void Profiler::calculatePercentiles(ComponentTiming& timing) {
    // Simple percentile calculation for demonstration
    // In real implementation, would maintain sorted list of all durations
    double minDuration = timing.minDuration.count() / 1000000.0; // ms
    double maxDuration = timing.maxDuration.count() / 1000000.0; // ms
    double avgDuration = timing.averageDuration;
    
    // Estimate 95th and 99th percentiles
    timing.p95Duration = maxDuration * 0.95;
    timing.p99Duration = maxDuration * 0.99;
}

void Profiler::cleanupExpiredData() {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    // Remove old component timings
    auto it = componentTimings_.begin();
    while (it != componentTimings_.end()) {
        if (it->second.callCount == 0) {
            it = componentTimings_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string Profiler::generateHtmlReport() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>NLM Performance Report</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
    oss << "        .metric { margin: 10px 0; padding: 10px; background-color: #f9f9f9; border-radius: 3px; }" << std::endl;
    oss << "        .critical { border-left: 4px solid #ff0000; }" << std::endl;
    oss << "        .warning { border-left: 4px solid #ff9900; }" << std::endl;
    oss << "        .good { border-left: 4px solid #00cc00; }" << std::endl;
    oss << "        table { border-collapse: collapse; width: 100%; }" << std::endl;
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "        th { background-color: #f2f2f2; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    
    oss << "    <div class=\"header\">" << std::endl;
    oss << "        <h1>NLM Performance Report</h1>" << std::endl;
    oss << "        <p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>System Metrics</h2>" << std::endl;
    oss << "    <div class=\"metric\">" << std::endl;
    oss << "        <h3>CPU Usage: " << currentSystemMetrics_.cpuUsage << "%</h3>" << std::endl;
    oss << "        <div style=\"width: " << currentSystemMetrics_.cpuUsage << "%; height: 20px; background-color: " 
        << getCpuUsageColor(currentSystemMetrics_.cpuUsage) << ">" << std::endl;
    oss << "        </div>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>Component Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Component</th><th>Calls</th><th>Avg Time (ms)</th><th>P95 (ms)</th><th>P99 (ms)</th><th>Max (ms)</th></tr>" << std::endl;
    
    for (const auto& [name, timing] : componentTimings_) {
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << name << "</td>" << std::endl;
        oss << "            <td>" << timing.callCount << "</td>" << std::endl;
        oss << "            <td>" << timing.averageDuration << "</td>" << std::endl;
        oss << "            <td>" << timing.p95Duration << "</td>" << std::endl;
        oss << "            <td>" << timing.p99Duration << "</td>" << std::endl;
        oss << "            <td>" << (timing.maxDuration.count() / 1000000.0) << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "    <h2>Memory Allocations</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Component</th><th>Allocations</th><th>Deallocations</th><th>Net (bytes)</th><th>Max Allocated (bytes)</th></tr>" << std::endl;
    
    for (const auto& [name, allocation] : memoryAllocations_) {
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << name << "</td>" << std::endl;
        oss << "            <td>" << allocation.allocationCount << "</td>" << std::endl;
        oss << "            <td>" << allocation.deallocationCount << "</td>" << std::endl;
        oss << "            <td>" << allocation.netAllocation << "</td>" << std::endl;
        oss << "            <td>" << allocation.maxAllocated << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string Profiler::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string Profiler::getCpuUsageColor(double usage) const {
    if (usage > 90.0) {
        return "#ff0000"; // Red
    } else if (usage > 70.0) {
        return "#ff9900"; // Orange
    } else {
        return "#00cc00"; // Green
    }
}