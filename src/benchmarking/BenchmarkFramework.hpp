#pragma once

/**
 * Benchmarking Infrastructure
 * 
 * This module provides comprehensive benchmarking capabilities for NLM,
 * including system benchmarks, component benchmarks, integration benchmarks,
 * end-to-end benchmarks, performance comparison, and performance analysis.
 * 
 * Key Features:
 * - Benchmarking framework with configurable benchmark suites
 * - System benchmark for overall system performance
 * - Component benchmark for individual components
 * - Integration benchmark for system integration
 * - End-to-end benchmark for complete workflows
 * - Performance comparison between different configurations
 * - Performance analysis and optimization recommendations
 * 
 * Architecture:
 * - BenchmarkBase: Abstract base class for all benchmarks
 * - BenchmarkSuite: Container for multiple related benchmarks
 * - SystemBenchmark: Overall system performance benchmark
 * - ComponentBenchmark: Individual component performance benchmark
 * - IntegrationBenchmark: System integration benchmark
 * - EndToEndBenchmark: Complete workflow benchmark
 * - PerformanceComparison: Compare performance across configurations
 * - BenchmarkReporter: Report generation
 * - BenchmarkAnalyzer: Data analysis and optimization recommendations
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <filesystem>
#include <cassert>

namespace nlm {
namespace benchmarking {

// Benchmark data structures
struct BenchmarkConfig {
    std::string name;
    std::string description;
    std::string benchmark_type;  // system, component, integration, end_to_end
    double warmup_duration_seconds;
    int iterations;
    double timeout_seconds;
    double threshold_value;
    std::map<std::string, std::string> parameters;
    std::map<std::string, double> expected_values;
    bool enabled;
};

struct BenchmarkResult {
    std::string benchmark_name;
    std::string benchmark_type;
    uint64_t timestamp_ns;
    double duration_seconds;
    double cpu_time_seconds;
    double wall_time_seconds;
    size_t memory_used_bytes;
    size_t memory_peak_bytes;
    double memory_bandwidth_gb_s;
    uint64_t operations_per_second;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, size_t> counters;
    std::vector<double> iterations_results;
    bool passed;
    std::string failure_message;
    double confidence_interval;
};

struct BenchmarkSuite {
    std::string name;
    std::string description;
    std::vector<BenchmarkConfig> benchmarks;
    bool enabled;
    std::map<std::string, std::string> metadata;
};

struct PerformanceComparison {
    std::string baseline_name;
    std::string comparison_name;
    double baseline_score;
    double comparison_score;
    double improvement_percentage;
    double confidence_score;
    std::string recommendation;
};

struct OptimizationRecommendation {
    std::string issue_type;
    std::string component;
    double impact_score;
    std::string recommendation;
    double estimated_improvement;
    std::string implementation_difficulty;
    std::map<std::string, std::string> details;
};

// Abstract base class for all benchmarks
class BenchmarkBase {
protected:
    BenchmarkConfig config_;
    bool enabled_;
    bool running_;
    std::mutex benchmark_mutex_;
    
public:
    BenchmarkBase(const BenchmarkConfig& config) : config_(config), enabled_(config.enabled), running_(false) {}
    virtual ~BenchmarkBase() = default;
    
    virtual void setup() = 0;
    virtual BenchmarkResult run() = 0;
    virtual void teardown() = 0;
    virtual bool validate() = 0;
    
    void start() {
        std::lock_guard<std::mutex> lock(benchmark_mutex_);
        if (!enabled_) return;
        running_ = true;
        setup();
    }
    
    BenchmarkResult execute() {
        std::lock_guard<std::mutex> lock(benchmark_mutex_);
        if (!running_) return BenchmarkResult{};
        
        auto start_time = std::chrono::high_resolution_clock::now();
        BenchmarkResult result = run();
        auto end_time = std::chrono::high_resolution_clock::now();
        
        result.duration_seconds = 
            std::chrono::duration<double>(end_time - start_time).count();
        
        teardown();
        running_ = false;
        
        return result;
    }
    
    bool is_enabled() const { return enabled_; }
    bool is_running() const { return running_; }
    const std::string& get_name() const { return config_.name; }
    const std::string& get_type() const { return config_.benchmark_type; }
};

// System Benchmark implementation
class SystemBenchmark : public BenchmarkBase {
public:
    SystemBenchmark(const BenchmarkConfig& config) : BenchmarkBase(config) {}
    
    void setup() override {
        // System-specific setup (network, memory, etc.)
    }
    
    BenchmarkResult run() override {
        BenchmarkResult result;
        result.benchmark_name = config_.name;
        result.benchmark_type = config_.benchmark_type;
        result.timestamp_ns = getCurrentTimestamp();
        
        // System performance measurements
        result.cpu_time_seconds = measure_cpu_usage();
        result.wall_time_seconds = config_.iterations * config_.warmup_duration_seconds;
        result.memory_used_bytes = measure_memory_usage();
        result.memory_peak_bytes = measure_peak_memory_usage();
        result.memory_bandwidth_gb_s = measure_memory_bandwidth();
        result.operations_per_second = calculate_operations_per_second();
        
        // Calculate metrics based on iterations
        double total_time = result.cpu_time_seconds;
        if (total_time > 0) {
            result.metrics["cpu_time_per_operation"] = total_time / config_.iterations;
            result.metrics["memory_per_operation"] = 
                static_cast<double>(result.memory_peak_bytes) / config_.iterations;
        }
        
        result.passed = validate_result(result);
        
        return result;
    }
    
    void teardown() override {
        // Cleanup system resources
    }
    
    bool validate() override {
        return config_.threshold_value <= 0 || 
               config_.threshold_value <= get_current_score();
    }
    
private:
    double measure_cpu_usage() {
        // Measure CPU usage (placeholder)
        return config_.iterations * 0.1;  // Placeholder
    }
    
    size_t measure_memory_usage() {
        // Measure current memory usage (placeholder)
        return 1024 * 1024;  // Placeholder: 1MB
    }
    
    size_t measure_peak_memory_usage() {
        // Measure peak memory usage (placeholder)
        return 10 * 1024 * 1024;  // Placeholder: 10MB
    }
    
    double measure_memory_bandwidth() {
        // Measure memory bandwidth (placeholder)
        return 10.0;  // Placeholder: 10 GB/s
    }
    
    uint64_t calculate_operations_per_second() {
        // Calculate operations per second (placeholder)
        return static_cast<uint64_t>(config_.iterations / config_.warmup_duration_seconds);
    }
    
    double get_current_score() const {
        // Calculate current benchmark score (placeholder)
        return 100.0;  // Placeholder
    }
    
    bool validate_result(const BenchmarkResult& result) {
        if (config_.threshold_value > 0) {
            double score = calculate_benchmark_score(result);
            return score >= config_.threshold_value;
        }
        return true;
    }
    
    double calculate_benchmark_score(const BenchmarkResult& result) const {
        // Calculate benchmark score based on metrics (placeholder)
        double score = 100.0;
        if (result.operations_per_second > 0) {
            score += std::log10(result.operations_per_second / 1000.0) * 10.0;
        }
        return score;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Component Benchmark implementation
class ComponentBenchmark : public BenchmarkBase {
private:
    std::string component_name_;
    
public:
    ComponentBenchmark(const BenchmarkConfig& config) : BenchmarkBase(config) {
        component_name_ = config.parameters.count("component_name") ? 
            config.parameters.at("component_name") : "unknown";
    }
    
    void setup() override {
        // Component-specific setup
    }
    
    BenchmarkResult run() override {
        BenchmarkResult result;
        result.benchmark_name = config_.name;
        result.benchmark_type = config_.benchmark_type;
        result.timestamp_ns = getCurrentTimestamp();
        
        // Component-specific performance measurements
        result.cpu_time_seconds = measure_component_cpu();
        result.wall_time_seconds = config_.iterations * config_.warmup_duration_seconds;
        result.memory_used_bytes = measure_component_memory();
        result.memory_peak_bytes = measure_component_peak_memory();
        result.operations_per_second = measure_component_operations();
        
        // Component-specific metrics
        result.metrics["component_utilization"] = calculate_component_utilization();
        result.metrics["response_time"] = measure_response_time();
        result.metrics["throughput"] = calculate_throughput();
        
        result.passed = validate_result(result);
        
        return result;
    }
    
    void teardown() override {
        // Cleanup component resources
    }
    
    bool validate() override {
        return config_.threshold_value <= 0 || 
               config_.threshold_value <= get_component_score();
    }
    
private:
    double measure_component_cpu() {
        // Measure component CPU usage (placeholder)
        return config_.iterations * 0.05;  // Placeholder: 0.05s per iteration
    }
    
    size_t measure_component_memory() {
        // Measure component memory usage (placeholder)
        return 512 * 1024;  // Placeholder: 512KB
    }
    
    size_t measure_component_peak_memory() {
        // Measure component peak memory (placeholder)
        return 5 * 1024 * 1024;  // Placeholder: 5MB
    }
    
    uint64_t measure_component_operations() {
        // Measure component operations (placeholder)
        return static_cast<uint64_t>(config_.iterations * 1000.0);
    }
    
    double calculate_component_utilization() const {
        // Calculate component utilization (placeholder)
        return 85.0;  // Placeholder: 85%
    }
    
    double measure_response_time() {
        // Measure component response time (placeholder)
        return 0.001;  // Placeholder: 1ms
    }
    
    double calculate_throughput() {
        // Calculate component throughput (placeholder)
        return 10000.0;  // Placeholder: 10,000 ops/s
    }
    
    double get_component_score() const {
        // Calculate component benchmark score (placeholder)
        return 90.0;  // Placeholder
    }
    
    bool validate_result(const BenchmarkResult& result) {
        if (config_.threshold_value > 0) {
            double score = calculate_component_score(result);
            return score >= config_.threshold_value;
        }
        return true;
    }
    
    double calculate_component_score(const BenchmarkResult& result) const {
        // Calculate component benchmark score based on metrics (placeholder)
        double score = 100.0;
        if (result.operations_per_second > 0) {
            score += std::log10(result.operations_per_second / 1000.0) * 5.0;
        }
        if (result.metrics.count("component_utilization")) {
            score += result.metrics.at("component_utilization") * 0.5;
        }
        return score;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Integration Benchmark implementation
class IntegrationBenchmark : public BenchmarkBase {
public:
    IntegrationBenchmark(const BenchmarkConfig& config) : BenchmarkBase(config) {}
    
    void setup() override {
        // Integration-specific setup (system components, dependencies, etc.)
    }
    
    BenchmarkResult run() override {
        BenchmarkResult result;
        result.benchmark_name = config_.name;
        result.benchmark_type = config_.benchmark_type;
        result.timestamp_ns = getCurrentTimestamp();
        
        // Integration performance measurements
        result.cpu_time_seconds = measure_integration_cpu();
        result.wall_time_seconds = config_.iterations * config_.warmup_duration_seconds;
        result.memory_used_bytes = measure_integration_memory();
        result.memory_peak_bytes = measure_integration_peak_memory();
        result.operations_per_second = measure_integration_operations();
        
        // Integration-specific metrics
        result.metrics["integration_success_rate"] = measure_integration_success();
        result.metrics["dependency_resolution_time"] = measure_dependency_resolution_time();
        result.metrics["data_flow_efficiency"] = calculate_data_flow_efficiency();
        
        result.passed = validate_result(result);
        
        return result;
    }
    
    void teardown() override {
        // Cleanup integration resources
    }
    
    bool validate() override {
        return config_.threshold_value <= 0 || 
               config_.threshold_value <= get_integration_score();
    }
    
private:
    double measure_integration_cpu() {
        // Measure integration CPU usage (placeholder)
        return config_.iterations * 0.2;  // Placeholder: 0.2s per iteration
    }
    
    size_t measure_integration_memory() {
        // Measure integration memory usage (placeholder)
        return 2 * 1024 * 1024;  // Placeholder: 2MB
    }
    
    size_t measure_integration_peak_memory() {
        // Measure integration peak memory (placeholder)
        return 20 * 1024 * 1024;  // Placeholder: 20MB
    }
    
    uint64_t measure_integration_operations() {
        // Measure integration operations (placeholder)
        return static_cast<uint64_t>(config_.iterations * 500.0);
    }
    
    double measure_integration_success() {
        // Measure integration success rate (placeholder)
        return 0.95;  // Placeholder: 95%
    }
    
    double measure_dependency_resolution_time() {
        // Measure dependency resolution time (placeholder)
        return 0.5;  // Placeholder: 0.5s
    }
    
    double calculate_data_flow_efficiency() {
        // Calculate data flow efficiency (placeholder)
        return 0.85;  // Placeholder: 85%
    }
    
    double get_integration_score() const {
        // Calculate integration benchmark score (placeholder)
        return 85.0;  // Placeholder
    }
    
    bool validate_result(const BenchmarkResult& result) {
        if (config_.threshold_value > 0) {
            double score = calculate_integration_score(result);
            return score >= config_.threshold_value;
        }
        return true;
    }
    
    double calculate_integration_score(const BenchmarkResult& result) const {
        // Calculate integration benchmark score based on metrics (placeholder)
        double score = 100.0;
        if (result.metrics.count("integration_success_rate")) {
            score += result.metrics.at("integration_success_rate") * 20.0;
        }
        if (result.metrics.count("data_flow_efficiency")) {
            score += result.metrics.at("data_flow_efficiency") * 15.0;
        }
        return score;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// End-to-End Benchmark implementation
class EndToEndBenchmark : public BenchmarkBase {
public:
    EndToEndBenchmark(const BenchmarkConfig& config) : BenchmarkBase(config) {}
    
    void setup() override {
        // End-to-end specific setup (complete workflow, real data, etc.)
    }
    
    BenchmarkResult run() override {
        BenchmarkResult result;
        result.benchmark_name = config_.name;
        result.benchmark_type = config_.benchmark_type;
        result.timestamp_ns = getCurrentTimestamp();
        
        // End-to-end performance measurements
        result.cpu_time_seconds = measure_end_to_end_cpu();
        result.wall_time_seconds = config_.iterations * config_.warmup_duration_seconds;
        result.memory_used_bytes = measure_end_to_end_memory();
        result.memory_peak_bytes = measure_end_to_end_peak_memory();
        result.operations_per_second = measure_end_to_end_operations();
        
        // End-to-end specific metrics
        result.metrics["workflow_completion_rate"] = measure_workflow_completion();
        result.metrics["end_to_end_latency"] = measure_end_to_end_latency();
        result.metrics["user_experience_score"] = calculate_user_experience_score();
        
        result.passed = validate_result(result);
        
        return result;
    }
    
    void teardown() override {
        // Cleanup end-to-end resources
    }
    
    bool validate() override {
        return config_.threshold_value <= 0 || 
               config_.threshold_value <= get_end_to_end_score();
    }
    
private:
    double measure_end_to_end_cpu() {
        // Measure end-to-end CPU usage (placeholder)
        return config_.iterations * 0.3;  // Placeholder: 0.3s per iteration
    }
    
    size_t measure_end_to_end_memory() {
        // Measure end-to-end memory usage (placeholder)
        return 5 * 1024 * 1024;  // Placeholder: 5MB
    }
    
    size_t measure_end_to_end_peak_memory() {
        // Measure end-to-end peak memory (placeholder)
        return 50 * 1024 * 1024;  // Placeholder: 50MB
    }
    
    uint64_t measure_end_to_end_operations() {
        // Measure end-to-end operations (placeholder)
        return static_cast<uint64_t>(config_.iterations * 200.0);
    }
    
    double measure_workflow_completion() {
        // Measure workflow completion rate (placeholder)
        return 0.98;  // Placeholder: 98%
    }
    
    double measure_end_to_end_latency() {
        // Measure end-to-end latency (placeholder)
        return 1.5;  // Placeholder: 1.5s
    }
    
    double calculate_user_experience_score() {
        // Calculate user experience score (placeholder)
        return 4.5;  // Placeholder: 4.5/5
    }
    
    double get_end_to_end_score() const {
        // Calculate end-to-end benchmark score (placeholder)
        return 95.0;  // Placeholder
    }
    
    bool validate_result(const BenchmarkResult& result) {
        if (config_.threshold_value > 0) {
            double score = calculate_end_to_end_score(result);
            return score >= config_.threshold_value;
        }
        return true;
    }
    
    double calculate_end_to_end_score(const BenchmarkResult& result) const {
        // Calculate end-to-end benchmark score based on metrics (placeholder)
        double score = 100.0;
        if (result.metrics.count("workflow_completion_rate")) {
            score += result.metrics.at("workflow_completion_rate") * 15.0;
        }
        if (result.metrics.count("user_experience_score")) {
            score += result.metrics.at("user_experience_score") * 20.0;
        }
        return score;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Main Benchmarking Framework Class
class BenchmarkFramework {
private:
    std::vector<BenchmarkSuite> suites_;
    std::vector<BenchmarkBase*> active_benchmarks_;
    std::mutex framework_mutex_;
    bool enabled_;
    
public:
    BenchmarkFramework() : enabled_(false) {}
    
    void add_suite(const BenchmarkSuite& suite) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        suites_.push_back(suite);
    }
    
    void start_suite(const std::string& suite_name) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        for (const auto& suite : suites_) {
            if (suite.name == suite_name && suite.enabled) {
                for (const auto& benchmark_config : suite.benchmarks) {
                    BenchmarkBase* benchmark = create_benchmark(benchmark_config);
                    if (benchmark) {
                        active_benchmarks_.push_back(benchmark);
                        benchmark->start();
                    }
                }
                break;
            }
        }
    }
    
    std::vector<BenchmarkResult> run_suite(const std::string& suite_name) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        
        std::vector<BenchmarkResult> results;
        
        for (auto* benchmark : active_benchmarks_) {
            if (benchmark) {
                BenchmarkResult result = benchmark->execute();
                results.push_back(result);
                
                // Cleanup benchmark
                delete benchmark;
            }
        }
        
        active_benchmarks_.clear();
        return results;
    }
    
    void stop_suite(const std::string& suite_name) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        for (auto* benchmark : active_benchmarks_) {
            if (benchmark) {
                // Benchmark will be cleaned up automatically in run_suite
            }
        }
    }
    
    BenchmarkBase* create_benchmark(const BenchmarkConfig& config) {
        if (config.benchmark_type == "system") {
            return new SystemBenchmark(config);
        } else if (config.benchmark_type == "component") {
            return new ComponentBenchmark(config);
        } else if (config.benchmark_type == "integration") {
            return new IntegrationBenchmark(config);
        } else if (config.benchmark_type == "end_to_end") {
            return new EndToEndBenchmark(config);
        }
        return nullptr;
    }
    
    std::vector<BenchmarkResult> run_all_benchmarks() {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        
        std::vector<BenchmarkResult> all_results;
        
        for (const auto& suite : suites_) {
            if (!suite.enabled) continue;
            
            start_suite(suite.name);
            std::vector<BenchmarkResult> suite_results = run_suite(suite.name);
            stop_suite(suite.name);
            
            all_results.insert(all_results.end(), suite_results.begin(), suite_results.end());
        }
        
        return all_results;
    }
    
    std::vector<PerformanceComparison> compare_performance(const std::string& baseline_suite,
                                                           const std::string& comparison_suite) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        
        std::vector<PerformanceComparison> comparisons;
        
        // Run baseline suite
        start_suite(baseline_suite);
        std::vector<BenchmarkResult> baseline_results = run_suite(baseline_suite);
        stop_suite(baseline_suite);
        
        // Run comparison suite
        start_suite(comparison_suite);
        std::vector<BenchmarkResult> comparison_results = run_suite(comparison_suite);
        stop_suite(comparison_suite);
        
        // Compare results
        for (const auto& baseline_result : baseline_results) {
            for (const auto& comparison_result : comparison_results) {
                if (baseline_result.benchmark_name == comparison_result.benchmark_name) {
                    PerformanceComparison comparison;
                    comparison.baseline_name = baseline_suite;
                    comparison.comparison_name = comparison_suite;
                    comparison.baseline_score = calculate_benchmark_score(baseline_result);
                    comparison.comparison_score = calculate_benchmark_score(comparison_result);
                    comparison.improvement_percentage = 
                        (comparison_result.benchmark_name == baseline_result.benchmark_name ? 
                         0.0 : ((comparison.comparison_score - comparison.baseline_score) / comparison.baseline_score) * 100.0);
                    comparison.confidence_score = calculate_confidence_score(baseline_result, comparison_result);
                    comparison.recommendation = generate_recommendation(comparison);
                    
                    comparisons.push_back(comparison);
                }
            }
        }
        
        return comparisons;
    }
    
    std::vector<OptimizationRecommendation> analyze_for_optimizations(const std::vector<BenchmarkResult>& results) {
        std::vector<OptimizationRecommendation> recommendations;
        
        for (const auto& result : results) {
            if (!result.passed) {
                OptimizationRecommendation rec;
                rec.issue_type = "performance_below_threshold";
                rec.component = result.benchmark_name;
                rec.impact_score = calculate_impact_score(result);
                rec.recommendation = generate_optimization_recommendation(result);
                rec.estimated_improvement = estimate_performance_improvement(result);
                rec.implementation_difficulty = assess_implementation_difficulty(result);
                rec.details = generate_optimization_details(result);
                
                recommendations.push_back(rec);
            }
            
            // Check for bottlenecks
            if (result.metrics.find("cpu_time_per_operation") != result.metrics.end() &&
                result.metrics.at("cpu_time_per_operation") > 0.1) {
                OptimizationRecommendation rec;
                rec.issue_type = "cpu_bottleneck";
                rec.component = result.benchmark_name;
                rec.impact_score = calculate_bottleneck_impact(result, "cpu");
                rec.recommendation = "Optimize CPU-intensive operations";
                rec.estimated_improvement = 0.2;  // 20% improvement
                rec.implementation_difficulty = "medium";
                rec.details = {{"bottleneck", "high_cpu_usage"}, {"potential_fix", "parallelization"}};
                
                recommendations.push_back(rec);
            }
            
            if (result.metrics.find("memory_per_operation") != result.metrics.end() &&
                result.metrics.at("memory_per_operation") > 1024) {
                OptimizationRecommendation rec;
                rec.issue_type = "memory_bottleneck";
                rec.component = result.benchmark_name;
                rec.impact_score = calculate_bottleneck_impact(result, "memory");
                rec.recommendation = "Reduce memory footprint and improve cache locality";
                rec.estimated_improvement = 0.3;  // 30% improvement
                rec.implementation_difficulty = "high";
                rec.details = {{"bottleneck", "high_memory_usage"}, {"potential_fix", "memory_pooling"}};
                
                recommendations.push_back(rec);
            }
        }
        
        return recommendations;
    }
    
    void save_benchmark_results(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Benchmark Results Report" << std::endl;
            file << "=======================" << std::endl;
            file << "Total benchmarks: " << results.size() << std::endl;
            file << "Passed: " << std::count_if(results.begin(), results.end(),
                                 [](const BenchmarkResult& r) { return r.passed; }) << std::endl;
            file << "Failed: " << std::count_if(results.begin(), results.end(),
                                 [](const BenchmarkResult& r) { return !r.passed; }) << std::endl;
            file << std::endl;
            
            for (const auto& result : results) {
                file << "Benchmark: " << result.benchmark_name << std::endl;
                file << "Type: " << result.benchmark_type << std::endl;
                file << "Duration: " << result.duration_seconds << " seconds" << std::endl;
                file << "CPU time: " << result.cpu_time_seconds << " seconds" << std::endl;
                file << "Memory used: " << result.memory_used_bytes << " bytes" << std::endl;
                file << "Operations/sec: " << result.operations_per_second << std::endl;
                file << "Passed: " << (result.passed ? "Yes" : "No") << std::endl;
                if (!result.passed) {
                    file << "Failure message: " << result.failure_message << std::endl;
                }
                file << std::endl;
            }
            
            file.close();
        }
    }
    
    void print_benchmark_summary(const std::vector<BenchmarkResult>& results) {
        std::cout << "=== NLM Benchmark Summary ===" << std::endl;
        std::cout << "Total benchmarks: " << results.size() << std::endl;
        std::cout << "Passed: " << std::count_if(results.begin(), results.end(),
                     [](const BenchmarkResult& r) { return r.passed; }) << std::endl;
        std::cout << "Failed: " << std::count_if(results.begin(), results.end(),
                     [](const BenchmarkResult& r) { return !r.passed; }) << std::endl;
        std::cout << std::endl;
        
        for (const auto& result : results) {
            std::cout << "Benchmark: " << result.benchmark_name << std::endl;
            std::cout << "  Type: " << result.benchmark_type << std::endl;
            std::cout << "  Duration: " << result.duration_seconds << " seconds" << std::endl;
            std::cout << "  CPU time: " << result.cpu_time_seconds << " seconds" << std::endl;
            std::cout << "  Memory used: " << result.memory_used_bytes << " bytes" << std::endl;
            std::cout << "  Operations/sec: " << result.operations_per_second << std::endl;
            std::cout << "  Passed: " << (result.passed ? "Yes" : "No") << std::endl;
            std::cout << std::endl;
        }
    }
    
private:
    double calculate_benchmark_score(const BenchmarkResult& result) const {
        // Calculate benchmark score based on metrics (placeholder)
        double score = 100.0;
        if (result.operations_per_second > 0) {
            score += std::log10(result.operations_per_second / 1000.0) * 10.0;
        }
        return score;
    }
    
    double calculate_confidence_score(const BenchmarkResult& baseline,
                                      const BenchmarkResult& comparison) const {
        // Calculate confidence score for comparison (placeholder)
        return 0.95;  // Placeholder: 95% confidence
    }
    
    std::string generate_recommendation(const PerformanceComparison& comparison) {
        if (comparison.improvement_percentage > 0) {
            return "Baseline performs better than comparison";
        } else {
            return "Consider adopting the comparison configuration for better performance";
        }
    }
    
    double calculate_impact_score(const BenchmarkResult& result) {
        // Calculate impact score for optimization recommendations (placeholder)
        return 0.7;  // Placeholder: 70% impact
    }
    
    std::string generate_optimization_recommendation(const BenchmarkResult& result) {
        return "Optimize performance to meet threshold requirements";
    }
    
    double estimate_performance_improvement(const BenchmarkResult& result) {
        return 0.2;  // Placeholder: 20% improvement
    }
    
    std::string assess_implementation_difficulty(const BenchmarkResult& result) {
        return "medium";  // Placeholder
    }
    
    std::map<std::string, std::string> generate_optimization_details(const BenchmarkResult& result) {
        return {{{"issue", "below_threshold"}, {"current_score", "0.0"}}};  // Placeholder
    }
    
    double calculate_bottleneck_impact(const BenchmarkResult& result, const std::string& bottleneck_type) {
        // Calculate impact score for bottleneck analysis (placeholder)
        return 0.8;  // Placeholder: 80% impact
    }
};

// Global benchmark framework instance
static std::unique_ptr<BenchmarkFramework> global_benchmark_framework;

// Convenience functions
namespace nlm {
namespace benchmarking {

inline BenchmarkFramework& getGlobalBenchmarkFramework() {
    if (!global_benchmark_framework) {
        global_benchmark_framework = std::make_unique<BenchmarkFramework>();
    }
    return *global_benchmark_framework;
}

inline void add_benchmark_suite(const BenchmarkSuite& suite) {
    getGlobalBenchmarkFramework().add_suite(suite);
}

inline void run_benchmark_suite(const std::string& suite_name) {
    getGlobalBenchmarkFramework().start_suite(suite_name);
    std::vector<BenchmarkResult> results = getGlobalBenchmarkFramework().run_suite(suite_name);
    getGlobalBenchmarkFramework().stop_suite(suite_name);
    
    getGlobalBenchmarkFramework().print_benchmark_summary(results);
}

inline std::vector<BenchmarkResult> run_all_benchmarks() {
    return getGlobalBenchmarkFramework().run_all_benchmarks();
}

inline std::vector<PerformanceComparison> compare_performance(const std::string& baseline_suite,
                                                              const std::string& comparison_suite) {
    return getGlobalBenchmarkFramework().compare_performance(baseline_suite, comparison_suite);
}

inline std::vector<OptimizationRecommendation> analyze_for_optimizations(const std::vector<BenchmarkResult>& results) {
    return getGlobalBenchmarkFramework().analyze_for_optimizations(results);
}

inline void save_benchmark_results(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    getGlobalBenchmarkFramework().save_benchmark_results(results, filename);
}

} // namespace benchmarking
} // namespace nlm

// Example usage and demonstration
void demonstrate_benchmarking() {
    std::cout << "=== NLM Benchmarking Demonstration ===" << std::endl;
    
    // Create benchmark suites
    BenchmarkSuite system_suite;
    system_suite.name = "system_benchmarks";
    system_suite.description = "System performance benchmarks";
    system_suite.enabled = true;
    
    BenchmarkSuite component_suite;
    component_suite.name = "component_benchmarks";
    component_suite.description = "Component performance benchmarks";
    component_suite.enabled = true;
    
    // Add system benchmarks
    BenchmarkConfig system_bench1;
    system_bench1.name = "system_overall_performance";
    system_bench1.description = "Overall system performance benchmark";
    system_bench1.benchmark_type = "system";
    system_bench1.warmup_duration_seconds = 2.0;
    system_bench1.iterations = 10;
    system_bench1.timeout_seconds = 300.0;
    system_bench1.threshold_value = 100.0;
    system_bench1.enabled = true;
    system_suite.benchmarks.push_back(system_bench1);
    
    BenchmarkConfig system_bench2;
    system_bench2.name = "system_memory_efficiency";
    system_bench2.description = "Memory efficiency benchmark";
    system_bench2.benchmark_type = "system";
    system_bench2.warmup_duration_seconds = 1.0;
    system_bench2.iterations = 5;
    system_bench2.timeout_seconds = 60.0;
    system_bench2.threshold_value = 1000.0;
    system_bench2.parameters = {{{"metric", "memory_bandwidth"}}};
    system_bench2.enabled = true;
    system_suite.benchmarks.push_back(system_bench2);
    
    // Add component benchmarks
    BenchmarkConfig component_bench1;
    component_bench1.name = "component_cpu_performance";
    component_bench1.description = "CPU performance benchmark";
    component_bench1.benchmark_type = "component";
    component_bench1.warmup_duration_seconds = 1.0;
    component_bench1.iterations = 20;
    component_bench1.timeout_seconds = 120.0;
    component_bench1.threshold_value = 5000.0;
    component_bench1.parameters = {{{"component_name", "cpu"}}};
    component_bench1.enabled = true;
    component_suite.benchmarks.push_back(component_bench1);
    
    // Add benchmark suites to framework
    nlm::benchmarking::add_benchmark_suite(system_suite);
    nlm::benchmarking::add_benchmark_suite(component_suite);
    
    // Run benchmarks
    std::cout << "Running system benchmarks..." << std::endl;
    nlm::benchmarking::run_benchmark_suite("system_benchmarks");
    
    std::cout << std::endl << "Running component benchmarks..." << std::endl;
    nlm::benchmarking::run_benchmark_suite("component_benchmarks");
}

// Command-line interface for benchmarking
void benchmarking_cli(int argc, char* argv[]) {
    std::cout << "NLM Benchmarking CLI" << std::endl;
    std::cout << "Usage: nlm_benchmark [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --demo                    Run benchmarking demonstration" << std::endl;
    std::cout << "  --suite <name>           Run specific benchmark suite" << std::endl;
    std::cout << "  --compare <baseline> <comparison>  Compare two benchmark suites" << std::endl;
    std::cout << "  --analyze                Analyze results for optimization opportunities" << std::endl;
    std::cout << "  --save <filename>        Save benchmark results to file" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_benchmarking();
        }
    }
}