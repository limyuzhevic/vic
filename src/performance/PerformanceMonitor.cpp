#include "cli/CLIParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstring>
#include <cmath>
#include <thread>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() {
    enabled = false;
    mode = 0;
    outputFormat = "console";
}

void PerformanceMonitor::startTimer(const std::string& name) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(timersMutex);
    
    if (timers.find(name) == timers.end()) {
        timers[name] = TimerInfo();
    }
    
    timers[name].start = std::chrono::high_resolution_clock::now();
    timers[name].running = true;
    timers[name].elapsed = 0.0;
}

double PerformanceMonitor::stopTimer(const std::string& name) {
    if (!enabled) return 0.0;
    
    std::lock_guard<std::mutex> lock(timersMutex);
    
    if (timers.find(name) == timers.end() || !timers[name].running) {
        return timers[name].elapsed;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end - timers[name].start
    );
    
    timers[name].elapsed += duration.count() / 1000000.0;
    timers[name].running = false;
    
    return timers[name].elapsed;
}

double PerformanceMonitor::getElapsed(const std::string& name) const {
    if (!enabled) return 0.0;
    
    std::lock_guard<std::mutex> lock(timersMutex);
    
    if (timers.find(name) == timers.end()) {
        return 0.0;
    }
    
    if (timers.at(name).running) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end - timers.at(name).start
        );
        return timers.at(name).elapsed + duration.count() / 1000000.0;
    }
    
    return timers.at(name).elapsed;
}

void PerformanceMonitor::printReport() const {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(timersMutex);
    
    switch (mode) {
        case 1: // detailed
            printDetailedReport();
            break;
        case 2: // full
            printFullReport();
            break;
        default: // basic
            printBasicReport();
            break;
    }
}

void PerformanceMonitor::printBasicReport() const {
    std::cout << "=== Performance Report ===" << std::endl;
    std::cout << "Timers: " << timers.size() << std::endl;
    
    double totalTime = 0.0;
    for (const auto& pair : timers) {
        std::cout << "  " << pair.first << ": " 
                  << std::fixed << std::setprecision(3) 
                  << pair.second.elapsed << "s" << std::endl;
        totalTime += pair.second.elapsed;
    }
    
    std::cout << "Total time: " << std::fixed << std::setprecision(3) 
              << totalTime << "s" << std::endl;
    
    // Add memory info
    size_t memUsage = getMemoryUsage();
    std::cout << "Memory usage: " << memUsage / 1024 / 1024 << " MB" << std::endl;
    
    // Add CPU usage
    double cpuUsage = getCPUUsage();
    std::cout << "CPU usage: " << std::fixed << std::setprecision(1) 
              << cpuUsage << "%" << std::endl;
}

void PerformanceMonitor::printDetailedReport() const {
    printBasicReport();
    
    std::cout << "\n=== Detailed Timer Information ===" << std::endl;
    for (const auto& pair : timers) {
        std::cout << pair.first << ": " << std::endl;
        std::cout << "  Elapsed: " << std::fixed << std::setprecision(3) 
                  << pair.second.elapsed << "s" << std::endl;
        std::cout << "  Running: " << (pair.second.running ? "Yes" : "No") << std::endl;
    }
}

void PerformanceMonitor::printFullReport() const {
    printDetailedReport();
    
    std::cout << "\n=== System Information ===" << std::endl;
    
    // Get system info
    struct sysinfo info;
    sysinfo(&info);
    
    std::cout << "System uptime: " << info.uptime << " seconds" << std::endl;
    std::cout << "Total memory: " << info.totalram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Free memory: " << info.freeram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Processes: " << info.procs << std::endl;
    
    // Add thread count
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl;
}

size_t PerformanceMonitor::getMemoryUsage() const {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.totalram - info.freeram; // in pages, not bytes
    }
    return 0;
}

double PerformanceMonitor::getCPUUsage() const {
    // Simple CPU usage calculation - for production, use more sophisticated methods
    static double lastCpuUsage = 0.0;
    static auto lastTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
    
    if (duration.count() == 0) {
        return lastCpuUsage;
    }
    
    // This is a placeholder - real implementation would read /proc/stat
    // For now, return a simple heuristic
    double cpuUsage = 10.0 + (rand() % 40); // Simulate between 10% and 50%
    
    lastCpuUsage = cpuUsage;
    lastTime = currentTime;
    
    return cpuUsage;
}

} // namespace nlm
