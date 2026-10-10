#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "NLMExceptions.hpp"
#include "DebugInfrastructure.hpp"
#include "DiagnosticTools.hpp"
#include "MonitoringInfrastructure.hpp"

namespace nlm {

// Visualization data structures
struct VisualizationData {
    std::string id;
    std::string type;           // "debug", "performance", "memory", "thread", "network"
    std::string title;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::vector<std::pair<double, double>> dataPoints; // (x, y) for plotting
    std::string format;         // "text", "csv", "json", "html", "svg"
    std::string content;
    std::map<std::string, std::string> metadata;
};

// Debug visualization data
struct DebugVisualizationData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<DebugEntry> debugEntries;
    std::vector<StackTrace> callStacks;
    std::map<std::string, std::string> variableValues;
    std::map<std::string, double> performanceMetrics;
    std::string fileFilter;
    int lineFilter = 0;
    std::string levelFilter;
};

// Performance visualization data
struct PerformanceVisualizationData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<PerformanceInfo> performanceData;
    std::map<std::string, std::vector<double>> metricTimeSeries;
    std::map<std::string, std::vector<std::pair<double, double>>> performancePlots;
    std::string operationFilter;
    std::string categoryFilter;
    double timeWindowSeconds = 3600.0;
};

// Memory visualization data
struct MemoryVisualizationData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::map<uintptr_t, std::string> allocations;
    std::map<uintptr_t, size_t> allocationSizes;
    std::vector<std::pair<uintptr_t, size_t>> memoryBlocks;
    double totalMemory = 0.0;
    double usedMemory = 0.0;
    std::string allocationTypeFilter;
    bool showLeaks = false;
};

// Thread visualization data
struct ThreadVisualizationData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<std::string> activeThreads;
    std::map<std::string, std::vector<std::string>> threadActivities;
    std::map<std::string, std::chrono::system_clock::time_point> threadStartTimes;
    std::map<std::string, double> threadCpuUsage;
    std::string threadFilter;
    bool showStackTraces = false;
};

// Network visualization data
struct NetworkVisualizationData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, size_t>>> connectionHistory;
    std::map<std::string, size_t> connectionsByHost;
    std::map<std::string, size_t> bytesTransferred;
    std::map<std::string, double> connectionDurations;
    std::string hostFilter;
    bool showProtocolBreakdown = true;
};

// DebugVisualizer - Debug information visualization
class DebugVisualizer {
public:
    DebugVisualizer();
    ~DebugVisualizer();
    
    // Visualize debug entries
    std::string visualizeDebugEntries(const DebugVisualizationData& data) const;
    std::string visualizeDebugTimeline(const DebugVisualizationData& data) const;
    std::string visualizeDebugCallStack(const DebugVisualizationData& data) const;
    std::string visualizeDebugVariables(const DebugVisualizationData& data) const;
    
    // Generate debug report
    std::string generateDebugReport(const DebugVisualizationData& data) const;
    
    // Export visualizations
    bool exportVisualization(const VisualizationData& vizData, const std::string& filepath) const;
    
    // Get supported formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Get global debug visualizer instance
    static DebugVisualizer& getGlobal();
    
    // Set global debug visualizer
    static void setGlobal(std::shared_ptr<DebugVisualizer> visualizer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PerformanceVisualizer - Performance metrics visualization
class PerformanceVisualizer {
public:
    PerformanceVisualizer();
    ~PerformanceVisualizer();
    
    // Visualize performance data
    std::string visualizePerformanceData(const PerformanceVisualizationData& data) const;
    std::string visualizePerformanceTimeline(const PerformanceVisualizationData& data) const;
    std::string visualizePerformanceComparison(const PerformanceVisualizationData& data) const;
    std::string visualizePerformanceHeatmap(const PerformanceVisualizationData& data) const;
    
    // Generate performance report
    std::string generatePerformanceReport(const PerformanceVisualizationData& data) const;
    
    // Export visualizations
    bool exportVisualization(const VisualizationData& vizData, const std::string& filepath) const;
    
    // Get supported formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Get global performance visualizer instance
    static PerformanceVisualizer& getGlobal();
    
    // Set global performance visualizer
    static void setGlobal(std::shared_ptr<PerformanceVisualizer> visualizer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// MemoryVisualizer - Memory usage visualization
class MemoryVisualizer {
public:
    MemoryVisualizer();
    ~MemoryVisualizer();
    
    // Visualize memory data
    std::string visualizeMemoryUsage(const MemoryVisualizationData& data) const;
    std::string visualizeMemoryTimeline(const MemoryVisualizationData& data) const;
    std::string visualizeMemoryAllocationHeatmap(const MemoryVisualizationData& data) const;
    std::string visualizeMemoryLeaks(const MemoryVisualizationData& data) const;
    
    // Generate memory report
    std::string generateMemoryReport(const MemoryVisualizationData& data) const;
    
    // Export visualizations
    bool exportVisualization(const VisualizationData& vizData, const std::string& filepath) const;
    
    // Get supported formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Get global memory visualizer instance
    static MemoryVisualizer& getGlobal();
    
    // Set global memory visualizer
    static void setGlobal(std::shared_ptr<MemoryVisualizer> visualizer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ThreadVisualizer - Thread activity visualization
class ThreadVisualizer {
public:
    ThreadVisualizer();
    ~ThreadVisualizer();
    
    // Visualize thread data
    std::string visualizeThreadTimeline(const ThreadVisualizationData& data) const;
    std::string visualizeThreadActivityChart(const ThreadVisualizationData& data) const;
    std::string visualizeThreadCallStack(const ThreadVisualizationData& data) const;
    std::string visualizeThreadCpuUsage(const ThreadVisualizationData& data) const;
    
    // Generate thread report
    std::string generateThreadReport(const ThreadVisualizationData& data) const;
    
    // Export visualizations
    bool exportVisualization(const VisualizationData& vizData, const std::string& filepath) const;
    
    // Get supported formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Get global thread visualizer instance
    static ThreadVisualizer& getGlobal();
    
    // Set global thread visualizer
    static void setGlobal(std::shared_ptr<ThreadVisualizer> visualizer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// NetworkVisualizer - Network activity visualization
class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Visualize network data
    std::string visualizeNetworkTimeline(const NetworkVisualizationData& data) const;
    std::string visualizeNetworkTrafficChart(const NetworkVisualizationData& data) const;
    std::string visualizeNetworkConnectionGraph(const NetworkVisualizationData& data) const;
    std::string visualizeNetworkProtocolBreakdown(const NetworkVisualizationData& data) const;
    
    // Generate network report
    std::string generateNetworkReport(const NetworkVisualizationData& data) const;
    
    // Export visualizations
    bool exportVisualization(const VisualizationData& vizData, const std::string& filepath) const;
    
    // Get supported formats
    std::vector<std::string> getSupportedFormats() const;
    
    // Get global network visualizer instance
    static NetworkVisualizer& getGlobal();
    
    // Set global network visualizer
    static void setGlobal(std::shared_ptr<NetworkVisualizer> visualizer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
