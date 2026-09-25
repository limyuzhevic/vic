#pragma once

/**
 * Performance Profiler for NLM Phase 6 Demo
 * 
 * Comprehensive performance profiling system for tracking execution time,
 * CPU/memory usage, and simulation metrics across all brain systems.
 * 
 * Features:
 * - High-resolution timing with microsecond precision
 * - Memory usage tracking
 * - System-wide profiling integration
 * - Multiple output formats (text, JSON, CSV)
 * - Command-line control options
 * - Automatic profiling in Phase6Demo
 */

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <map>
#include <cmath>

namespace nlm {

// Forward declarations
class PerformanceProfiler;

/**
 * Profiling metrics structure for recording performance data
 */
struct ProfilingMetrics {
    // Timing metrics (in microseconds)
    double totalTimeUs;
    double cpuTimeUs;
    double memoryTimeUs;
    double simulationTimeUs;
    double visualizationTimeUs;
    double exportTimeUs;
    
    // Memory metrics (in bytes)
    size_t peakMemoryUsage;
    size_t currentMemoryUsage;
    size_t totalAllocatedMemory;
    
    // Performance metrics
    double simulationSpeed; // steps per second
    double fps; // frames per second
    double memoryPerStep; // bytes per step
    double cpuUsagePercent;
    
    // Event counts
    uint64_t totalSteps;
    uint64_t spikeEvents;
    uint64_t plasticityEvents;
    uint64_t developmentEvents;
    uint64_t predictionEvents;
    uint64_t memoryEvents;
    
    // System-specific timing
    std::unordered_map<std::string, double> systemTimingsUs;
    
    ProfilingMetrics()
        : totalTimeUs(0.0)
        , cpuTimeUs(0.0)
        , memoryTimeUs(0.0)
        , simulationTimeUs(0.0)
        , visualizationTimeUs(0.0)
        , exportTimeUs(0.0)
        , peakMemoryUsage(0)
        , currentMemoryUsage(0)
        , totalAllocatedMemory(0)
        , simulationSpeed(0.0)
        , fps(0.0)
        , memoryPerStep(0.0)
        , cpuUsagePercent(0.0)
        , totalSteps(0)
        , spikeEvents(0)
        , plasticityEvents(0)
        , developmentEvents(0)
        , predictionEvents(0)
        , memoryEvents(0) {}
};

/**
 * Individual profiling session for specific components
 */
class ProfilingSession {
public:
    ProfilingSession(const std::string& name, bool enabled = true)
        : name_(name), enabled_(enabled), startTime_(), totalTime_(0.0) {
        if (enabled_) {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    ~ProfilingSession() {
        if (enabled_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::micro>(endTime - startTime_);
            totalTime_ = duration.count();
        }
    }
    
    void stop() {
        if (enabled_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::micro>(endTime - startTime_);
            totalTime_ = duration.count();
        }
    }
    
    double getTotalTime() const { return totalTime_; }
    bool isEnabled() const { return enabled_; }
    const std::string& getName() const { return name_; }
    
private:
    std::string name_;
    bool enabled_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double totalTime_;
};

/**
 * High-resolution performance profiler with system-wide monitoring
 */
class PerformanceProfiler {
public:
    PerformanceProfiler(bool enabled = false, const std::string& outputDir = ".", 
                       const std::string& format = "text", bool consoleOutput = true);
    ~PerformanceProfiler();
    
    // Core profiling operations
    void start();
    void stop();
    void reset();
    
    // Session management
    ProfilingSession* startSession(const std::string& name, bool enabled = true);
    void endSession(const std::string& name);
    
    // Metric recording
    void recordMetric(const std::string& name, double value);
    void recordMemoryUsage(size_t memoryBytes);
    void recordStepMetrics(uint64_t step, double timeUs);
    void recordSystemEvent(const std::string& eventType, double timeUs);
    
    // System monitoring
    void updateSystemMetrics();
    double getCurrentCPULoad() const;
    size_t getCurrentMemoryUsage() const;
    
    // Data access
    const ProfilingMetrics& getMetrics() const { return metrics_; }
    double getTotalTime() const { return metrics_.totalTimeUs; }
    bool isRunning() const { return running_; }
    
    // Output operations
    void exportToText(const std::string& filename) const;
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    void printSummary() const;
    
    // Brain system specific profiling
    void profileBrainSystems(const class Brain& brain, uint64_t step);
    void profileNeuromodulation(const class Neuromodulator& neuromodulator, double timeUs);
    void profileMemorySystems(const class NeuralWorkingMemory& workingMem, 
                             const class NeuralEpisodicMemory& episodicMem,
                             double timeUs);
    void profilePrediction(const class PredictionSystem& prediction, double timeUs);
    void profileCognition(const class NeuralPlanner& planner, double timeUs);
    void profileDevelopment(const class DevelopmentSystem& development, double timeUs);
    
    // Configuration
    void setEnabled(bool enabled) { enabled_ = enabled; }
    void setOutputDirectory(const std::string& dir) { outputDir_ = dir; }
    void setOutputFormat(const std::string& format) { outputFormat_ = format; }
    void setConsoleOutput(bool output) { consoleOutput_ = output; }
    void setSamplingInterval(double intervalUs) { samplingIntervalUs_ = intervalUs; }
    
    // Command-line options
    static std::string getCommandLineHelp();
    static bool parseCommandLine(int argc, char* argv[], PerformanceProfiler& profiler);
    
private:
    bool enabled_;
    bool running_;
    std::string outputDir_;
    std::string outputFormat_;
    bool consoleOutput_;
    double samplingIntervalUs_;
    
    ProfilingMetrics metrics_;
    std::unordered_map<std::string, ProfilingSession*> activeSessions_;
    std::unordered_map<std::string, std::vector<double>> recordedMetrics_;
    
    mutable std::mutex mutex_;
    
    // System monitoring thread
    std::thread monitoringThread_;
    bool monitoringActive_;
    
    // Internal methods
    void monitoringLoop();
    void updateCPUMetrics();
    void updateMemoryMetrics();
    void calculateStatistics();
    void ensureOutputDirectory() const;
    double getCurrentTimestamp() const;
    std::string getTimestampString() const;
};

/**
 * Performance profiling utilities for statistical analysis
 */
class PerformanceUtils {
public:
    // Statistical calculations
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateMin(const std::vector<double>& values);
    static double calculateMax(const std::vector<double>& values);
    static double calculateMedian(const std::vector<double>& values);
    
    // Performance analysis
    static double calculateFPS(double timePerStepUs, uint64_t steps);
    static double calculateMemoryPerStep(size_t totalMemory, uint64_t steps);
    static double calculateCPUUsage(double cpuTimeUs, double totalTimeUs);
    static double calculateSimulationSpeed(double totalSteps, double totalTimeUs);
    
    // String formatting
    static std::string formatTime(double timeUs);
    static std::string formatBytes(size_t bytes);
    static std::string formatPercentage(double value);
    
    // System information
    static std::string getSystemInfo();
    static std::string getPerformanceSummary(const ProfilingMetrics& metrics);
};

/**
 * Integration utilities for Phase6Demo
 */
class Phase6ProfilingIntegration {
public:
    // Integration functions for Phase6Demo.cpp
    static void setupProfiling(PerformanceProfiler& profiler, int argc, char* argv[]);
    static void startProfilingCycle(PerformanceProfiler& profiler);
    static void recordStepMetrics(PerformanceProfiler& profiler, uint64_t step, 
                                 const class Brain& brain, double stepTimeUs);
    static void endProfilingCycle(PerformanceProfiler& profiler);
    static void exportProfilingResults(PerformanceProfiler& profiler, 
                                      const std::string& baseFilename);
};

} // namespace nlm
