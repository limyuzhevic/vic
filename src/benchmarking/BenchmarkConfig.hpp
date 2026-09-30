// NLM Benchmark Configuration - Header
// BenchmarkConfig.hpp

#pragma once

#include <string>
#include <map>
#include <vector>

namespace nlm {
namespace benchmarking {

// Performance threshold configuration
struct PerformanceThreshold {
    std::string metric;           // Metric name (e.g., "steps_per_second", "memory_usage")
    double threshold;             // Threshold value
    std::string comparison;       // "<", "<=", ">", ">=", "==", "!="
    std::string action;           // "warning", "error", "fail"
    std::string message;          // Warning/error message
    
    PerformanceThreshold() : threshold(0.0) {}
    PerformanceThreshold(const std::string& metric, double threshold, 
                         const std::string& comparison = ">",
                         const std::string& action = "warning",
                         const std::string& message = "")
        : metric(metric), threshold(threshold), comparison(comparison),
          action(action), message(message) {}
};

// Benchmark configuration
struct BenchmarkConfig {
    // Basic configuration
    std::string name;              // Benchmark name
    std::string mode;              // Benchmark type (standard, memory, plasticity, etc.)
    size_t neuronCount;            // Number of neurons
    size_t steps;                  // Number of simulation steps
    size_t regions;                // Number of brain regions
    double durationLimit;          // Maximum duration in seconds (0 = unlimited)
    
    // Monitoring configuration
    bool enableMemoryTracking;     // Enable memory usage tracking
    bool enableCPUMonitoring;      // Enable CPU monitoring
    bool enableRealTimeReporting;  // Enable real-time progress reporting
    bool enableNeuralMonitoring;   // Enable neural activity monitoring
    size_t monitoringInterval;     // Monitoring interval in milliseconds
    
    // Output configuration
    std::string outputFormat;      // Output format: console, json, csv, html
    std::string outputFile;        // Output file path
    bool verboseOutput;            // Enable verbose output
    bool quietOutput;              // Suppress non-essential output
    
    // Analysis configuration
    bool enableRegressionDetection; // Enable performance regression detection
    bool enableComparisonMode;     // Enable comparison with saved benchmarks
    std::string comparisonBaseline; // Baseline benchmark file for comparison
    std::vector<PerformanceThreshold> thresholds; // Performance thresholds
    
    // Control configuration
    bool allowPause;               // Allow pausing benchmarks
    bool allowCancel;              // Allow cancelling benchmarks
    bool allowResume;              // Allow resuming benchmarks
    
    // Customization
    std::map<std::string, double> customParameters; // Custom benchmark parameters
    std::vector<std::string> customBenchmarkSuites;  // Custom benchmark suite names
    
    BenchmarkConfig() : neuronCount(500), steps(1000), regions(1),
                       durationLimit(0.0), enableMemoryTracking(true),
                       enableCPUMonitoring(true), enableRealTimeReporting(true),
                       enableNeuralMonitoring(true), monitoringInterval(100),
                       outputFormat("console"), verboseOutput(false), quietOutput(false),
                       enableRegressionDetection(true), enableComparisonMode(false),
                       allowPause(true), allowCancel(true), allowResume(true) {}
    
    // Helper methods
    bool hasOutputFormat(const std::string& format) const {
        return outputFormat == format;
    }
    
    bool isMetricMonitored(const std::string& metric) const {
        // Check if metric should be monitored based on configuration
        return true; // Simplified: monitor all metrics
    }
    
    bool meetsThreshold(const std::string& metric, double value) const {
        for (const auto& threshold : thresholds) {
            if (threshold.metric == metric) {
                bool passes = false;
                if (threshold.comparison == "<") passes = value < threshold.threshold;
                else if (threshold.comparison == "<=") passes = value <= threshold.threshold;
                else if (threshold.comparison == ">") passes = value > threshold.threshold;
                else if (threshold.comparison == ">=") passes = value >= threshold.threshold;
                else if (threshold.comparison == "==") passes = std::abs(value - threshold.threshold) < 1e-6;
                else if (threshold.comparison == "!=") passes = std::abs(value - threshold.threshold) >= 1e-6;
                
                if (!passes) {
                    return false;
                }
            }
        }
        return true; // All thresholds passed (or no thresholds for this metric)
    }
    
    std::string getBenchmarkTypeString() const {
        static const std::map<std::string, std::string> typeMap = {
            {"standard", "Standard Performance Benchmark"},
            {"memory", "Memory Usage Benchmark"},
            {"plasticity", "Plasticity Learning Benchmark"},
            {"spike", "Spike Generation Benchmark"},
            {"integration", "System Integration Benchmark"},
            {"scalability", "Scalability Benchmark"}
        };
        
        auto it = typeMap.find(mode);
        return (it != typeMap.end()) ? it->second : "Unknown Benchmark";
    }
};

}}} // namespace nlm::benchmarking
