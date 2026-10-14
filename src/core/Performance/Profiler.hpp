#pragma once

#include <chrono>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <iostream>

namespace nlm {

// Performance measurement precision
using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;

// Performance measurement levels
enum class PerformanceLevel {
    Off,      // No measurement
    Basic,    // Per-operation timing
    Detailed, // Per-frame with call stack
    Profile   // Full profiling with aggregation
};

// Performance measurement result
struct PerformanceResult {
    std::string operationName;
    Duration duration;
    size_t callCount;
    double averageDuration;
    Duration minDuration;
    Duration maxDuration;
    
    PerformanceResult(const std::string& name) 
        : operationName(name), duration(0), callCount(0), 
          averageDuration(0), minDuration(std::chrono::duration<double>::max()), 
          maxDuration(0) {}
    
    void update(Duration dur) {
        duration += dur;
        ++callCount;
        averageDuration = duration.count() / callCount;
        if (dur < minDuration) minDuration = dur;
        if (dur > maxDuration) maxDuration = dur;
    }
};

// Performance profiler interface
class PerformanceProfiler {
public:
    virtual ~PerformanceProfiler() = default;
    
    // Start profiling an operation
    virtual void startProfile(const std::string& operationName) = 0;
    
    // End profiling and record result
    virtual void endProfile(const std::string& operationName) = 0;
    
    // Get all profiling results
    virtual std::vector<PerformanceResult> getResults() const = 0;
    
    // Clear all results
    virtual void clear() = 0;
    
    // Set performance level
    virtual void setLevel(PerformanceLevel level) = 0;
    
    // Save results to file
    virtual void saveToFile(const std::string& filename) const = 0;
    
    // Get global profiler instance
    static PerformanceProfiler& getInstance();
};

// Basic performance profiler implementation
class BasicPerformanceProfiler : public PerformanceProfiler {
public:
    BasicPerformanceProfiler() : level(PerformanceLevel::Off) {}
    
    void startProfile(const std::string& operationName) override {
        if (level == PerformanceLevel::Off) return;
        
        currentProfile[operationName] = std::chrono::high_resolution_clock::now();
    }
    
    void endProfile(const std::string& operationName) override {
        if (level == PerformanceLevel::Off) return;
        
        auto it = currentProfile.find(operationName);
        if (it == currentProfile.end()) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - it->second);
        
        // Update results
        results[operationName].update(duration);
        
        // Clean up
        currentProfile.erase(it);
    }
    
    std::vector<PerformanceResult> getResults() const override {
        std::vector<PerformanceResult> resultVec;
        for (const auto& pair : results) {
            resultVec.push_back(pair.second);
        }
        return resultVec;
    }
    
    void clear() override {
        results.clear();
        currentProfile.clear();
    }
    
    void setLevel(PerformanceLevel level) override {
        this->level = level;
    }
    
    void saveToFile(const std::string& filename) const override {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "Performance Profiling Results" << std::endl;
        file << "=================================" << std::endl;
        
        for (const auto& pair : results) {
            const auto& result = pair.second;
            file << "Operation: " << result.operationName << std::endl;
            file << "  Duration: " << result.duration.count() << " seconds" << std::endl;
            file << "  Call Count: " << result.callCount << std::endl;
            file << "  Average Duration: " << result.averageDuration << " seconds" << std::endl;
            file << "  Min Duration: " << result.minDuration.count() << " seconds" << std::endl;
            file << "  Max Duration: " << result.maxDuration.count() << " seconds" << std::endl;
            file << std::endl;
        }
    }
    
private:
    PerformanceLevel level;
    std::unordered_map<std::string, TimePoint> currentProfile;
    std::unordered_map<std::string, PerformanceResult> results;
};

// Macro for easy profiling
#define PROFILE_SCOPE(operationName) \
    nlm::ProfileScope profileScope_##__LINE__(operationName)

class ProfileScope {
public:
    ProfileScope(const std::string& operationName) : operationName(operationName) {
        PerformanceProfiler::getInstance().startProfile(operationName);
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~ProfileScope() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        PerformanceProfiler::getInstance().endProfile(operationName);
    }
    
private:
    std::string operationName;
    TimePoint startTime;
};

// Timer utility class
class Timer {
public:
    Timer(const std::string& operationName) : operationName(operationName) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        PerformanceProfiler::getInstance().endProfile(operationName);
    }
    
    Duration elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime);
    }
    
private:
    std::string operationName;
    TimePoint startTime;
};

// Profiling utilities
namespace profiling {
    // Start profiling a specific operation
    inline void start(const std::string& operationName) {
        PerformanceProfiler::getInstance().startProfile(operationName);
    }
    
    // End profiling and record result
    inline void end(const std::string& operationName) {
        PerformanceProfiler::getInstance().endProfile(operationName);
    }
    
    // Get profiling results
    inline std::vector<PerformanceResult> getResults() {
        return PerformanceProfiler::getInstance().getResults();
    }
    
    // Set profiling level
    inline void setLevel(PerformanceLevel level) {
        PerformanceProfiler::getInstance().setLevel(level);
    }
    
    // Save results to file
    inline void saveToFile(const std::string& filename) {
        PerformanceProfiler::getInstance().saveToFile(filename);
    }
    
    // Create a scoped timer
    inline Timer makeTimer(const std::string& operationName) {
        return Timer(operationName);
    }
}

} // namespace nlm
