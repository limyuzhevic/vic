// Advanced Debugger
// This class provides advanced debugging and profiling capabilities

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace debug {

class AdvancedDebugger {
public:
    AdvancedDebugger();
    ~AdvancedDebugger();
    
    // Debugging control
    bool startDebugging(const std::string& debugLevel = "standard");
    void stopDebugging();
    void pauseDebugging();
    void resumeDebugging();
    
    // Execution tracing
    struct ExecutionTrace {
        std::chrono::nanoseconds timestamp;
        std::string threadName;
        std::string functionName;
        std::string fileName;
        int lineNumber;
        std::string operation;
        std::chrono::nanoseconds duration;
        std::map<std::string, std::string> variables;
        
        ExecutionTrace() : timestamp(0), lineNumber(0), duration(0) {}
    };
    
    void traceExecution(const std::string& operation, const std::string& function = "", const std::string& file = "", int line = 0);
    void traceExecution(const std::string& operation, std::chrono::nanoseconds duration, const std::string& function = "", const std::string& file = "", int line = 0);
    
    std::vector<ExecutionTrace> getExecutionTrace(const std::string& filter = "") const;
    
    // Memory leak detection
    bool startMemoryTracking();
    void stopMemoryTracking();
    bool detectMemoryLeaks();
    std::vector<std::string> getMemoryLeakReport() const;
    
    // Performance bottleneck identification
    bool analyzePerformanceBottlenecks();
    std::vector<std::string> getBottleneckAnalysis() const;
    
    // Custom profiling
    bool startCustomProfiling(const std::string& profileName, const std::map<std::string, std::string>& parameters);
    bool stopCustomProfiling();
    bool isProfilingActive() const;
    std::string getCustomProfileResults() const;
    
    // Memory profiling
    bool startMemoryProfiling();
    bool stopMemoryProfiling();
    bool enableHeapTracking();
    bool enableAllocationTracking();
    std::string getMemoryProfileResults() const;
    
    // Debugging status
    bool isDebuggingActive() const { return debuggingActive_; }
    bool isMemoryTrackingActive() const { return memoryTrackingActive_; }
    bool isProfilingActive() const { return profilingActive_; }
    
private:
    bool debuggingActive_;
    bool memoryTrackingActive_;
    bool profilingActive_;
    
    std::chrono::nanoseconds monitoringInterval_;
    
    mutable std::mutex mutex_;
    
    std::vector<ExecutionTrace> executionTrace_;
    std::vector<std::pair<std::string, std::chrono::nanoseconds>> memoryAllocations_;
    std::vector<std::pair<std::string, std::chrono::nanoseconds>> memoryDeallocations_;
    
    // Performance analysis
    void analyzeExecutionTrace();
    void identifyMemoryLeaks();
    void detectAllocationPatterns();
    
    // Trace management
    void addExecutionTrace(const ExecutionTrace& trace);
    std::vector<ExecutionTrace> filterExecutionTrace(const std::string& filter) const;
    
    // Memory tracking
    void trackMemoryAllocation(const std::string& location, std::chrono::nanoseconds size);
    void trackMemoryDeallocation(const std::string& location);
    
    // Profile generation
    std::string generateExecutionTraceReport() const;
    std::string generateMemoryLeakReport() const;
    std::string generateBottleneckReport() const;
    std::string generateCustomProfileReport() const;
};

} // namespace debug
} // namespace nlm
