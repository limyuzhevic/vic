#include "DebugInterface.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace nlm {

DebugInterface* DebugManager::instance_ = nullptr;

// ConsoleDebug implementation details
void ConsoleDebug::log(const DebugMessage& message) {
    if (message.level < currentLevel) return;
    
    std::string levelStr;
    switch (message.level) {
        case DebugLevel::Debug: levelStr = "DEBUG"; break;
        case DebugLevel::Info: levelStr = "INFO"; break;
        case DebugLevel::Warning: levelStr = "WARN"; break;
        case DebugLevel::Error: levelStr = "ERROR"; break;
        case DebugLevel::Critical: levelStr = "CRIT"; break;
    }
    
    std::cout << "[" << levelStr << "] " << message.source << ": " 
              << message.message << std::endl;
    
    if (message.level >= DebugLevel::Error) {
        std::cerr << "[ERROR CONTEXT] File: " << message.file 
                  << ", Line: " << message.line << std::endl;
    }
}

void ConsoleDebug::memoryAllocated(const MemoryAllocation& alloc) {
    std::cout << "[MEMORY] Allocated " << alloc.size << " bytes at " 
              << alloc.file << ":" << alloc.line << std::endl;
}

void ConsoleDebug::memoryDeallocated(size_t size) {
    std::cout << "[MEMORY] Deallocated " << size << " bytes" << std::endl;
}

void ConsoleDebug::performanceMeasured(const PerformanceMetric& metric) {
    std::cout << "[PERF] " << metric.name << ": " 
              << metric.averageTime * 1000 << " ms avg (" 
              << metric.calls << " calls)" << std::endl;
}

void ConsoleDebug::startProfiling(const std::string& name) {
    profilingSessions[name] = std::chrono::system_clock::now();
}

void ConsoleDebug::stopProfiling(const std::string& name) {
    auto it = profilingSessions.find(name);
    if (it != profilingSessions.end()) {
        auto duration = std::chrono::system_clock::now() - it->second;
        double seconds = std::chrono::duration<double>(duration).count();
        
        auto& metric = performanceMetrics[name];
        metric.name = name;
        metric.duration = duration;
        metric.calls++;
        metric.minTime = (metric.calls == 1) ? seconds : std::min(metric.minTime, seconds);
        metric.maxTime = (metric.calls == 1) ? seconds : std::max(metric.maxTime, seconds);
        metric.averageTime = (metric.minTime + metric.maxTime) / 2.0; // Simplified
        
        profilingSessions.erase(it);
    }
}

std::vector<PerformanceMetric> ConsoleDebug::getPerformanceMetrics() const {
    std::vector<PerformanceMetric> metrics;
    for (const auto& pair : performanceMetrics) {
        metrics.push_back(pair.second);
    }
    return metrics;
}

void ConsoleDebug::saveDebugInfo(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Debug Information" << std::endl;
        file << "=================" << std::endl;
        file << "Debug Level: " << static_cast<int>(currentLevel) << std::endl;
        file << "Messages: " << messages.size() << std::endl;
        file << "Memory Allocations: " << memoryAllocations.size() << std::endl;
        file << "Performance Metrics: " << performanceMetrics.size() << std::endl;
        file.close();
    }
}

void ConsoleDebug::loadDebugInfo(const std::string& filename) {
    // In a real implementation, this would load debug information from file
    // For now, it's a placeholder
}

} // namespace nlm