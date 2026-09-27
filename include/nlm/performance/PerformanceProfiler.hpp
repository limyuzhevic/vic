#pragma once

/**
 * PerformanceProfiler - Integration with Python profiling tools
 * 
 * Provides integration with Python profiling tools, Python bindings,
 * and compatibility with existing monitoring systems for hybrid C++/Python
 * neural simulation workflows.
 * 
 * Features:
 * - Integration with Python profiling tools (cProfile, snakeviz)
 * - Python bindings for performance monitoring
 * - Compatibility with existing monitoring systems
 * - Cross-language performance correlation
 * - Python-based performance analysis
 * - Integration with visualization tools
 * 
 * Biological Motivation:
 * Neural systems integrate multiple processing pathways. This profiler
 * integrates C++ performance monitoring with Python-based analysis
 * tools for comprehensive performance insights.
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

namespace nlm {

/**
 * Python integration metrics
 */
struct PythonMetrics {
    std::chrono::high_resolution_clock::time_point timestamp;
    double pythonCpuTime{0.0};
    double pythonMemoryUsage{0.0};
    size_t pythonFunctionCalls{0};
    double pythonInferenceTime{0.0};
    std::string currentPythonContext;
    std::unordered_map<std::string, double> pythonMetricData;
    std::vector<std::string> activePythonThreads;
    std::string pythonVersion;
};

/**
 * Cross-language performance correlation
 */
struct CrossLanguageCorrelation {
    std::string componentName;
    double cppPerformance;
    double pythonPerformance;
    double correlationCoefficient;
    std::chrono::high_resolution_clock::duration totalTime;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string bottleneckLocation;
    std::string synchronizationPoint;
};

/**
 * Python profiling session information
 */
struct PythonProfilingSession {
    std::string sessionId;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::string scriptName;
    std::string profilingMethod; // "cProfile", "snakeviz", "memory_profiler"
    double totalCpuTime{0.0};
    double totalMemoryUsage{0.0};
    std::vector<std::string> profiledFunctions;
    std::unordered_map<std::string, double> functionTimes;
    std::unordered_map<std::string, size_t> functionCallCounts;
    std::string sessionOutput;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * PerformanceProfiler - Integration with Python profiling tools
 */
class PerformanceProfiler {
public:
    using PythonIntegrationCallback = std::function<void(const PythonMetrics&)>
;    using CrossLanguageCorrelationCallback = std::function<void(const CrossLanguageCorrelation&)>
;    using ProfilingSessionCallback = std::function<void(const PythonProfilingSession&)>
;    
    /**
     * Create a performance profiler
     * @param enablePythonIntegration Enable Python tool integration
     * @param profilingOutputDir Directory for profiling output
     */
    PerformanceProfiler(
        bool enablePythonIntegration = true,
        const std::string& profilingOutputDir = "python_profiles");
    
    /**
     * Start profiling with Python integration
     * @param scriptName Name of Python script to profile
     */
    void startPythonProfiling(const std::string& scriptName = "nlm_simulation.py",
                             const std::string& profilingMethod = "cProfile");
    
    /**
     * Stop profiling
     */
    void stop();
    
    /**
     * Check if profiler is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Integrate with Python performance monitoring
     * @param pythonMetrics Python metrics to integrate
     */
    void integratePythonMetrics(const PythonMetrics& pythonMetrics);
    
    /**
     * Start profiling Python script
     * @param scriptPath Path to Python script
     * @param profilingArgs Arguments for profiler
     */
    std::string profilePythonScript(const std::string& scriptPath,
                                   const std::vector<std::string>& profilingArgs = {});
    
    /**
     * Get current Python profiling session
     */
    std::optional<PythonProfilingSession> getCurrentProfilingSession() const;
    
    /**
     * Get cross-language correlations
     */
    std::vector<CrossLanguageCorrelation> getCrossLanguageCorrelations() const;
    
    /**
     * Get Python metrics
     */
    std::vector<PythonMetrics> getPythonMetrics() const;
    
    /**
     * Analyze Python performance bottlenecks
     */
    std::vector<std::string> analyzePythonPerformanceBottlenecks() const;
    
    /**
     * Generate Python integration report
     */
    std::string generatePythonIntegrationReport() const;
    
    /**
     * Add Python integration callback
     * @param callback Function to call when Python metrics are integrated
     */
    void addPythonIntegrationCallback(const PythonIntegrationCallback& callback);
    
    /**
     * Add cross-language correlation callback
     * @param callback Function to call when cross-language correlation is detected
     */
    void addCrossLanguageCorrelationCallback(const CrossLanguageCorrelationCallback& callback);
    
    /**
     * Add profiling session callback
     * @param callback Function to call when profiling session is completed
     */
    void addProfilingSessionCallback(const ProfilingSessionCallback& callback);
    
    /**
     * Get profiling statistics
     */
    struct ProfilingStats {
        size_t totalPythonProfiles{0};
        size_t totalCppMetrics{0};
        double averageIntegrationLatency{0.0};
        std::chrono::high_resolution_clock::duration totalProfilingTime{0};
        std::chrono::high_resolution_clock::time_point firstProfileTime;
        std::chrono::high_resolution_clock::time_point lastProfileTime;
    };
    
    ProfilingStats getProfilingStats() const;

private:
    // Thread management
    void profilerThreadFunc();
    
    // Python integration
    void updatePythonMetrics();
    void analyzeCrossLanguagePerformance();
    
    // Profiling session management
    void completeProfilingSession(const PythonProfilingSession& session);
    
    // Performance analysis
    void detectPythonCppCorrelation();
    
    // Internal data structures
    mutable std::mutex pythonMetricsMutex_;
    mutable std::mutex correlationMutex_;
    mutable std::mutex sessionMutex_;
    mutable std::mutex callbackMutex_;
    
    std::atomic<bool> running_{false};
    
    bool enablePythonIntegration_;
    std::string profilingOutputDir_;
    
    // Python integration
    std::vector<PythonMetrics> pythonMetrics_;
    std::vector<CrossLanguageCorrelation> crossLanguageCorrelations_;
    std::optional<PythonProfilingSession> currentSession_;
    
    // Profiling data
    std::vector<PythonProfilingSession> profilingSessions_;
    
    // Statistics
    ProfilingStats stats_;
    
    // Callbacks
    std::vector<PythonIntegrationCallback> pythonIntegrationCallbacks_;
    std::vector<CrossLanguageCorrelationCallback> correlationCallbacks_;
    std::vector<ProfilingSessionCallback> sessionCallbacks_;
    
    // Background thread
    std::thread profilerThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm