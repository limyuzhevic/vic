// Performance measurement utilities for NLM testing framework
// Provides comprehensive performance monitoring and metrics collection

#ifndef PERFORMANCE_MEASUREMENT_UTILITIES_H
#define PERFORMANCE_MEASUREMENT_UTILITIES_H

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace test {

// Performance measurement utilities for the NLM testing framework
struct PerformanceMetrics {
    double execution_time_ms;
    size_t memory_usage_kb;
    double throughput_ops_per_sec;
    double cpu_usage_percent;
    std::unordered_map<std::string, double> custom_metrics;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    
    PerformanceMetrics() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    void end() {
        end_time = std::chrono::high_resolution_clock::now();
        execution_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
    double getExecutionTimeMs() const { return execution_time_ms; }
    double getThroughput(double ops) const { return ops / (execution_time_ms / 1000.0); }
    
    void setCustomMetric(const std::string& name, double value) {
        custom_metrics[name] = value;
    }
    
    double getCustomMetric(const std::string& name) const {
        auto it = custom_metrics.find(name);
        return (it != custom_metrics.end()) ? it->second : 0.0;
    }
    
    void printSummary(const std::string& test_name) const {
        std::cout << "Performance Summary for " << test_name << ":" << std::endl;
        std::cout << "  Execution Time: " << std::fixed << std::setprecision(2) 
                 << execution_time_ms << " ms" << std::endl;
        std::cout << "  Throughput: " << std::fixed << std::setprecision(2) 
                 << throughput_ops_per_sec << " ops/sec" << std::endl;
        std::cout << "  Memory Usage: " << memory_usage_kb << " KB" << std::endl;
        std::cout << "  CPU Usage: " << std::fixed << std::setprecision(1) 
                 << cpu_usage_percent << " %" << std::endl;
        
        if (!custom_metrics.empty()) {
            std::cout << "  Custom Metrics:" << std::endl;
            for (const auto& metric : custom_metrics) {
                std::cout << "    " << metric.first << ": " 
                         << std::fixed << std::setprecision(4) << metric.second << std::endl;
            }
        }
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"execution_time_ms\": " << execution_time_ms << ",\n";
        ss << "  \"memory_usage_kb\": " << memory_usage_kb << ",\n";
        ss << "  \"throughput_ops_per_sec\": " << throughput_ops_per_sec << ",\n";
        ss << "  \"cpu_usage_percent\": " << cpu_usage_percent << ",\n";
        
        if (!custom_metrics.empty()) {
            ss << "  \"custom_metrics\": {\n";
            bool first = true;
            for (const auto& metric : custom_metrics) {
                if (!first) ss << ",\n";
                ss << "    \"" << metric.first << "\": " << metric.second;
                first = false;
            }
            ss << "\n  }\n";
        }
        
        ss << "}";
        return ss.str();
    }
};

// Performance regression detection and analysis
class PerformanceRegressionDetector {
private:
    struct BenchmarkResult {
        std::string test_name;
        std::vector<double> metrics;
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    };
    
    std::vector<BenchmarkResult> baseline_results;
    std::vector<BenchmarkResult> current_results;
    double regression_threshold;
    bool auto_detect_regressions;
    
public:
    PerformanceRegressionDetector(double threshold = 0.10, bool auto_detect = true)
        : regression_threshold(threshold), auto_detect_regressions(auto_detect) {}
    
    void addBaseline(const std::string& test_name, const std::vector<double>& metrics) {
        BenchmarkResult result;
        result.test_name = test_name;
        result.metrics = metrics;
        result.timestamp = std::chrono::high_resolution_clock::now();
        baseline_results.push_back(result);
    }
    
    void addCurrent(const std::string& test_name, const std::vector<double>& metrics) {
        BenchmarkResult result;
        result.test_name = test_name;
        result.metrics = metrics;
        result.timestamp = std::chrono::high_resolution_clock::now();
        current_results.push_back(result);
    }
    
    bool hasRegression(const std::string& test_name) const {
        auto baseline_it = std::find_if(baseline_results.begin(), baseline_results.end(),
                                       [&test_name](const BenchmarkResult& r) { return r.test_name == test_name; });
        auto current_it = std::find_if(current_results.begin(), current_results.end(),
                                      [&test_name](const BenchmarkResult& r) { return r.test_name == test_name; });
        
        if (baseline_it == baseline_results.end() || current_it == current_results.end()) {
            return false;
        }
        
        const auto& baseline = baseline_it->metrics;
        const auto& current = current_it->metrics;
        
        if (baseline.size() != current.size()) {
            return false;
        }
        
        for (size_t i = 0; i < baseline.size(); ++i) {
            if (baseline[i] == 0.0) continue; // Skip zero division
            
            double regression = (current[i] - baseline[i]) / baseline[i];
            if (regression > regression_threshold) {
                return true;
            }
        }
        
        return false;
    }
    
    std::vector<std::string> getRegressionTests() const {
        std::vector<std::string> regressions;
        for (const auto& result : current_results) {
            if (hasRegression(result.test_name)) {
                regressions.push_back(result.test_name);
            }
        }
        return regressions;
    }
    
    void printRegressionReport() const {
        std::cout << "=== Performance Regression Analysis ===" << std::endl;
        std::cout << "Baseline Results: " << baseline_results.size() << std::endl;
        std::cout << "Current Results: " << current_results.size() << std::endl;
        
        auto regressions = getRegressionTests();
        if (regressions.empty()) {
            std::cout << "No regressions detected." << std::endl;
        } else {
            std::cout << "Regressions detected in:" << std::endl;
            for (const auto& test_name : regressions) {
                std::cout << "  - " << test_name << std::endl;
                
                auto baseline_it = std::find_if(baseline_results.begin(), baseline_results.end(),
                                               [&test_name](const BenchmarkResult& r) { return r.test_name == test_name; });
                auto current_it = std::find_if(current_results.begin(), current_results.end(),
                                              [&test_name](const BenchmarkResult& r) { return r.test_name == test_name; });
                
                if (baseline_it != baseline_results.end() && current_it != current_results.end()) {
                    std::cout << "    Baseline -> Current:" << std::endl;
                    const auto& baseline = baseline_it->metrics;
                    const auto& current = current_it->metrics;
                    
                    for (size_t i = 0; i < baseline.size(); ++i) {
                        double regression = (current[i] - baseline[i]) / baseline[i] * 100;
                        std::cout << "      Metric " << i << ": " << baseline[i] << " -> " 
                                 << current[i] << " (" << regression << "%)" << std::endl;
                    }
                }
            }
        }
    }
    
    void clear() {
        baseline_results.clear();
        current_results.clear();
    }
    
    size_t getBaselineCount() const { return baseline_results.size(); }
    size_t getCurrentCount() const { return current_results.size(); }
};

// Performance benchmark result aggregation
struct BenchmarkResult {
    std::string test_name;
    std::string test_category;
    double execution_time_ms;
    size_t memory_usage_kb;
    double throughput_ops_per_sec;
    double cpu_usage_percent;
    std::unordered_map<std::string, double> custom_metrics;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    bool passed;
    std::string details;
    
    BenchmarkResult(const std::string& name, const std::string& category)
        : test_name(name), test_category(category), passed(false), timestamp(std::chrono::high_resolution_clock::now()) {}
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"test_name\": \"" << test_name << "\",\n";
        ss << "  \"test_category\": \"" << test_category << "\",\n";
        ss << "  \"execution_time_ms\": " << execution_time_ms << ",\n";
        ss << "  \"memory_usage_kb\": " << memory_usage_kb << ",\n";
        ss << "  \"throughput_ops_per_sec\": " << throughput_ops_per_sec << ",\n";
        ss << "  \"cpu_usage_percent\": " << cpu_usage_percent << ",\n";
        ss << "  \"passed\": " << (passed ? "true" : "false") << ",\n";
        ss << "  \"details\": \"" << details << "\",\n";
        
        if (!custom_metrics.empty()) {
            ss << "  \"custom_metrics\": {\n";
            bool first = true;
            for (const auto& metric : custom_metrics) {
                if (!first) ss << ",\n";
                ss << "    \"" << metric.first << "\": " << metric.second;
                first = false;
            }
            ss << "\n  }\n";
        }
        
        ss << "}";
        return ss.str();
    }
    
    void printSummary() const {
        std::cout << "Benchmark: " << test_name << " (" << test_category << ")" << std::endl;
        std::cout << "  Status: " << (passed ? "PASSED" : "FAILED") << std::endl;
        std::cout << "  Execution Time: " << std::fixed << std::setprecision(2) 
                 << execution_time_ms << " ms" << std::endl;
        std::cout << "  Throughput: " << std::fixed << std::setprecision(2) 
                 << throughput_ops_per_sec << " ops/sec" << std::endl;
        std::cout << "  Memory Usage: " << memory_usage_kb << " KB" << std::endl;
        std::cout << "  CPU Usage: " << std::fixed << std::setprecision(1) 
                 << cpu_usage_percent << " %" << std::endl;
        std::cout << "  Details: " << details << std::endl;
    }
};

// Performance test result aggregator
class BenchmarkAggregator {
private:
    std::vector<BenchmarkResult> results;
    
public:
    void addResult(const BenchmarkResult& result) {
        results.push_back(result);
    }
    
    void generateReport() const {
        std::cout << "=== Benchmark Results Report ===" << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        
        int passed = 0, failed = 0;
        double total_time = 0.0;
        double avg_throughput = 0.0;
        size_t total_memory = 0;
        
        for (const auto& result : results) {
            if (result.passed) passed++; else failed++;
            total_time += result.execution_time_ms;
            avg_throughput += result.throughput_ops_per_sec;
            total_memory += result.memory_usage_kb;
        }
        
        if (!results.empty()) {
            avg_throughput /= results.size();
        }
        
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Average Execution Time: " << std::fixed << std::setprecision(2) 
                 << (total_time / results.size()) << " ms" << std::endl;
        std::cout << "Average Throughput: " << std::fixed << std::setprecision(2) 
                 << avg_throughput << " ops/sec" << std::endl;
        std::cout << "Average Memory Usage: " << total_memory / results.size() << " KB" << std::endl;
        std::cout << std::endl;
        
        // Print detailed results
        std::cout << "=== Detailed Results ===" << std::endl;
        for (const auto& result : results) {
            result.printSummary();
            std::cout << std::endl;
        }
    }
    
    void exportResults(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
        
        file << "[Benchmark Results]" << std::endl;
        file << "Total Tests: " << results.size() << std::endl;
        file << "Passed: " << std::count_if(results.begin(), results.end(),
                          [](const BenchmarkResult& r) { return r.passed; }) << std::endl;
        file << "Failed: " << std::count_if(results.begin(), results.end(),
                         [](const BenchmarkResult& r) { return !r.passed; }) << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << "Test: " << result.test_name << std::endl;
            file << "Category: " << result.test_category << std::endl;
            file << "Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
            file << "Execution Time: " << result.execution_time_ms << " ms" << std::endl;
            file << "Throughput: " << result.throughput_ops_per_sec << " ops/sec" << std::endl;
            file << "Memory Usage: " << result.memory_usage_kb << " KB" << std::endl;
            file << "Details: " << result.details << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
}; 

} // namespace test
} // namespace nlm

#endif // PERFORMANCE_MEASUREMENT_UTILITIES_H
