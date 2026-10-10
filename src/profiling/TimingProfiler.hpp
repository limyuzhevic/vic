#pragma once

/**
 * High-resolution timing profiler for NLM (Neural Learning Machine)
 * Provides detailed timing analysis for simulation steps and operations
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <atomic>
#include <functional>

namespace nlm {

// Forward declarations
class Profiler;

/**
 * Timing event structure
 */
struct TimingEvent {
    std::string eventName;
    std::string eventType;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double duration; // in seconds
    size_t threadId;
    uint64_t stepId;
    std::string context;
    
    TimingEvent(const std::string& name, const std::string& type, 
                const std::chrono::steady_clock::time_point& start,
                uint64_t step, const std::string& ctx = "")
        : eventName(name), eventType(type), startTime(start), 
          endTime(std::chrono::steady_clock::now()), duration(0.0),
          stepId(step), context(ctx) {
        updateDuration();
    }
    
    void updateDuration() {
        duration = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void setEndTime(const std::chrono::steady_clock::time_point& end) {
        endTime = end;
        updateDuration();
    }
};

/**
 * Function call profile
 */
struct FunctionProfile {
    std::string functionName;
    std::string moduleName;
    size_t lineNumber;
    int callCount;
    double totalDuration;
    double minDuration;
    double maxDuration;
    double avgDuration;
    double stdDevDuration;
    
    FunctionProfile(const std::string& name, const std::string& module, size_t line)
        : functionName(name), moduleName(module), lineNumber(line),
          callCount(0), totalDuration(0.0), minDuration(1e9), maxDuration(0.0) {}
    
    void addSample(double duration) {
        callCount++;
        totalDuration += duration;
        minDuration = std::min(minDuration, duration);
        maxDuration = std::max(maxDuration, duration);
        avgDuration = totalDuration / callCount;
        
        // Update standard deviation using Welford's algorithm
        if (callCount == 1) {
            stdDevDuration = 0.0;
        } else {
            double delta = duration - avgDuration;
            stdDevDuration = std::sqrt(stdDevDuration * stdDevDuration + delta * delta / callCount);
        }
    }
};

/**
 * Timing profiler configuration
 */
struct TimingProfilerConfig {
    // Sampling configuration
    int samplingInterval = 1000; // microseconds
    bool enableStackSampling = false;
    int stackSampleDepth = 10;
    bool enableThreadSampling = true;
    
    // Detailed profiling options
    bool enableFunctionLevelProfiling = true;
    bool enableEventTracing = true;
    bool enableFlameGraphGeneration = true;
    bool enableTimelineVisualization = true;
    
    // Performance thresholds
    double slowOperationThreshold = 0.001; // 1ms
    double verySlowOperationThreshold = 0.01; // 10ms
    
    // Output configuration
    bool exportToJSON = true;
    bool exportToCSV = true;
    bool exportToText = true;
    bool exportFlameGraph = false;
    
    // Integration settings
    bool integrateWithProfiler = true;
    bool integrateWithPhase6 = true;
    
    TimingProfilerConfig() = default;
};

/**
 * High-resolution timing profiler class
 * Manages timing measurements, function profiling, and event tracing
 */
class TimingProfiler {
public:
    TimingProfiler(const TimingProfilerConfig& config = TimingProfilerConfig());
    ~TimingProfiler();
    
    // Core timing operations
    void startEvent(const std::string& eventName, const std::string& eventType = "general",
                   const std::string& context = "", uint64_t stepId = 0);
    void endEvent(const std::string& eventName, uint64_t stepId = 0);
    void pauseEvent(const std::string& eventName);
    void resumeEvent(const std::string& eventName);
    
    // Function profiling
    void startFunctionProfile(const std::string& functionName, 
                             const std::string& moduleName = "",
                             size_t lineNumber = 0,
                             const std::string& context = "");
    void endFunctionProfile(const std::string& functionName);
    
    // Session management
    void startSession(const std::string& sessionName);
    void endSession();
    const std::string& getCurrentSessionName() const { return currentSessionName; }
    
    // Data access
    std::vector<TimingEvent> getEvents(const std::string& eventType = "") const;
    std::vector<FunctionProfile> getFunctionProfiles() const { return functionProfiles; }
    std::map<std::string, std::vector<TimingEvent>> getEventsByType() const;
    
    // Statistics and analysis
    double getTotalTime() const { return totalTime; }
    double getAverageEventTime(const std::string& eventType = "") const;
    double getAverageFunctionTime(const std::string& functionName = "") const;
    size_t getEventCount(const std::string& eventType = "") const;
    size_t getFunctionCallCount(const std::string& functionName = "") const;
    
    // Session statistics
    struct SessionStats {
        double sessionDuration = 0.0;
        size_t eventCount = 0;
        size_t functionCallCount = 0;
        double totalFunctionTime = 0.0;
        double avgFunctionTime = 0.0;
        double maxFunctionTime = 0.0;
        std::map<std::string, size_t> eventTypeCounts;
    };
    
    SessionStats getSessionStats() const { return currentSessionStats; }
    
    // Report generation
    bool generateTimelineReport(const std::string& filename = "") const;
    bool generateFlameGraphReport(const std::string& filename = "") const;
    bool generateFunctionProfileReport(const std::string& filename = "") const;
    bool generateEventTypeReport(const std::string& filename = "") const;
    
    // File operations
    bool exportToJSON(const std::string& filename = "") const;
    bool exportToCSV(const std::string& filename = "") const;
    bool exportToText(const std::string& filename = "") const;
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Integration with profiler
    void setProfiler(Profiler* profiler) { parentProfiler = profiler; }
    
    // Prevent copy construction and assignment
    TimingProfiler(const TimingProfiler&) = delete;
    TimingProfiler& operator=(const TimingProfiler&) = delete;
    
private:
    // Configuration
    TimingProfilerConfig config_;
    
    // Current session
    std::string currentSessionName;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Event management
    std::map<std::string, std::chrono::steady_clock::time_point> activeEvents;
    std::map<std::string, std::chrono::steady_clock::time_point> pausedEvents;
    std::vector<TimingEvent> recordedEvents;
    std::vector<FunctionProfile> functionProfiles;
    
    // Statistics
    double totalTime = 0.0;
    SessionStats currentSessionStats;
    
    // Thread management
    mutable std::mutex mutex_;
    std::map<size_t, std::string> threadNames;
    std::atomic<uint64_t> stepCounter{0};
    
    // Function call stack for detailed profiling
    std::vector<std::string> currentFunctionStack;
    std::map<std::string, std::chrono::steady_clock::time_point> currentFunctionTimes;
    
    // Parent profiler reference
    Profiler* parentProfiler;
    
    // File and directory management
    std::string outputDir;
    
    // Internal helper methods
    void updateStatistics();
    void saveEvent(const TimingEvent& event);
    void startNewStep();
    void endCurrentStep();
    std::string getCurrentFunctionStackString() const;
    bool validateEventName(const std::string& eventName) const;
    void generateFlameGraphData(const std::string& filename) const;
    void generateTimelineData(const std::string& filename) const;
    std::string getDefaultFilename(const std::string& extension) const;
};

} // namespace nlm
