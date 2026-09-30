// Comprehensive benchmark framework for NLM performance benchmarking
// Includes benchmarking templates, performance reporting, and validation systems

#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <memory>
#include <fstream>
#include <mutex>

namespace nlm {

/**
 * Performance metrics collection
 */
struct BenchmarkMetrics {
    double avg_time_ms;
    double ops_per_second;
    double memory_usage_mb;
    double cpu_time_ms;
    double wall_time_ms;
    size_t iterations;
    std::string scalability_trend;
    std::vector<std::string> optimization_suggestions;
    
    BenchmarkMetrics() : avg_time_ms(0.0), ops_per_second(0.0), 
                        memory_usage_mb(0.0), cpu_time_ms(0.0),
                        wall_time_ms(0.0), iterations(0) {}
};

/**
 * Performance report structure
 */
struct PerformanceReport {
    std::string benchmark_name;
    double avg_time_ms;
    double ops_per_second;
    double memory_usage_mb;
    std::string scalability_trend;
    std::string optimization_suggestions;
    std::vector<double> iteration_times;
    double std_deviation;
    double min_time;
    double max_time;
    
    PerformanceReport() : avg_time_ms(0.0), ops_per_second(0.0),
                         memory_usage_mb(0.0), std_deviation(0.0),
                         min_time(0.0), max_time(0.0) {}
};

/**
 * Benchmark framework for comprehensive performance testing
 */
class Benchmark {
public:
    Benchmark(const std::string& name) : name_(name) {}
    
    ~Benchmark() {
        printResults();
    }
    
    template<typename Func>
    void run(const std::string& test_name, Func&& func, int iterations = 1000) {
        std::vector<double> times;
        
        // Warm-up
        for (int i = 0; i < std::min(10, iterations); ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            times.push_back(duration.count() / 1e6); // Convert to milliseconds
        }
        
        // Actual benchmarking
        auto total_start = std::chrono::high_resolution_clock::now();
        auto cpu_start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            times.push_back(duration.count() / 1e6); // Convert to milliseconds
        }
        
        auto total_end = std::chrono::high_resolution_clock::now();
        auto cpu_end = std::chrono::high_resolution_clock::now();
        
        auto wall_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(total_end - total_start);
        auto cpu_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_end - cpu_start);
        
        // Calculate statistics
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double mean = sum / times.size();
        
        double sq_sum = 0.0;
        for (double t : times) {
            sq_sum += (t - mean) * (t - mean);
        }
        double std_dev = std::sqrt(sq_sum / times.size());
        
        double min_time = *std::min_element(times.begin(), times.end());
        double max_time = *std::max_element(times.begin(), times.end());
        
        double avg_time_ms = mean;
        double wall_time_ms = wall_duration.count() / 1e6;
        double ops_per_second = iterations * 1e9 / wall_duration.count();
        
        // Record results
        PerformanceReport report;
        report.benchmark_name = test_name;
        report.avg_time_ms = avg_time_ms;
        report.ops_per_second = ops_per_second;
        report.wall_time_ms = wall_time_ms;
        report.cpu_time_ms = cpu_duration.count() / 1e6;
        report.memory_usage_mb = estimate_memory_usage();
        report.iteration_times = times;
        report.std_deviation = std_dev;
        report.min_time = min_time;
        report.max_time = max_time;
        report.scalability_trend = analyze_scalability(times, iterations);
        report.optimization_suggestions = identify_optimization_opportunities(times);
        
        reports_.push_back(report);
        
        // Print individual test results
        std::cout << "[BENCHMARK] " << test_name << ": " 
                  << std::fixed << std::setprecision(3) 
                  << avg_time_ms << " ms avg, "
                  << ops_per_second << " ops/sec, "
                  << iterations << " iterations" << std::endl;
    }
    
    void printResults() const {
        std::cout << "\n=== Benchmark Results for " << name_ << " ===" << std::endl;
        
        for (const auto& report : reports_) {
            std::cout << std::left << std::setw(30) << report.benchmark_name << ": "
                      << std::fixed << std::setprecision(3)
                      << report.avg_time_ms << " ms avg, "
                      << report.ops_per_second << " ops/sec, "
                      << report.std_deviation << " ms stddev, "
                      << report.memory_usage_mb << " MB memory" << std::endl;
        }
    }
    
    std::vector<PerformanceReport> getReports() const { return reports_; }
    
    void saveResults(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        file << "Benchmark Results for " << name_ << "\n\n";
        
        for (const auto& report : reports_) {
            file << "Test: " << report.benchmark_name << "\n";
            file << "  Average Time (ms): " << report.avg_time_ms << "\n";
            file << "  Operations per Second: " << report.ops_per_second << "\n";
            file << "  Memory Usage (MB): " << report.memory_usage_mb << "\n";
            file << "  Wall Time (ms): " << report.wall_time_ms << "\n";
            file << "  CPU Time (ms): " << report.cpu_time_ms << "\n";
            file << "  Standard Deviation: " << report.std_deviation << "\n";
            file << "  Min Time (ms): " << report.min_time << "\n";
            file << "  Max Time (ms): " << report.max_time << "\n";
            file << "  Scalability Trend: " << report.scalability_trend << "\n";
            file << "  Optimization Suggestions: " << report.optimization_suggestions << "\n\n";
        }
        
        file.close();
        std::cout << "Benchmark results saved to: " << filename << std::endl;
    }
    
private:
    std::string name_;
    std::vector<PerformanceReport> reports_;
    
    double estimate_memory_usage() {
        // Placeholder for memory usage estimation
        return 0.0;
    }
    
    std::string analyze_scalability(const std::vector<double>& times, int iterations) {
        if (times.size() < 10) return "insufficient_data";
        
        // Simple linear regression for trend analysis
        double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
        int n = times.size() - 10; // Exclude warm-up
        
        for (int i = 10; i < times.size(); ++i) {
            double x = i - 10;
            double y = times[i];
            sum_x += x;
            sum_y += y;
            sum_xy += x * y;
            sum_x2 += x * x;
        }
        
        double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
        
        if (slope < -0.1) {
            return "improving";
        } else if (slope > 0.1) {
            return "degrading";
        } else {
            return "stable";
        }
    }
    
    std::string identify_optimization_opportunities(const std::vector<double>& times) {
        if (times.empty()) return "no_data";
        
        std::vector<std::string> suggestions;
        
        double max_time = *std::max_element(times.begin(), times.end());
        double min_time = *std::min_element(times.begin(), times.end());
        double variance = max_time - min_time;
        
        if (variance > min_time * 0.5) {
            suggestions.push_back("high_variance_detected_consider_cache_blocking");
        }
        
        // Check for warm-up behavior
        if (times.front() > times.back() * 2) {
            suggestions.push_back("warm_up_effect_consider_initialization");
        }
        
        if (suggestions.empty()) {
            return "none_detected";
        }
        
        std::stringstream ss;
        for (size_t i = 0; i < suggestions.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << suggestions[i];
        }
        
        return ss.str();
    }
};

/**
 * Performance validation system for comparing baseline vs current
 */
class PerformanceValidator {
public:
    static bool validate_performance_improvement(
        const std::string& baseline_file,
        const std::string& current_file,
        double improvement_threshold = 0.05)  // 5% improvement
    {
        std::vector<PerformanceReport> baseline_reports = load_performance_reports(baseline_file);
        std::vector<PerformanceReport> current_reports = load_performance_reports(current_file);
        
        if (baseline_reports.size() != current_reports.size()) {
            std::cerr << "Error: Mismatched number of benchmark runs" << std::endl;
            return false;
        }
        
        bool all_improved = true;
        
        for (size_t i = 0; i < baseline_reports.size(); ++i) {
            const auto& baseline = baseline_reports[i];
            const auto& current = current_reports[i];
            
            double improvement_ratio = (baseline.avg_time_ms - current.avg_time_ms) / baseline.avg_time_ms;
            
            if (improvement_ratio < improvement_threshold) {
                std::cout << "Warning: " << current.benchmark_name 
                          << " improvement below threshold: " << (improvement_ratio * 100) << "%" << std::endl;
                all_improved = false;
            }
            
            std::cout << current.benchmark_name << ": "
                      << std::fixed << std::setprecision(2)
                      << (improvement_ratio * 100) << "% improvement" << std::endl;
        }
        
        return all_improved;
    }
    
    static PerformanceReport run_performance_validation(
        const std::string& benchmark_name,
        std::function<void()> benchmark_function,
        const std::string& baseline_file)
    {
        Benchmark benchmark("validation_" + benchmark_name);
        benchmark.run(benchmark_name, benchmark_function, 1000);
        
        auto reports = benchmark.getReports();
        if (!reports.empty()) {
            return reports[0];
        }
        
        return PerformanceReport();
    }
    
private:
    static std::vector<PerformanceReport> load_performance_reports(const std::string& filename) {
        std::vector<PerformanceReport> reports;
        // Implementation would load reports from file
        // For now, return empty vector
        return reports;
    }
};

/**
 * Performance comparison system
 */
class PerformanceComparator {
public:
    static std::string compare_performance(
        const PerformanceReport& report1,
        const PerformanceReport& report2,
        const std::string& test_name1 = "baseline",
        const std::string& test_name2 = "optimized")
    {
        std::stringstream ss;
        
        ss << "=== Performance Comparison: " << test_name1 << " vs " << test_name2 << " ===\n\n";
        
        ss << "Test Name           | " << std::left << std::setw(15) << test_name1 << " | "
           << std::setw(15) << test_name2 << " | Improvement\n";
        ss << "-------------------|---------------------|---------------------|-------------\n";
        
        // Time comparison
        double time_improvement = (report1.avg_time_ms - report2.avg_time_ms) / report1.avg_time_ms * 100;
        ss << std::left << std::setw(20) << "Average Time (ms)" << " | "
           << std::setw(15) << std::fixed << std::setprecision(3) << report1.avg_time_ms << " | "
           << std::setw(15) << std::fixed << std::setprecision(3) << report2.avg_time_ms << " | "
           << std::setw(10) << std::fixed << std::setprecision(2) << time_improvement << "%\n";
        
        // Operations per second comparison
        double ops_improvement = (report2.ops_per_second - report1.ops_per_second) / report1.ops_per_second * 100;
        ss << std::left << std::setw(20) << "Operations/sec" << " | "
           << std::setw(15) << std::fixed << std::setprecision(0) << report1.ops_per_second << " | "
           << std::setw(15) << std::fixed << std::setprecision(0) << report2.ops_per_second << " | "
           << std::setw(10) << std::fixed << std::setprecision(2) << ops_improvement << "%\n";
        
        // Memory usage comparison
        double memory_improvement = (report1.memory_usage_mb - report2.memory_usage_mb) / report1.memory_usage_mb * 100;
        ss << std::left << std::setw(20) << "Memory Usage (MB)" << " | "
           << std::setw(15) << std::fixed << std::setprecision(3) << report1.memory_usage_mb << " | "
           << std::setw(15) << std::fixed << std::setprecision(3) << report2.memory_usage_mb << " | "
           << std::setw(10) << std::fixed << std::setprecision(2) << memory_improvement << "%\n";
        
        // Determine overall recommendation
        std::string recommendation;
        if (time_improvement > 10 && ops_improvement > 5) {
            recommendation = "significant_improvement";
        } else if (time_improvement > 5) {
            recommendation = "moderate_improvement";
        } else {
            recommendation = "insufficient_improvement";
        }
        
        ss << "\nOverall Recommendation: " << recommendation << "\n";
        
        return ss.str();
    }
};

} // namespace nlm