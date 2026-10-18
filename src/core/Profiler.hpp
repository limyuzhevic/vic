#include "Logger.hpp"
#include <chrono>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace nlm {

// Timer for measuring execution time
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()),
              totalDuration(std::chrono::microseconds(0)),
              callCount(0) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        totalDuration += duration;
        callCount++;
    }
    
    double getTotalSeconds() const {
        return totalDuration.count() / 1000000.0;
    }
    
    double getAverageSeconds() const {
        return callCount > 0 ? getTotalSeconds() / callCount : 0.0;
    }
    
    size_t getCallCount() const { return callCount; }
    
    std::string getFormattedTotal() const {
        return formatDuration(getTotalSeconds());
    }
    
    std::string getFormattedAverage() const {
        return formatDuration(getAverageSeconds());
    }
    
private:
    std::string formatDuration(double seconds) {
        if (seconds >= 3600.0) {
            return std::to_string(seconds / 3600.0) + "h";
        } else if (seconds >= 60.0) {
            return std::to_string(seconds / 60.0) + "m";
        } else if (seconds >= 1.0) {
            return std::to_string(seconds) + "s";
        } else if (seconds >= 0.001) {
            return std::to_string(seconds * 1000.0) + "ms";
        } else {
            return std::to_string(seconds * 1000000.0) + "us";
        }
    }
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::microseconds totalDuration;
    size_t callCount;
};

// Performance profiler for tracking function execution
class PerformanceProfiler {
public:
    PerformanceProfiler() : enabled(true) {}
    
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
    void startFunction(const std::string& name) {
        if (!enabled) return;
        timers[name].start();
    }
    
    void stopFunction(const std::string& name) {
        if (!enabled) return;
        timers[name].stop();
    }
    
    const Timer& getTimer(const std::string& name) const {
        auto it = timers.find(name);
        if (it != timers.end()) {
            return it->second;
        }
        return dummyTimer;
    }
    
    std::vector<std::string> getFunctionNames() const {
        std::vector<std::string> names;
        for (const auto& pair : timers) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::string getProfileReport() const {
        std::ostringstream oss;
        oss << "Performance Profile" << std::endl;
        oss << "=" << std::string(50, '=') << std::endl;
        
        for (const auto& pair : timers) {
            oss << pair.first << ": ";
            oss << "Total: " << pair.second.getFormattedTotal() << " ";
            oss << "Avg: " << pair.second.getFormattedAverage() << " ";
            oss << "Calls: " << pair.second.getCallCount() << std::endl;
        }
        
        return oss.str();
    }
    
    void reset() {
        timers.clear();
    }
    
private:
    bool enabled;
    std::unordered_map<std::string, Timer> timers;
    Timer dummyTimer;
};

// Main profiling and debugging class
class Profiler {
public:
    Profiler() : enabled(true), memoryEnabled(true), performanceEnabled(true), leakDetectionEnabled(true) {}
    
    void setEnabled(bool enabled) { 
        this->enabled = enabled; 
        if (!enabled) {
            performanceProfiler.reset();
        }
    }
    
    void setMemoryEnabled(bool enabled) { memoryEnabled = enabled; }
    void setPerformanceEnabled(bool enabled) { performanceEnabled = enabled; }
    void setLeakDetectionEnabled(bool enabled) { leakDetectionEnabled = enabled; }
    
    void startTimer(const std::string& name) {
        if (performanceEnabled) {
            performanceProfiler.startFunction(name);
        }
    }
    
    void stopTimer(const std::string& name) {
        if (performanceEnabled) {
            performanceProfiler.stopFunction(name);
        }
    }
    
    std::string getProfileReport() const {
        std::ostringstream oss;
        oss << "=== System Profiling Report ===" << std::endl;
        
        if (performanceEnabled) {
            oss << performanceProfiler.getProfileReport() << std::endl;
        }
        
        return oss.str();
    }
    
    void reset() {
        performanceProfiler.reset();
    }
    
private:
    bool enabled;
    bool memoryEnabled;
    bool performanceEnabled;
    bool leakDetectionEnabled;
    PerformanceProfiler performanceProfiler;
};

// Global profiling instance
Profiler& getProfiler() {
    static Profiler profiler;
    return profiler;
}

// Macros for easy profiling
#define PROFILE_FUNCTION(name) nlm::ProfilerScope name##_scope(#name); nlm::getProfiler().startTimer(#name)
#define PROFILE_FUNCTION_STOP(name) nlm::getProfiler().stopTimer(#name)

// Profiler scope for RAII-style profiling
class ProfilerScope {
public:
    ProfilerScope(const std::string& name) : functionName(name) {
        getProfiler().startTimer(functionName);
    }
    
    ~ProfilerScope() {
        getProfiler().stopTimer(functionName);
    }
    
private:
    std::string functionName;
};

} // namespace nlm