#include "PerformanceMonitor.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>

namespace nlm {

struct PerformanceMonitor::Impl {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool enabled;
    mutable std::mutex metricsMutex;
    
    PerformanceMetrics currentMetrics;
    std::chrono::system_clock::time_point cpuStartTime;
    
    Impl() : enabled(true), cpuStartTime(std::chrono::system_clock::now()) {
        reset();
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startTimer() {
    if (!pImpl->enabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->cpuStartTime = std::chrono::system_clock::now();
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::stopTimer() {
    if (!pImpl->enabled) return PerformanceMetrics();
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto end = std::chrono::high_resolution_clock::now();
    pImpl->endTime = end;
    
    // Calculate wall clock time
    auto duration = std::chrono::duration<double>(end - pImpl->startTime).count();
    pImpl->currentMetrics.wallClockTime = duration;
    
    // Calculate CPU time (approximate)
    auto cpuEnd = std::chrono::system_clock::now();
    auto cpuDuration = std::chrono::duration<double>(cpuEnd - pImpl->cpuStartTime).count();
    pImpl->currentMetrics.cpuTime = cpuDuration;
    
    return pImpl->currentMetrics;
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    if (!pImpl->enabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->currentMetrics.customMetrics[name] = value;
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::getMetrics() const {
    if (!pImpl->enabled) return PerformanceMetrics();
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->currentMetrics;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->currentMetrics = PerformanceMetrics();
}

std::string PerformanceMonitor::generateReport() const {
    if (!pImpl->enabled) return "Performance monitoring is disabled.";
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::ostringstream oss;
    oss << "=== Performance Report ===" << std::endl;
    oss << "Wall Clock Time: " << std::fixed << std::setprecision(3) 
        << pImpl->currentMetrics.wallClockTime << " seconds" << std::endl;
    oss << "CPU Time: " << std::fixed << std::setprecision(3) 
        << pImpl->currentMetrics.cpuTime << " seconds" << std::endl;
    oss << "Steps per second: " << (pImpl->currentMetrics.totalSteps / 
        std::max(1.0, pImpl->currentMetrics.wallClockTime)) << std::endl;
    oss << "Spikes per second: " << (pImpl->currentMetrics.totalSpikes / 
        std::max(1.0, pImpl->currentMetrics.wallClockTime)) << std::endl;
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.avgFiringRate << " Hz" << std::endl;
    oss << "Memory Usage: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.memoryUsageMB << " MB" << std::endl;
    
    if (!pImpl->currentMetrics.customMetrics.empty()) {
        oss << "\nCustom Metrics:" << std::endl;
        for (const auto& pair : pImpl->currentMetrics.customMetrics) {
            oss << "  " << pair.first << ": " << std::fixed << std::setprecision(4) 
                << pair.second << std::endl;
        }
    }
    
    return oss.str();
}

bool PerformanceMonitor::isEnabled() const {
    return pImpl->enabled;
}

void PerformanceMonitor::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

} // namespace nlm
