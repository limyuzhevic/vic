#pragma once

/**
 * Advanced Profiling System for NLM Framework
 * 
 * This system provides comprehensive profiling capabilities:
 * - Call stack profiling with detailed stack traces
 * - Memory allocation profiling with allocation/deallocation tracking
 * - Event-driven profiling with detailed event timelines
 * - Performance bottleneck detection and hot path identification
 * - Flame graph generation for visual performance analysis
 * - Baseline establishment and regression detection
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include <variant>
#include <span>
#include <stacktrace>
#include "../core/Config/Config.hpp"
#include "PerformanceMonitor.hpp"

namespace nlm {

/**
 * Profiling configuration
 */
struct ProfilingConfig {
    MonitoringLevel monitoringLevel;
    bool enableCallStackProfiling;
    bool enableMemoryProfiling;
    bool enableEventProfiling;
    bool enableBottleneckDetection;
    bool generateFlameGraphs;
    size_t maxStackDepth;
    size_t maxMemorySamples;
    std::chrono::milliseconds profileInterval;
    std::vector<std::string> monitoredFunctions;
    std::vector<std::string> ignoredFunctions;
    std::unordered_map<std::string, size_t> memoryThresholds;
    std::unordered_map<std::string, uint64_t> eventThresholds;
};

/**
 * Call stack frame information
 */
struct CallStackFrame {
    std::string functionName;
    std::string filePath;
    int lineNumber;
    std::string moduleName;
    uint64_t address;
    std::string formattedLine() const;
};

/**
 * Memory allocation record
 */
struct MemoryAllocation {
    void* address;
    size_t size;
    std::string type;
    std::string allocationSite;
    std::chrono::microseconds timestamp;
    std::vector<std::string> stackTrace;
};

/**
 * Event record
 */
struct EventRecord {
    std::string eventName;
    std::chrono::microseconds timestamp;
    uint64_t threadId;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    uint64_t duration;
};

/**
 * Bottleneck detection result
 */
struct BottleneckInfo {
    std::string functionName;
    PerformanceCategory category;
    double impactFactor;  // 0.0 to 1.0, where 1.0 is most impactful
    std::chrono::microseconds duration;
    uint64_t callCount;
    double averageTimePerCall;
    std::string recommendation;
};

/**
 * Advanced profiling system for detailed performance analysis
 */
class Profiler {
public:
    Profiler(std::shared_ptr<Config> config);
    ~Profiler();
    
    // Profile configuration
    void setMonitoringLevel(MonitoringLevel level);
    MonitoringLevel getMonitoringLevel() const;
    
    // Call stack profiling
    void startCallStackProfiling();
    void stopCallStackProfiling();
    void recordCallStack(const std::vector<CallStackFrame>& stack);
    void recordCallStack(const std::stacktrace& trace);
    std::unordered_map<std::string, uint64_t> getCallStackCounts() const;
    std::vector<CallStackFrame> getCurrentCallStack() const;
    bool generateCallStackReport(const std::string& filename) const;
    
    // Memory allocation profiling
    void startMemoryProfiling();
    void stopMemoryProfiling();
    void recordAllocation(void* address, size_t size, const std::string& type,
                         const std::string& allocationSite = "");
    void recordDeallocation(void* address);
    std::unordered_map<std::string, uint64_t> getAllocationCounts() const;
    std::unordered_map<std::string, size_t> getAllocationSizes() const;
    std::vector<MemoryAllocation> getAllocationHistory() const;
    bool generateMemoryReport(const std::string& filename) const;
    
    // Event-driven profiling
    void startEventProfiling();
    void stopEventProfiling();
    void recordEvent(const std::string& eventName, uint64_t timestamp,
                    const std::unordered_map<std::string, std::string>& metadata = {});
    void recordTimedEvent(const std::string& eventName, std::function<void()> eventFunction);
    std::vector<EventRecord> getEventTimeline() const;
    std::vector<EventRecord> getEvents(const std::string& eventName) const;
    bool generateEventReport(const std::string& filename) const;
    
    // Performance bottleneck detection
    void updateBottleneckData();
    std::vector<BottleneckInfo> detectBottlenecks() const;
    std::vector<std::string> identifyHotPaths() const;
    std::string getBottleneckDetails() const;
    std::string getHotPathDetails() const;
    bool generateBottleneckReport(const std::string& filename) const;
    
    // Flame graph generation
    void updateFlameGraphData(const std::string& functionName, const std::string& filePath,
                           int lineNumber, uint64_t samples = 1);
    bool generateFlameGraph(const std::string& filename) const;
    std::unique_ptr<FlameGraphEntry> getFlameGraph() const;
    void clearFlameGraph();
    
    // Profile history
    std::vector<PerformanceProfile> getProfileHistory() const;
    void addProfileToHistory();
    void clearProfileHistory();
    
    // Performance comparison
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName, const std::string& outputFile = "") const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    void setProfilingConfig(const ProfilingConfig& config);
    ProfilingConfig getProfilingConfig() const;
    
    // Global profiling control
    static Profiler& getInstance();
    static void setInstance(std::shared_ptr<Profiler> profiler);
    static bool isInitialized();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor for singleton
    Profiler();
    Profiler(std::shared_ptr<Config> config, bool initialized);
};

} // namespace nlm

// Global profiler instance for convenience
nlm::Profiler& getGlobalProfiler();