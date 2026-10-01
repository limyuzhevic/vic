#pragma once

#include "Performance.hpp"
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <mutex>

namespace nlm {

/**
 * Performance Profiler for NLM
 * 
 * This profiler tracks various performance metrics during NLM operation:
 * - Time spent in different brain components
 * - Memory usage patterns
 * - Event processing rates
 * - Simulation throughput
 * - System bottlenecks
 * 
 * Phase 6 enables comprehensive profiling to monitor integration effectiveness.
 */

class PerformanceProfiler {
public:
    struct ProfileData {
        double totalTime = 0.0;
        double neuronUpdateTime = 0.0;
        double spikeProcessingTime = 0.0;
        double memoryUpdateTime = 0.0;
        double neuromodulationTime = 0.0;
        double plasticityTime = 0.0;
        double predictionTime = 0.0;
        double conceptFormationTime = 0.0;
        double attentionTime = 0.0;
        double developmentTime = 0.0;
        double stepTime = 0.0;
        
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t spikeCount = 0;
        size_t memoryTraceCount = 0;
        size_t episodicMemoryCount = 0;
        size_t predictionErrorCount = 0;
        
        double getTotalCpuTime() const {
            return neuronUpdateTime + spikeProcessingTime + 
                   memoryUpdateTime + neuromodulationTime + 
                   plasticityTime + predictionTime + conceptFormationTime +
                   attentionTime + developmentTime + stepTime;
        }
        
        double getIntegrationEfficiency() const {
            if (totalTime <= 0.0) return 0.0;
            return (neuronUpdateTime + memoryUpdateTime + neuromodulationTime) / totalTime * 100.0;
        }
    };

    struct EventProfile {
        std::string eventName;
        double totalTime = 0.0;
        size_t callCount = 0;
        double avgTime() const { return callCount > 0 ? totalTime / callCount : 0.0; }
    };

    struct MemoryProfile {
        size_t totalAllocated = 0;
        size_t totalDeallocated = 0;
        size_t currentUsage = 0;
        std::unordered_map<std::string, size_t> poolUsage;
        
        double getMemoryUsageMB() const { return currentUsage / (1024.0 * 1024.0); }
    };

    struct SystemProfile {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        double stepRate = 0.0;  // steps per second
        double spikeRate = 0.0;  // spikes per second
        double activityLevel = 0.0;  // Normalized activity 0-1
        std::chrono::steady_clock::time_point lastUpdate;
    };

    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Start/stop profiling
    void startProfile();
    void stopProfile();
    void reset();
    
    // Component timing
    void startTimer(const std::string& component);
    void stopTimer(const std::string& component);
    
    // Data collection
    void updateBrainStats(const ProfileData& stats);
    void updateMemoryStats(const MemoryProfile& mem);
    void updateSystemStats(const SystemProfile& sys);
    
    // Event tracking
    void recordEvent(const std::string& eventName, double duration);
    
    // Profiling configuration
    void enableProfiling(bool enable) { profilingEnabled_ = enable; }
    bool isProfilingEnabled() const { return profilingEnabled_; }
    
    void setProfilingInterval(size_t interval) { profilingInterval_ = interval; }
    void setOutputFile(const std::string& filename) { outputFile_ = filename; }
    
    // Analysis and reporting
    ProfileData getProfileData() const { return currentProfile_; }
    std::vector<EventProfile> getEventProfiles() const { return eventProfiles_; }
    MemoryProfile getMemoryProfile() const { return memoryProfile_; }
    SystemProfile getSystemProfile() const { return systemProfile_; }
    
    // Report generation
    std::string generateReport() const;
    void exportProfileData(const std::string& filename) const;
    void exportCSV(const std::string& filename) const;
    void exportJSON(const std::string& filename) const;
    
    // Integration analysis
    std::string analyzeIntegrationEfficiency() const;
    std::string analyzeSystemBalance() const;
    std::string analyzeBottlenecks() const;
    
    // Configuration
    void configure(bool enableProfiling, const std::string& outputFile = "performance_profile.csv",
                   size_t interval = 1000, bool exportOnStop = true);
    
    // Utility functions
    static double getCurrentTimestamp();
    static std::string formatTime(double seconds);
    static std::string formatMemory(size_t bytes);

private:
    bool profilingEnabled_;
    bool exportOnStop_;
    size_t profilingInterval_;
    std::string outputFile_;
    
    ProfileData currentProfile_;
    std::vector<EventProfile> eventProfiles_;
    MemoryProfile memoryProfile_;
    SystemProfile systemProfile_;
    
    std::unordered_map<std::string, double> timerStartTimes_;
    std::unordered_map<std::string, double> timerAccumulatedTimes_;
    std::unordered_map<std::string, size_t> timerCallCounts_;
    
    size_t stepCount_;
    std::chrono::steady_clock::time_point sessionStartTime_;
    double totalSessionTime_;
    
    mutable std::mutex profileMutex_;
    
    void updateEventProfiles();
    void updateSystemProfile();
    void exportProfileDataInternal() const;
    std::string formatCSVHeader() const;
    std::string formatCSVRow(double timestamp, const ProfileData& profile) const;
};

/**
 * Brain Profiler Integration
 * 
 * Integrates performance profiling with the NLM brain systems:
 * - Wraps brain step operations with timing
 * - Tracks system resource usage
 * - Monitors integration efficiency
 * - Collects component-specific metrics
 */

class BrainProfiler {
public:
    explicit BrainProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    ~BrainProfiler();
    
    // Disable copying
    BrainProfiler(const BrainProfiler&) = delete;
    BrainProfiler& operator=(const BrainProfiler&) = delete;
    
    // Integration with brain systems
    void startStepProfiling(SimulationStep step, Timestamp time);
    void endStepProfiling();
    
    // Component-specific profiling
    void profileMemorySystems(const ProfileData& stats);
    void profileNeuromodulation(const ProfileData& stats);
    void profilePredictionSystem(const ProfileData& stats);
    void profileCognitiveSystems(const ProfileData& stats);
    void profileDevelopmentSystem(const ProfileData& stats);
    
    // Configuration
    void enableComponentProfiling(bool enable) { componentProfilingEnabled_ = enable; }
    bool isComponentProfilingEnabled() const { return componentProfilingEnabled_; }
    
    void setStepLogging(bool enable) { stepLoggingEnabled_ = enable; }
    bool isStepLoggingEnabled() const { return stepLoggingEnabled_; }
    
    // Analysis
    std::string getIntegrationEfficiencyReport() const;
    std::string getSystemBalanceReport() const;
    std::string getPerformanceBottleneckReport() const;
    
    // Status
    bool isProfiling() const { return profiler_ != nullptr; }
    size_t getStepCount() const { return stepCount_; }
    double getTotalSimulationTime() const { return totalSimulationTime_; }

private:
    std::shared_ptr<PerformanceProfiler> profiler_;
    bool componentProfilingEnabled_;
    bool stepLoggingEnabled_;
    
    size_t stepCount_;
    double totalSimulationTime_;
    std::chrono::steady_clock::time_point stepStartTime_;
};

/**
 * CLI Tools for Performance Analysis
 * 
 * Command-line tools for analyzing NLM performance data:
 * - Profile analysis
 * - Bottleneck identification
 * - System optimization
 * - Integration diagnostics
 */

namespace CLITools {
    class PerformanceAnalyzer {
    public:
        PerformanceAnalyzer() = default;
        ~PerformanceAnalyzer() = default;
        
        // File analysis
        bool analyzeProfileFile(const std::string& filename);
        bool analyzeCSVDataset(const std::string& filename);
        bool analyzeJSONDataset(const std::string& filename);
        
        // Report generation
        std::string generatePerformanceReport(const std::string& filename, 
                                              const std::string& reportType = "summary");
        std::string generateIntegrationReport(const std::string& filename);
        std::string generateOptimizationReport(const std::string& filename);
        
        // Visualization
        std::string generateTimelineChart(const std::string& filename);
        std::string generateComponentChart(const std::string& filename);
        std::string generateBottleneckChart(const std::string& filename);
        
        // Comparison
        std::string compareProfiles(const std::vector<std::string>& filenames);
        std::string findPerformanceTrends(const std::string& filename);
        
        // Configuration
        void setOutputDirectory(const std::string& dir) { outputDirectory_ = dir; }
        void setReportFormat(const std::string& format) { reportFormat_ = format; }
        
    private:
        std::string outputDirectory_;
        std::string reportFormat_;
    };
    
    class BenchmarkRunner {
    public:
        BenchmarkRunner() = default;
        ~BenchmarkRunner() = default;
        
        // Benchmark configurations
        struct BenchmarkConfig {
            std::string name;
            size_t neuronCount;
            size_t regionCount;
            float connectionProbability;
            float simulationTimestep;
            size_t durationSteps;
            bool enableAllSystems;
            std::string outputFile;
        };
        
        // Standard benchmarks
        bool runNeuralCoreBenchmark(const BenchmarkConfig& config);
        bool runIntegrationBenchmark(const BenchmarkConfig& config);
        bool runMemoryBenchmark(const BenchmarkConfig& config);
        bool runPredictionBenchmark(const BenchmarkConfig& config);
        bool runCognitiveBenchmark(const BenchmarkConfig& config);
        
        // Comprehensive benchmark
        bool runComprehensiveBenchmark(const BenchmarkConfig& config);
        
        // Result analysis
        std::string analyzeBenchmarkResults(const std::string& resultsFile);
        std::string generateBenchmarkReport(const std::string& resultsFile);
        
        // Configuration presets
        BenchmarkConfig getQuickBenchmarkConfig() const;
        BenchmarkConfig getResearchBenchmarkConfig() const;
        BenchmarkConfig getProductionBenchmarkConfig() const;
        BenchmarkConfig getStressBenchmarkConfig() const;
        
    private:
        std::string benchmarkResultsDir_;
    };
}

/**
 * Utility Functions for Performance Analysis
 */

namespace PerformanceUtils {
    // Data file operations
    bool readCSVProfile(const std::string& filename, std::vector<std::vector<std::string>>& data);
    bool readJSONProfile(const std::string& filename, std::string& jsonData);
    bool writeProfileData(const std::string& filename, const std::string& data);
    
    // Data analysis
    double calculateMovingAverage(const std::vector<double>& data, size_t window);
    double calculateStandardDeviation(const std::vector<double>& data);
    size_t findMaxIndex(const std::vector<double>& data);
    size_t findMinIndex(const std::vector<double>& data);
    
    // Performance metrics
    double calculateThroughput(const std::vector<double>& times, const std::vector<double>& steps);
    double calculateEfficiency(const std::vector<double>& actual, const std::vector<double>& ideal);
    double calculateScalability(const std::vector<size_t>& neurons, const std::vector<double>& performance);
    
    // Visualization helpers
    std::string escapeHTML(const std::string& text);
    std::string formatNumber(size_t number);
    std::string formatPercentage(double value);
    std::string formatTimeDelta(double seconds);
    
    // Configuration
    void setProfileOutputDirectory(const std::string& dir);
    void setBenchmarkResultsDirectory(const std::string& dir);
}

} // namespace nlm

#endif // PERFORMANCE_PROFILER_HPP
