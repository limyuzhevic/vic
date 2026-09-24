// Performance Test Framework
// NLM Phase 5: Performance Regression Testing

#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>

// Type definitions
using BenchmarkTime = std::chrono::high_resolution_clock::time_point;
using BenchmarkDuration = std::chrono::duration<double>;
using MetricValue = double;
using MetricName = std::string;

// Benchmark result structure
struct BenchmarkResult {
    std::string testName;
    std::string component;
    size_t scaleFactor;  // Population size scaling factor
    
    // Timing metrics
    double wallTime;      // Wall clock time in seconds
    double cpuTime;       // CPU time in seconds
    double stepsPerSecond;
    
    // Resource metrics
    size_t memoryUsage;   // Peak memory usage in bytes
    double cpuCycles;     // CPU cycles if measurable
    
    // Operation metrics
    size_t operations;    // Number of operations performed
    double opsPerSecond;  // Operations per second
    
    // Test metadata
    int64_t timestamp;    // Unix timestamp
    std::string gitHash;  // Git commit hash
    
    BenchmarkResult() : scaleFactor(1), wallTime(0.0), cpuTime(0.0),
                       stepsPerSecond(0.0), memoryUsage(0), cpuCycles(0.0),
                       operations(0), opsPerSecond(0.0), timestamp(0) {}
};

// Performance threshold structure
struct PerformanceThreshold {
    std::string metricName;
    double threshold;     // Acceptable threshold (lower is better for most metrics)
    std::string regressionType;  // "allowable", "strict", "warning"
    bool enabled;         // Whether this threshold is active
};

// Scale configuration for testing
scale_t {
    Small = 1,     // Baseline small population
    Medium = 10,    // Medium population size
    Large = 100,    // Large population size
    ExtraLarge = 1000  // Extra large population size
};

// Performance test framework
class PerformanceTestFramework {
private:
    std::string testName;
    std::string component;
    std::vector<BenchmarkResult> results;
    std::vector<PerformanceThreshold> thresholds;
    std::ofstream resultsFile;
    bool fileOpen;
    
public:
    PerformanceTestFramework(const std::string& name, const std::string& comp)
        : testName(name), component(comp), fileOpen(false) {
        // Open results file for appending
        resultsFile.open("performance_results.csv", std::ios::out | std::ios::app);
        if (resultsFile.is_open()) {
            fileOpen = true;
            // Write header if file is empty
            resultsFile.seekp(0, std::ios::end);
            if (resultsFile.tellp() == 0) {
                resultsFile << "timestamp,component,test_name,scale_factor,wall_time,cpu_time,steps_per_second,memory_bytes,cpu_cycles,operations,ops_per_second,git_hash" << std::endl;
            }
        }
    }
    
    ~PerformanceTestFramework() {
        if (fileOpen) {
            resultsFile.close();
        }
        saveResults();
    }
    
    // Start timing
    BenchmarkTime startTiming() {
        return std::chrono::high_resolution_clock::now();
    }
    
    // Get current time
    BenchmarkTime getCurrentTime() {
        return std::chrono::high_resolution_clock::now();
    }
    
    // Calculate elapsed time
    double getElapsedTime(const BenchmarkTime& start, const BenchmarkTime& end) {
        return std::chrono::duration<double>(end - start).count();
    }
    
    // Record a benchmark result
    void recordResult(const BenchmarkResult& result) {
        results.push_back(result);
        
        // Write to CSV if file is open
        if (fileOpen) {
            resultsFile << result.timestamp << ","
                       << result.component << ","
                       << result.testName << ","
                       << result.scaleFactor << ","
                       << std::setprecision(6) << result.wallTime << ","
                       << std::setprecision(6) << result.cpuTime << ","
                       << std::setprecision(6) << result.stepsPerSecond << ","
                       << result.memoryUsage << ","
                       << std::setprecision(6) << result.cpuCycles << ","
                       << result.operations << ","
                       << std::setprecision(6) << result.opsPerSecond << ","
                       << result.gitHash << std::endl;
        }
    }
    
    // Add a performance threshold
    void addThreshold(const std::string& metric, double threshold, 
                     const std::string& type = "allowable", bool enabled = true) {
        PerformanceThreshold t;
        t.metricName = metric;
        t.threshold = threshold;
        t.regressionType = type;
        t.enabled = enabled;
        thresholds.push_back(t);
    }
    
    // Check if a result meets thresholds
    bool checkThresholds(const BenchmarkResult& result) {
        bool passed = true;
        for (const auto& threshold : thresholds) {
            if (!threshold.enabled) continue;
            
            double actualValue = 0.0;
            if (threshold.metricName == "wall_time") actualValue = result.wallTime;
            else if (threshold.metricName == "steps_per_second") actualValue = result.stepsPerSecond;
            else if (threshold.metricName == "memory_usage") actualValue = static_cast<double>(result.memoryUsage);
            else if (threshold.metricName == "ops_per_second") actualValue = result.opsPerSecond;
            
            if (threshold.regressionType == "strict") {
                if (actualValue > threshold.threshold) {
                    std::cerr << "STRICT THRESHOLD VIOLATION: " << threshold.metricName 
                             << " = " << actualValue 
                             << " (threshold = " << threshold.threshold << ")" << std::endl;
                    passed = false;
                }
            } else if (threshold.regressionType == "allowable") {
                if (actualValue > threshold.threshold * 1.1) { // 10% tolerance
                    std::cerr << "ALLOWABLE THRESHOLD VIOLATION: " << threshold.metricName 
                             << " = " << actualValue 
                             << " (threshold = " << threshold.threshold << ")" << std::endl;
                    passed = false;
                }
            }
        }
        return passed;
    }
    
    // Save results to file
    void saveResults() {
        // Results are already saved to CSV during recording
        // This function can be used for additional serialization
        std::cout << "Performance test \"" << testName << "\" completed." << std::endl;
        std::cout << "  Component: " << component << std::endl;
        std::cout << "  Recorded " << results.size() << " benchmark results." << std::endl;
    }
    
    // Get all results
    const std::vector<BenchmarkResult>& getResults() const {
        return results;
    }
    
    // Generate scalability analysis
    void analyzeScalability() {
        std::cout << "=== Scalability Analysis ===" << std::endl;
        
        // Group results by scale factor
        std::map<size_t, std::vector<const BenchmarkResult*>> scaleGroups;
        for (const auto& result : results) {
            scaleGroups[result.scaleFactor].push_back(&result);
        }
        
        // Analyze performance vs scale
        for (const auto& [scale, groupResults] : scaleGroups) {
            if (groupResults.empty()) continue;
            
            double totalTime = 0.0;
            double totalOps = 0.0;
            
            for (const auto* result : groupResults) {
                totalTime += result->wallTime;
                totalOps += result->operations;
            }
            
            double avgTime = totalTime / groupResults.size();
            double avgOps = totalOps / groupResults.size();
            
            std::cout << "Scale Factor " << scale << ": " 
                     << "Avg Wall Time: " << std::scientific << avgTime << "s, "
                     << "Avg Ops/sec: " << std::scientific << avgOps << "/s" << std::endl;
        }
    }
    
    // Generate comparative analysis (with/without optimizations)
    void analyzeComparativePerformance() {
        std::cout << "=== Comparative Analysis ===" << std::endl;
        // This would compare optimized vs non-optimized versions
        // Implementation depends on how optimizations are marked
        std::cout << "Comparative analysis requires additional baseline measurements." << std::endl;
    }
};

// Global performance test instance
PerformanceTestFramework* globalPerformanceTest = nullptr;

// Helper function to get current git hash
std::string getGitHash() {
    // Implementation would run git command to get current commit hash
    return "unknown";
}

// Helper function to get current timestamp
int64_t getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return static_cast<int64_t>(seconds);
}