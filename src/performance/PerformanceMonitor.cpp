#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : monitoring_(false), monitoringThreadActive_(false),
    memoryThresholdMB_(512.0), cpuThresholdPercent_(80.0), timeThresholdSeconds_(300.0),
    systemMonitoringEnabled_(false), updateInterval_(0.1) {}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
    if (fileOutput_.is_open()) {
        fileOutput_.close();
    }
}

void PerformanceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!monitoring_) {
        monitoring_ = true;
        monitoringThreadActive_ = true;
        monitorThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    }
}

void PerformanceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    monitoring_ = false;
    monitoringThreadActive_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

void PerformanceMonitor::pauseMonitoring(bool pause) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Implementation would pause monitoring thread
    // For simplicity, we'll just track pause state
}

void PerformanceMonitor::recordMetrics(size_t neurons, size_t totalNeurons, size_t firingNeurons,
                                       size_t totalSpikes, float avgFiringRate, double simTime,
                                       size_t steps, double cpuUsage, double memoryUsage) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PerformanceMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.cpuUsage = cpuUsage;
    metrics.memoryUsage = memoryUsage;
    metrics.memoryPercent = totalNeurons > 0 ? (neurons / (double)totalNeurons) * 100.0 : 0.0;
    metrics.activeNeurons = neurons;
    metrics.totalNeurons = totalNeurons;
    metrics.firingNeurons = firingNeurons;
    metrics.totalSpikes = totalSpikes;
    metrics.averageFiringRate = avgFiringRate;
    metrics.simulationTime = simTime;
    metrics.simulationSteps = steps;
    
    // Update current metrics
    currentMetrics_ = metrics;
    
    // Update statistics
    currentStats_.totalTime = simTime;
    currentStats_.totalSteps = steps;
    currentStats_.avgCpuUsage = (currentStats_.avgCpuUsage * (steps - 1) + cpuUsage) / steps;
    currentStats_.avgMemoryUsage = (currentStats_.avgMemoryUsage * (steps - 1) + memoryUsage) / steps;
    currentStats_.maxMemoryUsage = std::max(currentStats_.maxMemoryUsage, memoryUsage);
    currentStats_.avgFiringRate = (currentStats_.avgFiringRate * (steps - 1) + avgFiringRate) / steps;
    currentStats_.totalSpikesPerSecond = totalSpikes / simTime;
    currentStats_.peakNeurons = std::max(currentStats_.peakNeurons, neurons);
    currentStats_.peakFiringNeurons = std::max(currentStats_.peakFiringNeurons, firingNeurons);
    
    // Check thresholds
    if (memoryUsage > memoryThresholdMB_) {
        triggerAlerts("Memory usage exceeded threshold: " + std::to_string(memoryUsage) + "MB", 1.0);
        currentStats_.performanceViolations++;
    }
    if (cpuUsage > cpuThresholdPercent_) {
        triggerAlerts("CPU usage exceeded threshold: " + std::to_string(cpuUsage) + "%", 1.0);
        currentStats_.performanceViolations++;
    }
    if (simTime > timeThresholdSeconds_) {
        triggerAlerts("Simulation time exceeded threshold: " + std::to_string(simTime) + "s", 1.0);
        currentStats_.performanceViolations++;
    }
    
    // Record benchmark
    recordBenchmark("step_metrics", simTime);
}

void PerformanceMonitor::recordBenchmark(const std::string& name, double duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    BenchmarkInfo info;
    info.name = name;
    info.duration = duration;
    info.startTime = std::chrono::system_clock::now();
    
    activeBenchmarks_.push_back(info);
    
    // Record completed benchmarks
    std::vector<BenchmarkResult> results;
    for (const auto& benchmark : activeBenchmarks_) {
        BenchmarkResult result;
        result.name = benchmark.name;
        result.duration = benchmark.duration;
        result.iterations = 1;
        result.averageTimePerIteration = benchmark.duration;
        result.minTime = benchmark.duration;
        result.maxTime = benchmark.duration;
        result.standardDeviation = 0.0;
        results.push_back(result);
    }
    
    benchmarkResults_ = results;
    activeBenchmarks_.clear();
}

void PerformanceMonitor::monitoringLoop() {
    while (monitoringThreadActive_) {
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(updateInterval_ * 1000)));
        
        if (monitoringThreadActive_) {
            updateSystemMetrics();
            checkThresholds();
        }
    }
}

void PerformanceMonitor::updateSystemMetrics() {
    if (!systemMonitoringEnabled_) return;
    
    // Update system metrics (simplified implementation)
    // In a real implementation, this would read from /proc, /sys, or other system interfaces
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(now - lastSystemUpdate_).count();
    
    if (duration >= 1.0) { // Update once per second
        // Placeholder values - would be replaced with actual system readings
        double cpuUsage = 0.0; // Read from /proc/stat
        double memoryUsage = 0.0; // Read from /proc/meminfo
        
        // Update current metrics
        currentMetrics_.cpuUsage = cpuUsage;
        currentMetrics_.memoryUsage = memoryUsage;
        currentMetrics_.timestamp = now;
        
        lastSystemUpdate_ = now;
    }
}

void PerformanceMonitor::checkThresholds() {
    // Threshold checking already done in recordMetrics
}

void PerformanceMonitor::triggerAlerts(const std::string& message, double severity) {
    for (const auto& callback : alertCallbacks_) {
        try {
            callback(message, severity);
        } catch (const std::exception& e) {
            // Ignore callback errors to prevent monitoring thread crash
        }
    }
}

void PerformanceMonitor::enableFileOutput(const std::string& filepath, bool append) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileOutput_.is_open()) {
        fileOutput_.close();
    }
    
    fileOutput_.open(filepath, append ? std::ios::app : std::ios::trunc);
    currentLogFile_ = filepath;
    
    if (!fileOutput_.is_open()) {
        throw std::runtime_error("Failed to open performance log file: " + filepath);
    }
    
    // Write header
    fileOutput_ << "NLM Performance Log" << std::endl;
    fileOutput_ << "Generated at: " << getCurrentTimestamp() << std::endl;
    fileOutput_ << "=" * 50 << std::endl;
}

void PerformanceMonitor::disableFileOutput() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileOutput_.is_open()) {
        fileOutput_.close();
    }
    currentLogFile_ = "";
}

std::string PerformanceMonitor::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "NLM Performance Report" << std::endl;
    ss << "Generated at: " << getCurrentTimestamp() << std::endl;
    ss << "=" * 50 << std::endl << std::endl;
    
    ss << "Total Simulation Time: " << currentStats_.totalTime << " seconds" << std::endl;
    ss << "Total Steps: " << currentStats_.totalSteps << std::endl;
    ss << "Average CPU Usage: " << currentStats_.avgCpuUsage << "%" << std::endl;
    ss << "Average Memory Usage: " << currentStats_.avgMemoryUsage << " MB" << std::endl;
    ss << "Peak Memory Usage: " << currentStats_.maxMemoryUsage << " MB" << std::endl;
    ss << "Average Firing Rate: " << currentStats_.avgFiringRate << " spikes/second" << std::endl;
    ss << "Total Spikes Per Second: " << currentStats_.totalSpikesPerSecond << std::endl;
    ss << "Peak Neurons Active: " << currentStats_.peakNeurons << std::endl;
    ss << "Peak Firing Neurons: " << currentStats_.peakFiringNeurons << std::endl;
    ss << "Performance Violations: " << currentStats_.performanceViolations << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::generateBenchmarkReport() const {
    std::stringstream ss;
    ss << "NLM Benchmark Report" << std::endl;
    ss << "Generated at: " << getCurrentTimestamp() << std::endl;
    ss << "=" * 50 << std::endl << std::endl;
    
    if (benchmarkResults_.empty()) {
        ss << "No benchmarks recorded." << std::endl;
        return ss.str();
    }
    
    ss << "Benchmark Results:" << std::endl;
    ss << std::left << std::setw(20) << "Name" << std::setw(15) << "Duration (s)" 
       << std::setw(15) << "Avg Time (ms)" << std::setw(12) << "Min Time (ms)" 
       << std::setw(12) << "Max Time (ms)" << std::endl;
    ss << std::string(74, '-') << std::endl;
    
    for (const auto& result : benchmarkResults_) {
        ss << std::left << std::setw(20) << result.name 
           << std::setw(15) << std::fixed << std::setprecision(3) << result.duration
           << std::setw(15) << std::fixed << std::setprecision(3) << result.averageTimePerIteration * 1000
           << std::setw(12) << std::fixed << std::setprecision(3) << result.minTime * 1000
           << std::setw(12) << std::fixed << std::setprecision(3) << result.maxTime * 1000 << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, double> PerformanceMonitor::analyzePerformanceTrends(size_t windowSize) const {
    std::map<std::string, double> trends;
    
    // Simple trend analysis (would be more sophisticated in real implementation)
    trends["cpuTrend"] = 0.0; // 0 = stable, >0 = increasing, <0 = decreasing
    trends["memoryTrend"] = 0.0;
    trends["firingRateTrend"] = 0.0;
    
    return trends;
}

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm