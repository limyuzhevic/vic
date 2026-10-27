// NLM Performance Benchmarking Module Implementation
// Implementation of the enhanced performance benchmarking functionality
// Provides comprehensive performance monitoring and analysis

#include "NLMBenchmarks.hpp"
#include "PlatformImplementation.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace performance {

// Platform-specific implementations
// MemoryTracker implementation
void MemoryTracker::startTracking(double interval) {
    tracking = true;
    trackingThread = std::thread([this, interval]() {
        while (tracking) {
            size_t memUsage = getCurrentMemoryUsage();
            std::lock_guard<std::mutex> lock(mutex);
            memorySamples.push_back(memUsage);
            if (memUsage > peakMemory) {
                peakMemory = memUsage;
            }
            currentMemory = memUsage;
            std::this_thread::sleep_for(std::chrono::duration<double>(interval));
        }
    });
}

double MemoryTracker::getCurrentMemoryUsage() const {
    return getSystemMemoryUsage();
}

// CPUMonitor implementation
void CPUMonitor::startMonitoring(double interval) {
    monitoring = true;
    monitoringThread = std::thread([this, interval]() {
        while (monitoring) {
            double cpuUsage = getCPUUsage();
            std::lock_guard<std::mutex> lock(mutex);
            cpuSamples.push_back(cpuUsage);
            std::this_thread::sleep_for(std::chrono::duration<double>(interval));
        }
    });
}

double CPUMonitor::getCPUUsage() const {
    return getSystemCPUUsage();
}

// Helper function implementations
std::string NLMBenchmark::getSystemInfo() const {
    return getSystemInfo();
}

std::string NLMBenchmark::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace performance
} // namespace nlm
