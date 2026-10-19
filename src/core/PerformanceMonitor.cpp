// Performance monitor implementation for NLM
// This file implements the performance monitoring and profiling functionality

#include "PerformanceMonitor.hpp"
#include <sysinfoapi.h>
#include <psapi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Windows-specific performance data collection
#ifdef _WIN32

// Get current CPU usage (simplified implementation)
double PerformanceMonitor::collectCPUUsage() {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        static ULONGLONG lastTotalKernel = 0;
        static ULONGLONG lastTotalUser = 0;
        static ULONGLONG lastIdle = 0;
        
        ULONGLONG idle = combineFileTime(&idleTime);
        ULONGLONG kernel = combineFileTime(&kernelTime);
        ULONGLONG user = combineFileTime(&userTime);
        
        ULONGLONG total = kernel + user;
        ULONGLONG totalIdle = idle;
        
        if (lastTotalKernel != 0) {
            double idleTimePercent = static_cast<double>(totalIdle - lastIdle) / (total - lastTotalKernel);
            return (1.0 - idleTimePercent) * 100.0;
        }
        
        lastTotalKernel = total;
        lastTotalUser = user;
        lastIdle = totalIdle;
    }
    
    return 0.0;
}

ULONGLONG PerformanceMonitor::combineFileTime(const FILETIME* ft) {
    return (static_cast<ULONGLONG>(ft->dwHighDateTime) << 32) + ft->dwLowDateTime;
}

#else

// Non-Windows implementation

double PerformanceMonitor::collectCPUUsage() {
    // Simple implementation for non-Windows systems
    // Read from /proc/stat on Linux, or use platform-specific APIs
    std::ifstream cpuInfo("/proc/stat");
    if (!cpuInfo.is_open()) {
        return 0.0;
    }
    
    std::string line;
    std::getline(cpuInfo, line);
    cpuInfo.close();
    
    if (line.find("cpu ") != 0) {
        return 0.0;
    }
    
    std::istringstream iss(line.substr(5)); // Skip "cpu "
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    
    static unsigned long long lastTotalIdle = 0;
    static unsigned long long lastTotal = 0;
    
    unsigned long long totalIdle = idle + iowait;
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    
    if (lastTotal == 0) {
        lastTotalIdle = totalIdle;
        lastTotal = total;
        return 0.0;
    }
    
    double idlePercent = static_cast<double>(totalIdle - lastTotalIdle) / (total - lastTotal);
    lastTotalIdle = totalIdle;
    lastTotal = total;
    
    return (1.0 - idlePercent) * 100.0;
}

#endif

// Get current memory usage in MB
double PerformanceMonitor::collectMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0); // MB
    }
#else
    // Linux/macOS implementation
    std::ifstream memInfo("/proc/self/status");
    if (!memInfo.is_open()) {
        return 0.0;
    }
    
    std::string line;
    while (std::getline(memInfo, line)) {
        if (line.find("VmRSS:") == 0) {
            std::istringstream iss(line.substr(6)); // Skip "VmRSS:"
            unsigned long long kb;
            iss >> kb;
            memInfo.close();
            return static_cast<double>(kb) / 1024.0; // MB
        }
    }
    memInfo.close();
#endif
    
    return 0.0;
}

// Collect simulation speed from config
double PerformanceMonitor::collectSimulationSpeed() {
    if (!config_) return 0.0;
    
    // Get simulation speed from config or calculate based on timing
    double speed = config_->getOr<double>("simulation_speed", 0.0);
    if (speed > 0.0) return speed;
    
    // Estimate from step timing
    auto currentTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSampleTime_);
    
    if (duration.count() > 0) {
        return config_->getOr<size_t>("simulation_steps", 0) / duration.count();
    }
    
    return 0.0;
}

// Collect firing rate from brain system (simplified)
double PerformanceMonitor::collectFiringRate() {
    // In a real implementation, this would query the brain system
    // For now, return a simulated value
    return 12.5 + ((std::rand() % 20) / 10.0); // 12.5-22.5 Hz
}

// Collect synapse activity from brain system (simplified)
double PerformanceMonitor::collectSynapseActivity() {
    // In a real implementation, this would query the brain system
    // For now, return a simulated value
    return 150.0 + (std::rand() % 100); // 150-250 connections per neuron
}

// Collect learning efficiency from brain system (simplified)
double PerformanceMonitor::collectLearningEfficiency() {
    // In a real implementation, this would query the brain system
    // For now, return a simulated value
    return 0.7 + ((std::rand() % 100) / 100.0 * 0.3); // 0.7-1.0
}

// Collect network stability from brain system (simplified)
double PerformanceMonitor::collectNetworkStability() {
    // In a real implementation, this would query the brain system
    // For now, return a simulated value
    return 0.8 + ((std::rand() % 100) / 100.0 * 0.2); // 0.8-1.0
}

} // namespace nlm
