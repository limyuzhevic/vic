#include "PerformanceProfiler.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

namespace nlm {

PerformanceProfiler::PerformanceProfiler() : systemMonitoring(false) {
}

void PerformanceProfiler::startProfiling(const std::string& sessionName) {
    if (sessions.find(sessionName) != sessions.end()) {
        log("ERROR: Session already exists: " + sessionName);
        return;
    }
    
    ProfilingSession session;
    session.sessionName = sessionName;
    session.startTime = std::chrono::steady_clock::now();
    session.isActive = true;
    
    sessions[sessionName] = session;
    log("Started profiling session: " + sessionName);
}

void PerformanceProfiler::stopProfiling(const std::string& sessionName) {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return;
    }
    
    sessions[sessionName].endTime = std::chrono::steady_clock::now();
    sessions[sessionName].isActive = false;
    
    log("Stopped profiling session: " + sessionName);
}

void PerformanceProfiler::recordMetric(const std::string& sessionName, const std::string& metricName, 
                                     double value, const std::string& unit) {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return;
    }
    
    PerformanceData data;
    data.metricName = metricName;
    data.value = value;
    data.timestamp = std::chrono::steady_clock::now();
    data.unit = unit;
    
    sessions[sessionName].metrics.push_back(data);
}

double PerformanceProfiler::getAverageTime(const std::string& sessionName, const std::string& metricName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return 0.0;
    }
    
    double total = 0.0;
    size_t count = 0;
    
    for (const auto& data : sessions.at(sessionName).metrics) {
        if (data.metricName == metricName) {
            total += data.value;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0.0;
}

double PerformanceProfiler::getStandardDeviation(const std::string& sessionName, const std::string& metricName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return 0.0;
    }
    
    std::vector<double> values;
    for (const auto& data : sessions.at(sessionName).metrics) {
        if (data.metricName == metricName) {
            values.push_back(data.value);
        }
    }
    
    if (values.size() <= 1) {
        return 0.0;
    }
    
    double mean = getAverageTime(sessionName, metricName);
    double variance = 0.0;
    
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    
    return std::sqrt(variance / values.size());
}

PerformanceProfiler::BenchmarkResult PerformanceProfiler::runBenchmark(const std::string& benchmarkName, 
                                                                       const std::function<void()>& benchmarkFunction) {
    BenchmarkResult result;
    result.benchmarkName = benchmarkName;
    
    auto startTime = std::chrono::steady_clock::now();
    benchmarkFunction();
    auto endTime = std::chrono::steady_clock::now();
    
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    result.cpuUsage = calculateCpuUsage();
    result.memoryUsage = getMemoryUsage();
    result.peakMemoryUsage = result.memoryUsage;  // Placeholder
    result.efficiencyScore = calculateEfficiency(result.executionTime, result.memoryUsage, result.cpuUsage);
    
    log("Benchmark completed: " + benchmarkName + " - Time: " + std::to_string(result.executionTime) + "s");
    
    return result;
}

PerformanceProfiler::BenchmarkResult PerformanceProfiler::runBenchmark(const std::string& benchmarkName,
                                                                       const std::vector<std::function<void()>>& benchmarkFunctions) {
    BenchmarkResult result;
    result.benchmarkName = benchmarkName;
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& func : benchmarkFunctions) {
        func();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    result.cpuUsage = calculateCpuUsage();
    result.memoryUsage = getMemoryUsage();
    result.peakMemoryUsage = result.memoryUsage;
    result.efficiencyScore = calculateEfficiency(result.executionTime, result.memoryUsage, result.cpuUsage);
    
    log("Batch benchmark completed: " + benchmarkName + " - Time: " + std::to_string(result.executionTime) + "s");
    
    return result;
}

std::string PerformanceProfiler::analyzePerformance(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    std::stringstream analysis;
    analysis << "Performance Analysis for Session: " << sessionName << std::endl;
    analysis << "=========================================" << std::endl;
    
    const auto& session = sessions.at(sessionName);
    analysis << "Duration: " << std::chrono::duration<double>(session.endTime - session.startTime).count() << " seconds" << std::endl;
    analysis << "Metrics Collected: " << session.metrics.size() << std::endl;
    
    // Group metrics by name
    std::unordered_map<std::string, std::vector<double>> metricGroups;
    for (const auto& data : session.metrics) {
        metricGroups[data.metricName].push_back(data.value);
    }
    
    analysis << std::endl;
    for (const auto& pair : metricGroups) {
        const auto& values = pair.second;
        if (values.size() <= 1) continue;
        
        double sum = 0.0, min = values[0], max = values[0];
        for (double value : values) {
            sum += value;
            if (value < min) min = value;
            if (value > max) max = value;
        }
        
        double mean = sum / values.size();
        double stdDev = 0.0;
        for (double value : values) {
            stdDev += (value - mean) * (value - mean);
        }
        stdDev = std::sqrt(stdDev / values.size());
        
        analysis << pair.first << ": " << mean << " ± " << stdDev << " (min: " << min << ", max: " << max << ")" << std::endl;
    }
    
    return analysis.str();
}

std::string PerformanceProfiler::generatePerformanceReport(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    std::stringstream report;
    report << "Performance Report for Session: " << sessionName << std::endl;
    report << "=====================================" << std::endl;
    report << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    report << std::endl;
    
    const auto& session = sessions.at(sessionName);
    
    // Summary
    report << "Summary:" << std::endl;
    report << "  - Number of metrics: " << session.metrics.size() << std::endl;
    report << "  - Duration: " << std::chrono::duration<double>(session.endTime - session.startTime).count() << " seconds" << std::endl;
    report << "  - Efficiency Score: " << calculateEfficiencyScore(sessionName) << std::endl;
    report << std::endl;
    
    // Detailed metrics
    report << "Detailed Metrics:" << std::endl;
    std::unordered_map<std::string, std::vector<PerformanceData>> metricGroups;
    for (const auto& data : session.metrics) {
        metricGroups[data.metricName].push_back(data);
    }
    
    for (const auto& pair : metricGroups) {
        report << "  " << pair.first << ": " << pair.second.size() << " samples" << std::endl;
    }
    
    return report.str();
}

double PerformanceProfiler::calculateEfficiencyScore(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return 0.0;
    }
    
    double totalTime = 0.0;
    double totalMemory = 0.0;
    double totalCpu = 0.0;
    
    const auto& session = sessions.at(sessionName);
    for (const auto& data : session.metrics) {
        totalTime += data.value;
        totalMemory += data.value;  // Placeholder
        totalCpu += data.value;     // Placeholder
    }
    
    size_t count = session.metrics.size();
    if (count == 0) return 0.0;
    
    return calculateEfficiency(totalTime / count, totalMemory / count, totalCpu / count);
}

void PerformanceProfiler::setProfilingConfig(const ProfilingConfig& config) {
    this->config = config;
}

const PerformanceProfiler::ProfilingConfig& PerformanceProfiler::getProfilingConfig() const {
    return config;
}

bool PerformanceProfiler::saveSessionData(const std::string& sessionName, const std::string& filepath) const {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Simple JSON-like output (placeholder for actual JSON library)
    file << "{\n  \"sessionName\": \"" << sessionName << "\"," << std::endl;
    file << "  \"duration\": " << std::chrono::duration<double>(sessions.at(sessionName).endTime - sessions.at(sessionName).startTime).count() << "," << std::endl;
    file << "  \"metrics\": [" << std::endl;
    
    const auto& session = sessions.at(sessionName);
    for (size_t i = 0; i < session.metrics.size(); ++i) {
        const auto& data = session.metrics[i];
        file << "    {\"name\": \"" << data.metricName << "\", \"value\": " << data.value << ", \"unit\": \"" << data.unit << "\"}";
        if (i < session.metrics.size() - 1) file << ",";
        file << std::endl;
    }
    
    file << "  ]\n}";
    
    file.close();
    log("Session data saved to: " + filepath);
    return true;
}

bool PerformanceProfiler::loadSessionData(const std::string& sessionName, const std::string& filepath) {
    // TODO: Implement session data loading
    log("Loading session data from: " + filepath);
    return false;
}

bool PerformanceProfiler::hasSession(const std::string& sessionName) const {
    return sessions.find(sessionName) != sessions.end();
}

std::vector<std::string> PerformanceProfiler::getSessionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : sessions) {
        names.push_back(pair.first);
    }
    return names;
}

void PerformanceProfiler::clearSession(const std::string& sessionName) {
    if (sessions.find(sessionName) != sessions.end()) {
        sessions.erase(sessionName);
        log("Cleared session: " + sessionName);
    }
}

void PerformanceProfiler::clearAllSessions() {
    sessions.clear();
    log("Cleared all profiling sessions");
}

std::string PerformanceProfiler::optimizePerformance(const std::string& sessionName) {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    return "Optimization recommendations for " + sessionName + " will be generated here.";
}

std::vector<std::string> PerformanceProfiler::getOptimizationRecommendations(const std::string& sessionName) const {
    std::vector<std::string> recommendations;
    
    if (!hasSession(sessionName)) {
        return recommendations;
    }
    
    recommendations.push_back("Consider increasing batch size for better cache utilization");
    recommendations.push_back("Optimize memory access patterns to reduce cache misses");
    recommendations.push_back("Consider parallelization for CPU-intensive tasks");
    
    return recommendations;
}

void PerformanceProfiler::startSystemMonitoring(const std::string& sessionName) {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return;
    }
    
    systemMonitoring = true;
    systemMonitoringThread = std::thread([this, sessionName]() {
        while (systemMonitoring && hasSession(sessionName)) {
            // Record system metrics
            double cpuUsage = calculateCpuUsage();
            size_t memoryUsage = getMemoryUsage();
            recordMetric(sessionName, "cpu_usage", cpuUsage, "%");
            recordMetric(sessionName, "memory_usage", static_cast<double>(memoryUsage), "MB");
            
            std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(config.samplingInterval)));
        }
    });
    
    log("Started system monitoring for session: " + sessionName);
}

void PerformanceProfiler::stopSystemMonitoring(const std::string& sessionName) {
    systemMonitoring = false;
    if (systemMonitoringThread.joinable()) {
        systemMonitoringThread.join();
    }
    
    log("Stopped system monitoring for session: " + sessionName);
}

// Implementation of helper functions
double PerformanceProfiler::calculateCpuUsage() const {
    // Placeholder implementation
    return 50.0;  // Return 50% CPU usage as placeholder
}

size_t PerformanceProfiler::getMemoryUsage() const {
    // Placeholder implementation
    return 100 * 1024 * 1024;  // Return 100 MB as placeholder
}

double PerformanceProfiler::calculateEfficiency(double executionTime, size_t memoryUsage, double cpuUsage) const {
    if (executionTime <= 0.0) return 0.0;
    
    double memoryEfficiency = 1.0 / (1.0 + static_cast<double>(memoryUsage) / (100 * 1024 * 1024));  // Normalize to 100MB
    double cpuEfficiency = 1.0 / (1.0 + std::abs(cpuUsage - 50.0) / 50.0);  // Normalize around 50% CPU
    
    return (executionTime > 0.0) ? (1.0 / executionTime) * memoryEfficiency * cpuEfficiency : 0.0;
}

std::string PerformanceProfiler::generateBenchmarkReport(const BenchmarkResult& result) const {
    std::stringstream report;
    report << "Benchmark Report" << std::endl;
    report << "===============" << std::endl;
    report << "Benchmark Name: " << result.benchmarkName << std::endl;
    report << "Execution Time: " << result.executionTime << " seconds" << std::endl;
    report << "CPU Usage: " << result.cpuUsage << "%" << std::endl;
    report << "Memory Usage: " << result.memoryUsage << " MB" << std::endl;
    report << "Peak Memory Usage: " << result.peakMemoryUsage << " MB" << std::endl;
    report << "Efficiency Score: " << result.efficiencyScore << std::endl;
    report << "Recommendations: " << result.optimizationRecommendations << std::endl;
    
    return report.str();
}

} // namespace nlm