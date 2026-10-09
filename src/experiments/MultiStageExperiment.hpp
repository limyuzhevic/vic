#pragma once

#include "../batch_processing/BatchUtils.hpp"
#include "../performance/ParallelProcessing.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace nlm {

// Execution time tracking and optimization recommendations
class ExecutionTracker {
public:
    struct ExecutionEvent {
        std::string eventType;
        std::chrono::high_resolution_clock::time_point timestamp;
        double duration; // in seconds
        size_t threadId;
        std::string description;
        std::map<std::string, std::string> metadata;
        
        ExecutionEvent(const std::string& type, const std::string& desc,
                      size_t thread, const std::map<std::string, std::string>& meta = {})
            : eventType(type), timestamp(std::chrono::high_resolution_clock::now()),
              duration(0), threadId(thread), description(desc), metadata(meta) {}
    };
    
    ExecutionTracker();
    ~ExecutionTracker();
    
    // Record an execution event
    void recordEvent(const std::string& eventType, const std::string& description,
                    size_t threadId = ThreadPool::getCurrentThreadId(),
                    const std::map<std::string, std::string>& metadata = {});
    
    // Start timing a section
    void startTiming(const std::string& sectionName, size_t threadId = ThreadPool::getCurrentThreadId());
    
    // End timing a section
    double endTiming(const std::string& sectionName, size_t threadId = ThreadPool::getCurrentThreadId());
    
    // Get execution summary for batch
    std::map<std::string, double> getExecutionSummary(const std::vector<BatchSimulationResult>& results) const;
    
    // Generate optimization recommendations based on timing data
    std::vector<std::string> generateOptimizationRecommendations() const;
    
    // Get execution history
    const std::vector<ExecutionEvent>& getEventHistory() const { return events_; }
    
    // Clear event history
    void clearEventHistory() { events_.clear(); }
    
private:
    std::vector<ExecutionEvent> events_;
    mutable std::mutex eventsMutex_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> activeTimers_;
    std::unordered_map<std::string, double> sectionTimes_;
};

// Memory usage monitoring
class MemoryMonitor {
public:
    struct MemoryStats {
        size_t totalPhysicalMemory;
        size_t availableMemory;
        size_t usedMemory;
        size_t processMemory;
        size_t processVirtualMemory;
        double memoryUtilizationPercent;
        
        MemoryStats() : totalPhysicalMemory(0), availableMemory(0), usedMemory(0),
                       processMemory(0), processVirtualMemory(0), memoryUtilizationPercent(0) {}
    };
    
    MemoryMonitor();
    ~MemoryMonitor();
    
    // Get current memory statistics
    MemoryStats getCurrentMemoryStats() const;
    
    // Get historical memory usage
    std::vector<MemoryStats> getMemoryHistory(size_t maxHistory = 1000) const;
    
    // Check for memory leaks (if memory usage grows over time)
    bool detectMemoryLeak(size_t sampleSize = 10) const;
    
    // Get memory usage trend
    double getMemoryUsageTrend(size_t windowSize = 10) const;
    
private:
    mutable std::mutex statsMutex_;
    std::vector<MemoryStats> memoryHistory_;
    std::thread monitorThread_;
    std::atomic<bool> running_{false};
    
    void monitorFunc();
    size_t getProcessMemoryUsage() const;
    size_t getProcessVirtualMemoryUsage() const;
};

// Experiment result aggregation and analysis
class ExperimentResultAggregator {
public:
    struct AggregatedResult {
        std::string experimentName;
        size_t totalRuns;
        size_t successfulRuns;
        size_t failedRuns;
        double meanExecutionTime;
        double stdExecutionTime;
        double minExecutionTime;
        double maxExecutionTime;
        std::map<std::string, double> meanMetrics;
        std::map<std::string, double> stdMetrics;
        std::map<std::string, double> minMetrics;
        std::map<std::string, double> maxMetrics;
        std::map<std::string, std::vector<double>> rawData;
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "Experiment: " << experimentName << "\n"
               << "  Runs: " << totalRuns << " (" << successfulRuns << " successful, " << failedRuns << " failed)\n"
               << "  Execution time: mean=" << meanExecutionTime << "s (std=" << stdExecutionTime << "s), "
               << "range=[" << minExecutionTime << "s, " << maxExecutionTime << "s]\n";
            
            for (const auto& [metric, meanValue] : meanMetrics) {
                if (rawData.find(metric) != rawData.end()) {
                    ss << "  " << metric << ": " << meanValue << " (std=" << stdMetrics[metric] << ")\n";
                }
            }
            
            return ss.str();
        }
    };
    
    // Aggregate batch simulation results
    static AggregatedResult aggregateBatchResults(const std::vector<BatchSimulationResult>& results);
    
    // Perform statistical analysis on results
    static std::vector<double> performStatisticalAnalysis(const std::vector<double>& data,
                                                          const std::string& analysisType = "basic");
    
    // Generate result summary report
    static std::string generateResultSummary(const std::vector<AggregatedResult>& aggregatedResults);
};

// Result comparison and visualization helpers
class ResultComparator {
public:
    struct ComparisonResult {
        std::string baselineExperiment;
        std::string currentExperiment;
        std::map<std::string, double> baselineMetrics;
        std::map<std::string, double> currentMetrics;
        std::map<std::string, double> differences; // current - baseline
        std::map<std::string, double> percentChanges; // ((current - baseline) / baseline) * 100
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "Comparison: " << baselineExperiment << " vs " << currentExperiment << "\n";
            
            for (const auto& [metric, diff] : differences) {
                double percentChange = percentChanges.at(metric);
                ss << "  " << metric << ": " << diff << " (" << percentChange << "%)" << std::endl;
            }
            
            return ss.str();
        }
    };
    
    // Compare two experiment results
    static ComparisonResult compareExperiments(const AggregatedResult& baseline,
                                              const AggregatedResult& current);
    
    // Generate comparison visualization data
    static std::vector<std::string> generateVisualizationData(const ComparisonResult& comparison,
                                                             const std::string& chartType = "bar");
};nlm