// Debugging test mode
// Detailed analysis and debugging of NLM internal state

#pragma once

#include "experiments/Experiment.hpp"
#include "core/PerformanceProfiler.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>

namespace nlm {

// Debug information
struct DebugInfo {
    std::string componentName; // "brain", "memory", "plasticity", "neuromodulation"
    std::string debugType; // "allocation", "execution", "state", "performance"
    std::string message;
    std::map<std::string, std::string> metadata; // additional debugging data
    std::chrono::system_clock::time_point timestamp;
    std::string severity; // "info", "warning", "error", "critical"
    bool resolved; // whether the debug issue was resolved
    
    DebugInfo() : timestamp(), resolved(false) {}
};

// Watchpoint information
struct Watchpoint {
    std::string componentName;
    std::string variableName;
    std::string condition; // expression to evaluate
    std::string expectedValue;
    std::string actualValue;
    std::chrono::system_clock::time_point triggerTime;
    bool triggered;
    std::string resolution;
    
    Watchpoint() : triggered(false) {}
};

// Memory allocation tracking
struct MemoryAllocation {
    void* address;
    size_t size;
    std::string allocator; // "memory_pool", "heap", "system"
    std::chrono::system_clock::time_point allocationTime;
    std::string componentName;
    bool tracked;
    
    MemoryAllocation() : address(nullptr), size(0), tracked(false) {}
};

// Debug experiment
class DebugExperiment : public Experiment {
public:
    DebugExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~DebugExperiment() = default;
    
    // Experiment interface
    bool setup() override;
    bool run() override;
    bool cleanup() override;
    
    // Debug-specific methods
    bool enableBreakpoints();
    bool enableWatchpoints();
    bool enableLogging(const std::string& logLevel = "debug");
    bool trackAllocation();
    bool trackExecution();
    
    // Debug analysis
    std::vector<DebugInfo> getDebugInfo() const { return debugInfo_; }
    std::vector<Watchpoint> getWatchpoints() const { return watchpoints_; }
    std::vector<MemoryAllocation> getMemoryAllocations() const { return memoryAllocations_; }
    
    // Debug resolution
    bool resolveDebugIssue(const std::string& issueId, const std::string& resolution);
    bool triggerWatchpoint(const std::string& component, const std::string& variable);
    
    // Output generation
    std::string generateReport() const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
protected:
    std::vector<DebugInfo> debugInfo_;
    std::vector<Watchpoint> watchpoints_;
    std::vector<MemoryAllocation> memoryAllocations_;
    std::shared_ptr<PerformanceProfiler> profiler_;
    DebugConfig debugConfig_;
};

} // namespace nlm
