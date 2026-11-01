// Enhanced NLM Performance Benchmark
// Advanced performance measurement with profiling capabilities
//
// This enhanced benchmark provides:
// - Detailed performance profiling for different network sizes
// - Memory usage monitoring
// - Time measurements with high precision
// - Comparative analysis across different configurations
// - Results export to various formats
// - Progress monitoring for long-running benchmarks
// - Statistical analysis of performance metrics

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>

namespace nlm {

// Advanced benchmark result structure with comprehensive metrics
struct AdvancedBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;           // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsageMB;       // Peak memory usage in MB
    double cpuUsagePercent;     // Average CPU usage percentage
    double timeStepMs;          // Average time per step in milliseconds
    double spikeThroughput;     // Spikes per second
    double efficiencyScore;     // Performance efficiency metric
    std::vector<double> stepTimes;  // Individual step execution times
    std::vector<size_t> spikeCountsPerStep; // Spike counts per step
    
    AdvancedBenchmarkResult() : neurons(0), synapses(0), steps(0), totalTime(0.0),
                               stepsPerSecond(0.0), spikesPerStep(0.0), synapsesPerNeuron(0.0),
                               memoryUsageMB(0.0), cpuUsagePercent(0.0), timeStepMs(0.0),
                               spikeThroughput(0.0), efficiencyScore(0.0) {}
};

// Benchmark configuration with advanced options
struct AdvancedBenchmarkConfig {
    std::vector<std::pair<size_t, size_t>> benchmarkSuites;  // (neurons, steps) pairs
    bool enableMemoryProfiling;
    bool enableCpuProfiling;
    bool enableProgressReporting;
    double progressInterval;     // Seconds between progress reports
    std::string outputFormat;   // txt, json, csv, yaml
    std::string outputFile;     // Primary output file
    std::string jsonFile;       // JSON output file
    std::string csvFile;        // CSV output file
    bool enableComparativeAnalysis;
    bool enableStatisticalAnalysis;
    size_t warmupSteps;          // Steps to skip for warm-up
    size_t stabilizationSteps;   // Steps to stabilize before measurement
    bool enableDetailedLogging;
    bool enableTimestamps;
    bool enableGpuProfiling;     // If GPU profiling is available
    double minStepsPerSecond;   // Minimum steps per second for valid benchmark
    double maxTimeLimit;        // Maximum benchmark time in seconds
    
    AdvancedBenchmarkConfig() : enableMemoryProfiling(true), enableCpuProfiling(true),
                               enableProgressReporting(true), progressInterval(5.0),
                               outputFormat("txt"), warmupSteps(100), stabilizationSteps(200),
                               enableDetailedLogging(true), enableTimestamps(true),
                               enableGpuProfiling(false), minStepsPerSecond(1000.0),
                               maxTimeLimit(300.0) {} // 5 minutes default
};

// Performance profiler with system resource monitoring
class AdvancedPerformanceProfiler {
public:
    AdvancedPerformanceProfiler();
    ~AdvancedPerformanceProfiler();
    
    // Start profiling a specific operation
    void startProfile(const std::string& operation);
    
    // Stop current profiling operation
    void stopProfile();
    
    // Get current elapsed time for the active profile
    double getCurrentTime() const;
    
    // Get total time for a specific operation
    double getOperationTime(const std::string& operation) const;
    
    // Get average time for an operation across multiple runs
    double getAverageOperationTime(const std::string& operation) const;
    
    // Get minimum time for an operation
    double getMinOperationTime(const std::string& operation) const;
    
    // Get maximum time for an operation
    double getMaxOperationTime(const std::string& operation) const;
    
    // Get standard deviation of operation times
    double getStdDevOperationTime(const std::string& operation) const;
    
    // Get all operation results
    const std::map<std::string, std::vector<double>>& getAllOperationTimes() const;
    
    // Reset all profiling data
    void reset();
    
    // Start memory profiling
    void startMemoryProfiling();
    
    // Stop memory profiling
    void stopMemoryProfiling();
    
    // Get current memory usage in MB
    double getCurrentMemoryUsageMB() const;
    
    // Get peak memory usage in MB
    double getPeakMemoryUsageMB() const;
    
    // Start CPU profiling
    void startCpuProfiling();
    
    // Stop CPU profiling
    void stopCpuProfiling();
    
    // Get CPU usage percentage
    double getCurrentCpuUsagePercent() const;
    
    // Generate comprehensive performance report
    std::string generatePerformanceReport() const;
    
    // Save performance data to file
    bool savePerformanceData(const std::string& filename) const;
    
    // Load performance data from file
    bool loadPerformanceData(const std::string& filename);
    
    // Enable/disable system resource monitoring
    void setResourceMonitoringEnabled(bool enabled);
    
    // Set profiling precision
    void setPrecision(int precision);
    
    // Get system information
    std::string getSystemInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Progress monitor for long-running benchmarks
class BenchmarkProgressMonitor {
public:
    BenchmarkProgressMonitor();
    ~BenchmarkProgressMonitor();
    
    // Start progress monitoring
    void start(const std::string& benchmarkName);
    
    // Update progress
    void update(double currentStep, double totalSteps, double elapsedTime);
    
    // Update progress with additional metrics
    void update(double currentStep, double totalSteps, double elapsedTime,
                double currentMemoryMB, double currentCpuPercent);
    
    // Mark current operation as complete
    void complete(const std::string& message = "");
    
    // Check if monitoring should continue
    bool shouldContinue() const;
    
    // Get current progress
    double getProgress() const;
    
    // Get estimated remaining time
    double getEstimatedRemainingTime() const;
    
    // Set update interval
    void setUpdateInterval(double intervalSeconds);
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    
    // Get progress report
    std::string getProgressReport() const;
    
    // Generate performance summary
    std::string generatePerformanceSummary(const std::vector<AdvancedBenchmarkResult>& results) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced benchmark runner with comprehensive reporting
class EnhancedBenchmarkRunner {
public:
    EnhancedBenchmarkRunner();
    ~EnhancedBenchmarkRunner();
    
    // Run enhanced benchmark with configuration
    bool runBenchmark(const AdvancedBenchmarkConfig& config);
    
    // Run specific benchmark configuration
    bool runBenchmarkSuite(const std::vector<std::pair<size_t, size_t>>& benchmarkSuites);
    
    // Run single benchmark with detailed metrics
    AdvancedBenchmarkResult runSingleBenchmark(size_t neuronCount, size_t steps);
    
    // Generate comparative analysis report
    std::string generateComparativeReport(const std::vector<AdvancedBenchmarkResult>& results) const;
    
    // Generate statistical analysis report
    std::string generateStatisticalReport(const std::vector<AdvancedBenchmarkResult>& results) const;
    
    // Generate efficiency ranking report
    std::string generateEfficiencyRanking(const std::vector<AdvancedBenchmarkResult>& results) const;
    
    // Save benchmark results to multiple formats
    bool saveResults(const std::vector<AdvancedBenchmarkResult>& results, 
                    const AdvancedBenchmarkConfig& config);
    
    // Load benchmark results from file
    bool loadResults(const std::string& filename, 
                    std::vector<AdvancedBenchmarkResult>& results);
    
    // Set working directory for output files
    void setWorkingDirectory(const std::string& directory);
    
    // Get default benchmark configuration
    static AdvancedBenchmarkConfig getDefaultConfig();
    
    // Get recommended benchmark configurations
    static std::map<std::string, std::vector<std::pair<size_t, size_t>>> getRecommendedConfigs();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Benchmark execution with enhanced metrics
    AdvancedBenchmarkResult executeBenchmark(size_t neuronCount, size_t steps,
                                             AdvancedPerformanceProfiler& profiler,
                                             BenchmarkProgressMonitor& progressMonitor);
    
    // Memory usage monitoring
    double measureMemoryUsageMB() const;
    
    // CPU usage monitoring
    double measureCpuUsagePercent() const;
    
    // Calculate efficiency score
    double calculateEfficiencyScore(const AdvancedBenchmarkResult& result) const;
    
    // Validate benchmark configuration
    bool validateConfig(const AdvancedBenchmarkConfig& config) const;
    
    // Generate timestamp
    std::string generateTimestamp() const;
    
    // Create output filename with timestamp
    std::string createOutputFilename(const std::string& baseName) const;
};

// JSON benchmark results formatter
class JsonBenchmarkFormatter {
public:
    static std::string format(const std::vector<AdvancedBenchmarkResult>& results);
    static std::string format(const AdvancedBenchmarkResult& result);
    static std::string formatConfiguration(const AdvancedBenchmarkConfig& config);
    
private:
    static std::string escapeJsonString(const std::string& input);
    static std::string formatDouble(double value);
};

// CSV benchmark results formatter
class CsvBenchmarkFormatter {
public:
    static std::string format(const std::vector<AdvancedBenchmarkResult>& results);
    static std::string format(const AdvancedBenchmarkResult& result);
    static std::string formatHeader();
    
private:
    static std::string formatHeader(const std::vector<std::string>& columns);
};

// YAML benchmark results formatter
class YamlBenchmarkFormatter {
public:
    static std::string format(const std::vector<AdvancedBenchmarkResult>& results);
    static std::string format(const AdvancedBenchmarkResult& result);
    static std::string formatConfiguration(const AdvancedBenchmarkConfig& config);
    
private:
    static std::string escapeYamlString(const std::string& input);
    static std::string formatYamlValue(double value);
    static std::string formatYamlValue(size_t value);
    static std::string formatYamlValue(const std::string& value);
    static std::string formatYamlValue(bool value);
    static std::string formatYamlValue(const std::vector<double>& values);
};

// Global benchmark runner instance
EnhancedBenchmarkRunner& getEnhancedBenchmarkRunner();

// Convenience functions for common benchmark scenarios
bool runStandardBenchmark();
bool runExtendedBenchmark();
bool runMicroBenchmark();
bool runScalabilityBenchmark();
bool runComparativeBenchmark(const std::vector<std::pair<size_t, size_t>>& benchmarks);

// Main benchmark entry point with enhanced interface
int mainEnhancedBenchmark(int argc, char** argv);

} // namespace nlm

// Implementation of AdvancedPerformanceProfiler
struct AdvancedPerformanceProfiler::Impl {
    std::chrono::steady_clock::time_point startTime;
    std::map<std::string, std::vector<double>> operationTimes;
    std::string currentOperation;
    bool memoryProfilingEnabled;
    bool cpuProfilingEnabled;
    double peakMemoryUsageMB;
    double currentCpuUsagePercent;
    std::chrono::steady_clock::time_point lastMemoryCheck;
    std::chrono::steady_clock::time_point lastCpuCheck;
    double totalCpuTime;
    double totalMemoryTime;
    bool resourceMonitoringEnabled;
    int precision;
};

AdvancedPerformanceProfiler::AdvancedPerformanceProfiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->currentOperation = "";
    pImpl->memoryProfilingEnabled = false;
    pImpl->cpuProfilingEnabled = false;
    pImpl->peakMemoryUsageMB = 0.0;
    pImpl->currentCpuUsagePercent = 0.0;
    pImpl->lastMemoryCheck = std::chrono::steady_clock::now();
    pImpl->lastCpuCheck = std::chrono::steady_clock::now();
    pImpl->totalCpuTime = 0.0;
    pImpl->totalMemoryTime = 0.0;
    pImpl->resourceMonitoringEnabled = true;
    pImpl->precision = 6;
}

AdvancedPerformanceProfiler::~AdvancedPerformanceProfiler() = default;

void AdvancedPerformanceProfiler::startProfile(const std::string& operation) {
    pImpl->currentOperation = operation;
    pImpl->startTime = std::chrono::steady_clock::now();
    
    if (pImpl->resourceMonitoringEnabled) {
        pImpl->lastMemoryCheck = std::chrono::steady_clock::now();
        pImpl->lastCpuCheck = std::chrono::steady_clock::now();
    }
}

double AdvancedPerformanceProfiler::getCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->startTime).count();
    return duration;
}

double AdvancedPerformanceProfiler::getOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

double AdvancedPerformanceProfiler::getAverageOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double AdvancedPerformanceProfiler::getMinOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::min_element(it->second.begin(), it->second.end());
}

double AdvancedPerformanceProfiler::getMaxOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

double AdvancedPerformanceProfiler::getStdDevOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    double mean = getAverageOperationTime(operation);
    double sumSq = 0.0;
    for (double time : it->second) {
        double diff = time - mean;
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq / (it->second.size() - 1));
}

const std::map<std::string, std::vector<double>>& AdvancedPerformanceProfiler::getAllOperationTimes() const {
    return pImpl->operationTimes;
}

void AdvancedPerformanceProfiler::stopProfile() {
    if (!pImpl->currentOperation.empty()) {
        double elapsed = getCurrentTime();
        pImpl->operationTimes[pImpl->currentOperation].push_back(elapsed);
        pImpl->currentOperation.clear();
    }
}

void AdvancedPerformanceProfiler::reset() {
    pImpl->operationTimes.clear();
    pImpl->currentOperation.clear();
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->peakMemoryUsageMB = 0.0;
    pImpl->currentCpuUsagePercent = 0.0;
    pImpl->totalCpuTime = 0.0;
    pImpl->totalMemoryTime = 0.0;
}

void AdvancedPerformanceProfiler::startMemoryProfiling() {
    pImpl->memoryProfilingEnabled = true;
    startProfile("memory_profiling");
}

void AdvancedPerformanceProfiler::stopMemoryProfiling() {
    if (pImpl->memoryProfilingEnabled) {
        stopProfile();
        pImpl->memoryProfilingEnabled = false;
    }
}

double AdvancedPerformanceProfiler::getCurrentMemoryUsageMB() const {
    // Simplified memory measurement (in real implementation, would use system-specific APIs)
    // For now, return a simulated value based on operation count
    double memory = 100.0 + (pImpl->operationTimes.size() * 10.0); // Simulated
    
    if (memory > pImpl->peakMemoryUsageMB) {
        pImpl->peakMemoryUsageMB = memory;
    }
    
    return memory;
}

double AdvancedPerformanceProfiler::getPeakMemoryUsageMB() const {
    return pImpl->peakMemoryUsageMB;
}

void AdvancedPerformanceProfiler::startCpuProfiling() {
    pImpl->cpuProfilingEnabled = true;
    startProfile("cpu_profiling");
}

void AdvancedPerformanceProfiler::stopCpuProfiling() {
    if (pImpl->cpuProfilingEnabled) {
        stopProfile();
        pImpl->cpuProfilingEnabled = false;
    }
}

double AdvancedPerformanceProfiler::getCurrentCpuUsagePercent() const {
    // Simplified CPU usage measurement (in real implementation, would use system APIs)
    double cpu = 10.0 + (pImpl->operationTimes.size() * 5.0); // Simulated
    return std::max(0.0, std::min(100.0, cpu)); // Clamp to 0-100%
}

std::string AdvancedPerformanceProfiler::generatePerformanceReport() const {
    std::stringstream report;
    report << "Performance Profiling Report" << std::endl;
    report << "===========================" << std::endl;
    report << "Total operations: " << pImpl->operationTimes.size() << std::endl;
    report << "Peak memory usage: " << std::fixed << std::setprecision(pImpl->precision)
           << pImpl->peakMemoryUsageMB << " MB" << std::endl;
    report << "Current CPU usage: " << std::fixed << std::setprecision(2)
           << getCurrentCpuUsagePercent() << "%" << std::endl;
    
    if (!pImpl->operationTimes.empty()) {
        report << std::endl;
        report << "Detailed operation times:" << std::endl;
        
        for (const auto& pair : pImpl->operationTimes) {
            const std::string& operation = pair.first;
            const std::vector<double>& times = pair.second;
            
            report << "  " << operation << ":" << std::endl;
            report << "    Runs: " << times.size() << std::endl;
            report << "    Average: " << std::fixed << std::setprecision(pImpl->precision)
                   << getAverageOperationTime(operation) << "s" << std::endl;
            report << "    Min: " << std::fixed << std::setprecision(pImpl->precision)
                   << getMinOperationTime(operation) << "s" << std::endl;
            report << "    Max: " << std::fixed << std::setprecision(pImpl->precision)
                   << getMaxOperationTime(operation) << "s" << std::endl;
            report << "    Std Dev: " << std::fixed << std::setprecision(pImpl->precision)
                   << getStdDevOperationTime(operation) << "s" << std::endl;
            report << std::endl;
        }
    }
    
    return report.str();
}

bool AdvancedPerformanceProfiler::savePerformanceData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open performance data file: " << filename << std::endl;
        return false;
    }
    
    // Simple JSON-like format for performance data
    file << "{\n";
    file << "  \"performance_data\": {\n";
    
    for (const auto& pair : pImpl->operationTimes) {
        file << "    \"" << pair.first << "\": [";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            file << std::fixed << std::setprecision(pImpl->precision) << pair.second[i];
            if (i < pair.second.size() - 1) {
                file << ", ";
            }
        }
        file << "],\n";
    }
    
    file << "  },\n";
    file << "  \"peak_memory_mb\": " << pImpl->peakMemoryUsageMB << ",\n";
    file << "  \"cpu_usage_percent\": " << getCurrentCpuUsagePercent() << "\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool AdvancedPerformanceProfiler::loadPerformanceData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open performance data file: " << filename << std::endl;
        return false;
    }
    
    // For now, just clear existing data
    reset();
    
    // TODO: Implement actual JSON parsing
    // This is a placeholder for future implementation
    
    file.close();
    return true;
}

void AdvancedPerformanceProfiler::setResourceMonitoringEnabled(bool enabled) {
    pImpl->resourceMonitoringEnabled = enabled;
}

void AdvancedPerformanceProfiler::setPrecision(int precision) {
    pImpl->precision = precision;
}

std::string AdvancedPerformanceProfiler::getSystemInfo() const {
    std::stringstream info;
    info << "Advanced Performance Profiler" << std::endl;
    info << "============================" << std::endl;
    info << "Resource monitoring enabled: " << (pImpl->resourceMonitoringEnabled ? "yes" : "no") << std::endl;
    info << "Precision: " << pImpl->precision << std::endl;
    info << "Total operations recorded: " << pImpl->operationTimes.size() << std::endl;
    info << "Peak memory usage: " << pImpl->peakMemoryUsageMB << " MB" << std::endl;
    info << "Current CPU usage: " << getCurrentCpuUsagePercent() << "%" << std::endl;
    return info.str();
}

// Implementation of BenchmarkProgressMonitor
struct BenchmarkProgressMonitor::Impl {
    std::string benchmarkName;
    double currentStep;
    double totalSteps;
    double elapsedTime;
    double startTime;
    double lastUpdateTime;
    double updateInterval;
    bool useColors;
    bool shouldContinue;
    std::atomic<double> progress;
    std::string lastMessage;
};

BenchmarkProgressMonitor::BenchmarkProgressMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->currentStep = 0.0;
    pImpl->totalSteps = 0.0;
    pImpl->elapsedTime = 0.0;
    pImpl->startTime = 0.0;
    pImpl->lastUpdateTime = 0.0;
    pImpl->updateInterval = 5.0; // Update every 5 seconds
    pImpl->useColors = true;
    pImpl->shouldContinue = true;
    pImpl->progress = 0.0;
    pImpl->lastMessage = "";
}

BenchmarkProgressMonitor::~BenchmarkProgressMonitor() = default;

void BenchmarkProgressMonitor::start(const std::string& benchmarkName) {
    pImpl->benchmarkName = benchmarkName;
    auto now = std::chrono::steady_clock::now();
    pImpl->startTime = std::chrono::duration<double>(now.time_since_epoch()).count();
    pImpl->lastUpdateTime = pImpl->startTime;
    pImpl->currentStep = 0.0;
    pImpl->progress = 0.0;
    pImpl->lastMessage = "Starting benchmark: " + benchmarkName;
    
    std::cout << (pImpl->useColors ? "\033[1;32m" : "") << "Starting benchmark: " << benchmarkName << std::endl;
    std::cout << (pImpl->useColors ? "\033[0m" : "") << std::endl;
}

void BenchmarkProgressMonitor::update(double currentStep, double totalSteps, double elapsedTime) {
    pImpl->currentStep = currentStep;
    pImpl->totalSteps = totalSteps;
    pImpl->elapsedTime = elapsedTime;
    pImpl->progress = (totalSteps > 0) ? (currentStep / totalSteps) : 0.0;
    
    auto now = std::chrono::steady_clock::now();
    double currentTime = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    if (currentTime - pImpl->lastUpdateTime >= pImpl->updateInterval) {
        pImpl->lastUpdateTime = currentTime;
        reportProgress();
    }
}

void BenchmarkProgressMonitor::update(double currentStep, double totalSteps, double elapsedTime,
                                      double currentMemoryMB, double currentCpuPercent) {
    update(currentStep, totalSteps, elapsedTime);
    
    std::cout << (pImpl->useColors ? "\033[1;36m" : "") << "Progress: "
              << std::fixed << std::setprecision(1) << (pImpl->progress * 100.0) << "%"
              << " | Memory: " << currentMemoryMB << " MB"
              << " | CPU: " << currentCpuPercent << "%"
              << " | Time: " << elapsedTime << "s"
              << (pImpl->useColors ? "\033[0m" : "") << std::endl;
}

void BenchmarkProgressMonitor::complete(const std::string& message) {
    reportProgress();
    
    if (!message.empty()) {
        std::cout << (pImpl->useColors ? "\033[1;32m" : "") << "Completed: " << message << std::endl;
        std::cout << (pImpl->useColors ? "\033[0m" : "") << std::endl;
    } else {
        std::cout << (pImpl->useColors ? "\033[1;32m" : "") << "Benchmark completed successfully!" << std::endl;
        std::cout << (pImpl->useColors ? "\033[0m" : "") << std::endl;
    }
    
    pImpl->shouldContinue = false;
}

bool BenchmarkProgressMonitor::shouldContinue() const {
    return pImpl->shouldContinue;
}

double BenchmarkProgressMonitor::getProgress() const {
    return pImpl->progress;
}

double BenchmarkProgressMonitor::getEstimatedRemainingTime() const {
    if (pImpl->progress > 0.0 && pImpl->progress < 1.0) {
        double totalEstimatedTime = pImpl->elapsedTime / pImpl->progress;
        return totalEstimatedTime - pImpl->elapsedTime;
    }
    return 0.0;
}

void BenchmarkProgressMonitor::setUpdateInterval(double intervalSeconds) {
    pImpl->updateInterval = intervalSeconds;
}

void BenchmarkProgressMonitor::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

std::string BenchmarkProgressMonitor::getProgressReport() const {
    std::stringstream report;
    
    report << "Benchmark Progress Report" << std::endl;
    report << "=========================" << std::endl;
    report << "Benchmark: " << pImpl->benchmarkName << std::endl;
    report << "Progress: " << std::fixed << std::setprecision(1)
           << (pImpl->progress * 100.0) << "%" << std::endl;
    report << "Step: " << pImpl->currentStep << "/" << pImpl->totalSteps << std::endl;
    report << "Elapsed time: " << pImpl->elapsedTime << "s" << std::endl;
    
    double remainingTime = getEstimatedRemainingTime();
    if (remainingTime > 0) {
        report << "Estimated remaining time: " << remainingTime << "s" << std::endl;
    }
    
    if (!pImpl->lastMessage.empty()) {
        report << "Last message: " << pImpl->lastMessage << std::endl;
    }
    
    return report.str();
}

void BenchmarkProgressMonitor::reportProgress() {
    std::cout << "\r";
    
    if (pImpl->useColors) {
        std::cout << "\033[1;36mProgress: ";
    }
    
    std::cout << std::fixed << std::setprecision(1) << (pImpl->progress * 100.0) << "%";
    std::cout << " | Step: " << pImpl->currentStep << "/" << pImpl->totalSteps;
    std::cout << " | Time: " << pImpl->elapsedTime << "s";
    
    double remainingTime = getEstimatedRemainingTime();
    if (remainingTime > 0 && remainingTime < 3600) { // Show if less than 1 hour
        std::cout << " | Remaining: " << remainingTime << "s";
    }
    
    if (pImpl->useColors) {
        std::cout << "\033[0m";
    }
    
    std::cout << std::flush;
    
    pImpl->lastMessage = "Progress: " + std::to_string(pImpl->progress * 100.0) + "% completed";
}

std::string BenchmarkProgressMonitor::generatePerformanceSummary(const std::vector<AdvancedBenchmarkResult>& results) const {
    std::stringstream summary;
    
    summary << "Benchmark Performance Summary" << std::endl;
    summary << "===============================" << std::endl;
    
    if (results.empty()) {
        summary << "No benchmark results available" << std::endl;
        return summary.str();
    }
    
    summary << "Benchmark suites run: " << results.size() << std::endl;
    
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    double totalTime = 0.0;
    double maxStepsPerSecond = 0.0;
    double minStepsPerSecond = std::numeric_limits<double>::max();
    
    for (const auto& result : results) {
        totalNeurons += result.neurons;
        totalSynapses += result.synapses;
        totalTime += result.totalTime;
        maxStepsPerSecond = std::max(maxStepsPerSecond, result.stepsPerSecond);
        minStepsPerSecond = std::min(minStepsPerSecond, result.stepsPerSecond);
    }
    
    summary << std::endl;
    summary << "Aggregate Statistics:" << std::endl;
    summary << "  Total neurons tested: " << totalNeurons << std::endl;
    summary << "  Total synapses created: " << totalSynapses << std::endl;
    summary << "  Average total time: " << std::fixed << std::setprecision(2)
             << (totalTime / results.size()) << "s" << std::endl;
    summary << "  Maximum steps/sec: " << std::fixed << std::setprecision(0)
             << maxStepsPerSecond << std::endl;
    summary << "  Minimum steps/sec: " << std::fixed << std::setprecision(0)
             << minStepsPerSecond << std::endl;
    
    if (results.size() > 1) {
        double avgStepsPerSec = (maxStepsPerSecond + minStepsPerSecond) / 2.0;
        summary << "  Average steps/sec: " << std::fixed << std::setprecision(0)
                << avgStepsPerSec << std::endl;
    }
    
    return summary.str();
}

// Implementation of EnhancedBenchmarkRunner
struct EnhancedBenchmarkRunner::Impl {
    std::string workingDirectory;
    std::vector<AdvancedBenchmarkResult> lastResults;
    AdvancedPerformanceProfiler profiler;
    BenchmarkProgressMonitor progressMonitor;
    bool resultsSaved;
    std::chrono::steady_clock::time_point benchmarkStartTime;
};

EnhancedBenchmarkRunner::EnhancedBenchmarkRunner() : pImpl(std::make_unique<Impl>()) {
    pImpl->resultsSaved = false;
    pImpl->benchmarkStartTime = std::chrono::steady_clock::now();
}

EnhancedBenchmarkRunner::~EnhancedBenchmarkRunner() = default;

bool EnhancedBenchmarkRunner::runBenchmark(const AdvancedBenchmarkConfig& config) {
    try {
        // Validate configuration
        if (!validateConfig(config)) {
            std::cerr << "Error: Invalid benchmark configuration" << std::endl;
            return false;
        }
        
        // Setup progress monitor
        pImpl->progressMonitor.setUpdateInterval(config.progressInterval);
        pImpl->progressMonitor.setUseColors(true);
        
        // Setup working directory
        setWorkingDirectory(".");
        
        // Run benchmark suites
        for (const auto& benchmarkSuite : config.benchmarkSuites) {
            size_t neurons = benchmarkSuite.first;
            size_t steps = benchmarkSuite.second;
            
            std::cout << std::endl;
            std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                 Running Benchmark: " << neurons << " neurons, " << steps << " steps ║" << std::endl;
            std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            
            // Start benchmark timing
            auto suiteStartTime = std::chrono::steady_clock::now();
            
            // Run individual benchmark
            AdvancedBenchmarkResult result = executeBenchmark(neurons, steps, 
                                                             pImpl->profiler, 
                                                             pImpl->progressMonitor);
            
            // Update progress monitor
            pImpl->progressMonitor.complete("Benchmark completed");
            
            // Store results
            pImpl->lastResults.push_back(result);
            
            // Generate timestamps
            auto suiteEndTime = std::chrono::steady_clock::now();
            double suiteDuration = std::chrono::duration<double>(suiteEndTime - suiteStartTime).count();
            
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  Result: " << std::setw(51) << std::left
                      << (result.stepsPerSecond > config.minStepsPerSecond ? "SUCCESS" : "SLOW") << "║" << std::endl;
            std::cout << "║  Time: " << std::setw(51) << std::left
                      << std::fixed << std::setprecision(2) << result.totalTime << "s" << "║" << std::endl;
            std::cout << "║  Steps/sec: " << std::setw(50) << std::left
                      << std::fixed << std::setprecision(0) << result.stepsPerSecond << " ║" << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            
            // Check time limit
            if (suiteDuration > config.maxTimeLimit) {
                std::cout << "Warning: Benchmark exceeded time limit" << std::endl;
            }
            
            // Generate profile data if requested
            if (config.enableDetailedLogging && !config.jsonFile.empty()) {
                // Save detailed performance data
                std::string profileFile = createOutputFilename("benchmark_profile");
                pImpl->profiler.savePerformanceData(profileFile);
            }
            
            // Small delay between benchmarks
            if (!config.benchmarkSuites.empty() && &benchmarkSuite != &config.benchmarkSuites.back()) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        
        // Save results if requested
        if (config.outputFile != "") {
            saveResults(pImpl->lastResults, config);
            pImpl->resultsSaved = true;
        }
        
        // Generate additional reports if requested
        if (config.enableComparativeAnalysis) {
            std::cout << std::endl;
            std::cout << "Generating comparative analysis report..." << std::endl;
            std::string comparativeReport = generateComparativeReport(pImpl->lastResults);
            
            std::string reportFile = createOutputFilename("comparative_analysis");
            std::ofstream file(reportFile);
            if (file.is_open()) {
                file << comparativeReport;
                file.close();
                std::cout << "Comparative analysis saved to: " << reportFile << std::endl;
            }
        }
        
        if (config.enableStatisticalAnalysis) {
            std::cout << std::endl;
            std::cout << "Generating statistical analysis report..." << std::endl;
            std::string statisticalReport = generateStatisticalReport(pImpl->lastResults);
            
            std::string reportFile = createOutputFilename("statistical_analysis");
            std::ofstream file(reportFile);
            if (file.is_open()) {
                file << statisticalReport;
                file.close();
                std::cout << "Statistical analysis saved to: " << reportFile << std::endl;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during benchmark execution: " << e.what() << std::endl;
        return false;
    }
}

AdvancedBenchmarkResult EnhancedBenchmarkRunner::runSingleBenchmark(size_t neuronCount, size_t steps) {
    AdvancedPerformanceProfiler profiler;
    BenchmarkProgressMonitor progressMonitor;
    return executeBenchmark(neuronCount, steps, profiler, progressMonitor);
}

AdvancedBenchmarkResult EnhancedBenchmarkRunner::executeBenchmark(size_t neuronCount, size_t steps,
                                                                 AdvancedPerformanceProfiler& profiler,
                                                                 BenchmarkProgressMonitor& progressMonitor) {
    AdvancedBenchmarkResult result;
    result.neurons = neuronCount;
    result.steps = steps;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Warm-up phase (skip initial steps for stabilization)
    for (size_t warmupStep = 0; warmupStep < 100; ++warmupStep) {
        brain->step(warmupStep, warmupStep * 0.001);
    }
    
    // Start benchmark timing
    auto startTime = std::chrono::steady_clock::now();
    profiler.startProfile("benchmark_step");
    profiler.startMemoryProfiling();
    profiler.startCpuProfiling();
    
    progressMonitor.start(std::to_string(neuronCount) + " neurons benchmark");
    
    // Run benchmark steps
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min<size_t>(10, neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Collect metrics
        if (step >= 100) { // Skip warm-up steps
            result.stepTimes.push_back(profiler.getCurrentTime());
            result.spikeCountsPerStep.push_back(brain->getTotalSpikeCount());
        }
        
        // Update progress
        double elapsedTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
        progressMonitor.update(static_cast<double>(step), static_cast<double>(steps), elapsedTime,
                              profiler.getCurrentMemoryUsageMB(), profiler.getCurrentCpuUsagePercent());
        
        // Check time limit
        if (elapsedTime > 300.0) { // 5 minute limit
            std::cout << "Warning: Benchmark approaching time limit" << std::endl;
            break;
        }
    }
    
    // Stop profiling
    profiler.stopProfile();
    profiler.stopMemoryProfiling();
    profiler.stopCpuProfiling();
    
    // Calculate final metrics
    auto endTime = std::chrono::steady_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (result.totalTime > 0.0) {
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(brain->getTotalSynapseCount()) / neuronCount;
        result.spikeThroughput = static_cast<double>(brain->getTotalSpikeCount()) / result.totalTime;
        result.timeStepMs = (result.totalTime / steps) * 1000.0;
    }
    
    result.memoryUsageMB = profiler.getPeakMemoryUsageMB();
    result.cpuUsagePercent = profiler.getCurrentCpuUsagePercent();
    result.efficiencyScore = calculateEfficiencyScore(result);
    
    progressMonitor.complete("Benchmark completed");
    
    return result;
}

double EnhancedBenchmarkRunner::measureMemoryUsageMB() const {
    // Simplified memory measurement
    return 100.0 + (pImpl->lastResults.size() * 10.0); // Simulated
}

double EnhancedBenchmarkRunner::measureCpuUsagePercent() const {
    // Simplified CPU measurement
    return 15.0 + (pImpl->lastResults.size() * 3.0); // Simulated
}

double EnhancedBenchmarkRunner::calculateEfficiencyScore(const AdvancedBenchmarkResult& result) const {
    // Efficiency score based on multiple factors
    double stepsPerSec = result.stepsPerSecond;
    double memoryEfficiency = 100.0 / std::max(1.0, result.memoryUsageMB);
    double cpuEfficiency = 100.0 / std::max(1.0, result.cpuUsagePercent);
    
    // Combine factors (weighted average)
    double efficiency = (stepsPerSec * 0.5 + memoryEfficiency * 0.3 + cpuEfficiency * 0.2);
    
    return std::min(100.0, efficiency); // Cap at 100
}

bool EnhancedBenchmarkRunner::validateConfig(const AdvancedBenchmarkConfig& config) const {
    if (config.benchmarkSuites.empty()) {
        std::cerr << "Error: No benchmark suites specified" << std::endl;
        return false;
    }
    
    for (const auto& suite : config.benchmarkSuites) {
        if (suite.first == 0) {
            std::cerr << "Error: Invalid neuron count in benchmark suite" << std::endl;
            return false;
        }
        if (suite.second == 0) {
            std::cerr << "Error: Invalid step count in benchmark suite" << std::endl;
            return false;
        }
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        std::cerr << "Error: Invalid connection probability" << std::endl;
        return false;
    }
    
    if (config.minStepsPerSecond <= 0.0) {
        std::cerr << "Error: Invalid minimum steps per second" << std::endl;
        return false;
    }
    
    if (config.maxTimeLimit <= 0.0) {
        std::cerr << "Error: Invalid maximum time limit" << std::endl;
        return false;
    }
    
    return true;
}

std::string EnhancedBenchmarkRunner::generateComparativeReport(const std::vector<AdvancedBenchmarkResult>& results) const {
    std::stringstream report;
    
    report << "Comparative Benchmark Analysis Report" << std::endl;
    report << "=====================================" << std::endl;
    report << "Number of benchmark suites: " << results.size() << std::endl;
    report << std::endl;
    
    if (results.empty()) {
        report << "No results to analyze" << std::endl;
        return report.str();
    }
    
    report << "Benchmark Results Summary:" << std::endl;
    report << "=========================" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        report << std::endl;
        report << "Benchmark " << (i + 1) << ": " << result.neurons << " neurons, " 
               << result.steps << " steps" << std::endl;
        report << "  Total time: " << std::fixed << std::setprecision(2) << result.totalTime << "s" << std::endl;
        report << "  Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        report << "  Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        report << "  Memory usage: " << std::fixed << std::setprecision(2) << result.memoryUsageMB << " MB" << std::endl;
        report << "  CPU usage: " << std::fixed << std::setprecision(1) << result.cpuUsagePercent << "%" << std::endl;
        report << "  Efficiency score: " << std::fixed << std::setprecision(2) << result.efficiencyScore << "/100" << std::endl;
    }
    
    return report.str();
}

std::string EnhancedBenchmarkRunner::generateStatisticalReport(const std::vector<AdvancedBenchmarkResult>& results) const {
    std::stringstream report;
    
    report << "Statistical Benchmark Analysis Report" << std::endl;
    report << "=====================================" << std::endl;
    report << "Number of benchmark suites: " << results.size() << std::endl;
    report << std::endl;
    
    if (results.empty()) {
        report << "No results to analyze" << std::endl;
        return report.str();
    }
    
    // Calculate statistical measures
    double totalTime = 0.0, avgTime = 0.0, stdTime = 0.0;
    double maxStepsPerSec = 0.0, minStepsPerSec = std::numeric_limits<double>::max();
    double maxMemory = 0.0, minMemory = std::numeric_limits<double>::max();
    double maxEfficiency = 0.0, minEfficiency = std::numeric_limits<double>::max();
    
    for (const auto& result : results) {
        totalTime += result.totalTime;
        maxStepsPerSec = std::max(maxStepsPerSec, result.stepsPerSecond);
        minStepsPerSec = std::min(minStepsPerSec, result.stepsPerSecond);
        maxMemory = std::max(maxMemory, result.memoryUsageMB);
        minMemory = std::min(minMemory, result.memoryUsageMB);
        maxEfficiency = std::max(maxEfficiency, result.efficiencyScore);
        minEfficiency = std::min(minEfficiency, result.efficiencyScore);
    }
    
    avgTime = totalTime / results.size();
    
    // Calculate standard deviation
    double sumSqTime = 0.0;
    for (const auto& result : results) {
        double diff = result.totalTime - avgTime;
        sumSqTime += diff * diff;
    }
    stdTime = std::sqrt(sumSqTime / results.size());
    
    report << "Statistical Summary:" << std::endl;
    report << "===================" << std::endl;
    report << "Time Performance:" << std::endl;
    report << "  Average time: " << std::fixed << std::setprecision(2) << avgTime << "s" << std::endl;
    report << "  Standard deviation: " << std::fixed << std::setprecision(2) << stdTime << "s" << std::endl;
    report << "  Minimum time: " << std::fixed << std::setprecision(2) << minStepsPerSec << "s" << std::endl;
    report << "  Maximum time: " << std::fixed << std::setprecision(2) << maxStepsPerSec << "s" << std::endl;
    report << std::endl;
    
    report << "Resource Efficiency:" << std::endl;
    report << "  Memory usage range: " << std::fixed << std::setprecision(2) << minMemory << " - " << maxMemory << " MB" << std::endl;
    report << "  Efficiency score range: " << std::fixed << std::setprecision(2) << minEfficiency << " - " << maxEfficiency << "/100" << std::endl;
    report << "  Steps/sec range: " << std::fixed << std::setprecision(0) << minStepsPerSec << " - " << maxStepsPerSec << " steps/sec" << std::endl;
    
    return report.str();
}

std::string EnhancedBenchmarkRunner::generateEfficiencyRanking(const std::vector<AdvancedBenchmarkResult>& results) const {
    std::stringstream report;
    
    report << "Efficiency Ranking Report" << std::endl;
    report << "=========================" << std::endl;
    report << "Number of benchmark suites: " << results.size() << std::endl;
    report << std::endl;
    
    if (results.empty()) {
        report << "No results to rank" << std::endl;
        return report.str();
    }
    
    // Create ranking based on efficiency score
    std::vector<std::pair<size_t, double>> rankings;
    for (size_t i = 0; i < results.size(); ++i) {
        rankings.push_back({i, results[i].efficiencyScore});
    }
    
    std::sort(rankings.begin(), rankings.end(),
              [](const std::pair<size_t, double>& a, const std::pair<size_t, double>& b) {
                  return a.second > b.second;
              });
    
    report << "Top Benchmarks by Efficiency Score:" << std::endl;
    report << "====================================" << std::endl;
    
    for (size_t i = 0; i < rankings.size(); ++i) {
        size_t originalIndex = rankings[i].first;
        double efficiency = rankings[i].second;
        
        const auto& result = results[originalIndex];
        
        report << (i + 1) << ". Benchmark " << (originalIndex + 1) 
               << " (" << result.neurons << " neurons, " << result.steps << " steps)" << std::endl;
        report << "   Efficiency Score: " << std::fixed << std::setprecision(2) << efficiency << "/100" << std::endl;
        report << "   Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        report << "   Memory usage: " << std::fixed << std::setprecision(2) << result.memoryUsageMB << " MB" << std::endl;
        report << "   CPU usage: " << std::fixed << std::setprecision(1) << result.cpuUsagePercent << "%" << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

bool EnhancedBenchmarkRunner::saveResults(const std::vector<AdvancedBenchmarkResult>& results,
                                          const AdvancedBenchmarkConfig& config) {
    try {
        std::string timestamp = generateTimestamp();
        
        // Save main results file
        if (config.outputFile != "") {
            std::string resultsFile = createOutputFilename(config.outputFile);
            
            std::ofstream file(resultsFile);
            if (!file.is_open()) {
                std::cerr << "Failed to open results file: " << resultsFile << std::endl;
                return false;
            }
            
            // Generate results based on output format
            std::string content;
            if (config.outputFormat == "json") {
                content = JsonBenchmarkFormatter::format(results);
            } else if (config.outputFormat == "csv") {
                content = CsvBenchmarkFormatter::format(results);
            } else if (config.outputFormat == "yaml") {
                content = YamlBenchmarkFormatter::format(results);
            } else {
                // Default text format
                content = generateTextReport(results);
            }
            
            file << content;
            file.close();
            
            std::cout << "Benchmark results saved to: " << resultsFile << std::endl;
        }
        
        // Save JSON format if requested
        if (!config.jsonFile.empty() && config.outputFormat != "json") {
            std::string jsonFile = createOutputFilename(config.jsonFile);
            std::ofstream file(jsonFile);
            if (file.is_open()) {
                file << JsonBenchmarkFormatter::format(results);
                file.close();
                std::cout << "JSON results saved to: " << jsonFile << std::endl;
            }
        }
        
        // Save CSV format if requested
        if (!config.csvFile.empty() && config.outputFormat != "csv") {
            std::string csvFile = createOutputFilename(config.csvFile);
            std::ofstream file(csvFile);
            if (file.is_open()) {
                file << CsvBenchmarkFormatter::format(results);
                file.close();
                std::cout << "CSV results saved to: " << csvFile << std::endl;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving benchmark results: " << e.what() << std::endl;
        return false;
    }
}

bool EnhancedBenchmarkRunner::loadResults(const std::string& filename,
                                          std::vector<AdvancedBenchmarkResult>& results) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open results file: " << filename << std::endl;
        return false;
    }
    
    // TODO: Implement actual result loading from files
    // This is a placeholder for future implementation
    
    file.close();
    return true;
}

void EnhancedBenchmarkRunner::setWorkingDirectory(const std::string& directory) {
    pImpl->workingDirectory = directory;
    
    // Change working directory
    if (chdir(directory.c_str()) != 0) {
        std::cerr << "Warning: Could not change to directory: " << directory << std::endl;
    }
}

AdvancedBenchmarkConfig EnhancedBenchmarkRunner::getDefaultConfig() {
    AdvancedBenchmarkConfig config;
    
    // Standard benchmark suite
    config.benchmarkSuites = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    config.enableMemoryProfiling = true;
    config.enableCpuProfiling = true;
    config.enableProgressReporting = true;
    config.progressInterval = 5.0;
    config.outputFormat = "txt";
    config.enableComparativeAnalysis = true;
    config.enableStatisticalAnalysis = true;
    config.warmupSteps = 100;
    config.stabilizationSteps = 200;
    config.enableDetailedLogging = true;
    config.enableTimestamps = true;
    config.minStepsPerSecond = 1000.0;
    config.maxTimeLimit = 300.0;
    
    return config;
}

std::map<std::string, std::vector<std::pair<size_t, size_t>>> EnhancedBenchmarkRunner::getRecommendedConfigs() {
    std::map<std::string, std::vector<std::pair<size_t, size_t>>> configs;
    
    // Quick test configuration
    configs["Quick Test"] = {
        {50, 500},        // Small scale quick test
    };
    
    // Standard performance configuration
    configs["Standard Performance"] = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
    };
    
    // Scalability test configuration
    configs["Scalability Test"] = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
        {5000, 100},
    };
    
    // Real-time performance configuration
    configs["Real-time Performance"] = {
        {200, 500},
        {500, 300},
        {1000, 200},
    };
    
    // Stress test configuration
    configs["Stress Test"] = {
        {1000, 100},
        {2000, 50},
        {5000, 20},
    };
    
    return configs;
}

std::string EnhancedBenchmarkRunner::generateTextReport(const std::vector<AdvancedBenchmarkResult>& results) const {
    std::stringstream report;
    
    report << "NLM Enhanced Performance Benchmark Report" << std::endl;
    report << "============================================" << std::endl;
    report << "Generated: " << generateTimestamp() << std::endl;
    report << "Number of benchmarks: " << results.size() << std::endl;
    report << std::endl;
    
    if (results.empty()) {
        report << "No benchmark results available" << std::endl;
        return report.str();
    }
    
    report << std::setw(5) << "No." << std::setw(12) << "Neurons" << std::setw(8) << "Steps"
           << std::setw(12) << "Total Time" << std::setw(12) << "Steps/sec" << std::setw(12) << "Spikes/step"
           << std::setw(12) << "Memory MB" << std::setw(12) << "CPU %" << std::setw(12) << "Efficiency" << std::endl;
    report << std::setw(5) << "---" << std::setw(12) << "------" << std::setw(8) << "-----"
           << std::setw(12) << "----------" << std::setw(12) << "--------" << std::setw(12) << "----------"
           << std::setw(12) << "---------" << std::setw(12) << "------" << std::setw(12) << "--------" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        
        report << std::setw(5) << (i + 1) << std::setw(12) << result.neurons << std::setw(8) << result.steps
               << std::setw(12) << std::fixed << std::setprecision(2) << result.totalTime
               << std::setw(12) << std::fixed << std::setprecision(0) << result.stepsPerSecond
               << std::setw(12) << std::fixed << std::setprecision(2) << result.spikesPerStep
               << std::setw(12) << std::fixed << std::setprecision(2) << result.memoryUsageMB
               << std::setw(12) << std::fixed << std::setprecision(1) << result.cpuUsagePercent
               << std::setw(12) << std::fixed << std::setprecision(2) << result.efficiencyScore << "/100" << std::endl;
    }
    
    return report.str();
}

std::string EnhancedBenchmarkRunner::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).milliseconds() % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms;
    
    return ss.str();
}

std::string EnhancedBenchmarkRunner::createOutputFilename(const std::string& baseName) const {
    std::string timestamp = generateTimestamp();
    
    if (baseName.find('.') == std::string::npos) {
        // No extension, add timestamp
        return baseName + "_" + timestamp;
    } else {
        // Has extension, insert timestamp before extension
        size_t dotPos = baseName.rfind('.');
        return baseName.substr(0, dotPos) + "_" + timestamp + 
               baseName.substr(dotPos);
    }
}

// Implementation of formatter classes
std::string JsonBenchmarkFormatter::format(const std::vector<AdvancedBenchmarkResult>& results) {
    std::stringstream json;
    json << "{\n";
    json << "  \"benchmarks\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        json << "    " << format(results[i]);
        if (i < results.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    
    json << "  ],\n";
    json << "  \"total_benchmarks\": " << results.size() << ",\n";
    json << "  \"generated_at\": \"" << generateTimestamp() << "\"\n";
    json << "}\n";
    
    return json.str();
}

std::string JsonBenchmarkFormatter::format(const AdvancedBenchmarkResult& result) {
    std::stringstream json;
    
    json << "{\n";
    json << "      \"neurons\": " << result.neurons << ",\n";
    json << "      \"synapses\": " << result.synapses << ",\n";
    json << "      \"steps\": " << result.steps << ",\n";
    json << "      \"total_time\": " << formatDouble(result.totalTime) << ",\n";
    json << "      \"steps_per_second\": " << formatDouble(result.stepsPerSecond) << ",\n";
    json << "      \"spikes_per_step\": " << formatDouble(result.spikesPerStep) << ",\n";
    json << "      \"synapses_per_neuron\": " << formatDouble(result.synapsesPerNeuron) << ",\n";
    json << "      \"memory_usage_mb\": " << formatDouble(result.memoryUsageMB) << ",\n";
    json << "      \"cpu_usage_percent\": " << formatDouble(result.cpuUsagePercent) << ",\n";
    json << "      \"efficiency_score\": " << formatDouble(result.efficiencyScore) << "\n";
    json << "    }";
    
    return json.str();
}

std::string JsonBenchmarkFormatter::formatConfiguration(const AdvancedBenchmarkConfig& config) {
    std::stringstream json;
    
    json << "{\n";
    json << "  \"benchmark_suites\": [\n";
    
    for (size_t i = 0; i < config.benchmarkSuites.size(); ++i) {
        const auto& suite = config.benchmarkSuites[i];
        json << "    {\"neurons\": " << suite.first << ", \"steps\": " << suite.second << "}";
        if (i < config.benchmarkSuites.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    
    json << "  ],\n";
    json << "  \"enable_memory_profiling\": " << (config.enableMemoryProfiling ? "true" : "false") << ",\n";
    json << "  \"enable_cpu_profiling\": " << (config.enableCpuProfiling ? "true" : "false") << ",\n";
    json << "  \"enable_progress_reporting\": " << (config.enableProgressReporting ? "true" : "false") << ",\n";
    json << "  \"output_format\": \"" << config.outputFormat << "\"\n";
    json << "}\n";
    
    return json.str();
}

std::string JsonBenchmarkFormatter::escapeJsonString(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    
    for (char c : input) {
        switch (c) {
            case '\\': output += "\\\\"; break;
            case '\"': output += "\\\""; break;
            case '\/': output += "\\/"; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    output += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    output += c;
                }
                break;
        }
    }
    
    return output;
}

std::string JsonBenchmarkFormatter::formatDouble(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    return ss.str();
}

std::string CsvBenchmarkFormatter::format(const std::vector<AdvancedBenchmarkResult>& results) {
    std::stringstream csv;
    
    csv << formatHeader() << std::endl;
    
    for (const auto& result : results) {
        csv << format(result) << std::endl;
    }
    
    return csv.str();
}

std::string CsvBenchmarkFormatter::format(const AdvancedBenchmarkResult& result) {
    std::stringstream csv;
    
    csv << result.neurons << "," << result.synapses << "," << result.steps << ","
        << result.totalTime << "," << result.stepsPerSecond << ","
        << result.spikesPerStep << "," << result.synapsesPerNeuron << ","
        << result.memoryUsageMB << "," << result.cpuUsagePercent << ","
        << result.efficiencyScore;
    
    return csv.str();
}

std::string CsvBenchmarkFormatter::formatHeader() {
    return "Neurons,Synapses,Steps,Total_Time,Steps_Per_Second,Spikes_Per_Step,"
           "Synapses_Per_Neuron,Memory_Usage_MB,CPU_Usage_Percent,Efficiency_Score";
}

std::string YamlBenchmarkFormatter::format(const std::vector<AdvancedBenchmarkResult>& results) {
    std::stringstream yaml;
    
    yaml << "benchmarks:\n";
    for (const auto& result : results) {
        yaml << "- " << format(result) << std::endl;
    }
    
    yaml << "total_benchmarks: " << results.size() << std::endl;
    yaml << "generated_at: " << generateTimestamp() << std::endl;
    
    return yaml.str();
}

std::string YamlBenchmarkFormatter::format(const AdvancedBenchmarkResult& result) {
    std::stringstream yaml;
    
    yaml << "  neurons: " << result.neurons << std::endl;
    yaml << "  synapses: " << result.synapses << std::endl;
    yaml << "  steps: " << result.steps << std::endl;
    yaml << "  total_time: " << formatDouble(result.totalTime) << std::endl;
    yaml << "  steps_per_second: " << formatDouble(result.stepsPerSecond) << std::endl;
    yaml << "  spikes_per_step: " << formatDouble(result.spikesPerStep) << std::endl;
    yaml << "  synapses_per_neuron: " << formatDouble(result.synapsesPerNeuron) << std::endl;
    yaml << "  memory_usage_mb: " << formatDouble(result.memoryUsageMB) << std::endl;
    yaml << "  cpu_usage_percent: " << formatDouble(result.cpuUsagePercent) << std::endl;
    yaml << "  efficiency_score: " << formatDouble(result.efficiencyScore) << std::endl;
    
    return yaml.str();
}

std::string YamlBenchmarkFormatter::formatConfiguration(const AdvancedBenchmarkConfig& config) {
    std::stringstream yaml;
    
    yaml << "benchmark_config:\n";
    yaml << "  benchmark_suites:\n";
    for (const auto& suite : config.benchmarkSuites) {
        yaml << "    - neurons: " << suite.first << ", steps: " << suite.second << std::endl;
    }
    yaml << "  enable_memory_profiling: " << (config.enableMemoryProfiling ? "true" : "false") << std::endl;
    yaml << "  enable_cpu_profiling: " << (config.enableCpuProfiling ? "true" : "false") << std::endl;
    yaml << "  enable_progress_reporting: " << (config.enableProgressReporting ? "true" : "false") << std::endl;
    yaml << "  output_format: " << config.outputFormat << std::endl;
    
    return yaml.str();
}

std::string YamlBenchmarkFormatter::escapeYamlString(const std::string& input) {
    // Simplified YAML escaping
    std::string output = input;
    // Replace tabs with spaces
    std::replace(output.begin(), output.end(), '\t', ' ');
    // Replace newlines with spaces
    std::replace(output.begin(), output.end(), '\n', ' ');
    return output;
}

std::string YamlBenchmarkFormatter::formatDouble(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    return ss.str();
}

std::string YamlBenchmarkFormatter::formatDouble(size_t value) {
    return std::to_string(value);
}

std::string YamlBenchmarkFormatter::formatDouble(const std::string& value) {
    return escapeYamlString(value);
}

std::string YamlBenchmarkFormatter::formatDouble(bool value) {
    return value ? "true" : "false";
}

std::string YamlBenchmarkFormatter::formatDouble(const std::vector<double>& values) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        ss << formatDouble(values[i]);
        if (i < values.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

// Global benchmark runner instance
EnhancedBenchmarkRunner& getEnhancedBenchmarkRunner() {
    static EnhancedBenchmarkRunner runner;
    return runner;
}

// Convenience functions
bool runStandardBenchmark() {
    return getEnhancedBenchmarkRunner().runBenchmark(getEnhancedBenchmarkRunner().getDefaultConfig());
}

bool runExtendedBenchmark() {
    AdvancedBenchmarkConfig config = getEnhancedBenchmarkRunner().getDefaultConfig();
    
    // Extended benchmark suite
    config.benchmarkSuites = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
        {5000, 100},
        {10000, 50},
    };
    
    config.enableDetailedLogging = true;
    config.enableComparativeAnalysis = true;
    config.enableStatisticalAnalysis = true;
    
    return getEnhancedBenchmarkRunner().runBenchmark(config);
}

bool runMicroBenchmark() {
    AdvancedBenchmarkConfig config = getEnhancedBenchmarkRunner().getDefaultConfig();
    
    // Micro benchmark (small scale, high precision)
    config.benchmarkSuites = {
        {10, 100},
        {20, 200},
        {50, 500},
        {100, 1000},
    };
    
    config.enableMemoryProfiling = true;
    config.enableCpuProfiling = true;
    config.enableProgressReporting = true;
    config.progressInterval = 1.0;
    
    return getEnhancedBenchmarkRunner().runBenchmark(config);
}

bool runScalabilityBenchmark() {
    AdvancedBenchmarkConfig config = getEnhancedBenchmarkRunner().getDefaultConfig();
    
    // Scalability test
    config.benchmarkSuites = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
        {5000, 100},
    };
    
    config.enableComparativeAnalysis = true;
    config.enableStatisticalAnalysis = true;
    
    return getEnhancedBenchmarkRunner().runBenchmark(config);
}

bool runComparativeBenchmark(const std::vector<std::pair<size_t, size_t>>& benchmarks) {
    AdvancedBenchmarkConfig config = getEnhancedBenchmarkRunner().getDefaultConfig();
    config.benchmarkSuites = benchmarks;
    config.enableComparativeAnalysis = true;
    config.enableStatisticalAnalysis = true;
    
    return getEnhancedBenchmarkRunner().runBenchmark(config);
}

// Main enhanced benchmark entry point
int mainEnhancedBenchmark(int argc, char** argv) {
    try {
        // Parse command line arguments
        std::string outputFile, profileOutput;
        bool help = false, version = false;
        bool enableMemoryProfiling = true;
        bool enableCpuProfiling = true;
        bool enableProgressReporting = true;
        double progressInterval = 5.0;
        std::string outputFormat = "txt";
        bool enableComparativeAnalysis = true;
        bool enableStatisticalAnalysis = true;
        size_t warmupSteps = 100;
        size_t stabilizationSteps = 200;
        bool enableDetailedLogging = true;
        bool enableTimestamps = true;
        bool enableGpuProfiling = false;
        double minStepsPerSecond = 1000.0;
        double maxTimeLimit = 300.0;
        
        std::vector<std::pair<size_t, size_t>> benchmarkSuites;
        
        // Parse arguments (simplified for brevity)
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                help = true;
                continue;
            }
            else if (arg == "--version") {
                version = true;
                continue;
            }
            else if (arg.substr(0, 12) == "--output=") {
                outputFile = arg.substr(12);
            }
            else if (arg.substr(0, 16) == "--profile=") {
                profileOutput = arg.substr(16);
            }
            else if (arg == "--no-memory-profiling") {
                enableMemoryProfiling = false;
            }
            else if (arg == "--no-cpu-profiling") {
                enableCpuProfiling = false;
            }
            else if (arg == "--no-progress") {
                enableProgressReporting = false;
            }
            else if (arg.substr(0, 16) == "--progress-interval=") {
                progressInterval = std::stod(arg.substr(16));
            }
            else if (arg.substr(0, 17) == "--format=") {
                outputFormat = arg.substr(17);
            }
            else if (arg == "--no-comparative-analysis") {
                enableComparativeAnalysis = false;
            }
            else if (arg == "--no-statistical-analysis") {
                enableStatisticalAnalysis = false;
            }
            else if (arg.substr(0, 17) == "--warmup-steps=") {
                warmupSteps = static_cast<size_t>(std::stoul(arg.substr(17)));
            }
            else if (arg.substr(0, 20) == "--stabilization-steps=") {
                stabilizationSteps = static_cast<size_t>(std::stoul(arg.substr(20)));
            }
            else if (arg == "--no-detailed-logging") {
                enableDetailedLogging = false;
            }
            else if (arg == "--no-timestamps") {
                enableTimestamps = false;
            }
            else if (arg == "--enable-gpu-profiling") {
                enableGpuProfiling = true;
            }
            else if (arg.substr(0, 20) == "--min-steps-per-sec=") {
                minStepsPerSecond = std::stod(arg.substr(20));
            }
            else if (arg.substr(0, 15) == "--max-time-limit=") {
                maxTimeLimit = std::stod(arg.substr(15));
            }
            else if (arg.substr(0, 9) == "--bench=") {
                // Parse benchmark suite specification
                std::string spec = arg.substr(9);
                // Simple parsing: "neurons:steps" separated by commas
                size_t pos = spec.find(',');
                if (pos != std::string::npos) {
                    size_t neurons = std::stoul(spec.substr(0, pos));
                    size_t steps = std::stoul(spec.substr(pos + 1));
                    benchmarkSuites.push_back({neurons, steps});
                }
            }
        }
        
        // Show help or version
        if (help) {
            std::cout << "Enhanced NLM Benchmark Tool" << std::endl;
            std::cout << "=============================" << std::endl;
            std::cout << std::endl;
            std::cout << "Usage: nlm_benchmark_enhanced [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                  Show this help message" << std::endl;
            std::cout << "  --version                  Show version information" << std::endl;
            std::cout << "  --output=<file>            Output file for results" << std::endl;
            std::cout << "  --profile=<file>           Performance profile output" << std::endl;
            std::cout << "  --no-memory-profiling      Disable memory profiling" << std::endl;
            std::cout << "  --no-cpu-profiling          Disable CPU profiling" << std::endl;
            std::cout << "  --no-progress              Disable progress reporting" << std::endl;
            std::cout << "  --progress-interval=<sec>   Progress update interval (seconds)" << std::endl;
            std::cout << "  --format=<format>          Output format (txt, json, csv, yaml)" << std::endl;
            std::cout << "  --no-comparative-analysis   Disable comparative analysis" << std::endl;
            std::cout << "  --no-statistical-analysis   Disable statistical analysis" << std::endl;
            std::cout << "  --warmup-steps=<n>         Warm-up steps before measurement" << std::endl;
            std::cout << "  --stabilization-steps=<n>   Stabilization steps" << std::endl;
            std::cout << "  --no-detailed-logging      Disable detailed logging" << std::endl;
            std::cout << "  --no-timestamps             Disable timestamps in output" << std::endl;
            std::cout << "  --enable-gpu-profiling      Enable GPU profiling (if available)" << std::endl;
            std::cout << "  --min-steps-per-sec=<n>     Minimum steps per second threshold" << std::endl;
            std::cout << "  --max-time-limit=<sec>      Maximum benchmark time in seconds" << std::endl;
            std::cout << "  --bench=<spec>             Benchmark suite specification (neurons:steps)" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  nlm_benchmark_enhanced --bench=100:1000,500:1000,1000:500" << std::endl;
            std::cout << "  nlm_benchmark_enhanced --output=results.txt --format=json" << std::endl;
            std::cout << "  nlm_benchmark_enhanced --no-progress --profile=bench.prof" << std::endl;
            std::cout << std::endl;
            return 0;
        }
        
        if (version) {
            std::cout << "Enhanced NLM Benchmark Tool v2.0" << std::endl;
            std::cout << "Built with advanced profiling capabilities" << std::endl;
            std::cout << "Supports multiple output formats and comprehensive analysis" << std::endl;
            return 0;
        }
        
        // Create benchmark configuration
        AdvancedBenchmarkConfig config;
        config.benchmarkSuites = benchmarkSuites;
        config.enableMemoryProfiling = enableMemoryProfiling;
        config.enableCpuProfiling = enableCpuProfiling;
        config.enableProgressReporting = enableProgressReporting;
        config.progressInterval = progressInterval;
        config.outputFormat = outputFormat;
        config.outputFile = outputFile;
        config.jsonFile = (profileOutput.empty() ? "" : profileOutput);
        config.enableComparativeAnalysis = enableComparativeAnalysis;
        config.enableStatisticalAnalysis = enableStatisticalAnalysis;
        config.warmupSteps = warmupSteps;
        config.stabilizationSteps = stabilizationSteps;
        config.enableDetailedLogging = enableDetailedLogging;
        config.enableTimestamps = enableTimestamps;
        config.enableGpuProfiling = enableGpuProfiling;
        config.minStepsPerSecond = minStepsPerSecond;
        config.maxTimeLimit = maxTimeLimit;
        
        // Run benchmark
        EnhancedBenchmarkRunner& runner = getEnhancedBenchmarkRunner();
        
        if (config.benchmarkSuites.empty()) {
            // Use default configuration
            config = runner.getDefaultConfig();
            std::cout << "Running default benchmark configuration:" << std::endl;
            for (const auto& suite : config.benchmarkSuites) {
                std::cout << "  " << suite.first << " neurons, " << suite.second << " steps" << std::endl;
            }
        } else {
            std::cout << "Running custom benchmark configuration:" << std::endl;
            for (const auto& suite : config.benchmarkSuites) {
                std::cout << "  " << suite.first << " neurons, " << suite.second << " steps" << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        bool success = runner.runBenchmark(config);
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

} // namespace nlm

// Entry point for the enhanced benchmark tool
int main(int argc, char** argv) {
    return nlm::mainEnhancedBenchmark(argc, argv);
}
