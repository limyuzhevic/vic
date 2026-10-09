#pragma once

#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include <numeric>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : 
    isMonitoring_(false), 
    samplingRate_(1.0f), 
    currentMetrics_(),
    lastUpdateTime_(0.0),
    updateCount_(0) {
    metricsHistory_.reserve(10000);
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isMonitoring_) {
        return;
    }
    
    isMonitoring_ = true;
    lastUpdateTime_ = std::chrono::system_clock::now();
    
    // Start monitoring thread
    monitoringThread_ = std::thread(&PerformanceMonitor::monitorLoop, this);
}

void PerformanceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isMonitoring_) {
        return;
    }
    
    isMonitoring_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceMonitor::monitorLoop() {
    while (isMonitoring_) {
        updateMetrics();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0f / samplingRate_)));
    }
}

void PerformanceMonitor::updateMetrics() {
    auto currentTime = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUpdateTime_);
    
    PerformanceMetrics metrics;
    
    // Update timestamp
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
    
    // Collect system metrics
    collectSystemMetrics(metrics);
    
    // Collect application metrics
    collectApplicationMetrics(metrics);
    
    // Check for performance issues
    checkPerformanceIssues();
    
    // Update history
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsHistory_.push_back(metrics);
        
        // Limit history size
        while (metricsHistory_.size() > 10000) {
            metricsHistory_.erase(metricsHistory_.begin());
        }
        
        currentMetrics_ = metrics;
        lastUpdateTime_ = currentTime;
        updateCount_++;
    }
}

void PerformanceMonitor::collectSystemMetrics(PerformanceMetrics& metrics) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    
    // CPU metrics (simplified - in real implementation, use platform-specific APIs)
    metrics.cpuUsage = 0.0f;
    metrics.cpuTemperature = 0.0f;
    metrics.systemLoad = 0.0f;
    
    // Memory metrics (simplified - in real implementation, use platform-specific APIs)
    metrics.memoryUsage = 0;
    metrics.memoryPeak = 0;
    metrics.memoryAvailable = 0;
    
    // Get system load average
    std::ifstream loadFile("/proc/loadavg");
    if (loadFile.is_open()) {
        float a, b, c;
        loadFile >> a >> b >> c;
        metrics.systemLoad = a;
        loadFile.close();
    }
}

void PerformanceMonitor::collectApplicationMetrics(PerformanceMetrics& metrics) {
    // Get brain instance if available (simplified - would need integration)
    // For now, use placeholder values
    
    metrics.activeNeurons = 0;
    metrics.activeSynapses = 0;
    metrics.averageFiringRate = 0.0f;
    metrics.totalSpikes = 0;
    
    // Calculate update time
    if (updateCount_ > 0) {
        auto currentTime = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastUpdateTime_);
        metrics.updateTime = elapsed.count() / 1000.0f; // Convert to milliseconds
    }
    
    // Calculate step time
    metrics.stepTime = 0.0f; // Would need to track step execution time
    
    // Calculate processing time
    metrics.processingTime = 0.0f; // Would need to track processing time
}

void PerformanceMonitor::checkPerformanceIssues() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceMetrics& metrics = currentMetrics_;
    
    // Check for memory issues
    if (metrics.memoryUsage > metrics.memoryPeak * 0.9) {
        addAlert("High memory usage: " + std::to_string(metrics.memoryUsage) + " bytes", "WARNING");
    }
    
    // Check for CPU issues
    if (metrics.cpuUsage > 0.8f) {
        addAlert("High CPU usage: " + std::to_string(metrics.cpuUsage * 100.0f) + "%", "WARNING");
    }
    
    // Check for system load issues
    if (metrics.systemLoad > 2.0f) {
        addAlert("High system load: " + std::to_string(metrics.systemLoad), "WARNING");
    }
    
    // Check for application performance
    if (metrics.updateTime > 100.0f) { // 100ms threshold
        addAlert("Slow update time: " + std::to_string(metrics.updateTime) + "ms", "WARNING");
    }
    
    if (metrics.processingTime > 50.0f) { // 50ms threshold
        addAlert("Slow processing time: " + std::to_string(metrics.processingTime) + "ms", "WARNING");
    }
}

void PerformanceMonitor::addAlert(const std::string& message, const std::string& severity) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    alerts_.push_back("[".append(severity).append("] ").append(message));
    
    // Keep only recent alerts
    if (alerts_.size() > 100) {
        alerts_.erase(alerts_.begin());
    }
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::getMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentMetrics_;
}

std::vector<PerformanceMonitor::PerformanceMetrics> PerformanceMonitor::getHistory(size_t maxSamples) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    std::vector<PerformanceMetrics> result;
    
    if (maxSamples >= metricsHistory_.size()) {
        result = metricsHistory_;
    } else {
        size_t startIndex = metricsHistory_.size() - maxSamples;
        result.assign(metricsHistory_.begin() + startIndex, metricsHistory_.end());
    }
    
    return result;
}

void PerformanceMonitor::enableMetric(const std::string& metric, bool enable) {
    // Implementation for enabling/disabling specific metrics
    // Would need to track which metrics are collected
}

bool PerformanceMonitor::isMetricEnabled(const std::string& metric) const {
    // Implementation for checking if metric is enabled
    // Would need to track which metrics are collected
    return true;
}

std::vector<std::string> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return alerts_;
}

void PerformanceMonitor::clear() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentMetrics_ = PerformanceMetrics();
    metricsHistory_.clear();
    alerts_.clear();
    
    updateCount_ = 0;
    lastUpdateTime_ = std::chrono::system_clock::now();
}

bool PerformanceMonitor::saveMetrics(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Performance Metrics" << std::endl;
        file << "===================" << std::endl;
        file << "Number of samples: " << metricsHistory_.size() << std::endl;
        file << std::endl;
        
        // Write header
        file << "Timestamp,CPU Usage,Memory Usage,Active Neurons,Average Firing Rate,Update Time" << std::endl;
        
        for (const auto& metrics : metricsHistory_) {
            file << metrics.timestamp << ",";
            file << metrics.cpuUsage << ",";
            file << metrics.memoryUsage << ",";
            file << metrics.activeNeurons << ",";
            file << metrics.averageFiringRate << ",";
            file << metrics.updateTime << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceMonitor::loadMetrics(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        bool isFirstLine = true;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (isFirstLine) {
                // Skip header line
                isFirstLine = false;
                continue;
            }
            
            std::stringstream lineStream(line);
            std::string token;
            PerformanceMetrics metrics;
            
            std::getline(lineStream, token, ',');
            metrics.timestamp = std::stoull(token);
            
            std::getline(lineStream, token, ',');
            metrics.cpuUsage = std::stof(token);
            
            std::getline(lineStream, token, ',');
            metrics.memoryUsage = std::stoull(token);
            
            std::getline(lineStream, token, ',');
            metrics.activeNeurons = std::stoull(token);
            
            std::getline(lineStream, token, ',');
            metrics.averageFiringRate = std::stof(token);
            
            std::getline(lineStream, token, ',');
            metrics.updateTime = std::stof(token);
            
            metricsHistory_.push_back(metrics);
        }
        
        file.close();
        
        if (!metricsHistory_.empty()) {
            currentMetrics_ = metricsHistory_.back();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace nlm
