#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>

namespace nlm {

class MemoryMonitor {
public:
    struct MemoryStats {
        std::string operation;
        size_t peakMemoryBytes;
        size_t averageMemoryBytes;
        size_t minMemoryBytes;
        size_t maxMemoryBytes;
        double peakMemoryMB;
        double averageMemoryMB;
        double minMemoryMB;
        double maxMemoryMB;
        size_t measurementCount;
        std::chrono::duration<double> elapsedTime;
        
        MemoryStats() : peakMemoryBytes(0), averageMemoryBytes(0), minMemoryBytes(0),
                       maxMemoryBytes(0), measurementCount(0), elapsedTime(0) {}
    };
    
    MemoryMonitor() = default;
    
    // Start memory measurement
    void startMemoryMeasurement(const std::string& operation) {
        currentMeasurement.operation = operation;
        currentMeasurement.peakMemoryBytes = 0;
        currentMeasurement.minMemoryBytes = SIZE_MAX;
        currentMeasurement.maxMemoryBytes = 0;
        currentMeasurement.measurementCount = 0;
        currentMeasurement.elapsedTime = std::chrono::duration<double>(0);
        measurementStart = std::chrono::high_resolution_clock::now();
    }
    
    // Record memory usage
    void recordMemoryUsage(size_t memoryBytes) {
        currentMeasurement.peakMemoryBytes = std::max(currentMeasurement.peakMemoryBytes, memoryBytes);
        currentMeasurement.minMemoryBytes = std::min(currentMeasurement.minMemoryBytes, memoryBytes);
        currentMeasurement.maxMemoryBytes = std::max(currentMeasurement.maxMemoryBytes, memoryBytes);
        currentMeasurement.measurementCount++;
    }
    
    // Stop memory measurement
    void stopMemoryMeasurement() {
        auto end = std::chrono::high_resolution_clock::now();
        currentMeasurement.elapsedTime = end - measurementStart;
        
        if (currentMeasurement.measurementCount > 0) {
            currentMeasurement.averageMemoryBytes = currentMeasurement.peakMemoryBytes / currentMeasurement.measurementCount;
            
            // Convert to MB
            currentMeasurement.peakMemoryMB = currentMeasurement.peakMemoryBytes / (1024.0 * 1024.0);
            currentMeasurement.averageMemoryMB = currentMeasurement.averageMemoryBytes / (1024.0 * 1024.0);
            currentMeasurement.minMemoryMB = currentMeasurement.minMemoryBytes / (1024.0 * 1024.0);
            currentMeasurement.maxMemoryMB = currentMeasurement.maxMemoryBytes / (1024.0 * 1024.0);
            
            memoryStats.push_back(currentMeasurement);
        }
    }
    
    // Get statistics for an operation
    MemoryStats getStats(const std::string& operation) const {
        for (const auto& stats : memoryStats) {
            if (stats.operation == operation) {
                return stats;
            }
        }
        return MemoryStats{};
    }
    
    // Get all memory statistics
    const std::vector<MemoryStats>& getAllStats() const {
        return memoryStats;
    }
    
    // Generate summary report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Memory Monitoring Report ===" << std::endl;
        oss << "Total measurements: " << memoryStats.size() << std::endl;
        oss << std::endl;
        
        for (const auto& stats : memoryStats) {
            oss << stats.operation << ":" << std::endl;
            oss << "  Measurements: " << stats.measurementCount << std::endl;
            oss << "  Peak memory: " << stats.peakMemoryMB << " MB" << std::endl;
            oss << "  Average memory: " << stats.averageMemoryMB << " MB" << std::endl;
            oss << "  Min memory: " << stats.minMemoryMB << " MB" << std::endl;
            oss << "  Max memory: " << stats.maxMemoryMB << " MB" << std::endl;
            oss << "  Duration: " << stats.elapsedTime.count() << "s" << std::endl;
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all statistics
    void clear() {
        memoryStats.clear();
    }
    
    // Estimate current memory usage (platform-specific implementation)
    static size_t getCurrentMemoryUsage() {
#ifdef _WIN32
        // Windows implementation
        return 0; // Placeholder - would need Windows API
#elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS implementation using /proc/self/statm or psutil
        return 0; // Placeholder - would need /proc parsing
#else
        return 0; // Generic placeholder
#endif
        return 0; // Fallback
    }
    
private:
    MemoryStats currentMeasurement;
    std::chrono::high_resolution_clock::time_point measurementStart;
    std::vector<MemoryStats> memoryStats;
};

} // namespace nlm