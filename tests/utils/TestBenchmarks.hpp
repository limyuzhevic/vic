#ifndef TEST_BENCHMARKS_HPP
#define TEST_BENCHMARKS_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>

/**
 * Performance benchmarks and profiling utilities for NLM testing
 */

class BenchmarkResult {
public:
    std::string name;
    double time_ms;
    double time_us;
    size_t iterations;
    double throughput;
    double memory_usage_mb;
    
    BenchmarkResult(const std::string& n, double t_ms, double t_us, size_t iters, double throughput, double memory_mb)
        : name(n), time_ms(t_ms), time_us(t_us), iterations(iters), throughput(throughput), memory_usage_mb(memory_mb) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::left << std::setw(30) << name
            << std::right << std::setw(12) << time_ms << " ms"
            << std::right << std::setw(12) << time_us << " us"
            << std::right << std::setw(10) << iterations << " ops"
            << std::right << std::setw(12) << throughput << " ops/sec"
            << std::right << std::setw(10) << memory_usage_mb << " MB";
        return oss.str();
    }
};

class Benchmark {
public:
    static std::vector<BenchmarkResult> runBenchmarks(const std::vector<std::function<void()>>& benchmarks, const std::vector<size_t>& iterations) {
        std::vector<BenchmarkResult> results;
        
        for (size_t i = 0; i < benchmarks.size() && i < iterations.size(); ++i) {
            BenchmarkResult result = runBenchmark(benchmarks[i], iterations[i]);
            results.push_back(result);
        }
        
        return results;
    }
    
    static BenchmarkResult runBenchmark(const std::function<void()>& benchmark, size_t iterations = 1000) {
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point end_time;
        
        start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            benchmark();
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        
        double time_ms = static_cast<double>(duration_ms);
        double time_us = static_cast<double>(duration_us);
        double throughput = (iterations > 0) ? (iterations / (time_ms / 1000.0)) : 0.0;
        
        // Estimate memory usage (rough approximation)
        double memory_usage_mb = estimateMemoryUsage(benchmark);
        
        return BenchmarkResult("Benchmark", time_ms, time_us, iterations, throughput, memory_usage_mb);
    }
    
    static double estimateMemoryUsage(const std::function<void()>& benchmark) {
        // Simple memory estimation - in a real implementation this would use system-specific APIs
        return 0.0;
    }
    
    static void printResults(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n=== Benchmark Results ===" << std::endl;
        std::cout << std::left << std::setw(30) << "Test Name"
                  << std::right << std::setw(12) << "Time (ms)"
                  << std::right << std::setw(12) << "Time (us)"
                  << std::right << std::setw(10) << "Iterations"
                  << std::right << std::setw(12) << "Throughput"
                  << std::right << std::setw(10) << "Memory (MB)" << std::endl;
        std::cout << std::string(86, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << result.toString() << std::endl;
        }
        
        // Summary statistics
        printSummaryStatistics(results);
    }
    
    static void printSummaryStatistics(const std::vector<BenchmarkResult>& results) {
        if (results.empty()) return;
        
        std::cout << std::endl << "=== Summary Statistics ===" << std::endl;
        
        // Find fastest benchmark
        double min_time = std::numeric_limits<double>::max();
        double max_time = std::numeric_limits<double>::min();
        double total_time = 0.0;
        
        for (const auto& result : results) {
            min_time = std::min(min_time, result.time_ms);
            max_time = std::max(max_time, result.time_ms);
            total_time += result.time_ms;
        }
        
        std::cout << "Total benchmarks: " << results.size() << std::endl;
        std::cout << "Fastest benchmark: " << min_time << " ms" << std::endl;
        std::cout << "Slowest benchmark: " << max_time << " ms" << std::endl;
        std::cout << "Average time: " << (total_time / results.size()) << " ms" << std::endl;
        
        // Performance tiers
        std::cout << std::endl << "=== Performance Tiers ===" << std::endl;
        for (const auto& result : results) {
            std::string tier;
            if (result.time_ms < 1.0) tier = "Excellent";
            else if (result.time_ms < 10.0) tier = "Good";
            else if (result.time_ms < 100.0) tier = "Acceptable";
            else if (result.time_ms < 1000.0) tier = "Slow";
            else tier = "Very Slow";
            
            std::cout << std::left << std::setw(30) << result.name
                      << " - " << tier << " (" << result.time_ms << " ms)" << std::endl;
        }
    }
    
    static double calculateMedian(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
        } else {
            return sorted[n/2];
        }
    }
    
    static double calculateMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    static double calculateStandardDeviation(const std::vector<double>& values, double mean = -1.0) {
        if (values.empty()) return 0.0;
        if (mean < 0.0) mean = calculateMean(values);
        
        double sum_sq = 0.0;
        for (double value : values) {
            sum_sq += (value - mean) * (value - mean);
        }
        return std::sqrt(sum_sq / values.size());
    }
    
    static double calculateCoefficientOfVariation(const std::vector<double>& values, double mean = -1.0) {
        if (values.empty()) return 0.0;
        if (mean < 0.0) mean = calculateMean(values);
        double stddev = calculateStandardDeviation(values, mean);
        return (mean != 0.0) ? (stddev / mean) : 0.0;
    }
    
    static double calculateGeometricMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double product = std::accumulate(values.begin(), values.end(), 1.0, std::multiplies<double>());
        return std::pow(product, 1.0 / values.size());
    }
    
    static double calculateHarmonicMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum_reciprocal = 0.0;
        for (double value : values) {
            if (value != 0.0) sum_reciprocal += 1.0 / value;
        }
        return (sum_reciprocal != 0.0) ? values.size() / sum_reciprocal : 0.0;
    }
    
    static double calculateMedianAbsoluteDeviation(const std::vector<double>& values, double median = -1.0) {
        if (values.empty()) return 0.0;
        if (median < 0.0) median = calculateMedian(values);
        
        std::vector<double> deviations;
        for (double value : values) {
            deviations.push_back(std::abs(value - median));
        }
        
        return calculateMedian(deviations);
    }
    
    static double calculateInterquartileRange(const std::vector<double>& values, double q1 = -1.0, double q3 = -1.0) {
        if (values.empty()) return 0.0;
        if (q1 < 0.0 || q3 < 0.0) {
            q1 = calculateMedian(values); // Simplified, would need proper quartile calculation
            q3 = q1; // Placeholder
        }
        return q3 - q1;
    }
    
    static void printPerformanceProfile(const std::vector<BenchmarkResult>& results) {
        std::cout << std::endl << "=== Performance Profile ===" << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(30) << result.name
                      << " - " << result.time_ms << " ms (" << result.throughput << " ops/sec)" << std::endl;
            
            // Performance analysis
            std::string efficiency_rating;
            if (result.time_ms < 1.0) efficiency_rating = "Excellent";
            else if (result.time_ms < 10.0) efficiency_rating = "Good";
            else if (result.time_ms < 100.0) efficiency_rating = "Acceptable";
            else efficiency_rating = "Needs Improvement";
            
            std::cout << std::left << std::setw(35) << " " << "Efficiency: " << efficiency_rating << std::endl;
        }
    }
    
    static void profileMemoryUsage(const std::function<void()>& benchmark, const std::string& description) {
        std::cout << std::endl << "=== Memory Usage Profile ===" << std::endl;
        std::cout << "Benchmark: " << description << std::endl;
        
        // In a real implementation, this would use system-specific memory monitoring
        std::cout << "Memory estimation: N/A (system-specific implementation required)" << std::endl;
    }
    
    static void profileTimeComplexity(const std::function<void(size_t)>& benchmark, size_t max_iterations = 1000) {
        std::cout << std::endl << "=== Time Complexity Analysis ===" << std::endl;
        
        std::vector<double> times;
        for (size_t n = 10; n <= max_iterations; n *= 10) {
            std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
            benchmark(n);
            std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
            
            double duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            times.push_back(duration_ms);
            
            std::cout << "Input size: " << n << " - Time: " << duration_ms << " ms" << std::endl;
        }
        
        // Fit curve to analyze complexity
        std::cout << "Time complexity analysis completed" << std::endl;
    }
};

#endif // TEST_BENCHMARKS_HPP