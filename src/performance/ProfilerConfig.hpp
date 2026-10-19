#pragma once

/**
 * Performance Profiler Configuration
 * 
 * Configuration options for the NLM performance profiling system.
 */

#include <chrono>
#include <string>
#include <unordered_map>

namespace nlm {
namespace profiling {

struct ProfilerConfig {
    // Basic configuration
    bool enabled = false;                    // Enable/disable profiling
    std::string outputFile = "performance_profile.json";  // Output file
    uint32_t sampleIntervalMs = 100;        // Sample interval in milliseconds
    uint32_t reportIntervalSeconds = 60;   // Report generation interval in seconds
    
    // Thresholds
    double memoryThresholdMB = 1024.0;      // Memory usage threshold (MB)
    double cpuThresholdPercent = 80.0;      // CPU usage threshold (%)
    double operationThresholdNs = 1000000;  // Operation duration threshold (1ms)
    double spikeRateThresholdPerSec = 100.0; // Spike rate threshold (spikes/sec)
    
    // Filtering
    std::string includeOperations;          // Comma-separated list of operations to include
    std::string excludeOperations;          // Comma-separated list of operations to exclude
    std::string includeContexts;            // Comma-separated list of contexts to include
    std::string excludeContexts;            // Comma-separated list of contexts to exclude
    std::vector<std::string> customTags;    // Custom tags to include
    
    // Output formatting
    bool outputTextReport = true;           // Generate text report
    bool outputCSVReport = true;            // Generate CSV report
    bool outputJSONReport = true;           // Generate JSON report
    bool consoleOutput = true;              // Output to console
    bool realTimeMonitoring = true;         // Enable real-time monitoring
    
    // Integration
    bool profileBrain = true;               // Profile brain operations
    bool profileMotor = true;               // Profile motor operations
    bool profileSensor = true;              // Profile sensor operations
    bool profileNeuromodulation = true;     // Profile neuromodulation operations
    bool profilePlasticity = true;          // Profile plasticity operations
    
    // Multi-threading
    bool threadLocalProfiling = true;       // Enable thread-local profiling
    bool globalAggregation = true;          // Enable global performance aggregation
    uint32_t maxThreads = 0;                // Maximum threads to profile (0 = auto-detect)
    
    // Analysis
    bool detectBottlenecks = true;         // Detect performance bottlenecks
    bool detectRegressions = true;         // Detect performance regressions
    bool comparativeProfiling = false;      // Enable comparative profiling
    std::string baselineFile;               // Baseline file for comparison
    
    // Alerting
    bool alertOnThreshold = true;           // Alert when thresholds are exceeded
    std::string alertLevel = "WARNING";     // Alert level (WARNING, ERROR, CRITICAL)
    std::string alertCallback;              // Alert callback function (if any)
    
    // Advanced
    bool enableMemoryTracking = true;      // Enable detailed memory tracking
    bool enableCPUTracking = true;          // Enable detailed CPU tracking
    bool enableIOTracking = false;          // Enable I/O tracking
    bool enableNetworkTracking = false;     // Enable network tracking
    bool compressedOutput = false;          // Compress output files
    uint32_t maxHistoryEntries = 10000;     // Maximum history entries to keep
    
    ProfilerConfig() {}
    
    // Merge with another config (child config overrides parent)
    void merge(const ProfilerConfig& other) {
        // For each field, if the other config has a non-default value, override
        if (other.enabled) enabled = other.enabled;
        if (!other.outputFile.empty()) outputFile = other.outputFile;
        if (other.sampleIntervalMs > 0) sampleIntervalMs = other.sampleIntervalMs;
        if (other.reportIntervalSeconds > 0) reportIntervalSeconds = other.reportIntervalSeconds;
        if (other.memoryThresholdMB > 0) memoryThresholdMB = other.memoryThresholdMB;
        if (other.cpuThresholdPercent > 0) cpuThresholdPercent = other.cpuThresholdPercent;
        if (other.operationThresholdNs > 0) operationThresholdNs = other.operationThresholdNs;
        if (other.spikeRateThresholdPerSec > 0) spikeRateThresholdPerSec = other.spikeRateThresholdPerSec;
        
        // Merge filters
        if (!other.includeOperations.empty()) includeOperations = other.includeOperations;
        if (!other.excludeOperations.empty()) excludeOperations = other.excludeOperations;
        if (!other.includeContexts.empty()) includeContexts = other.includeContexts;
        if (!other.excludeContexts.empty()) excludeContexts = other.excludeContexts;
        
        if (other.outputTextReport != outputTextReport) outputTextReport = other.outputTextReport;
        if (other.outputCSVReport != outputCSVReport) outputCSVReport = other.outputCSVReport;
        if (other.outputJSONReport != outputJSONReport) outputJSONReport = other.outputJSONReport;
        if (other.consoleOutput != consoleOutput) consoleOutput = other.consoleOutput;
        if (other.realTimeMonitoring != realTimeMonitoring) realTimeMonitoring = other.realTimeMonitoring;
        
        if (other.profileBrain != profileBrain) profileBrain = other.profileBrain;
        if (other.profileMotor != profileMotor) profileMotor = other.profileMotor;
        if (other.profileSensor != profileSensor) profileSensor = other.profileSensor;
        if (other.profileNeuromodulation != profileNeuromodulation) profileNeuromodulation = other.profileNeuromodulation;
        if (other.profilePlasticity != profilePlasticity) profilePlasticity = other.profilePlasticity;
        
        if (other.threadLocalProfiling != threadLocalProfiling) threadLocalProfiling = other.threadLocalProfiling;
        if (other.globalAggregation != globalAggregation) globalAggregation = other.globalAggregation;
        if (other.maxThreads > 0) maxThreads = other.maxThreads;
        
        if (other.detectBottlenecks != detectBottlenecks) detectBottlenecks = other.detectBottlenecks;
        if (other.detectRegressions != detectRegressions) detectRegressions = other.detectRegressions;
        if (!other.baselineFile.empty()) baselineFile = other.baselineFile;
        
        if (other.alertOnThreshold != alertOnThreshold) alertOnThreshold = other.alertOnThreshold;
        if (!other.alertLevel.empty()) alertLevel = other.alertLevel;
        if (!other.alertCallback.empty()) alertCallback = other.alertCallback;
        
        if (other.enableMemoryTracking != enableMemoryTracking) enableMemoryTracking = other.enableMemoryTracking;
        if (other.enableCPUTracking != enableCPUTracking) enableCPUTracking = other.enableCPUTracking;
        if (other.enableIOTracking != enableIOTracking) enableIOTracking = other.enableIOTracking;
        if (other.enableNetworkTracking != enableNetworkTracking) enableNetworkTracking = other.enableNetworkTracking;
        if (other.compressedOutput != compressedOutput) compressedOutput = other.compressedOutput;
        if (other.maxHistoryEntries > 0) maxHistoryEntries = other.maxHistoryEntries;
    }
    
    // Parse comma-separated string into vector
    static std::vector<std::string> parseCommaSeparated(const std::string& str) {
        std::vector<std::string> result;
        if (str.empty()) return result;
        
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, ',')) {
            // Trim whitespace
            size_t start = item.find_first_not_of(" \t\r\n");
            size_t end = item.find_last_not_of(" \t\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                item = item.substr(start, end - start + 1);
            }
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        
        return result;
    }
    
    // Check if a string should be included based on filters
    bool shouldIncludeOperation(const std::string& operation) const {
        if (includeOperations.empty()) return true;
        return isInFilter(operation, parseCommaSeparated(includeOperations));
    }
    
    bool shouldIncludeContext(const std::string& context) const {
        if (includeContexts.empty()) return true;
        return isInFilter(context, parseCommaSeparated(includeContexts));
    }
    
    bool shouldExclude(const std::string& operation, const std::string& context) const {
        if (!excludeOperations.empty()) {
            if (isInFilter(operation, parseCommaSeparated(excludeOperations))) {
                return true;
            }
        }
        if (!excludeContexts.empty()) {
            if (isInFilter(context, parseCommaSeparated(excludeContexts))) {
                return true;
            }
        }
        return false;
    }
    
private:
    bool isInFilter(const std::string& str, const std::vector<std::string>& filters) const {
        return std::find(filters.begin(), filters.end(), str) != filters.end();
    }
};

} // namespace profiling
} // namespace nlm
