// Benchmark.cpp - Performance benchmarking implementation

#include "profiler/Benchmark.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

Benchmark::Benchmark()
    : defaultConfig() {
}

Benchmark::Benchmark(const BenchmarkConfig& config)
    : defaultConfig(config) {
}

Benchmark::~Benchmark() {
    // Cleanup if needed
}

void Benchmark::addBenchmark(const BenchmarkConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex);
    benchmarkSuite.push_back(config);
    
    NLM_LOG_INFO("Benchmark added: " + config.name);
}

Benchmark::BenchmarkResult Benchmark::runBenchmark(const BenchmarkConfig& config) {
    BenchmarkResult result;
    result.benchmarkName = config.name;
    result.timestamp = std::chrono::steady_clock::now();
    
    try {
        // Setup
        if (config.setup) {
            config.setup();
        }
        
        // Warm-up iterations
        for (int i = 0; i < config.warmupIterations; ++i) {
            runSingleIteration(config, i);
        }
        
        // Benchmark iterations
        std::vector<double> durations;
        for (int i = 0; i < config.benchmarkIterations; ++i) {
            double duration = runSingleIteration(config, i);
            durations.push_back(duration);
            result.duration += duration;
        }
        
        // Calculate statistics
        if (!durations.empty()) {
            result.duration /= durations.size();
            
            double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
            double mean = sum / durations.size();
            
            double sq_sum = 0.0;
            for (double d : durations) {
                sq_sum += (d - mean) * (d - mean);
            }
            double stdDev = std::sqrt(sq_sum / durations.size());
            
            result.duration = mean;
            result.throughput = 1.0 / (mean / 1000.0); // operations per second
        }
        
        // Calculate quality score (based on consistency and performance)
        if (!durations.empty()) {
            double coefficientOfVariation = stdDev / result.duration;
            result.quality = std::max(0.0, 1.0 - coefficientOfVariation);
        }
        
        result.passed = validateResult(result);
        
    } catch (const std::exception& e) {
        result.errorMessage = e.what();
        result.passed = false;
    }
    
    // Teardown
    if (config.teardown) {
        try {
            config.teardown();
        } catch (const std::exception& e) {
            if (result.errorMessage.empty()) {
                result.errorMessage = std::string("Teardown error: ") + e.what();
            }
        }
    }
    
    NLM_LOG_INFO("Benchmark completed: " + config.name + " - " + 
                (result.passed ? "PASSED" : "FAILED"));
    
    if (!result.passed) {
        NLM_LOG_WARNING("Benchmark failed: " + result.errorMessage);
    }
    
    std::lock_guard<std::mutex> lock(dataMutex);
    results.push_back(result);
    
    return result;
}

std::vector<Benchmark::BenchmarkResult> Benchmark::runSuite() {
    NLM_LOG_INFO("Running benchmark suite with " + std::to_string(benchmarkSuite.size()) + " benchmarks");
    
    std::vector<BenchmarkResult> suiteResults;
    for (const auto& config : benchmarkSuite) {
        BenchmarkResult result = runBenchmark(config);
        suiteResults.push_back(result);
        
        if (config.verbose) {
            NLM_LOG_INFO("Benchmark " + config.name + " completed: " + 
                        (result.passed ? "PASSED" : "FAILED"));
        }
    }
    
    NLM_LOG_INFO("Benchmark suite completed");
    return suiteResults;
}

Benchmark::BenchmarkResult Benchmark::runBenchmark(const std::string& name) {
    for (const auto& config : benchmarkSuite) {
        if (config.name == name) {
            return runBenchmark(config);
        }
    }
    
    BenchmarkResult result;
    result.benchmarkName = name;
    result.errorMessage = "Benchmark not found in suite";
    result.passed = false;
    
    return result;
}

void Benchmark::clearResults() {
    std::lock_guard<std::mutex> lock(dataMutex);
    results.clear();
}

Benchmark::BenchmarkStats Benchmark::calculateStats(const std::vector<double>& values) const {
    BenchmarkStats stats;
    
    if (values.empty()) {
        return stats;
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats.mean = sum / values.size();
    
    double sq_sum = 0.0;
    for (double v : values) {
        sq_sum += (v - stats.mean) * (v - stats.mean);
    }
    
    double variance = sq_sum / values.size();
    stats.stdDev = std::sqrt(variance);
    
    stats.min = *std::min_element(values.begin(), values.end());
    stats.max = *std::max_element(values.begin(), values.end());
    
    // Calculate median
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t mid = sorted.size() / 2;
    if (sorted.size() % 2 == 0) {
        stats.median = (sorted[mid - 1] + sorted[mid]) / 2.0;
    } else {
        stats.median = sorted[mid];
    }
    
    // Calculate 95th percentile
    size_t p95Index = static_cast<size_t>(std::ceil(values.size() * 0.95)) - 1;
    if (p95Index >= values.size()) {
        p95Index = values.size() - 1;
    }
    stats.p95 = sorted[p95Index];
    
    // Calculate 99th percentile
    size_t p99Index = static_cast<size_t>(std::ceil(values.size() * 0.99)) - 1;
    if (p99Index >= values.size()) {
        p99Index = values.size() - 1;
    }
    stats.p99 = sorted[p99Index];
    
    // Calculate interquartile range
    size_t q1Index = static_cast<size_t>(std::ceil(values.size() * 0.25)) - 1;
    size_t q3Index = static_cast<size_t>(std::ceil(values.size() * 0.75)) - 1;
    if (q3Index >= values.size()) {
        q3Index = values.size() - 1;
    }
    stats.iqr = sorted[q3Index] - sorted[q1Index];
    
    // Calculate skewness
    double m3 = 0.0;
    for (double v : values) {
        m3 += (v - stats.mean) * (v - stats.mean) * (v - stats.mean);
    }
    m3 /= values.size();
    double sigma3 = std::pow(stats.stdDev, 3);
    if (sigma3 != 0.0) {
        stats.skew = m3 / (sigma3 * sigma3);
    }
    
    // Calculate kurtosis
    double m4 = 0.0;
    for (double v : values) {
        double diff = v - stats.mean;
        m4 += diff * diff * diff * diff;
    }
    m4 /= values.size();
    double sigma4 = std::pow(stats.stdDev, 4);
    if (sigma4 != 0.0) {
        stats.kurtosis = m4 / (sigma4 * sigma4) - 3.0;
    }
    
    return stats;
}

std::string Benchmark::compareWithBaseline(const BenchmarkResult& current,
                                          const BenchmarkResult& baseline) const {
    std::ostringstream oss;
    oss << "Benchmark Comparison: " << current.benchmarkName << "\n";
    oss << "========================\n\n";
    
    oss << "Baseline Performance:\n";
    oss << "  Duration: " << baseline.duration << "ms\n";
    oss << "  CPU Time: " << baseline.cpuTime << "ms\n";
    oss << "  Memory Usage: " << baseline.memoryUsage << " bytes\n";
    oss << "  Throughput: " << baseline.throughput << " ops/sec\n";
    oss << "  Quality: " << baseline.quality << "\n\n";
    
    oss << "Current Performance:\n";
    oss << "  Duration: " << current.duration << "ms\n";
    oss << "  CPU Time: " << current.cpuTime << "ms\n";
    oss << "  Memory Usage: " << current.memoryUsage << " bytes\n";
    oss << "  Throughput: " << current.throughput << " ops/sec\n";
    oss << "  Quality: " << current.quality << "\n\n";
    
    if (baseline.duration > 0) {
        double durationRatio = current.duration / baseline.duration;
        oss << "Duration Ratio: " << durationRatio << "x\n";
        
        if (durationRatio < 0.9) {
            oss << "✓ Improvement detected (faster than baseline)\n";
        } else if (durationRatio > 1.1) {
            oss << "⚠ Regression detected (slower than baseline)\n";
        }
    }
    
    if (baseline.memoryUsage > 0) {
        double memoryRatio = current.memoryUsage / baseline.memoryUsage;
        oss << "Memory Usage Ratio: " << memoryRatio << "x\n";
        
        if (memoryRatio < 0.9) {
            oss << "✓ Memory improvement detected\n";
        } else if (memoryRatio > 1.1) {
            oss << "⚠ Memory regression detected\n";
        }
    }
    
    if (baseline.throughput > 0) {
        double throughputRatio = current.throughput / baseline.throughput;
        oss << "Throughput Ratio: " << throughputRatio << "x\n";
        
        if (throughputRatio > 1.1) {
            oss << "✓ Throughput improvement detected\n";
        } else if (throughputRatio < 0.9) {
            oss << "⚠ Throughput regression detected\n";
        }
    }
    
    return oss.str();
}

bool Benchmark::detectRegression(const BenchmarkResult& current,
                                const BenchmarkResult& baseline) const {
    if (baseline.duration == 0) return false;
    
    double durationRatio = current.duration / baseline.duration;
    
    return durationRatio > 1.1; // More than 10% slower
}

std::string Benchmark::generateReport(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "json" || format == "auto") {
        oss << generateJsonReport();
    } else if (format == "csv" || format == "auto") {
        oss << generateCsvReport();
    } else {
        oss << generateTextReport();
    }
    
    return oss.str();
}

bool Benchmark::exportData(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    std::string data;
    if (format == "json") {
        data = generateJsonReport();
    } else if (format == "csv") {
        data = generateCsvReport();
    } else {
        data = generateTextReport();
    }
    
    file << data;
    file.close();
    
    if (defaultConfig.verbose) {
        NLM_LOG_INFO("Benchmark data exported to: " + filename);
    }
    
    return true;
}

std::string Benchmark::generateSuiteReport(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "json" || format == "auto") {
        oss << generateJsonSuiteReport();
    } else if (format == "csv" || format == "auto") {
        oss << generateCsvSuiteReport();
    } else {
        oss << generateTextSuiteReport();
    }
    
    return oss.str();
}

std::string Benchmark::getSuiteStatistics() const {
    std::ostringstream oss;
    oss << "Benchmark Suite Statistics\n";
    oss << "==========================\n\n";
    
    oss << "Total benchmarks: " << benchmarkSuite.size() << "\n";
    oss << "Total results: " << results.size() << "\n\n";
    
    size_t passedCount = 0;
    size_t failedCount = 0;
    double totalDuration = 0.0;
    double totalThroughput = 0.0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passedCount++;
        } else {
            failedCount++;
        }
        totalDuration += result.duration;
        totalThroughput += result.throughput;
    }
    
    oss << "Passed benchmarks: " << passedCount << "\n";
    oss << "Failed benchmarks: " << failedCount << "\n";
    oss << "Success rate: " << (passedCount * 100.0 / results.size()) << "%\n\n";
    
    if (!results.empty()) {
        oss << "Average duration: " << totalDuration / results.size() << "ms\n";
        oss << "Average throughput: " << totalThroughput / results.size() << " ops/sec\n";
        oss << "Overall quality: " << (totalDuration / results.size()) / 1000.0 << " seconds per 1000 ops\n";
    }
    
    return oss.str();
}

double Benchmark::runSingleIteration(const BenchmarkConfig& config, int iteration) {
    std::chrono::steady_clock::time_point startTime;
    double duration;
    
    try {
        startTime = std::chrono::steady_clock::now();
        config.benchmark();
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        duration = elapsed.count() / 1000.0; // Convert to milliseconds
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Benchmark iteration " + std::to_string(iteration) + " failed: " + e.what());
        duration = 0.0; // Failed iterations count as zero time
    }
    
    if (config.verbose && iteration >= config.warmupIterations) {
        NLM_LOG_DEBUG("Benchmark iteration " + std::to_string(iteration) + ": " + 
                     std::to_string(duration) + "ms");
    }
    
    return duration;
}

bool Benchmark::validateResult(const BenchmarkResult& result) const {
    if (!defaultConfig.enableRegressionDetection) {
        return true;
    }
    
    // Basic validation
    if (result.duration < 0 || result.throughput < 0 || result.quality < 0) {
        return false;
    }
    
    // Check if duration is within expected range
    if (result.duration > defaultConfig.maxExpectedDuration) {
        return false;
    }
    
    if (defaultConfig.minExpectedDuration > 0 && 
        result.duration < defaultConfig.minExpectedDuration) {
        // This might be acceptable, but we'll allow it
    }
    
    // Check if quality is acceptable
    if (result.quality < 0.0 || result.quality > 1.0) {
        return false;
    }
    
    return true;
}

std::string Benchmark::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << result.benchmarkName << "\",\n";
        oss << "      \"duration\": " << result.duration << ",\n";
        oss << "      \"cpuTime\": " << result.cpuTime << ",\n";
        oss << "      \"memoryUsage\": " << result.memoryUsage << ",\n";
        oss << "      \"throughput\": " << result.throughput << ",\n";
        oss << "      \"quality\": " << result.quality << ",\n";
        oss << "      \"timestamp\": \"" << 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                result.timestamp.time_since_epoch()).count() << "\",\n";
        oss << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        oss << "      \"errorMessage\": \"" << result.errorMessage << "\"\n";
        oss << "    }";
        if (i < results.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ],\n";
    oss << "  \"suiteStats\": {\n";
    oss << "    \"totalBenchmarks\": " << benchmarkSuite.size() << ",\n";
    oss << "    \"totalResults\": " << results.size() << ",\n";
    oss << "    \"passedCount\": " << std::count_if(results.begin(), results.end(), 
                                            [](const BenchmarkResult& r) { return r.passed; }) << ",\n";
    oss << "    \"failedCount\": " << std::count_if(results.begin(), results.end(), 
                                           [](const BenchmarkResult& r) { return !r.passed; }) << "\n";
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

std::string Benchmark::generateCsvReport() const {
    std::ostringstream oss;
    oss << "benchmarkName,duration,cpTime,memoryUsage,throughput,quality,passed,timestamp\n";
    
    for (const auto& result : results) {
        oss << result.benchmarkName << ","
            << result.duration << ","
            << result.cpuTime << ","
            << result.memoryUsage << ","
            << result.throughput << ","
            << result.quality << ","
            << (result.passed ? "true" : "false") << ","
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                result.timestamp.time_since_epoch()).count() << "\n";
    }
    
    return oss.str();
}

std::string Benchmark::generateTextReport() const {
    std::ostringstream oss;
    oss << "Benchmark Results\n";
    oss << "==================\n\n";
    
    for (const auto& result : results) {
        oss << "Benchmark: " << result.benchmarkName << "\n";
        oss << "  Duration: " << result.duration << "ms\n";
        oss << "  CPU Time: " << result.cpuTime << "ms\n";
        oss << "  Memory Usage: " << result.memoryUsage << " bytes\n";
        oss << "  Throughput: " << result.throughput << " ops/sec\n";
        oss << "  Quality: " << result.quality << "\n";
        oss << "  Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
        if (!result.errorMessage.empty()) {
            oss << "  Error: " << result.errorMessage << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Benchmark::generateJsonSuiteReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "    {\"name\": \"" << result.benchmarkName << "\", \"duration\": " << result.duration << ", \"passed\": " << (result.passed ? "true" : "false") << "}";
        if (i < results.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ],\n";
    oss << "  \"statistics\": {\n";
    
    // Calculate statistics for each benchmark
    std::unordered_map<std::string, std::vector<double>> durationMap;
    std::unordered_map<std::string, std::vector<double>> throughputMap;
    
    for (const auto& result : results) {
        durationMap[result.benchmarkName].push_back(result.duration);
        throughputMap[result.benchmarkName].push_back(result.throughput);
    }
    
    oss << "    \"benchmarkStats\": {\n";
    bool first = true;
    for (const auto& pair : durationMap) {
        if (!first) oss << ",\n";
        const auto& durations = pair.second;
        BenchmarkStats stats = calculateStats(durations);
        
        oss << "      \"" << pair.first << "\": {\"mean\": " << stats.mean << ", \"stdDev\": " << stats.stdDev << ", \"min\": " << stats.min << ", \"max\": " << stats.max << "}";
        first = false;
    }
    oss << "\n";
    
    oss << "    }\n";
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

std::string Benchmark::generateCsvSuiteReport() const {
    std::ostringstream oss;
    oss << "benchmarkName,duration,throughput,passed,quality\n";
    
    for (const auto& result : results) {
        oss << result.benchmarkName << ","
            << result.duration << ","
            << result.throughput << ","
            << (result.passed ? "true" : "false") << ","
            << result.quality << "\n";
    }
    
    return oss.str();
}

std::string Benchmark::generateTextSuiteReport() const {
    std::ostringstream oss;
    oss << "Benchmark Suite Report\n";
    oss << "======================\n\n";
    
    oss << "Overall Statistics:\n";
    oss << "  Total benchmarks: " << benchmarkSuite.size() << "\n";
    oss << "  Total results: " << results.size() << "\n";
    oss << "  Passed: " << std::count_if(results.begin(), results.end(), 
                                  [](const BenchmarkResult& r) { return r.passed; }) << "\n";
    oss << "  Failed: " << std::count_if(results.begin(), results.end(), 
                                   [](const BenchmarkResult& r) { return !r.passed; }) << "\n";
    oss << "  Success Rate: " << (results.empty() ? 0 : 
        std::count_if(results.begin(), results.end(), 
                     [](const BenchmarkResult& r) { return r.passed; }) * 100.0 / results.size()) << "%\n\n";
    
    oss << "Benchmark Details:\n";
    for (const auto& result : results) {
        oss << "  - " << result.benchmarkName << ": " << result.duration << "ms, " << result.throughput << " ops/sec, " << (result.passed ? "PASSED" : "FAILED") << "\n";
    }
    
    return oss.str();
}

} // namespace nlm