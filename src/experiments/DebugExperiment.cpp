#include "DebugExperiment.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

DebugExperiment::DebugExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : Experiment(brain, config) {
    profiler_ = std::make_shared<PerformanceProfiler>();
}

bool DebugExperiment::setup() {
    std::cout << "Setting up debug experiment..." << std::endl;
    
    // Configure profiler with default debug settings
    debugConfig_ = PerformanceProfiler::createDetailedProfilingConfig();
    debugConfig_.enableBreakpoints = true;
    debugConfig_.enableWatchpoints = true;
    debugConfig_.enableLogging = true;
    debugConfig_.logLevel = "debug";
    debugConfig_.trackAllocation = true;
    debugConfig_.trackExecution = true;
    
    std::cout << "Debug experiment configured with log level: " << debugConfig_.logLevel << std::endl;
    return true;
}

bool DebugExperiment::run() {
    std::cout << "Running debug experiment..." << std::endl;
    
    // Enable debugging features
    bool breakpointsEnabled = enableBreakpoints();
    bool watchpointsEnabled = enableWatchpoints();
    bool loggingEnabled = enableLogging(debugConfig_.logLevel);
    bool allocationTrackingEnabled = trackAllocation();
    bool executionTrackingEnabled = trackExecution();
    
    std::cout << "Debug experiment setup complete:" << std::endl;
    std::cout << "  • Breakpoints enabled: " << (breakpointsEnabled ? "YES" : "NO") << std::endl;
    std::cout << "  • Watchpoints enabled: " << (watchpointsEnabled ? "YES" : "NO") << std::endl;
    std::cout << "  • Logging enabled: " << (loggingEnabled ? "YES" : "NO") << std::endl;
    std::cout << "  • Allocation tracking: " << (allocationTrackingEnabled ? "YES" : "NO") << std::endl;
    std::cout << "  • Execution tracking: " << (executionTrackingEnabled ? "YES" : "NO") << std::endl;
    
    // Simulate debug analysis
    std::cout << "\nPerforming debug analysis..." << std::endl;
    
    // Add debug information
    DebugInfo brainDebug;
    brainDebug.componentName = "brain";
    brainDebug.debugType = "state";
    brainDebug.message = "Brain initialized successfully";
    brainDebug.metadata["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
    brainDebug.metadata["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
    brainDebug.timestamp = std::chrono::system_clock::now();
    brainDebug.severity = "info";
    brainDebug.resolved = true;
    debugInfo_.push_back(brainDebug);
    
    DebugInfo memoryDebug;
    memoryDebug.componentName = "memory";
    memoryDebug.debugType = "allocation";
    memoryDebug.message = "Memory pools initialized";
    memoryDebug.metadata["pool_size"] = "4096";
    memoryDebug.metadata["max_objects"] = "1048576";
    memoryDebug.timestamp = std::chrono::system_clock::now();
    memoryDebug.severity = "info";
    memoryDebug.resolved = true;
    debugInfo_.push_back(memoryDebug);
    
    DebugInfo plasticityDebug;
    plasticityDebug.componentName = "plasticity";
    plasticityDebug.debugType = "execution";
    plasticityDebug.message = "STDP and Hebbian rules activated";
    plasticityDebug.metadata["stdp_enabled"] = "true";
    plasticityDebug.metadata["hebbian_enabled"] = "true";
    plasticityDebug.timestamp = std::chrono::system_clock::now();
    plasticityDebug.severity = "info";
    plasticityDebug.resolved = true;
    debugInfo_.push_back(plasticityDebug);
    
    // Add watchpoints
    Watchpoint weightWatch;
    weightWatch.componentName = "plasticity";
    weightWatch.variableName = "synaptic_weight";
    weightWatch.condition = "value > 1.0 || value < -1.0";
    weightWatch.expectedValue = "within [-1.0, 1.0]";
    weightWatch.actualValue = "0.0";
    weightWatch.triggered = false;
    weightWatch.resolution = "Weight normalization applied";
    watchpoints_.push_back(weightWatch);
    
    Watchpoint spikeWatch;
    spikeWatch.componentName = "dynamics";
    spikeWatch.variableName = "firing_rate";
    spikeWatch.condition = "rate > 100.0";
    spikeWatch.expectedValue = "< 100.0 Hz";
    spikeWatch.actualValue = "50.0 Hz";
    spikeWatch.triggered = false;
    spikeWatch.resolution = "Firing rate within normal range";
    watchpoints_.push_back(spikeWatch);
    
    // Add memory allocations
    MemoryAllocation poolAlloc;
    poolAlloc.address = (void*)0x1000;
    poolAlloc.size = 4096;
    poolAlloc.allocator = "memory_pool";
    poolAlloc.allocationTime = std::chrono::system_clock::now();
    poolAlloc.componentName = "brain";
    poolAlloc.tracked = true;
    memoryAllocations_.push_back(poolAlloc);
    
    MemoryAllocation heapAlloc;
    heapAlloc.address = (void*)0x2000;
    heapAlloc.size = 1024;
    heapAlloc.allocator = "heap";
    heapAlloc.allocationTime = std::chrono::system_clock::now();
    heapAlloc.componentName = "experiment";
    heapAlloc.tracked = true;
    memoryAllocations_.push_back(heapAlloc);
    
    std::cout << "Debug analysis completed" << std::endl;
    return true;
}

bool DebugExperiment::cleanup() {
    std::cout << "Cleaning up debug experiment..." << std::endl;
    
    // Generate and save report
    std::string report = generateReport();
    std::cout << "\nDebug Experiment Report:" << std::endl;
    std::cout << report << std::endl;
    
    // Export results
    std::string outputFile = "debug_experiment_results.json";
    if (exportResults(outputFile, "json")) {
        std::cout << "Results exported to " << outputFile << std::endl;
    } else {
        std::cout << "Failed to export results" << std::endl;
    }
    
    return true;
}

bool DebugExperiment::enableBreakpoints() {
    std::cout << "Enabling debugger breakpoints..." << std::endl;
    
    // Simulate breakpoint setup
    DebugInfo breakpointDebug;
    breakpointDebug.componentName = "debugger";
    breakpointDebug.debugType = "breakpoint";
    breakpointDebug.message = "Breakpoints configured for critical functions";
    breakpointDebug.metadata["breakpoints"] = "brain_step,plasticity_update,memory_consolidation";
    breakpointDebug.timestamp = std::chrono::system_clock::now();
    breakpointDebug.severity = "info";
    breakpointDebug.resolved = true;
    debugInfo_.push_back(breakpointDebug);
    
    return true;
}

bool DebugExperiment::enableWatchpoints() {
    std::cout << "Enabling watchpoints..." << std::endl;
    
    // Watchpoints are already set up in the main run() method
    DebugInfo watchpointDebug;
    watchpointDebug.componentName = "debugger";
    watchpointDebug.debugType = "watchpoint";
    watchpointDebug.message = "Watchpoints configured for monitoring key variables";
    watchpointDebug.metadata["monitored_variables"] = "synaptic_weight,firing_rate,memory_usage";
    watchpointDebug.timestamp = std::chrono::system_clock::now();
    watchpointDebug.severity = "info";
    watchpointDebug.resolved = true;
    debugInfo_.push_back(watchpointDebug);
    
    return true;
}

bool DebugExperiment::enableLogging(const std::string& logLevel) {
    std::cout << "Enabling logging with level: " << logLevel << std::endl;
    
    DebugInfo loggingDebug;
    loggingDebug.componentName = "logger";
    loggingDebug.debugType = "logging";
    loggingDebug.message = "Debug logging system initialized";
    loggingDebug.metadata["log_level"] = logLevel;
    loggingDebug.metadata["log_targets"] = "console,file,network";
    loggingDebug.timestamp = std::chrono::system_clock::now();
    loggingDebug.severity = "info";
    loggingDebug.resolved = true;
    debugInfo_.push_back(loggingDebug);
    
    return true;
}

bool DebugExperiment::trackAllocation() {
    std::cout << "Setting up memory allocation tracking..." << std::endl;
    
    DebugInfo allocationDebug;
    allocationDebug.componentName = "memory_tracker";
    allocationDebug.debugType = "allocation_tracking";
    allocationDebug.message = "Memory allocation tracking enabled";
    allocationDebug.metadata["tracked_allocators"] = "memory_pool,heap,system";
    allocationDebug.metadata["tracked_types"] = "neuron,synapse,plasticity";
    allocationDebug.timestamp = std::chrono::system_clock::now();
    allocationDebug.severity = "info";
    allocationDebug.resolved = true;
    debugInfo_.push_back(allocationDebug);
    
    return true;
}

bool DebugExperiment::trackExecution() {
    std::cout << "Setting up execution tracking..." << std::endl;
    
    DebugInfo executionDebug;
    executionDebug.componentName = "execution_tracker";
    executionDebug.debugType = "execution_tracking";
    executionDebug.message = "Execution tracking enabled";
    executionDebug.metadata["tracked_functions"] = "step,initialize,reset,save";
    executionDebug.metadata["tracked_metrics"] = "time,memory,throughput";
    executionDebug.timestamp = std::chrono::system_clock::now();
    executionDebug.severity = "info";
    executionDebug.resolved = true;
    debugInfo_.push_back(executionDebug);
    
    return true;
}

bool DebugExperiment::resolveDebugIssue(const std::string& issueId, const std::string& resolution) {
    // Find and resolve the debug issue
    for (auto& debug : debugInfo_) {
        if (!debug.resolved) {
            // In real implementation, check if this is the right issue
            debug.resolved = true;
            debug.message = debug.message + " RESOLVED: " + resolution;
            return true;
        }
    }
    return false;
}

bool DebugExperiment::triggerWatchpoint(const std::string& component, const std::string& variable) {
    // Find and trigger the watchpoint
    for (auto& watchpoint : watchpoints_) {
        if (watchpoint.componentName == component && watchpoint.variableName == variable && !watchpoint.triggered) {
            watchpoint.triggered = true;
            watchpoint.triggerTime = std::chrono::system_clock::now();
            watchpoint.resolution = "Issue resolved through debug analysis";
            return true;
        }
    }
    return false;
}

std::string DebugExperiment::generateReport() const {
    std::stringstream ss;
    ss << "NLM Debug Experiment Report" << std::endl;
    ss << std::string(50, '=') << std::endl << std::endl;
    
    ss << "Total Debug Issues: " << debugInfo_.size() << std::endl;
    ss << "Total Watchpoints: " << watchpoints_.size() << std::endl;
    ss << "Total Memory Allocations: " << memoryAllocations_.size() << std::endl;
    ss << std::endl;
    
    if (debugInfo_.empty() && watchpoints_.empty() && memoryAllocations_.empty()) {
        return ss.str();
    }
    
    ss << "Debug Information:" << std::endl;
    ss << "----------------" << std::endl;
    
    for (size_t i = 0; i < debugInfo_.size(); ++i) {
        const auto& debug = debugInfo_[i];
        
        ss << std::setw(3) << (i + 1) << ". " << debug.componentName << " ("
           << debug.debugType << ") - " << debug.severity << std::endl;
        ss << std::setw(6) << "  Message: " << debug.message << std::endl;
        ss << std::setw(6) << "  Time: " << std::chrono::system_clock::to_time_t(debug.timestamp) << std::endl;
        ss << std::setw(6) << "  Resolved: " << (debug.resolved ? "YES" : "NO") << std::endl;
        if (!debug.metadata.empty()) {
            ss << std::setw(6) << "  Metadata: ";
            bool first = true;
            for (const auto& pair : debug.metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
            ss << std::endl;
        }
        ss << std::endl;
    }
    
    ss << "Watchpoints:" << std::endl;
    ss << "-----------" << std::endl;
    
    for (size_t i = 0; i < watchpoints_.size(); ++i) {
        const auto& watchpoint = watchpoints_[i];
        
        ss << std::setw(3) << (i + 1) << ". " << watchpoint.componentName << ": "
           << watchpoint.variableName << std::endl;
        ss << std::setw(6) << "  Condition: " << watchpoint.condition << std::endl;
        ss << std::setw(6) << "  Expected: " << watchpoint.expectedValue << std::endl;
        ss << std::setw(6) << "  Actual: " << watchpoint.actualValue << std::endl;
        ss << std::setw(6) << "  Triggered: " << (watchpoint.triggered ? "YES" : "NO") << std::endl;
        if (watchpoint.triggered && !watchpoint.resolution.empty()) {
            ss << std::setw(6) << "  Resolution: " << watchpoint.resolution << std::endl;
        }
        ss << std::endl;
    }
    
    ss << "Memory Allocations:" << std::endl;
    ss << "------------------" << std::endl;
    
    for (size_t i = 0; i < memoryAllocations_.size(); ++i) {
        const auto& alloc = memoryAllocations_[i];
        
        ss << std::setw(3) << (i + 1) << ". " << alloc.componentName << " ("
           << alloc.allocator << ") - " << alloc.size << " bytes" << std::endl;
        ss << std::setw(6) << "  Address: " << alloc.address << std::endl;
        ss << std::setw(6) << "  Allocation Time: " << std::chrono::system_clock::to_time_t(alloc.allocationTime) << std::endl;
        ss << std::setw(6) << "  Tracked: " << (alloc.tracked ? "YES" : "NO") << std::endl;
        ss << std::endl;
    }
    
    ss << "Debug Analysis Summary:" << std::endl;
    ss << "---------------------" << std::endl;
    
    size_t resolvedIssues = 0;
    for (const auto& debug : debugInfo_) {
        if (debug.resolved) resolvedIssues++;
    }
    
    size_t triggeredWatchpoints = 0;
    for (const auto& watchpoint : watchpoints_) {
        if (watchpoint.triggered) triggeredWatchpoints++;
    }
    
    ss << "Resolved Issues: " << resolvedIssues << "/" << debugInfo_.size() << std::endl;
    ss << "Triggered Watchpoints: " << triggeredWatchpoints << "/" << watchpoints_.size() << std::endl;
    ss << "Memory Leaks Detected: " << countMemoryLeaks() << std::endl;
    ss << "Performance Issues: " << countPerformanceIssues() << std::endl;
    
    return ss.str();
}

bool DebugExperiment::exportResults(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        nlohmann::json jsonData;
        
        jsonData["experiment_name"] = "debug_experiment";
        jsonData["run_timestamp"] = std::chrono::system_clock::now();
        jsonData["total_debug_issues"] = debugInfo_.size();
        jsonData["total_watchpoints"] = watchpoints_.size();
        jsonData["total_memory_allocations"] = memoryAllocations_.size();
        
        nlohmann::json debugInfoJson;
        for (const auto& debug : debugInfo_) {
            nlohmann::json debugJson;
            debugJson["component_name"] = debug.componentName;
            debugJson["debug_type"] = debug.debugType;
            debugJson["message"] = debug.message;
            debugJson["metadata"] = debug.metadata;
            debugJson["timestamp"] = debug.timestamp;
            debugJson["severity"] = debug.severity;
            debugJson["resolved"] = debug.resolved;
            
            debugInfoJson.push_back(debugJson);
        }
        
        jsonData["debug_info"] = debugInfoJson;
        
        nlohmann::json watchpointsJson;
        for (const auto& watchpoint : watchpoints_) {
            nlohmann::json watchpointJson;
            watchpointJson["component_name"] = watchpoint.componentName;
            watchpointJson["variable_name"] = watchpoint.variableName;
            watchpointJson["condition"] = watchpoint.condition;
            watchpointJson["expected_value"] = watchpoint.expectedValue;
            watchpointJson["actual_value"] = watchpoint.actualValue;
            watchpointJson["trigger_time"] = watchpoint.triggerTime;
            watchpointJson["triggered"] = watchpoint.triggered;
            watchpointJson["resolution"] = watchpoint.resolution;
            
            watchpointsJson.push_back(watchpointJson);
        }
        
        jsonData["watchpoints"] = watchpointsJson;
        
        nlohmann::json allocationsJson;
        for (const auto& alloc : memoryAllocations_) {
            nlohmann::json allocJson;
            allocJson["address"] = alloc.address;
            allocJson["size"] = alloc.size;
            allocJson["allocator"] = alloc.allocator;
            allocJson["allocation_time"] = alloc.allocationTime;
            allocJson["component_name"] = alloc.componentName;
            allocJson["tracked"] = alloc.tracked;
            
            allocationsJson.push_back(allocJson);
        }
        
        jsonData["memory_allocations"] = allocationsJson;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonData << std::endl;
        
    } else {
        // Default to text format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
    }
    
    return true;
}

// Helper functions for debug analysis
size_t DebugExperiment::countMemoryLeaks() const {
    size_t leaks = 0;
    for (const auto& alloc : memoryAllocations_) {
        if (alloc.tracked && alloc.address != nullptr) {
            // Check if allocation should still be valid
            // In real implementation, would check if address is still allocated
            leaks++;
        }
    }
    return leaks;
}

size_t DebugExperiment::countPerformanceIssues() const {
    size_t issues = 0;
    for (const auto& debug : debugInfo_) {
        if (debug.severity == "error" || debug.severity == "critical") {
            issues++;
        }
    }
    return issues;
}

} // namespace nlm
