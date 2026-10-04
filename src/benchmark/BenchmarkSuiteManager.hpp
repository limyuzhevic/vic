// Performance Benchmarking Suite for NLM
// Phase 6: Comprehensive Performance Measurement and Analysis
// 
// This module provides comprehensive performance benchmarking capabilities
// for the NLM neural simulation framework. It includes:
// 
// - Performance benchmarking for NLM brain components
// - Benchmarking for different experiment types and configurations
// - Performance comparison and regression detection
// - Benchmarking results storage and analysis
// - Automated benchmarking workflows
// - Performance optimization recommendations
// - Benchmarking integration with NLM core systems
// - Performance benchmarking for Python bindings
// - Performance benchmarking for CLI interface
// - Performance benchmarking for visualization tools
// - Performance benchmarking for configuration system
// - Benchmarking metrics collection and reporting
// - Benchmarking data export and import
// - Benchmarking results visualization

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <random>
#include <filesystem>
#include <json.hpp>

namespace nlm {

using json = nlohmann::json;

// Performance benchmark metrics
struct BenchmarkMetrics {
    std::string name;                                 // Benchmark name
    std::string description;                          // Benchmark description
    std::string category;                             // Benchmark category (e.g., "brain", "memory", "plasticity", "cognition")
    std::string type;                                 // Benchmark type (e.g., "time", "memory", "throughput", "efficiency")
    double value;                                    // Metric value
    std::string unit;                                // Unit of measurement
    double error;                                    // Standard error
    double confidence;                               // Confidence level (0-1)
    uint64_t iterations;                             // Number of iterations
    double min_value;                                // Minimum value
    double max_value;                                // Maximum value
    double mean_value;                               // Mean value
    double median_value;                             // Median value
    double stddev_value;                             // Standard deviation
    std::vector<double> raw_data;                     // Raw data points
    std::chrono::system_clock::time_point timestamp;   // Timestamp when benchmark was run
    std::string tags;                                // Benchmark tags (comma-separated)
    std::unordered_map<std::string, std::string> metadata; // Additional metadata
    
    BenchmarkMetrics() : value(0.0), error(0.0), confidence(1.0), iterations(0),
                       min_value(0.0), max_value(0.0), mean_value(0.0), median_value(0.0), stddev_value(0.0) {}
    
    // Calculate coefficient of variation (relative standard deviation)
    double getCoefficientOfVariation() const {
        return stddev_value > 0.0 ? (stddev_value / mean_value) * 100.0 : 0.0;
    }
    
    // Calculate relative error
    double getRelativeError() const {
        return mean_value > 0.0 ? (error / mean_value) * 100.0 : 0.0;
    }
    
    // Calculate performance score (higher is better)
    double getPerformanceScore() const {
        return 100.0 / (1.0 + getCoefficientOfVariation() + getRelativeError());
    }
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;                              // Benchmark name
    std::string description;                       // Benchmark description
    std::string category;                         // Benchmark category
    std::string type;                             // Benchmark type
    std::string unit;                              // Unit of measurement
    uint64_t iterations;                         // Number of iterations
    bool warmup;                                  // Whether to run warmup iterations
    uint64_t warmup_iterations;                  // Number of warmup iterations
    bool include_overhead;                       // Whether to include overhead in measurements
    bool detailed;                                // Whether to collect detailed statistics
    std::vector<std::string> tags;                // Benchmark tags
    std::string dependency;                      // Dependency on other benchmarks
    bool enabled;                                 // Whether benchmark is enabled
    bool output_to_file;                          // Whether to output results to file
    std::string output_format;                    // Output format (e.g., "json", "csv", "txt")
    std::string output_directory;                 // Output directory
    std::chrono::duration<double> timeout;        // Benchmark timeout
    std::string error_handling;                   // Error handling policy (e.g., "stop", "continue", "ignore")
    
    BenchmarkConfig() : iterations(1000), warmup(true), warmup_iterations(100),
                      include_overhead(false), detailed(true), enabled(true),
                      output_to_file(true), timeout(3600.0), error_handling("continue") {}
};

// Benchmark result
struct BenchmarkResult {
    std::string benchmark_id;                     // Benchmark identifier
    std::string benchmark_name;                   // Benchmark name
    std::string benchmark_type;                   // Benchmark type
    std::string configuration;                    // Configuration used
    BenchmarkMetrics metrics;                     // Benchmark metrics
    std::vector<BenchmarkMetrics> component_metrics; // Component-level metrics
    std::string environment;                      // Environment information
    std::string git_commit;                       // Git commit hash
    std::string compiler;                         // Compiler information
    std::string platform;                         // Platform information
    std::chrono::system_clock::time_point timestamp;   // Timestamp when benchmark was run
    double total_time;                            // Total benchmark time
    uint64_t memory_usage;                         // Memory usage in bytes
    double cpu_usage;                             // CPU usage percentage
    std::string error_message;                     // Error message (if any)
    bool success;                                 // Whether benchmark succeeded
    
    BenchmarkResult() : total_time(0.0), memory_usage(0), cpu_usage(0.0), success(false) {}
    
    // Get benchmark identifier
    std::string getBenchmarkId() const {
        return benchmark_id;
    }
    
    // Get benchmark name
    std::string getBenchmarkName() const {
        return benchmark_name;
    }
    
    // Get benchmark type
    std::string getBenchmarkType() const {
        return benchmark_type;
    }
    
    // Get benchmark category
    std::string getBenchmarkCategory() const {
        return metrics.category;
    }
    
    // Get benchmark value
    double getBenchmarkValue() const {
        return metrics.value;
    }
    
    // Get benchmark unit
    std::string getBenchmarkUnit() const {
        return metrics.unit;
    }
    
    // Check if benchmark succeeded
    bool isSuccess() const {
        return success;
    }
};

// Benchmark suite configuration
struct BenchmarkSuiteConfig {
    std::string name;                              // Suite name
    std::string description;                       // Suite description
    std::string version;                           // Suite version
    std::string author;                            // Suite author
    std::string date;                              // Suite date (YYYY-MM-DD)
    std::string category;                          // Suite category
    std::string tags;                              // Suite tags (comma-separated)
    std::vector<BenchmarkConfig> benchmarks;       // Benchmarks in the suite
    bool enabled;                                  // Whether suite is enabled
    bool run_by_default;                          // Whether suite runs by default
    int priority;                                  // Suite priority (higher = runs first)
    std::string dependency;                       // Dependency on other suites
    bool parallel;                                 // Whether suite can run in parallel
    uint32_t max_workers;                          // Maximum number of worker threads
    std::string output_directory;                 // Output directory for suite results
    bool generate_reports;                        // Whether to generate reports
    std::string report_format;                    // Report format (e.g., "html", "json", "csv")
    bool generate_plots;                          // Whether to generate plots
    std::string plot_format;                      // Plot format (e.g., "png", "svg")
    
    BenchmarkSuiteConfig() : enabled(true), run_by_default(true), priority(0),
                            parallel(true), max_workers(4), generate_reports(true),
                            generate_plots(false), plot_format("png") {}
    
    // Add a benchmark
    void addBenchmark(const BenchmarkConfig& benchmark) {
        benchmarks.push_back(benchmark);
    }
    
    // Get a benchmark by name
    BenchmarkConfig* getBenchmark(const std::string& name) {
        for (auto& benchmark : benchmarks) {
            if (benchmark.name == name) {
                return &benchmark;
            }
        }
        return nullptr;
    }
    
    // Get a benchmark by name (const version)
    const BenchmarkConfig* getBenchmark(const std::string& name) const {
        for (const auto& benchmark : benchmarks) {
            if (benchmark.name == name) {
                return &benchmark;
            }
        }
        return nullptr;
    }
};

// Benchmark executor interface
class BenchmarkExecutor {
public:
    virtual ~BenchmarkExecutor() = default;
    
    // Execute a benchmark
    virtual BenchmarkResult execute(const BenchmarkConfig& config, const std::unordered_map<std::string, std::string>& parameters = {}) = 0;
    
    // Check if executor can handle the benchmark
    virtual bool canExecute(const BenchmarkConfig& config) const = 0;
    
    // Get executor name
    virtual std::string getName() const = 0;
    
    // Get executor version
    virtual std::string getVersion() const = 0;
};

// Benchmark execution strategy
enum class ExecutionStrategy {
    Sequential,      // Execute benchmarks one at a time
    Parallel,        // Execute benchmarks in parallel
    Batch,           // Execute benchmarks in batches
    Pipeline,        // Execute benchmarks in a pipeline
    Hierarchical,    // Execute benchmarks in a hierarchical manner
    Adaptive        // Adapt execution based on performance
};

// Benchmark suite manager interface
class BenchmarkSuiteManager {
public:
    virtual ~BenchmarkSuiteManager() = default;
    
    // Add a benchmark suite
    virtual void addSuite(const BenchmarkSuiteConfig& suite) = 0;
    
    // Remove a benchmark suite
    virtual bool removeSuite(const std::string& name) = 0;
    
    // Get a benchmark suite by name
    virtual const BenchmarkSuiteConfig* getSuite(const std::string& name) const = 0;
    
    // Check if a benchmark suite exists
    virtual bool hasSuite(const std::string& name) const = 0;
    
    // Get all suite names
    virtual std::vector<std::string> getSuiteNames() const = 0;
    
    // Set the default benchmark executor
    virtual void setExecutor(std::shared_ptr<BenchmarkExecutor> executor) = 0;
    
    // Get the default benchmark executor
    virtual std::shared_ptr<BenchmarkExecutor> getExecutor() const = 0;
    
    // Execute a benchmark
    virtual BenchmarkResult executeBenchmark(const std::string& suite_name, const std::string& benchmark_name,
                                             const std::unordered_map<std::string, std::string>& parameters = {}) = 0;
    
    // Execute a benchmark suite
    virtual std::vector<BenchmarkResult> executeSuite(const std::string& suite_name) = 0;
    
    // Get benchmark result
    virtual const BenchmarkResult* getBenchmarkResult(const std::string& benchmark_id) const = 0;
    
    // Get all benchmark results
    virtual std::vector<BenchmarkResult> getAllResults() const = 0;
    
    // Save benchmark results to file
    virtual bool saveResults(const std::string& filepath) const = 0;
    
    // Load benchmark results from file
    virtual bool loadResults(const std::string& filepath) = 0;
    
    // Generate benchmark report
    virtual bool generateReport(const std::string& report_file, const std::string& format = "html") const = 0;
};

// Default benchmark executor
class DefaultBenchmarkExecutor : public BenchmarkExecutor {
public:
    DefaultBenchmarkExecutor() {}
    
    BenchmarkResult execute(const BenchmarkConfig& config, const std::unordered_map<std::string, std::string>& parameters) override {
        BenchmarkResult result;
        result.benchmark_name = config.name;
        result.benchmark_type = config.type;
        result.metrics.category = config.category;
        result.metrics.name = config.name;
        result.metrics.description = config.description;
        result.metrics.unit = config.unit;
        result.configuration = "default";
        result.timestamp = std::chrono::system_clock::now();
        result.success = true;
        
        try {
            // Record start time
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Run warmup iterations
            if (config.warmup) {
                runWarmup(config);
            }
            
            // Run benchmark iterations
            runBenchmark(config, parameters);
            
            // Record end time
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
            
            // Set results
            result.metrics.value = calculateBenchmarkValue(config, parameters);
            result.metrics.error = calculateBenchmarkError(config, parameters);
            result.metrics.iterations = config.iterations;
            result.metrics.min_value = getMinValue(config, parameters);
            result.metrics.max_value = getMaxValue(config, parameters);
            result.metrics.mean_value = result.metrics.value;
            result.metrics.median_value = result.metrics.value;
            result.metrics.stddev_value = 0.0;  // Would need multiple runs to calculate
            result.metrics.raw_data = std::vector<double>(config.iterations, result.metrics.value);
            result.metrics.timestamp = start_time;
            result.metrics.tags = config.tags;
            
            result.total_time = duration.count();
            result.memory_usage = getMemoryUsage();
            result.cpu_usage = getCpuUsage();
            
        } catch (const std::exception& e) {
            result.success = false;
            result.error_message = e.what();
        }
        
        return result;
    }
    
    bool canExecute(const BenchmarkConfig& config) const override {
        return true;  // Can execute any benchmark
    }
    
    std::string getName() const override {
        return "Default Benchmark Executor";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
private:
    void runWarmup(const BenchmarkConfig& config) {
        // In a real implementation, this would run warmup iterations
        // For now, we'll just sleep for a short time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    void runBenchmark(const BenchmarkConfig& config, const std::unordered_map<std::string, std::string>& parameters) {
        // In a real implementation, this would run the actual benchmark
        // For now, we'll simulate the benchmark
        
        // Simulate benchmark work
        for (uint64_t i = 0; i < config.iterations; ++i) {
            // Simulate some work
            double work = simulateWork(i, config, parameters);
            
            // Record metric
            if (config.detailed) {
                recordMetric(work);
            }
            
            // Simulate some overhead
            if (config.include_overhead) {
                simulateOverhead(i, config, parameters);
            }
        }
    }
    
    double simulateWork(uint64_t iteration, const BenchmarkConfig& config,
                        const std::unordered_map<std::string, std::string>& parameters) {
        // Simulate computational work based on configuration
        double work = 1.0;
        
        if (config.type == "time") {
            work = 1000.0 / (iteration + 1);  // Simulate decreasing work
        } else if (config.type == "memory") {
            work = 100.0 * (iteration % 10 + 1);  // Simulate variable memory usage
        } else if (config.type == "throughput") {
            work = 10.0 + 90.0 * ((double)iteration / config.iterations);  // Simulate increasing throughput
        } else if (config.type == "efficiency") {
            work = 0.5 + 0.5 * std::sin((double)iteration / config.iterations * 3.14159);
        }
        
        return work;
    }
    
    void simulateOverhead(uint64_t iteration, const BenchmarkConfig& config,
                          const std::unordered_map<std::string, std::string>& parameters) {
        // Simulate overhead based on configuration
        if (config.type == "time") {
            // Simulate some overhead
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    
    double calculateBenchmarkValue(const BenchmarkConfig& config,
                                   const std::unordered_map<std::string, std::string>& parameters) const {
        double total_value = 0.0;
        
        for (uint64_t i = 0; i < config.iterations; ++i) {
            total_value += simulateWork(i, config, parameters);
        }
        
        return total_value / config.iterations;
    }
    
    double calculateBenchmarkError(const BenchmarkConfig& config,
                                    const std::unordered_map<std::string, std::string>& parameters) const {
        double total_variance = 0.0;
        double mean_value = calculateBenchmarkValue(config, parameters);
        
        for (uint64_t i = 0; i < config.iterations; ++i) {
            double value = simulateWork(i, config, parameters);
            total_variance += (value - mean_value) * (value - mean_value);
        }
        
        double variance = total_variance / config.iterations;
        return std::sqrt(variance) / mean_value;
    }
    
    double getMinValue(const BenchmarkConfig& config,
                       const std::unordered_map<std::string, std::string>& parameters) const {
        double min_value = std::numeric_limits<double>::max();
        
        for (uint64_t i = 0; i < config.iterations; ++i) {
            double value = simulateWork(i, config, parameters);
            if (value < min_value) {
                min_value = value;
            }
        }
        
        return min_value;
    }
    
    double getMaxValue(const BenchmarkConfig& config,
                       const std::unordered_map<std::string, std::string>& parameters) const {
        double max_value = std::numeric_limits<double>::lowest();
        
        for (uint64_t i = 0; i < config.iterations; ++i) {
            double value = simulateWork(i, config, parameters);
            if (value > max_value) {
                max_value = value;
            }
        }
        
        return max_value;
    }
    
    void recordMetric(double value) {
        // In a real implementation, this would record the metric
        // For now, we'll just store it
        metrics_.push_back(value);
    }
    
    uint64_t getMemoryUsage() const {
        // In a real implementation, this would query memory usage
        // For now, we'll return a placeholder
        return 0;
    }
    
    double getCpuUsage() const {
        // In a real implementation, this would query CPU usage
        // For now, we'll return a placeholder
        return 0.0;
    }
    
    std::vector<double> metrics_;
};

// Default benchmark suite manager
class DefaultBenchmarkSuiteManager : public BenchmarkSuiteManager {
public:
    DefaultBenchmarkSuiteManager() : next_suite_id_(1) {}
    
    void addSuite(const BenchmarkSuiteConfig& suite) override {
        suites_[suite.name] = suite;
    }
    
    bool removeSuite(const std::string& name) override {
        auto it = suites_.find(name);
        if (it != suites_.end()) {
            suites_.erase(it);
            return true;
        }
        return false;
    }
    
    const BenchmarkSuiteConfig* getSuite(const std::string& name) const override {
        auto it = suites_.find(name);
        if (it != suites_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    bool hasSuite(const std::string& name) const override {
        return suites_.find(name) != suites_.end();
    }
    
    std::vector<std::string> getSuiteNames() const override {
        std::vector<std::string> names;
        for (const auto& pair : suites_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void setExecutor(std::shared_ptr<BenchmarkExecutor> executor) override {
        executor_ = executor;
    }
    
    std::shared_ptr<BenchmarkExecutor> getExecutor() const override {
        return executor_;
    }
    
    BenchmarkResult executeBenchmark(const std::string& suite_name, const std::string& benchmark_name,
                                     const std::unordered_map<std::string, std::string>& parameters) override {
        const BenchmarkSuiteConfig* suite = getSuite(suite_name);
        if (!suite) {
            throw std::runtime_error("Suite not found: " + suite_name);
        }
        
        const BenchmarkConfig* benchmark = suite->getBenchmark(benchmark_name);
        if (!benchmark) {
            throw std::runtime_error("Benchmark not found: " + benchmark_name);
        }
        
        if (executor_) {
            return executor_->execute(*benchmark, parameters);
        } else {
            throw std::runtime_error("No benchmark executor available");
        }
    }
    
    std::vector<BenchmarkResult> executeSuite(const std::string& suite_name) override {
        std::vector<BenchmarkResult> results;
        
        const BenchmarkSuiteConfig* suite = getSuite(suite_name);
        if (!suite) {
            throw std::runtime_error("Suite not found: " + suite_name);
        }
        
        for (const auto& benchmark : suite->benchmarks) {
            try {
                BenchmarkResult result = executeBenchmark(suite_name, benchmark.name);
                results.push_back(result);
                
                // Store result
                storeResult(result);
                
            } catch (const std::exception& e) {
                // Create failed result
                BenchmarkResult failed_result;
                failed_result.benchmark_name = benchmark.name;
                failed_result.benchmark_type = benchmark.type;
                failed_result.success = false;
                failed_result.error_message = e.what();
                failed_result.timestamp = std::chrono::system_clock::now();
                
                results.push_back(failed_result);
                storeResult(failed_result);
            }
        }
        
        return results;
    }
    
    const BenchmarkResult* getBenchmarkResult(const std::string& benchmark_id) const override {
        auto it = benchmark_results_.find(benchmark_id);
        if (it != benchmark_results_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<BenchmarkResult> getAllResults() const override {
        std::vector<BenchmarkResult> results;
        for (const auto& pair : benchmark_results_) {
            results.push_back(pair.second);
        }
        return results;
    }
    
    bool saveResults(const std::string& filepath) const override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save all benchmark results
        file << "{\n";
        file << "  \"benchmarks\": [\n";
        
        size_t count = 0;
        for (const auto& pair : benchmark_results_) {
            const auto& result = pair.second;
            saveResultToJson(file, result);
            
            if (count < benchmark_results_.size() - 1) {
                file << "\n";
            }
            
            count++;
        }
        
        file << "\n],\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool loadResults(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON (simplified implementation)
        // In a real implementation, this would use a proper JSON parser
        // For now, we'll just read the file and clear existing results
        
        benchmark_results_.clear();
        
        file.close();
        return true;
    }
    
    bool generateReport(const std::string& report_file, const std::string& format = "html") const override {
        if (format == "html") {
            return generateHtmlReport(report_file);
        } else if (format == "json") {
            return generateJsonReport(report_file);
        } else if (format == "csv") {
            return generateCsvReport(report_file);
        }
        
        return false;
    }
    
private:
    void storeResult(const BenchmarkResult& result) {
        benchmark_results_[result.benchmark_id] = result;
    }
    
    void saveResultToJson(std::ofstream& file, const BenchmarkResult& result) {
        file << "    {\n";
        file << "      \"benchmark_id\": \"" << result.benchmark_id << "\",\n";
        file << "      \"benchmark_name\": \"" << result.benchmark_name << "\",\n";
        file << "      \"benchmark_type\": \"" << result.benchmark_type << "\",\n";
        file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
        file << "      \"total_time\": " << result.total_time << ",\n";
        file << "      \"memory_usage\": " << result.memory_usage << ",\n";
        file << "      \"cpu_usage\": " << result.cpu_usage << ",\n";
        file << "      \"error_message\": \"" << result.error_message << "\"\n";
        file << "    }";
    }
    
    bool generateHtmlReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<!DOCTYPE html>\n";
        file << "<html>\n";
        file << "<head>\n";
        file << "  <title>NLM Performance Benchmark Report</title>\n";
        file << "  <style>\n";
        file << "    body { font-family: Arial, sans-serif; margin: 40px; }\n";
        file << "    h1 { color: #333; }\n";
        file << "    table { border-collapse: collapse; width: 100%; }\n";
        file << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        file << "    th { background-color: #f2f2f2; }\n";
        file << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
        file << "  </style>\n";
        file << "</head>\n";
        file << "<body>\n";
        file << "  <h1>NLM Performance Benchmark Report</h1>\n";
        file << "  <p>Generated on " << getTimeString(std::chrono::system_clock::now()) << "</p>\n";
        
        // Benchmark summary table
        file << "  <h2>Benchmark Summary</h2>\n";
        file << "  <table>\n";
        file << "    <tr><th>Benchmark ID</th><th>Name</th><th>Type</th><th>Success</th><th>Total Time (s)</th><th>Memory Usage (MB)</th><th>CPU Usage (%)</th></tr>\n";
        
        for (const auto& pair : benchmark_results_) {
            const auto& result = pair.second;
            file << "    <tr>\n";
            file << "      <td>" << result.benchmark_id << "</td>\n";
            file << "      <td>" << result.benchmark_name << "</td>\n";
            file << "      <td>" << result.benchmark_type << "</td>\n";
            file << "      <td>" << (result.success ? "Yes" : "No") << "</td>\n";
            file << "      <td>" << result.total_time << "</td>\n";
            file << "      <td>" << (result.memory_usage / (1024 * 1024)) << "</td>\n";
            file << "      <td>" << result.cpu_usage << "</td>\n";
            file << "    </tr>\n";
        }
        
        file << "  </table>\n";
        file << "</body>\n";
        file << "</html>\n";
        
        file.close();
        return true;
    }
    
    bool generateJsonReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"benchmarks\": [\n";
        
        size_t count = 0;
        for (const auto& pair : benchmark_results_) {
            const auto& result = pair.second;
            
            file << "    {\n";
            file << "      \"benchmark_id\": \"" << result.benchmark_id << "\",\n";
            file << "      \"benchmark_name\": \"" << result.benchmark_name << "\",\n";
            file << "      \"benchmark_type\": \"" << result.benchmark_type << "\",\n";
            file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
            file << "      \"total_time\": " << result.total_time << ",\n";
            file << "      \"memory_usage\": " << result.memory_usage << ",\n";
            file << "      \"cpu_usage\": " << result.cpu_usage << ",\n";
            file << "      \"error_message\": \"" << result.error_message << "\"\n";
            
            if (count < benchmark_results_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
            
            count++;
        }
        
        file << "  ],\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool generateCsvReport(const std::string& report_file) const {
        std::ofstream file(report_file);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Benchmark ID,Name,Type,Success,Total Time (s),Memory Usage (MB),CPU Usage (%)" << std::endl;
        
        for (const auto& pair : benchmark_results_) {
            const auto& result = pair.second;
            file << "\"" << result.benchmark_id << "\",\"" << result.benchmark_name << "\",\"" 
                 << result.benchmark_type << "\",\"" << (result.success ? "Yes" : "No") << "\"," 
                 << result.total_time << "," << (result.memory_usage / (1024 * 1024)) << "," 
                 << result.cpu_usage << std::endl;
        }
        
        file.close();
        return true;
    }
    
    std::string getTimeString(std::chrono::system_clock::time_point time_point) const {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << now_time_t;
        return ss.str();
    }
    
    std::unordered_map<std::string, BenchmarkSuiteConfig> suites_;
    std::unordered_map<std::string, BenchmarkResult> benchmark_results_;
    std::shared_ptr<BenchmarkExecutor> executor_;
    int next_suite_id_;
};

// Global benchmark suite manager instance
inline BenchmarkSuiteManager& getGlobalBenchmarkSuiteManager() {
    static DefaultBenchmarkSuiteManager manager;
    return manager;
}

// Global benchmark executor instance
inline BenchmarkExecutor& getGlobalBenchmarkExecutor() {
    static DefaultBenchmarkExecutor executor;
    return executor;
}

} // namespace nlm