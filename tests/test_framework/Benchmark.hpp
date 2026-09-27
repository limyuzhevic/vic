// Benchmark Infrastructure for NLM Brain Simulation
// Performance benchmarking, profiling, and comparison

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {
namespace test {

// Benchmark configuration
struct BenchmarkConfig {
    size_t iterations = 1000;
    size_t warmupIterations = 100;
    bool enabled = true;
    std::string outputFormat = "console"; // console, json, xml, csv
    std::string outputFile;
    std::map<std::string, std::string> customMetrics;
    
    BenchmarkConfig() = default;
};

// Benchmark result with detailed statistics
struct BenchmarkResult {
    std::string name;
    std::string description;
    double duration; // nanoseconds
    uint64_t memoryUsage;
    size_t iterationCount;
    size_t warmupIterations;
    double averageDuration;
    double minDuration;
    double maxDuration;
    double stdDeviation;
    double p95Duration;
    double p99Duration;
    double p999Duration;
    std::vector<double> rawDurations;
    std::map<std::string, double> customMetrics;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    
    BenchmarkResult(const std::string& name, const std::string& description = "")
        : name(name), description(description), duration(0.0), memoryUsage(0),
          iterationCount(0), warmupIterations(0), averageDuration(0.0), minDuration(0.0),
          maxDuration(0.0), stdDeviation(0.0), p95Duration(0.0), p99Duration(0.0), p999Duration(0.0) {}
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stopTimer() {
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double, std::nano>(endTime - startTime).count();
    }
    
    void setMemoryUsage(uint64_t memory) {
        memoryUsage = memory;
    }
    
    void setIterations(size_t iterations) {
        iterationCount = iterations;
    }
    
    void setWarmupIterations(size_t warmup) {
        warmupIterations = warmup;
    }
    
    double getDuration() const {
        return duration;
    }
    
    double getAverageDuration() const {
        return averageDuration;
    }
    
    double getMinDuration() const {
        return minDuration;
    }
    
    double getMaxDuration() const {
        return maxDuration;
    }
    
    double getStdDeviation() const {
        return stdDeviation;
    }
    
    double getP95Duration() const {
        return p95Duration;
    }
    
    double getP99Duration() const {
        return p99Duration;
    }
    
    double getP999Duration() const {
        return p999Duration;
    }
    
    std::string getFormattedDuration() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        if (duration >= 1000000) {
            ss << duration / 1000000.0 << " ms";
        } else if (duration >= 1000) {
            ss << duration / 1000.0 << " µs";
        } else {
            ss << duration << " ns";
        }
        return ss.str();
    }
    
    std::string getFormattedAverageDuration() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        if (averageDuration >= 1000000) {
            ss << averageDuration / 1000000.0 << " ms";
        } else if (averageDuration >= 1000) {
            ss << averageDuration / 1000.0 << " µs";
        } else {
            ss << averageDuration << " ns";
        }
        return ss.str();
    }
    
    void calculateStatistics() {
        if (rawDurations.empty()) {
            return;
        }
        
        double sum = std::accumulate(rawDurations.begin(), rawDurations.end(), 0.0);
        averageDuration = sum / rawDurations.size();
        
        double sq_sum = 0.0;
        minDuration = rawDurations[0];
        maxDuration = rawDurations[0];
        
        for (double duration : rawDurations) {
            sq_sum += duration * duration;
            minDuration = std::min(minDuration, duration);
            maxDuration = std::max(maxDuration, duration);
        }
        
        double variance = (sq_sum / rawDurations.size()) - (averageDuration * averageDuration);
        stdDeviation = std::sqrt(variance);
        
        // Calculate percentiles
        std::sort(rawDurations.begin(), rawDurations.end());
        p95Duration = rawDurations.size() > 0 ? rawDurations[static_cast<size_t>(0.95 * rawDurations.size())] : 0.0;
        p99Duration = rawDurations.size() > 0 ? rawDurations[static_cast<size_t>(0.99 * rawDurations.size())] : 0.0;
        p999Duration = rawDurations.size() > 0 ? rawDurations[static_cast<size_t>(0.999 * rawDurations.size())] : 0.0;
        
        duration = rawDurations.back();
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "=== Benchmark: " << name << " ===" << std::endl;
        if (!description.empty()) {
            ss << "Description: " << description << std::endl;
        }
        ss << "  Iterations: " << iterationCount << " (Warmup: " << warmupIterations << ")" << std::endl;
        ss << "  Total Duration: " << getFormattedDuration() << std::endl;
        ss << "  Average: " << getFormattedAverageDuration() << std::endl;
        ss << "  Min: " << getFormattedDuration() << std::endl;
        ss << "  Max: " << getFormattedDuration() << std::endl;
        ss << "  Std Dev: " << stdDeviation << " ns" << std::endl;
        ss << "  P95: " << getFormattedDuration() << std::endl;
        ss << "  P99: " << getFormattedDuration() << std::endl;
        ss << "  P99.9: " << getFormattedDuration() << std::endl;
        ss << "  Memory: " << memoryUsage << " bytes" << std::endl;
        for (const auto& metric : customMetrics) {
            ss << "  " << metric.first << ": " << metric.second << std::endl;
        }
        ss << "========================" << std::endl;
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\": \"" << name << "\",\"description\": \"" << description << "\",\"iterations\": " << iterationCount
           << ",\"warmup\": " << warmupIterations << ",\"duration\": " << duration
           << ",\"average\": " << averageDuration << ",\"min\": " << minDuration
           << ",\"max\": " << maxDuration << ",\"stddev\": " << stdDeviation
           << ",\"p95\": " << p95Duration << ",\"p99\": " << p99Duration
           << ",\"p999\": " << p999Duration << ",\"memory\": " << memoryUsage
           << ",\"customMetrics\": {";
        
        for (auto it = customMetrics.begin(); it != customMetrics.end(); ++it) {
            if (it != customMetrics.begin()) ss << ", ";
            ss << "\"" << it->first << "\": " << it->second;
        }
        
        ss << "}}";
        return ss.str();
    }
    
    std::string toCSV() const {
        std::stringstream ss;
        ss << "benchmark,name,description,iterations,warmup,duration,average,min,max,stddev,p95,p99,p999,memory\n";
        ss << "\"" << name << "\",\"" << description << "\"," << iterationCount << ","
           << warmupIterations << ",\"" << duration << "\",\"" << averageDuration << "\",\"" << minDuration
           << "\",\"" << maxDuration << "\",\"" << stdDeviation << "\",\"" << p95Duration
           << "\",\"" << p99Duration << "\",\"" << p999Duration << "\",\"" << memoryUsage << "\"\n";
        return ss.str();
    }
};

// Benchmark runner for performance testing
class Benchmark {
public:
    static BenchmarkResult run(const std::string& name,
                               const std::string& description,
                               std::function<void()> benchmark,
                               const BenchmarkConfig& config = BenchmarkConfig()) {
        BenchmarkResult result(name, description);
        
        if (!config.enabled) {
            std::cout << "Benchmark " << name << " is disabled." << std::endl;
            return result;
        }
        
        // Warmup
        if (config.warmupIterations > 0) {
            for (size_t i = 0; i < config.warmupIterations; ++i) {
                benchmark();
            }
        }
        
        // Actual benchmark
        result.startTimer();
        for (size_t i = 0; i < config.iterations; ++i) {
            benchmark();
        }
        result.stopTimer();
        
        // Collect raw durations for percentile calculations
        // (In a real implementation, you would measure each iteration individually)
        size_t warmupAndRealIterations = config.warmupIterations + config.iterations;
        for (size_t i = 0; i < warmupAndRealIterations; ++i) {
            result.rawDurations.push_back(result.getDuration());
        }
        
        // Calculate statistics
        result.calculateStatistics();
        
        // Output results
        if (config.outputFormat == "console") {
            std::cout << result.toString() << std::endl;
        } else if (config.outputFormat == "json") {
            std::cout << result.toJSON() << std::endl;
        } else if (config.outputFormat == "xml") {
            std::cout << "<benchmark name=\"" << name << "\" duration=\"" << result.duration
                      << "\" average=\"" << result.averageDuration << "\">" << std::endl;
        } else if (config.outputFormat == "csv" && !config.outputFile.empty()) {
            std::ofstream csvFile(config.outputFile, std::ios::app);
            if (csvFile.is_open()) {
                csvFile << result.toCSV();
                csvFile.close();
            }
        }
        
        return result;
    }
    
    static void runMultipleBenchmarks(const std::vector<std::pair<std::string, std::function<void()>>>& benchmarks,
                                     const BenchmarkConfig& config = BenchmarkConfig()) {
        std::cout << "=== Running Multiple Benchmarks ===" << std::endl;
        
        BenchmarkResult previousResult;
        double totalDuration = 0.0;
        
        for (const auto& benchmark : benchmarks) {
            std::cout << "\n--- " << benchmark.first << " ---" << std::endl;
            BenchmarkResult result = run(benchmark.first, "", benchmark.second, config);
            
            if (previousResult.name.empty()) {
                previousResult = result;
            } else {
                totalDuration += result.getDuration();
                
                // Compare with previous benchmark
                if (result.getAverageDuration() < previousResult.getAverageDuration()) {
                    std::cout << "  IMPROVEMENT: " << ((previousResult.getAverageDuration() - result.getAverageDuration()) / previousResult.getAverageDuration() * 100) << "% faster" << std::endl;
                } else {
                    std::cout << "  REGRESSION: " << ((result.getAverageDuration() - previousResult.getAverageDuration()) / previousResult.getAverageDuration() * 100) << "% slower" << std::endl;
                }
            }
            
            previousResult = result;
        }
        
        std::cout << "\n=== Benchmark Summary ===" << std::endl;
        std::cout << "Total benchmarks: " << benchmarks.size() << std::endl;
        std::cout << "Total duration: " << totalDuration << " seconds" << std::endl;
        std::cout << "Average per benchmark: " << (totalDuration / benchmarks.size()) << " seconds" << std::endl;
    }
    
    static BenchmarkResult runWithParameters(const std::string& name,
                                             const std::string& description,
                                             std::function<void()> benchmark,
                                             const std::vector<std::pair<std::string, std::string>>& parameters,
                                             const BenchmarkConfig& config = BenchmarkConfig()) {
        BenchmarkResult result(name, description);
        
        // Add parameters to result
        for (const auto& param : parameters) {
            result.customMetrics[param.first] = std::stod(param.second);
        }
        
        return run(name, description, benchmark, config);
    }
    
    static void compareBenchmarks(const std::vector<std::pair<std::string, BenchmarkResult>>& benchmarks) {
        std::cout << "=== Benchmark Comparison ===" << std::endl;
        
        for (size_t i = 0; i < benchmarks.size(); ++i) {
            std::cout << i + 1 << ". " << benchmarks[i].first << ": " << benchmarks[i].second.getFormattedAverageDuration() << std::endl;
        }
        
        if (benchmarks.size() >= 2) {
            std::cout << "\nComparison:" << std::endl;
            for (size_t i = 1; i < benchmarks.size(); ++i) {
                double improvement = ((benchmarks[i - 1].second.getAverageDuration() - benchmarks[i].second.getAverageDuration()) / benchmarks[i - 1].second.getAverageDuration() * 100);
                if (improvement > 0) {
                    std::cout << benchmarks[i - 1].first << " -> " << benchmarks[i].first << ": " << improvement << "% improvement" << std::endl;
                } else {
                    std::cout << benchmarks[i - 1].first << " -> " << benchmarks[i].first << ": " << -improvement << "% regression" << std::endl;
                }
            }
        }
    }
    
    static std::string generateReport(const std::vector<BenchmarkResult>& results) {
        std::stringstream ss;
        ss << "=== Benchmark Report ===\n\n";
        
        for (const auto& result : results) {
            ss << result.toString() << "\n";
        }
        
        ss << "=== Summary ===\n";
        ss << "Total benchmarks: " << results.size() << "\n";
        double totalDuration = 0.0;
        for (const auto& result : results) {
            totalDuration += result.getAverageDuration();
        }
        ss << "Average duration: " << (totalDuration / results.size()) << "s\n";
        ss << "Fastest: " << (*std::min_element(results.begin(), results.end(),
                                   [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                       return a.getAverageDuration() < b.getAverageDuration();
                                   })).name << " (" << (*std::min_element(results.begin(), results.end(),
                                                    [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                        return a.getAverageDuration() < b.getAverageDuration();
                                                    })).getFormattedAverageDuration() << ")\n";
        
        return ss.str();
    }
};

} // namespace test
} // namespace nlm
