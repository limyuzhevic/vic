// Performance monitor implementation
// Includes timers, profiling, memory tracking, and debugging capabilities

#include "performance/PerformanceMonitor.hpp"
#include <numeric>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace nlm {

namespace {
    // Simple CPU usage calculation (placeholder implementation)
    double getCpuUsage() {
        // This is a simplified implementation - real implementation would parse /proc/stat
        static double lastCpuUsage = 0.0;
        static auto lastTime = chrono::steady_clock::now();
        
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(currentTime - lastTime).count();
        
        if (elapsed >= 1) {
            lastCpuUsage = 50.0;  // Placeholder: simulate 50% CPU usage
            lastTime = currentTime;
        }
        
        return lastCpuUsage;
    }
    
    // Simple memory usage calculation (placeholder implementation)
    double getMemoryUsage() {
        // This is a simplified implementation - real implementation would parse /proc/meminfo
        // Return memory usage as percentage of system memory
        return 75.0;  // Placeholder: simulate 75% memory usage
    }
    
    // Get number of processor cores
    size_t getCoreCount() {
        return std::thread::hardware_concurrency();
    }
    
    // Parse /proc/stat for CPU time
    std::vector<double> getCpuTimes() {
        std::vector<double> times(4, 0.0);  // user, nice, system, idle
        
        std::ifstream file("/proc/stat");
        if (file.is_open()) {
            std::string line;
            std::getline(file, line);
            if (line.find("cpu") == 0) {
                std::istringstream iss(line.substr(5));
                std::string token;
                size_t i = 0;
                while (iss >> token && i < times.size()) {
                    try {
                        times[i++] = std::stod(token);
                    } catch (const std::exception& e) {
                        times[i++] = 0.0;
                    }
                }
            }
        }
        return times;
    }
}

// PerformanceMonitor method implementations
bool PerformanceProfiler::shouldRecordMetrics() const {
    return true;  // Always record in this simplified version
}

size_t PerformanceProfiler::getTimerCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return timers.size();
}

size_t PerformanceProfiler::getSystemMetricCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return systemMetrics.size();
}

void PerformanceMonitor::addTimer(const std::string& name, std::unique_ptr<Timer> timer) {
    std::lock_guard<std::mutex> lock(mutex);
    profiler->registerTimer(name, std::move(timer));
}

DebugController& PerformanceMonitor::getDebugController() {
    return *debugController;
}

MemoryProfiler& PerformanceMonitor::getMemoryProfiler() {
    return *memoryProfiler;
}

PerformanceProfiler& PerformanceMonitor::getProfiler() {
    return *profiler;
}

// Timer method implementations
Timer::Timer() : startTime(chrono::high_resolution_clock::now()),
                 totalDuration(0), count(0), maxDuration(0),
                 minDuration(std::numeric_limits<chrono::nanoseconds::rep>::max()) {}

std::string Timer::formatDuration(chrono::nanoseconds::rep nanoseconds) {
    if (nanoseconds >= chrono::nanoseconds::rep(chrono::hours(1).count())) {
        auto hours = nanoseconds / chrono::hours(1).count();
        nanoseconds %= chrono::hours(1).count();
        return std::to_string(hours) + "h " + formatDuration(nanoseconds);
    } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::minutes(1).count())) {
        auto minutes = nanoseconds / chrono::minutes(1).count();
        nanoseconds %= chrono::minutes(1).count();
        return std::to_string(minutes) + "m " + formatDuration(nanoseconds);
    } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::seconds(1).count())) {
        auto seconds = nanoseconds / chrono::seconds(1).count();
        nanoseconds %= chrono::seconds(1).count();
        return std::to_string(seconds) + "s " + formatDuration(nanoseconds);
    } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::milliseconds(1).count())) {
        auto ms = nanoseconds / chrono::milliseconds(1).count();
        nanoseconds %= chrono::milliseconds(1).count();
        return std::to_string(ms) + "ms " + formatDuration(nanoseconds);
    } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::microseconds(1).count())) {
        auto us = nanoseconds / chrono::microseconds(1).count();
        nanoseconds %= chrono::microseconds(1).count();
        return std::to_string(us) + "us " + formatDuration(nanoseconds);
    } else {
        return std::to_string(nanoseconds) + "ns";
    }
}