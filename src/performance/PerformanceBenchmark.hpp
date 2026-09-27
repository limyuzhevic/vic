#pragma once

/**
 * Performance Benchmark
 * 
 * Benchmarking framework for the NLM brain simulation project.
 * 
 * Features:
 * - Performance benchmarking
 * * Performance comparison
 * - Benchmark result tracking
 * - Performance benchmarking recommendations
 * - Performance benchmark visualization
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class PerformanceBenchmark {
public:
    /**
     * Benchmark test case
     */
    struct BenchmarkTestCase {
        std::string test_name;
        std::string test_description;
        std::function<void()> test_function;
        double execution_time; // seconds
        std::chrono::high_resolution_clock::time_point timestamp;
        bool passed;
        std::string failure_reason;
    };

    /**
     * Benchmark result
     */
    struct BenchmarkResult {
        std::string benchmark_name;
        double execution_time; // seconds
        double cpu_usage; // percentage (0-100)
        double memory_usage; // MB
        size_t allocations_count;
        size_t deallocations_count;
        bool passed; // true if execution time < threshold
    };

    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    /** Add benchmark test case */
    void addBenchmarkTestCase(const std::string& test_name,
                             const std::string& test_description,
                             const std::function<void()>& test_function);
    
    /** Run benchmark test */
    BenchmarkResult runBenchmarkTest(const std::string& test_name,
                                     double time_threshold = 1.0);
    
    /** Run all benchmark tests */
    std::vector<BenchmarkResult> runAllBenchmarkTests(double time_threshold = 1.0);
    
    /** Get benchmark results */
    const std::vector<BenchmarkResult>& getBenchmarkResults() const { return benchmark_results_; }
    
    /** Get benchmark test cases */
    const std::vector<BenchmarkTestCase>& getBenchmarkTestCases() const { return test_cases_; }
    
    /** Get benchmark summary */
    struct BenchmarkSummary {
        double total_execution_time;
        double average_execution_time;
        double total_cpu_usage;
        double average_cpu_usage;
        double total_memory_usage;
        double average_memory_usage;
        int passed_tests;
        int failed_tests;
        double overall_success_rate;
    };
    
    BenchmarkSummary getBenchmarkSummary() const;
    
    /** Generate benchmark recommendations */
    std::vector<std::string> generateBenchmarkRecommendations() const;
    
    /** Clear benchmark results */
    void clear();
    
    /** Enable/disable benchmarking */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if benchmarking is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    std::vector<BenchmarkTestCase> test_cases_;
    std::vector<BenchmarkResult> benchmark_results_;
    bool enabled_;
    double time_threshold_;
};

} // namespace nlm
