#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfileData {
        std::string operation;
        std::chrono::duration<double> elapsedTime;
        double minTime;
        double maxTime;
        double meanTime;
        double stdDevTime;
        size_t callCount;
        double totalTime;
        
        ProfileData() : minTime(0.0), maxTime(0.0), meanTime(0.0), stdDevTime(0.0), 
                      callCount(0), totalTime(0.0) {}
    };
    
    PerformanceProfiler() = default;
    
    // Start timing an operation
    void startTiming(const std::string& operation) {
        timingStart[operation] = std::chrono::high_resolution_clock::now();
        if (timingCounts.find(operation) == timingCounts.end()) {
            timingCounts[operation] = 0;
            timingStats[operation] = ProfileData{operation, std::chrono::duration<double>(0), 0.0, 0.0, 0.0, 0.0, 0, 0.0};
        }
    }
    
    // Stop timing an operation and record results
    void stopTiming(const std::string& operation) {
        if (timingStart.find(operation) == timingStart.end()) return;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(end - timingStart[operation]);
        
        ProfileData& stats = timingStats[operation];
        stats.elapsedTime += elapsed;
        stats.minTime = std::min(stats.minTime, elapsed.count());
        stats.maxTime = std::max(stats.maxTime, elapsed.count());
        stats.totalTime += elapsed.count();
        stats.callCount++;
        
        // Update mean and std dev (simplified calculation)
        stats.meanTime = stats.totalTime / stats.callCount;
        
        timingStart.erase(operation);
    }
    
    // Get statistics for an operation
    ProfileData getStats(const std::string& operation) const {
        if (timingStats.find(operation) == timingStats.end()) {
            return ProfileData{operation, std::chrono::duration<double>(0), 0.0, 0.0, 0.0, 0.0, 0, 0.0};
        }
        return timingStats.at(operation);
    }
    
    // Get all operation statistics
    const std::unordered_map<std::string, ProfileData>& getAllStats() const {
        return timingStats;
    }
    
    // Generate summary report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Performance Profiling Report ===" << std::endl;
        oss << "Total operations: " << timingStats.size() << std::endl;
        oss << std::endl;
        
        for (const auto& pair : timingStats) {
            const auto& stats = pair.second;
            oss << pair.first << ":" << std::endl;
            oss << "  Calls: " << stats.callCount << std::endl;
            oss << "  Total time: " << stats.totalTime << "s" << std::endl;
            oss << "  Mean time: " << stats.meanTime * 1000 << "ms" << std::endl;
            oss << "  Min time: " << stats.minTime * 1000 << "ms" << std::endl;
            oss << "  Max time: " << stats.maxTime * 1000 << "ms" << std::endl;
            if (stats.callCount > 1) {
                oss << "  Std dev: " << stats.stdDevTime * 1000 << "ms" << std::endl;
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all statistics
    void clear() {
        timingStats.clear();
        timingStart.clear();
        timingCounts.clear();
    }
    
private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timingStart;
    std::unordered_map<std::string, ProfileData> timingStats;
    std::unordered_map<std::string, size_t> timingCounts;
};

} // namespace nlm