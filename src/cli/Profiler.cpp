// Profiler.cpp - Performance profiling implementation for NLM

#include "cli/Profiler.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>

namespace nlm {

Profiler::Profiler()
    : isActive(false),
      sessionName("default"),
      startTime(),
      endTime(),
      totalCPUTime(0),
      peakMemoryUsage(0),
      operationsCount(0),
      errorInjectionActive(false),
      errorStartTime(),
      errorCount(0) {
}

Profiler::~Profiler() = default;

void Profiler::startProfiling(const std::string& sessionName) {
    NLM_LOG_INFO("Starting profiling session: " + sessionName);
    
    this->sessionName = sessionName;
    isActive = true;
    startTime = std::chrono::steady_clock::now();
    errorStartTime = std::chrono::steady_clock::now();
    errorCount = 0;
    
    // Record initial memory usage
    recordMemoryUsage(getPeakMemoryUsage());
    
    NLM_LOG_INFO("Profiling session started");
}

void Profiler::stopProfiling() {
    if (!isActive) {
        NLM_LOG_WARNING("Cannot stop profiling: profiling not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping profiling session: " + sessionName);
    
    endTime = std::chrono::steady_clock::now();
    isActive = false;
    
    // Update CPU time
    updateCPUTime();
    
    NLM_LOG_INFO("Profiling session stopped");
    NLM_LOG_INFO("  Total operations: " + std::to_string(operationsCount));
    NLM_LOG_INFO("  Elapsed time: " + formatTime(getElapsedTime()) + "ms");
    NLM_LOG_INFO("  Peak memory usage: " + formatMemory(peakMemoryUsage));
}

int64_t Profiler::getElapsedTime() const {
    if (!isActive) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return elapsed.count();
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    return elapsed.count();
}

void Profiler::recordOperation(const std::string& operationName, int64_t duration) {
    if (!isActive) return;
    
    operationTimes[operationName] = duration;
    operationsCount++;
    
    if (verbose) {
        NLM_LOG_DEBUG("Recorded operation: " + operationName + " duration: " + 
                     std::to_string(duration) + "ms");
    }
}

void Profiler::recordMemoryUsage(int64_t memoryUsage) {
    if (!isActive) return;
    
    memoryUsages.push_back(memoryUsage);
    
    if (memoryUsage > peakMemoryUsage) {
        peakMemoryUsage = memoryUsage;
    }
    
    if (verbose) {
        NLM_LOG_DEBUG("Recorded memory usage: " + formatMemory(memoryUsage));
    }
}

void Profiler::updateCPUTime() {
    totalCPUTime = getElapsedTime();
}

double Profiler::calculateOperationsPerSecond() const {
    int64_t elapsed = getElapsedTime();
    if (elapsed == 0) return 0.0;
    
    return static_cast<double>(operationsCount) / static_cast<double>(elapsed) * 1000.0;
}

std::string Profiler::formatTime(int64_t milliseconds) const {
    std::ostringstream oss;
    
    int hours = milliseconds / (1000 * 60 * 60);
    milliseconds %= (1000 * 60 * 60);
    int minutes = milliseconds / (1000 * 60);
    milliseconds %= (1000 * 60);
    int seconds = milliseconds / 1000;
    milliseconds %= 1000;
    
    oss << hours << "h " << minutes << "m " << seconds << "s " << milliseconds << "ms";
    return oss.str();
}

std::string Profiler::formatMemory(int64_t bytes) const {
    std::ostringstream oss;
    
    double kb = bytes / 1024.0;
    double mb = kb / 1024.0;
    double gb = mb / 1024.0;
    
    if (gb >= 1.0) {
        oss << std::fixed << std::setprecision(2) << gb << " GB";
    } else if (mb >= 1.0) {
        oss << std::fixed << std::setprecision(2) << mb << " MB";
    } else if (kb >= 1.0) {
        oss << std::fixed << std::setprecision(2) << kb << " KB";
    } else {
        oss << bytes << " B";
    }
    
    return oss.str();
}

std::string Profiler::generateReport() const {
    std::ostringstream oss;
    oss << "Profiling Report: " << sessionName << "\n";
    oss << "=================\n\n";
    
    oss << "Session Information:\n";
    oss << "  Session name: " << sessionName << "\n";
    oss << "  Started: " << formatTime(getElapsedTime()) << "ms\n";
    oss << "  Status: " << (isActive ? "Active" : "Stopped") << "\n\n";
    
    oss << "Performance Statistics:\n";
    oss << "  Total operations: " << operationsCount << "\n";
    oss << "  Operations per second: " << std::fixed << std::setprecision(2) 
        << calculateOperationsPerSecond() << "\n";
    oss << "  Total CPU time: " << formatTime(totalCPUTime) << "ms\n";
    oss << "  Peak memory usage: " << formatMemory(peakMemoryUsage) << "\n";
    
    // Memory usage statistics
    if (!memoryUsages.empty()) {
        double sum = std::accumulate(memoryUsages.begin(), memoryUsages.end(), 0.0);
        double avg = sum / memoryUsages.size();
        double max = *std::max_element(memoryUsages.begin(), memoryUsages.end());
        double min = *std::min_element(memoryUsages.begin(), memoryUsages.end());
        
        oss << "  Memory usage statistics:\n";
        oss << "    Average: " << formatMemory(static_cast<int64_t>(avg)) << "\n";
        oss << "    Minimum: " << formatMemory(min) << "\n";
        oss << "    Maximum: " << formatMemory(max) << "\n";
    }
    
    // Operation timing statistics
    if (!operationTimes.empty()) {
        oss << "\nOperation Timing Statistics:\n";
        for (const auto& pair : operationTimes) {
            oss << "  " << pair.first << ": " << formatTime(pair.second) << "\n";
        }
    }
    
    // Error injection statistics
    if (errorCount > 0) {
        oss << "\nError Injection Statistics:\n";
        oss << "  Errors injected: " << errorCount << "\n";
        oss << "  Duration: " << formatTime(getElapsedTime()) << "ms\n";
    }
    
    // Performance recommendations
    oss << "\nPerformance Recommendations:\n";
    if (calculateOperationsPerSecond() < 1000.0) {
        oss << "  - Low operations per second detected. Consider optimizing code.\n";
    }
    if (peakMemoryUsage > 1024 * 1024 * 1024) { // > 1 GB
        oss << "  - High memory usage detected. Consider memory optimization.\n";
    }
    if (getElapsedTime() > 60000) { // > 1 minute
        oss << "  - Long execution time detected. Consider performance improvements.\n";
    }
    
    oss << "\nReport generated at " << getCurrentTime() << "\n";
    
    return oss.str();
}

void Profiler::startErrorInjectionSimulation() {
    if (errorInjectionActive) {
        NLM_LOG_WARNING("Error injection simulation already active");
        return;
    }
    
    NLM_LOG_INFO("Starting error injection simulation");
    errorInjectionActive = true;
    errorStartTime = std::chrono::steady_clock::now();
}

void Profiler::stopErrorInjectionSimulation() {
    if (!errorInjectionActive) {
        NLM_LOG_WARNING("Error injection simulation not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping error injection simulation");
    errorInjectionActive = false;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - errorStartTime);
    
    NLM_LOG_INFO("Error injection simulation completed:");
    NLM_LOG_INFO("  Duration: " + std::to_string(duration.count()) + "ms");
    NLM_LOG_INFO("  Errors simulated: " + std::to_string(errorCount));
}

const std::string Profiler::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace nlm