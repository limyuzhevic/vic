// Performance Profiling Implementation
// Comprehensive real-time performance monitoring for NLM brain emulation

#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : isMonitoring_(false)
    , samplingInterval_(0.1)  // 100 ms default
    , metricsLevel_(2)        // Medium detail
    , cpuProfilingEnabled_(true)
    , memoryProfilingEnabled_(true)
    , threadProfilingEnabled_(true)
    , monitoringThread_(&PerformanceMonitor::monitoringThread, this)
{
    setupSignalHandlers();
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isMonitoring_) {
        isMonitoring_ = true;
        monitoringThread_ = std::thread(&PerformanceMonitor::monitoringThread, this);
    }
}

void PerformanceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    isMonitoring_ = false;
}

void PerformanceMonitor::recordMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    metricsHistory_.push_back(metrics);
    
    // Keep only recent history (last 10,000 samples)
    if (metricsHistory_.size() > 10000) {
        metricsHistory_.erase(metricsHistory_.begin(), metricsHistory_.end() - 10000);
    }
    
    // Update statistics
    statistics_.recordMetrics(metricsHistory_.back().message, 
                              metricsHistory_.back().value,
                              metricsHistory_.back().timestamp);
}

void PerformanceMonitor::recordCPUSnapshot(const CPUSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    cpuHistory_.push_back(snapshot);
    statistics_.addCPUProfile(snapshot);
    
    if (cpuHistory_.size() > 10000) {
        cpuHistory_.erase(cpuHistory_.begin(), cpuHistory_.end() - 10000);
    }
}

void PerformanceMonitor::recordMemoryRecord(const MemoryRecord& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryHistory_.push_back(record);
    statistics_.addMemoryRecord(record);
    
    if (memoryHistory_.size() > 10000) {
        memoryHistory_.erase(memoryHistory_.begin(), memoryHistory_.end() - 10000);
    }
}

void PerformanceMonitor::recordThreadRecord(const ThreadRecord& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    threadHistory_.push_back(record);
    statistics_.addThreadRecord(record);
    
    if (threadHistory_.size() > 10000) {
        threadHistory_.erase(threadHistory_.begin(), threadHistory_.end() - 10000);
    }
}

void PerformanceMonitor::recordEvent(const PerformanceEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventHistory_.push_back(event);
    
    if (eventHistory_.size() > 10000) {
        eventHistory_.erase(eventHistory_.begin(), eventHistory_.end() - 10000);
    }
}

void PerformanceMonitor::recordSystemEvent(const std::string& source,
                                           PerformanceEvent::EventType type,
                                           const std::string& message,
                                           double value) {
    uint64_t timestamp = getCurrentTimestamp();
    PerformanceEvent event(type, timestamp, 0, source, message, value);
    recordEvent(event);
}

PerformanceMetrics PerformanceMonitor::getLatestMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metricsHistory_.empty()) {
        return PerformanceMetrics();
    }
    return metricsHistory_.back();
}

std::vector<PerformanceEvent> PerformanceMonitor::getRecentEvents(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t startIdx = std::max<size_t>(0, eventHistory_.size() - count);
    return std::vector<PerformanceEvent>(eventHistory_.begin() + startIdx, 
                                         eventHistory_.end());
}

std::string PerformanceMonitor::generateFlameGraph(size_t maxSamples) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Aggregate CPU samples
    std::map<std::string, double> functionSamples;
    size_t totalSamples = 0;
    
    for (const auto& snapshot : cpuHistory_) {
        if (totalSamples >= maxSamples) break;
        
        for (const auto& frame : snapshot.callStack) {
            functionSamples[frame.functionName] += frame.samples;
            totalSamples++;
        }
    }
    
    // Generate flame graph HTML
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head><title>NLM Performance Flame Graph</title></head>\n";
    html << "<body>\n";
    html << "<h1>NLM Performance Flame Graph</h1>\n";
    html << "<pre style=\"font-family: monospace; font-size: 12px;\">\n";
    
    // Simple flame graph visualization
    double maxSamplesPerFunction = 0.0;
    for (const auto& pair : functionSamples) {
        if (pair.second > maxSamplesPerFunction) {
            maxSamplesPerFunction = pair.second;
        }
    }
    
    for (const auto& pair : functionSamples) {
        double percentage = (pair.second / totalSamples) * 100.0;
        size_t barLength = static_cast<size_t>((percentage / 100.0) * 80.0);
        html << std::string(barLength, ' ') << pair.first 
             << " (" << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    
    html << "</pre>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void PerformanceMonitor::addAlertThreshold(const std::string& metric,
                                           double threshold,
                                           PerformanceEvent::EventType severity) {
    std::lock_guard<std::mutex> lock(mutex_);
    alertThresholds_[metric] = threshold;
}

void PerformanceMonitor::removeAlertThreshold(const std::string& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    alertThresholds_.erase(metric);
}

void PerformanceMonitor::monitoringThread() {
    while (isMonitoring_) {
        collectSystemMetrics();
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void PerformanceMonitor::collectSystemMetrics() {
    // Collect metrics using system calls
    PerformanceMetrics metrics;
    metrics.timestamp = getCurrentTimestamp();
    metrics.stepId = 0;  // Will be updated by brain simulation
    
    // CPU metrics
    metrics.cpuUtilization = getCPUUtilization();
    metrics.memoryUtilization = getMemoryUtilization();
    metrics.activeThreads = getActiveThreadCount();
    
    // Memory metrics
    metrics.allocatedMemory = getAllocatedMemory();
    metrics.peakMemory = getPeakMemory();
    metrics.memoryFragments = getMemoryFragments();
    
    // I/O metrics
    metrics.diskIOPS = getDiskIOPS();
    metrics.networkIOPS = getNetworkIOPS();
    
    // Record metrics
    recordMetrics(metrics);
    
    // Check for alerts
    checkAlerts(metrics);
}

void PerformanceMonitor::checkAlerts(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : alertThresholds_) {
        const std::string& metricName = pair.first;
        double threshold = pair.second;
        
        double value = 0.0;
        // Extract metric value based on name
        if (metricName == "cpu_utilization") value = metrics.cpuUtilization;
        else if (metricName == "memory_utilization") value = metrics.memoryUtilization;
        else if (metricName == "steps_per_second") value = metrics.stepsPerSecond;
        
        if (value > threshold) {
            std::string severity;
            if (metricName.find("cpu") != std::string::npos || 
                metricName.find("memory") != std::string::npos) {
                severity = "WARNING";
            } else {
                severity = "ERROR";
            }
            
            recordSystemEvent(metricName, PerformanceEvent::EventType::WARNING,
                             "Threshold exceeded: " + metricName + " = " + 
                             std::to_string(value) + " > " + std::to_string(threshold),
                             value - threshold);
        }
    }
}

// Helper functions
std::string PerformanceMonitor::getCurrentThreadName() const {
    return "Thread-" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

uint64_t PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}

void PerformanceMonitor::setupSignalHandlers() {
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, PerformanceMonitor::signalHandler);
    signal(SIGTERM, PerformanceMonitor::signalHandler);
}

void PerformanceMonitor::signalHandler(int signal) {
    // Handle shutdown signals
    if (signal == SIGINT || signal == SIGTERM) {
        // This is a simplified approach - in production you'd want a more robust solution
    }
}

// PerformanceStatistics implementation
void PerformanceStatistics::addMetric(const std::string& name, double value, 
                                     uint64_t timestamp) {
    std::lock_guard<std::mutex> lock(mutex_);
    metricHistory_[name].emplace_back(timestamp, value);
}

void PerformanceStatistics::addCPUProfile(const CPUSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Add each frame in the call stack
    for (const auto& frame : snapshot.callStack) {
        addMetric("cpu_samples_" + frame.functionName, frame.samples, snapshot.timestamp);
    }
}

void PerformanceStatistics::addMemoryRecord(const MemoryRecord& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    addMetric("memory_usage", record.currentMemoryUsage, record.timestamp);
    if (record.isAllocation) {
        addMetric("allocations", 1.0, record.timestamp);
    } else {
        addMetric("deallocations", 1.0, record.timestamp);
    }
}

void PerformanceStatistics::addThreadRecord(const ThreadRecord& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    addMetric("thread_cpu_time", record.cpuTime, record.timestamp);
    addMetric("thread_active_time", record.activeTime, record.timestamp);
}

std::vector<double> PerformanceStatistics::getMetricHistory(const std::string& name,
                                                            size_t lastN) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metricHistory_.find(name);
    if (it == metricHistory_.end()) {
        return std::vector<double>();
    }
    
    const auto& history = it->second;
    size_t startIdx = std::max<size_t>(0, static_cast<long>(history.size()) - lastN);
    
    std::vector<double> result;
    for (size_t i = startIdx; i < history.size(); ++i) {
        result.push_back(history[i].second);
    }
    return result;
}

double PerformanceStatistics::calculateTrend(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metricHistory_.find(name);
    if (it == metricHistory_.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    std::vector<double> x;
    std::vector<double> y;
    for (const auto& pair : it->second) {
        x.push_back(static_cast<double>(pair.first));
        y.push_back(pair.second);
    }
    
    double trend, r2;
    return calculateLinearRegression(x, y, trend, r2);
}

void PerformanceStatistics::detectAnomalies(double threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : metricHistory_) {
        const std::string& name = pair.first;
        const auto& history = pair.second;
        
        if (history.size() < 5) continue;
        
        // Calculate mean and standard deviation
        double sum = 0.0, sumSq = 0.0;
        for (const auto& record : history) {
            sum += record.second;
            sumSq += record.second * record.second;
        }
        
        double mean = sum / history.size();
        double variance = (sumSq / history.size()) - (mean * mean);
        double stddev = std::sqrt(variance);
        
        if (stddev < 1e-10) continue;  // No variation
        
        // Check the most recent value
        double latestValue = history.back().second;
        double deviation = std::abs(latestValue - mean) / stddev;
        
        if (deviation > threshold) {
            Anomaly anomaly;
            anomaly.metricName = name;
            anomaly.timestamp = history.back().first;
            anomaly.value = latestValue;
            anomaly.expectedValue = mean;
            anomaly.deviation = deviation;
            anomaly.severity = deviation > 5.0 ? "CRITICAL" : 
                              deviation > 3.0 ? "ERROR" : "WARNING";
            anomaly.description = "Value " + std::to_string(latestValue) +
                                " deviates from expected " + std::to_string(mean) +
                                " by " + std::to_string(deviation) + " sigma";
            
            anomalies_.push_back(anomaly);
        }
    }
}

std::string PerformanceStatistics::generateReport() const {
    std::stringstream report;
    report << "=== NLM Performance Analysis Report ===\n\n";
    
    // Summary statistics
    report << "Trends:\n";
    for (const auto& trend : trends_) {
        report << "  - " << trend.metricName << ": trend = " << trend.trend 
               << ", r² = " << trend.r2 << "\n";
    }
    
    report << "\nAnomalies Detected: " << anomalies_.size() << "\n";
    for (const auto& anomaly : anomalies_) {
        report << "  - " << anomaly.metricName << " (" << anomaly.severity << ")\n";
        report << "    Value: " << anomaly.value << ", Expected: " << anomaly.expectedValue 
               << ", Deviation: " << anomaly.deviation << " sigma\n";
    }
    
    report << "\n=== End Report ===\n";
    return report.str();
}

PerformanceStatistics::Trend PerformanceStatistics::calculateTrend(
    const std::vector<double>& x, const std::vector<double>& y,
    double& trend, double& r2) const
{
    Trend result;
    if (x.size() != y.size() || x.size() < 2) {
        return result;
    }
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    size_t n = x.size();
    
    for (size_t i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
    }
    
    double denominator = (n * sumX2 - sumX * sumX);
    if (std::abs(denominator) < 1e-10) {
        return result;
    }
    
    trend = (n * sumXY - sumX * sumY) / denominator;
    double meanX = sumX / n;
    double meanY = sumY / n;
    
    double numerator = (n * sumXY - sumX * sumY);
    double denom2 = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    
    if (std::abs(denom2) < 1e-10) {
        r2 = 0.0;
    } else {
        r2 = numerator * numerator / (denom2 * denom2);
    }
    
    // Calculate other statistics
    result.mean = meanY;
    double variance = (sumY2 / n) - (meanY * meanY);
    result.stddev = std::sqrt(variance > 0.0 ? variance : 0.0);
    result.min = *std::min_element(y.begin(), y.end());
    result.max = *std::max_element(y.begin(), y.end());
    
    return result;
}

// NeuralPerformanceAnalyzer implementation
NeuralPerformanceAnalyzer::NeuralPerformanceAnalyzer()
    : monitor_(), statistics_() {}

std::vector<std::string> NeuralPerformanceAnalyzer::identifyPerformanceIssues(
    const PerformanceMetrics& metrics) const
{
    std::vector<std::string> issues;
    
    if (metrics.cpuUtilization > 0.9) {
        issues.push_back("CPU utilization is high (" + 
                        std::to_string(metrics.cpuUtilization * 100) + "%)");
    }
    
    if (metrics.memoryUtilization > 0.8) {
        issues.push_back("Memory utilization is high (" + 
                        std::to_string(metrics.memoryUtilization * 100) + "%)");
    }
    
    if (metrics.stepsPerSecond < 100) {
        issues.push_back("Steps per second is low (" + 
                        std::to_string(metrics.stepsPerSecond) + ")");
    }
    
    if (metrics.realTimeFactor < 0.5) {
        issues.push_back("Real-time factor is low (" + 
                        std::to_string(metrics.realTimeFactor) + ")");
    }
    
    if (metrics.spikeRate > 10000) {
        issues.push_back("Spike rate is very high (" + 
                        std::to_string(metrics.spikeRate) + " Hz)");
    }
    
    return issues;
}

std::map<std::string, double> NeuralPerformanceAnalyzer::getOptimizationRecommendations(
    const PerformanceMetrics& metrics) const
{
    std::map<std::string, double> recommendations;
    
    // Calculate efficiency metrics
    double efficiency = calculateEfficiency(metrics);
    double bottleneckScore = calculateBottleneckScore(metrics);
    
    // Recommend optimizations based on bottlenecks
    if (bottleneckScore > 0.7) {
        recommendations["parallel_optimization"] = 1.0 - efficiency;
        recommendations["memory_optimization"] = 0.5;
    }
    
    if (metrics.cpuUtilization < 0.5) {
        recommendations["load_balancing"] = 1.0 - metrics.cpuUtilization;
    }
    
    if (metrics.cacheHitRate < 0.8) {
        recommendations["cache_optimization"] = 1.0 - metrics.cacheHitRate;
    }
    
    return recommendations;
}

NeuralPerformanceAnalyzer::NeuralPerformanceTrend
NeuralPerformanceAnalyzer::analyzeTrends(const PerformanceMetrics& metrics,
                                         size_t historySize) const
{
    NeuralPerformanceTrend trend;
    
    // Extract trends from metrics
    trend.neuronScaling = metrics.stepsPerSecond / static_cast<double>(metrics.totalNeurons);
    trend.spikeEfficiency = metrics.spikeRate / metrics.stepsPerSecond;
    trend.plasticityEfficiency = 0.0;  // Would need to track separately
    trend.memoryEfficiency = metrics.totalNeurons / (metrics.allocatedMemory / 1024.0 / 1024.0); // neurons per MB
    trend.simulationSpeed = metrics.stepsPerSecond;
    trend.realTimeFactor = metrics.realTimeFactor;
    
    return trend;
}

std::vector<NeuralPerformanceAnalyzer::BenchmarkResult>
NeuralPerformanceAnalyzer::runBenchmarks(
    const PerformanceMetrics& metrics,
    const std::vector<std::tuple<std::string, size_t, size_t>>& benchmarks) const
{
    std::vector<BenchmarkResult> results;
    
    for (const auto& bench : benchmarks) {
        BenchmarkResult result;
        result.benchmarkName = std::get<0>(bench);
        result.neurons = std::get<1>(bench);
        result.synapses = std::get<2>(bench);
        result.steps = metrics.steps;  // Need to track steps separately
        result.targetStepsPerSecond = 1000.0; // Would be benchmark specific
        result.actualStepsPerSecond = metrics.stepsPerSecond;
        result.passed = metrics.stepsPerSecond >= result.targetStepsPerSecond;
        result.performanceRatio = metrics.stepsPerSecond / result.targetStepsPerSecond;
        
        // Generate details
        std::stringstream ss;
        ss << "Target: " << result.targetStepsPerSecond << " SPS, "
           << "Actual: " << result.actualStepsPerSecond << " SPS\n";
        ss << "Efficiency: " << (result.performanceRatio * 100) << "%\n";
        if (!result.passed) {
            ss << "FAILED: Performance below target\n";
        }
        result.details = ss.str();
        
        results.push_back(result);
    }
    
    return results;
}

} // namespace nlm