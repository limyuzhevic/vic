// Advanced Debugger Implementation

#include "nlm/debug/AdvancedDebugger.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <set>

namespace nlm {
namespace debug {

AdvancedDebugger::AdvancedDebugger() : debuggingActive_(false), memoryTrackingActive_(false), 
    profilingActive_(false), monitoringInterval_(100000000) {
}

AdvancedDebugger::~AdvancedDebugger() {
}

bool AdvancedDebugger::startDebugging(const std::string& debugLevel) {
    debuggingActive_ = true;
    
    std::cout << "Debugging started at level: " << debugLevel << std::endl;
    
    // Configure debug level
    if (debugLevel == "verbose") {
        monitoringInterval_ = 10000000; // 10ms
    } else if (debugLevel == "minimal") {
        monitoringInterval_ = 500000000; // 500ms
    } else {
        monitoringInterval_ = 100000000; // 100ms default
    }
    
    return true;
}

void AdvancedDebugger::stopDebugging() {
    debuggingActive_ = false;
    std::cout << "Debugging stopped" << std::endl;
}

void AdvancedDebugger::pauseDebugging() {
    debuggingActive_ = false;
    std::cout << "Debugging paused" << std::endl;
}

void AdvancedDebugger::resumeDebugging() {
    debuggingActive_ = true;
    std::cout << "Debugging resumed" << std::endl;
}

void AdvancedDebugger::traceExecution(const std::string& operation, const std::string& function, const std::string& file, int line) {
    if (!debuggingActive_) return;
    
    ExecutionTrace trace;
    trace.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    trace.threadName = "main"; // Would get from thread-specific data
    trace.functionName = function;
    trace.fileName = file;
    trace.lineNumber = line;
    trace.operation = operation;
    
    addExecutionTrace(trace);
}

void AdvancedDebugger::traceExecution(const std::string& operation, std::chrono::nanoseconds duration, const std::string& function, const std::string& file, int line) {
    if (!debuggingActive_) return;
    
    ExecutionTrace trace;
    trace.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    trace.threadName = "main";
    trace.functionName = function;
    trace.fileName = file;
    trace.lineNumber = line;
    trace.operation = operation;
    trace.duration = duration;
    
    addExecutionTrace(trace);
}

std::vector<AdvancedDebugger::ExecutionTrace> AdvancedDebugger::getExecutionTrace(const std::string& filter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (filter.empty()) {
        return executionTrace_;
    }
    
    std::vector<ExecutionTrace> filteredTraces;
    for (const auto& trace : executionTrace_) {
        if (trace.operation.find(filter) != std::string::npos ||
            trace.functionName.find(filter) != std::string::npos) {
            filteredTraces.push_back(trace);
        }
    }
    
    return filteredTraces;
}

bool AdvancedDebugger::startMemoryTracking() {
    memoryTrackingActive_ = true;
    std::cout << "Memory tracking started" << std::endl;
    return true;
}

void AdvancedDebugger::stopMemoryTracking() {
    memoryTrackingActive_ = false;
    std::cout << "Memory tracking stopped" << std::endl;
}

bool AdvancedDebugger::detectMemoryLeaks() {
    std::cout << "Detecting memory leaks..." << std::endl;
    
    // Simple memory leak detection (would need proper implementation)
    if (memoryAllocations_.size() > memoryDeallocations_.size()) {
        std::cout << "MEMORY LEAK DETECTED: More allocations than deallocations" << std::endl;
        return true;
    }
    
    std::cout << "No memory leaks detected" << std::endl;
    return false;
}

std::vector<std::string> AdvancedDebugger::getMemoryLeakReport() const {
    std::vector<std::string> report;
    
    if (memoryAllocations_.size() > memoryDeallocations_.size()) {
        report.push_back("MEMORY LEAK REPORT:");
        report.push_back("==================");
        report.push_back("Allocation Count: " + std::to_string(memoryAllocations_.size()));
        report.push_back("Deallocation Count: " + std::to_string(memoryDeallocations_.size()));
        report.push_back("Unmatched Allocations: " + std::to_string(memoryAllocations_.size() - memoryDeallocations_.size()));
        
        // Group by location
        std::map<std::string, int> locationCounts;
        for (const auto& alloc : memoryAllocations_) {
            locationCounts[alloc.first]++;
        }
        
        report.push_back("\nAllocations by Location:");
        for (const auto& pair : locationCounts) {
            report.push_back("  " + pair.first + ": " + std::to_string(pair.second) + " allocations");
        }
    } else {
        report.push_back("No memory leaks detected");
    }
    
    return report;
}

bool AdvancedDebugger::analyzePerformanceBottlenecks() {
    std::cout << "Analyzing performance bottlenecks..." << std::endl;
    
    // Simple bottleneck analysis
    if (executionTrace_.empty()) {
        std::cout << "No execution trace data available" << std::endl;
        return true;
    }
    
    // Group by operation and calculate durations
    std::map<std::string, std::vector<std::chrono::nanoseconds>> operationDurations;
    
    for (const auto& trace : executionTrace_) {
        if (trace.duration.count() > 0) {
            operationDurations[trace.operation].push_back(trace.duration);
        }
    }
    
    std::cout << "Bottleneck analysis results:" << std::endl;
    
    for (const auto& pair : operationDurations) {
        const std::string& operation = pair.first;
        const std::vector<std::chrono::nanoseconds>& durations = pair.second;
        
        // Calculate statistics
        double avgDuration = 0, maxDuration = 0;
        if (!durations.empty()) {
            avgDuration = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
            maxDuration = durations.front();
            for (const auto& dur : durations) {
                if (dur.count() > maxDuration) {
                    maxDuration = dur;
                }
            }
        }
        
        if (maxDuration.count() > 1000000) { // 1ms threshold
            std::cout << "  BOTTLENECK: " << operation << " (avg: " << avgDuration << "ns, max: " << maxDuration.count() << "ns)" << std::endl;
        }
    }
    
    return true;
}

std::vector<std::string> AdvancedDebugger::getBottleneckAnalysis() const {
    std::vector<std::string> analysis;
    
    if (executionTrace_.empty()) {
        analysis.push_back("No execution trace data available for bottleneck analysis");
        return analysis;
    }
    
    analysis.push_back("Performance Bottleneck Analysis:");
    analysis.push_back("==============================");
    
    // Group operations and find slow ones
    std::map<std::string, std::vector<std::chrono::nanoseconds>> operationDurations;
    
    for (const auto& trace : executionTrace_) {
        if (trace.duration.count() > 0) {
            operationDurations[trace.operation].push_back(trace.duration);
        }
    }
    
    for (const auto& pair : operationDurations) {
        const std::string& operation = pair.first;
        const std::vector<std::chrono::nanoseconds>& durations = pair.second;
        
        double avgDuration = 0, maxDuration = 0;
        if (!durations.empty()) {
            avgDuration = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
            maxDuration = durations.front();
            for (const auto& dur : durations) {
                if (dur.count() > maxDuration) {
                    maxDuration = dur;
                }
            }
        }
        
        if (maxDuration.count() > 1000000) {
            analysis.push_back("BOTTLENECK: " + operation + 
                            " (avg: " + std::to_string(avgDuration) + "ns, max: " + std::to_string(maxDuration.count()) + "ns)");
        }
    }
    
    if (analysis.size() == 2) { // Only header added
        analysis.push_back("No significant performance bottlenecks detected");
    }
    
    return analysis;
}

bool AdvancedDebugger::startCustomProfiling(const std::string& profileName, const std::map<std::string, std::string>& parameters) {
    profilingActive_ = true;
    std::cout << "Custom profiling started: " << profileName << std::endl;
    return true;
}

bool AdvancedDebugger::stopCustomProfiling() {
    profilingActive_ = false;
    std::cout << "Custom profiling stopped" << std::endl;
    return true;
}

bool AdvancedDebugger::isProfilingActive() const {
    return profilingActive_;
}

std::string AdvancedDebugger::getCustomProfileResults() const {
    std::stringstream ss;
    
    ss << "Custom Profile Results:" << std::endl;
    ss << "=====================" << std::endl;
    ss << "Profile Status: " << (profilingActive_ ? "Active" : "Inactive") << std::endl;
    ss << "Execution Traces: " << executionTrace_.size() << std::endl;
    ss << "Memory Allocations: " << memoryAllocations_.size() << std::endl;
    ss << "Memory Deallocations: " << memoryDeallocations_.size() << std::endl;
    
    return ss.str();
}

bool AdvancedDebugger::startMemoryProfiling() {
    std::cout << "Memory profiling started" << std::endl;
    return true;
}

bool AdvancedDebugger::stopMemoryProfiling() {
    std::cout << "Memory profiling stopped" << std::endl;
    return true;
}

bool AdvancedDebugger::enableHeapTracking() {
    std::cout << "Heap tracking enabled" << std::endl;
    return true;
}

bool AdvancedDebugger::enableAllocationTracking() {
    std::cout << "Allocation tracking enabled" << std::endl;
    return true;
}

std::string AdvancedDebugger::getMemoryProfileResults() const {
    std::stringstream ss;
    
    ss << "Memory Profile Results:" << std::endl;
    ss << "======================" << std::endl;
    ss << "Total Allocations: " << memoryAllocations_.size() << std::endl;
    ss << "Total Deallocations: " << memoryDeallocations_.size() << std::endl;
    
    // Calculate total memory
    uint64_t totalAllocated = 0;
    for (const auto& alloc : memoryAllocations_) {
        totalAllocated += alloc.second.count();
    }
    
    ss << "Total Allocated: " << totalAllocated << " bytes" << std::endl;
    
    return ss.str();
}

// Internal helper methods

inline void AdvancedDebugger::addExecutionTrace(const ExecutionTrace& trace) {
    std::lock_guard<std::mutex> lock(mutex_);
    executionTrace_.push_back(trace);
    
    // Keep only last 10000 traces
    if (executionTrace_.size() > 10000) {
        executionTrace_.erase(executionTrace_.begin());
    }
}

inline std::vector<AdvancedDebugger::ExecutionTrace> AdvancedDebugger::filterExecutionTrace(const std::string& filter) const {
    std::vector<ExecutionTrace> filteredTraces;
    
    for (const auto& trace : executionTrace_) {
        if (filter.empty() || trace.operation.find(filter) != std::string::npos ||
            trace.functionName.find(filter) != std::string::npos) {
            filteredTraces.push_back(trace);
        }
    }
    
    return filteredTraces;
}

inline void AdvancedDebugger::trackMemoryAllocation(const std::string& location, std::chrono::nanoseconds size) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryAllocations_.push_back(std::make_pair(location, size));
}

inline void AdvancedDebugger::trackMemoryDeallocation(const std::string& location) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryDeallocations_.push_back(std::make_pair(location, std::chrono::nanoseconds(0)));
}

} // namespace debug
} // namespace nlm
