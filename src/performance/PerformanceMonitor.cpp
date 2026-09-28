// Performance Monitor Implementation
// Implementation of the PerformanceMonitor class and related monitoring tools

#include "performance/PerformanceMonitor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <chrono>
#include <sys/resource.h> // For memory usage (Linux)
#include <unistd.h> // For CPU usage (Linux)
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Helper function to get current memory usage in MB (Linux)
double getCurrentMemoryUsageMB() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024.0; // Convert KB to MB on Linux
}

// Helper function to get CPU usage percentage (simplified implementation)
double getCurrentCPUUsagePercent() {
    // This is a simplified implementation - in a real implementation,
    // you would need to track CPU time over multiple samples
    static double previousCPU = 0.0;
    static auto previousTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - previousTime).count();
    
    if (elapsed < 0.1) { // Don't calculate too frequently
        return 0.0;
    }
    
    // Get CPU time
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    double currentCPU = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
    
    double cpuUsage = ((currentCPU - previousCPU) / elapsed) * 100.0;
    previousCPU = currentCPU;
    previousTime = currentTime;
    
    return std::max(0.0, std::min(100.0, cpuUsage));
}

PerformanceMonitor::PerformanceMonitor(const MonitorConfig& config) : config_(config), enabled_(true) {
    if (enabled_) {
        monitoringThreadRunning_ = true;
        monitoringThread_ = std::thread(&PerformanceMonitor::collectPerformanceData, this);
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    enabled_ = false;
    if (monitoringThreadRunning_) {
        monitoringThread_.join();
    }
}

std::string PerformanceMonitor::startSession(const std::string& sessionId, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    sessions_[sessionId] = ProfilingSession(sessionId, description);
    return sessionId;
}

bool PerformanceMonitor::endSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return false;
    }
    
    it->second.endTime = std::chrono::high_resolution_clock::now();
    return true;
}

void PerformanceMonitor::recordMetric(const std::string& sessionId, const std::string& name,
                                      const std::string& category, double value,
                                      const std::unordered_map<std::string, std::string>& tags) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return; // Session not found
    }
    
    PerformanceMetric metric(name, category, value);
    metric.tags = tags;
    it->second.metrics.push_back(metric);
    
    // Update aggregates
    updateAggregates(metric);
}

std::vector<PerformanceMonitor::PerformanceMetric> PerformanceMonitor::getMetrics(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return std::vector<PerformanceMetric>();
    }
    
    return it->second.metrics;
}

std::unordered_map<std::string, double> PerformanceMonitor::getAggregatedMetrics(const std::string& sessionId,
                                                                                   const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::unordered_map<std::string, double> aggregates;
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return aggregates;
    }
    
    const auto& session = it->second;
    
    for (const auto& metric : session.metrics) {
        if (!category.empty() && metric.category != category) {
            continue;
        }
        
        aggregates[metric.name] = metric.value;
    }
    
    return aggregates;
}

std::vector<std::string> PerformanceMonitor::detectBottlenecks(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return std::vector<std::string>();
    }
    
    return generateBottleneckReport(it->second);
}

std::unordered_map<std::string, double> PerformanceMonitor::getStatistics(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::unordered_map<std::string, double> statistics;
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return statistics;
    }
    
    const auto& session = it->second;
    
    // Calculate basic statistics for each category
    std::unordered_map<std::string, std::vector<double>> categoryValues;
    
    for (const auto& metric : session.metrics) {
        categoryValues[metric.category].push_back(metric.value);
    }
    
    for (const auto& pair : categoryValues) {
        const auto& values = pair.second;
        if (values.empty()) continue;
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sqSum = 0.0;
        for (double value : values) {
            sqSum += (value - mean) * (value - mean);
        }
        double stdDev = std::sqrt(sqSum / values.size());
        
        statistics[pair.first + "_mean"] = mean;
        statistics[pair.first + "_stddev"] = stdDev;
        statistics[pair.first + "_min"] = *std::min_element(values.begin(), values.end());
        statistics[pair.first + "_max"] = *std::max_element(values.begin(), values.end());
        statistics[pair.first + "_count"] = static_cast<double>(values.size());
    }
    
    return statistics;
}

bool PerformanceMonitor::exportData(const std::string& sessionId, const std::string& filename,
                                     const std::string& format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return false;
    }
    
    if (format == "json") {
        return exportToJSON(filename, it->second);
    } else if (format == "csv") {
        return exportToCSV(filename, it->second);
    } else if (format == "html") {
        return exportToHTML(filename, it->second);
    }
    
    return false;
}

std::string PerformanceMonitor::generateReport(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return "Session not found: " + sessionId;
    }
    
    const auto& session = it->second;
    
    std::stringstream report;
    report << "=== Performance Report ===" << std::endl;
    report << "Session ID: " << session.sessionId << std::endl;
    report << "Description: " << session.description << std::endl;
    report << "Duration: " << session.getDuration() << " seconds" << std::endl;
    report << "Number of metrics: " << session.metrics.size() << std::endl;
    report << std::endl;
    
    // Summary statistics
    report << "=== Category Statistics ===" << std::endl;
    std::unordered_map<std::string, std::vector<double>> categoryValues;
    
    for (const auto& metric : session.metrics) {
        categoryValues[metric.category].push_back(metric.value);
    }
    
    for (const auto& pair : categoryValues) {
        const auto& values = pair.second;
        if (values.empty()) continue;
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        report << pair.first << ": count=" << values.size()
               << ", min=" << *std::min_element(values.begin(), values.end())
               << ", max=" << *std::max_element(values.begin(), values.end())
               << ", mean=" << mean << std::endl;
    }
    
    report << std::endl;
    
    // Bottleneck detection
    auto bottlenecks = generateBottleneckReport(session);
    if (!bottlenecks.empty()) {
        report << "=== Detected Bottlenecks ===" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            report << "- " << bottleneck << std::endl;
        }
    }
    
    return report.str();
}

void PerformanceMonitor::cleanupOldSessions(double maxAge) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    double maxAgeDuration = maxAge;
    
    std::vector<std::string> toRemove;
    for (const auto& pair : sessions_) {
        auto duration = std::chrono::duration<double>(currentTime - pair.second.startTime).count();
        if (duration > maxAgeDuration) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& sessionId : toRemove) {
        sessions_.erase(sessionId);
    }
}

void PerformanceMonitor::collectPerformanceData() {
    while (enabled_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(config_.samplingInterval));
        
        // Collect performance data for all active sessions
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double currentMemory = getCurrentMemoryUsageMB();
        double currentCPU = getCurrentCPUUsagePercent();
        
        // Record system metrics for each session
        for (auto& pair : sessions_) {
            auto& session = pair.second;
            
            // Record memory usage
            recordMetric(pair.first, "memory_usage", "memory", currentMemory);
            
            // Record CPU usage
            recordMetric(pair.first, "cpu_usage", "cpu", currentCPU);
            
            // Record simulation duration
            double duration = session.getDuration();
            recordMetric(pair.first, "simulation_duration", "simulation", duration);
        }
    }
}

void PerformanceMonitor::updateAggregates(const PerformanceMetric& metric) {
    // Update session aggregates
    for (auto& pair : sessions_) {
        auto& session = pair.second;
        
        auto it = std::find_if(session.metrics.begin(), session.metrics.end(),
                              [&metric](const PerformanceMetric& m) {
                                  return m.name == metric.name && m.category == metric.category;
                              });n
        if (it != session.metrics.end()) {
            // Update aggregate
            double count = it->count;
            double oldSum = it->average * count;
            double newSum = oldSum + metric.value;
            it->average = newSum / (count + 1);
            it->count = count + 1;
            it->minValue = std::min(it->minValue, metric.value);
            it->maxValue = std::max(it->maxValue, metric.value);
            
            double variance = (it->average - it->minValue) * (it->average - it->minValue);
            it->standardDeviation = std::sqrt(variance);
        }
    }
}

std::vector<std::string> PerformanceMonitor::generateBottleneckReport(const ProfilingSession& session) const {
    std::vector<std::string> bottlenecks;
    
    // Check for high memory usage
    auto memoryMetrics = getAggregatedMetrics(session.sessionId, "memory");
    if (memoryMetrics.find("memory_usage_mean") != memoryMetrics.end()) {
        double avgMemory = memoryMetrics["memory_usage_mean"];
        if (avgMemory > 1000.0) { // More than 1GB
            bottlenecks.push_back("High memory usage: " + std::to_string(avgMemory) + " MB average");
        }
    }
    
    // Check for high CPU usage
    auto cpuMetrics = getAggregatedMetrics(session.sessionId, "cpu");
    if (cpuMetrics.find("cpu_usage_mean") != cpuMetrics.end()) {
        double avgCPU = cpuMetrics["cpu_usage_mean"];
        if (avgCPU > 80.0) { // More than 80%
            bottlenecks.push_back("High CPU usage: " + std::to_string(avgCPU) + "% average");
        }
    }
    
    // Check for long simulation durations
    auto simMetrics = getAggregatedMetrics(session.sessionId, "simulation");
    if (simMetrics.find("simulation_duration_mean") != simMetrics.end()) {
        double avgDuration = simMetrics["simulation_duration_mean"];
        if (avgDuration > 60.0) { // More than 1 minute
            bottlenecks.push_back("Long simulation duration: " + std::to_string(avgDuration) + " seconds average");
        }
    }
    
    return bottlenecks;
}

bool PerformanceMonitor::exportToJSON(const std::string& filename, const ProfilingSession& session) const {
    json j;
    
    j["sessionId"] = session.sessionId;
    j["description"] = session.description;
    j["startTime"] = std::chrono::duration<double>(session.startTime.time_since_epoch()).count();
    j["endTime"] = std::chrono::duration<double>(session.endTime.time_since_epoch()).count();
    j["duration"] = session.getDuration();
    
    // Add metrics
    j["metrics"] = json::array();
    for (const auto& metric : session.metrics) {
        json metricJson;
        metricJson["name"] = metric.name;
        metricJson["category"] = metric.category;
        metricJson["value"] = metric.value;
        metricJson["minValue"] = metric.minValue;
        metricJson["maxValue"] = metric.maxValue;
        metricJson["average"] = metric.average;
        metricJson["standardDeviation"] = metric.standardDeviation;
        metricJson["count"] = metric.count;
        metricJson["timestamp"] = std::chrono::duration<double>(metric.timestamp.time_since_epoch()).count();
        
        // Add tags
        if (!metric.tags.empty()) {
            metricJson["tags"] = metric.tags;
        }
        
        j["metrics"].push_back(metricJson);
    }
    
    // Add aggregates
    j["aggregates"] = getAggregatedMetrics(session.sessionId);
    
    // Add bottlenecks
    auto bottlenecks = generateBottleneckReport(session);
    if (!bottlenecks.empty()) {
        j["bottlenecks"] = bottlenecks;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    file.close();
    
    return true;
}

bool PerformanceMonitor::exportToCSV(const std::string& filename, const ProfilingSession& session) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "Metric Name,Category,Value,Min Value,Max Value,Average,Standard Deviation,Count,Timestamp" << std::endl;
    
    // Write metrics
    for (const auto& metric : session.metrics) {
        file << metric.name << "," << metric.category << "," << metric.value
             << "," << metric.minValue << "," << metric.maxValue
             << "," << metric.average << "," << metric.standardDeviation
             << "," << metric.count << ","
             << std::chrono::duration<double>(metric.timestamp.time_since_epoch()).count()
             << std::endl;
    }
    
    file.close();
    return true;
}

bool PerformanceMonitor::exportToHTML(const std::string& filename, const ProfilingSession& session) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write HTML content
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "<title>Performance Report - " << session.sessionId << "</title>" << std::endl;
    file << "<style>" << std::endl;
    file << "body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    file << "h1 { color: #333; }" << std::endl;
    file << "h2 { color: #666; border-bottom: 2px solid #ccc; padding-bottom: 10px; }" << std::endl;
    file << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }" << std::endl;
    file << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    file << "th { background-color: #f2f2f2; }" << std::endl;
    file << "tr:nth-child(even) { background-color: #f9f9f9; }" << std::endl;
    file << "</style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    
    // Write report header
    file << "<h1>Performance Report - " << session.sessionId << "</h1>" << std::endl;
    file << "<p><strong>Description:</strong> " << session.description << "</p>" << std::endl;
    file << "<p><strong>Duration:</strong> " << session.getDuration() << " seconds</p>" << std::endl;
    file << "<p><strong>Number of Metrics:</strong> " << session.metrics.size() << "</p>" << std::endl;
    
    // Write metrics table
    file << "<h2>Metrics</h2>" << std::endl;
    file << "<table>" << std::endl;
    file << "<tr><th>Name</th><th>Category</th><th>Value</th><th>Min</th><th>Max</th><th>Average</th><th>Std Dev</th><th>Count</th><th>Timestamp</th></tr>" << std::endl;
    
    for (const auto& metric : session.metrics) {
        file << "<tr>" << std::endl;
        file << "<td>" << metric.name << "</td><td>" << metric.category << "</td><td>" << metric.value << "</td><td>" << metric.minValue << "</td><td>" << metric.maxValue << "</td><td>" << metric.average << "</td><td>" << metric.standardDeviation << "</td><td>" << metric.count << "</td><td>" << std::chrono::duration<double>(metric.timestamp.time_since_epoch()).count() << "</td>" << std::endl;
        file << "</tr>" << std::endl;
    }
    
    file << "</table>" << std::endl;
    
    // Write bottleneck report
    auto bottlenecks = generateBottleneckReport(session);
    if (!bottlenecks.empty()) {
        file << "<h2>Detected Bottlenecks</h2>" << std::endl;
        file << "<ul>" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            file << "<li>" << bottleneck << "</li>" << std::endl;
        }
        file << "</ul>" << std::endl;
    }
    
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer(const std::string& name, const std::string& category) 
    : name_(name), category_(category), running_(false) {
    reset();
}

PerformanceTimer::~PerformanceTimer() {
    if (running_) {
        // Timer was not stopped before destruction
        std::cerr << "Warning: PerformanceTimer " << name_ << " was not stopped before destruction" << std::endl;
    }
}

double PerformanceTimer::getElapsed() const {
    if (!running_) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - startTime_).count();
    return elapsed;
}

void PerformanceTimer::reset() {
    startTime_ = std::chrono::high_resolution_clock::now();
    running_ = true;
}

// MemoryUsageMonitor implementation
MemoryUsageMonitor::MemoryUsageMonitor() : monitoring_(false) {
}

MemoryUsageMonitor::~MemoryUsageMonitor() {
    stopMonitoring();
}

double MemoryUsageMonitor::getCurrentMemoryUsage() const {
    if (!monitoring_) {
        return 0.0;
    }
    return getCurrentMemoryUsageMB();
}

double MemoryUsageMonitor::getPeakMemoryUsage() const {
    return peakMemoryUsage_;
}

void MemoryUsageMonitor::reset() {
    peakMemoryUsage_ = 0.0;
    memoryHistory_.clear();
}

void MemoryUsageMonitor::startMonitoring() {
    if (monitoring_) {
        return;
    }
    
    monitoring_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void MemoryUsageMonitor::stopMonitoring() {
    monitoring_ = false;
}

std::vector<double> MemoryUsageMonitor::getMemoryHistory() const {
    return memoryHistory_;
}

std::unordered_map<std::string, double> MemoryUsageMonitor::getStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (memoryHistory_.empty()) {
        return stats;
    }
    
    double sum = std::accumulate(memoryHistory_.begin(), memoryHistory_.end(), 0.0);
    double mean = sum / memoryHistory_.size();
    
    double sqSum = 0.0;
    for (double value : memoryHistory_) {
        sqSum += (value - mean) * (value - mean);
    }
    double stdDev = std::sqrt(sqSum / memoryHistory_.size());
    
    stats["mean"] = mean;
    stats["stddev"] = stdDev;
    stats["min"] = *std::min_element(memoryHistory_.begin(), memoryHistory_.end());
    stats["max"] = *std::max_element(memoryHistory_.begin(), memoryHistory_.end());
    stats["count"] = static_cast<double>(memoryHistory_.size());
    
    return stats;
}

// CPUUsageMonitor implementation
CPUUsageMonitor::CPUUsageMonitor() : monitoring_(false) {
}

CPUUsageMonitor::~CPUUsageMonitor() {
    stopMonitoring();
}

double CPUUsageMonitor::getCurrentCPUUsage() const {
    if (!monitoring_) {
        return 0.0;
    }
    return getCurrentCPUUsagePercent();
}

double CPUUsageMonitor::getAverageCPUUsage() const {
    if (cpuHistory_.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(cpuHistory_.begin(), cpuHistory_.end(), 0.0);
    return sum / cpuHistory_.size();
}

void CPUUsageMonitor::reset() {
    totalCPUUsage_ = 0.0;
    sampleCount_ = 0;
    cpuHistory_.clear();
}

void CPUUsageMonitor::startMonitoring() {
    if (monitoring_) {
        return;
    }
    
    monitoring_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void CPUUsageMonitor::stopMonitoring() {
    monitoring_ = false;
}

std::vector<double> CPUUsageMonitor::getCPUHistory() const {
    return cpuHistory_;
}

std::unordered_map<std::string, double> CPUUsageMonitor::getStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (cpuHistory_.empty()) {
        return stats;
    }
    
    double sum = std::accumulate(cpuHistory_.begin(), cpuHistory_.end(), 0.0);
    double mean = sum / cpuHistory_.size();
    
    double sqSum = 0.0;
    for (double value : cpuHistory_) {
        sqSum += (value - mean) * (value - mean);
    }
    double stdDev = std::sqrt(sqSum / cpuHistory_.size());
    
    stats["mean"] = mean;
    stats["stddev"] = stdDev;
    stats["min"] = *std::min_element(cpuHistory_.begin(), cpuHistory_.end());
    stats["max"] = *std::max_element(cpuHistory_.begin(), cpuHistory_.end());
    stats["count"] = static_cast<double>(cpuHistory_.size());
    
    return stats;
}

// SimulationProfiler implementation
SimulationProfiler::SimulationProfiler(const std::string& sessionId, const PerformanceMonitor::MonitorConfig& config)
    : sessionId_(sessionId), performanceMonitor_(config), memoryMonitor_(), cpuMonitor_() {
}

SimulationProfiler::~SimulationProfiler() {
}

std::unique_ptr<PerformanceTimer> SimulationProfiler::profileStep(int stepNumber) {
    std::string timerName = "step_" + std::to_string(stepNumber);
    return std::make_unique<PerformanceTimer>(timerName, "simulation");
}

void SimulationProfiler::recordSimulationMetrics(int stepNumber,
                                                   const std::unordered_map<std::string, double>& metrics) {
    for (const auto& pair : metrics) {
        performanceMonitor_.recordMetric(sessionId_, pair.first, "simulation", pair.second);
    }
}

std::string SimulationProfiler::getResults() const {
    return performanceMonitor_.generateReport(sessionId_);
}

bool SimulationProfiler::exportData(const std::string& filename) {
    return performanceMonitor_.exportData(sessionId_, filename);
}

} // namespace nlm