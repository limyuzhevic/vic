// PerformanceProfiler.cpp - Main performance profiling implementation

#include "profiler/PerformanceProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <sys/resource.h>
#include <unistd.h>
#include <cmath>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : isActive(false), sessionName("default"), monitoringActive(false) {
}

PerformanceProfiler::PerformanceProfiler(const ProfilerConfig& config)
    : isActive(false), sessionName("default"), monitoringActive(false), profilerConfig(config) {
}

PerformanceProfiler::~PerformanceProfiler() {
    if (isActive) {
        stopProfiling();
    }
    
    if (monitoringThread.joinable()) {
        monitoringActive = false;
        monitoringThread.join();
    }
}

void PerformanceProfiler::startProfiling(const std::string& sessionName) {
    NLM_LOG_INFO("Starting performance profiling session: " + sessionName);
    
    this->sessionName = sessionName;
    isActive = true;
    monitoringActive = true;
    
    startTime = std::chrono::steady_clock::now();
    endTime = std::chrono::steady_clock::time_point();
    
    performanceHistory.clear();
    addPerformanceSample();
    
    if (profilerConfig.verbose) {
        NLM_LOG_INFO("Performance profiling started with config:");
        NLM_LOG_INFO("  - Wall clock monitoring: " + std::to_string(profilerConfig.enableWallClock));
        NLM_LOG_INFO("  - CPU time monitoring: " + std::to_string(profilerConfig.enableCPUTime));
        NLM_LOG_INFO("  - Memory profiling: " + std::to_string(profilerConfig.enableMemoryProfiling));
        NLM_LOG_INFO("  - Thread profiling: " + std::to_string(profilerConfig.enableThreadProfiling));
        NLM_LOG_INFO("  - Real-time monitoring: " + std::to_string(profilerConfig.enableRealTimeMonitoring));
        NLM_LOG_INFO("  - Sampling interval: " + std::to_string(profilerConfig.samplingIntervalMs) + "ms");
    }
    
    if (profilerConfig.enableRealTimeMonitoring) {
        monitoringThread = std::thread(&PerformanceProfiler::monitoringFunction, this);
    }
    
    NLM_LOG_INFO("Performance profiling session started");
}

void PerformanceProfiler::stopProfiling() {
    if (!isActive) {
        NLM_LOG_WARNING("Cannot stop profiling: profiling not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping performance profiling session: " + sessionName);
    
    monitoringActive = false;
    
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
    
    endTime = std::chrono::steady_clock::now();
    isActive = false;
    
    cleanupOldData();
    
    NLM_LOG_INFO("Performance profiling session stopped");
}

void PerformanceProfiler::setCustomMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(dataMutex);
    customMetrics[name] = value;
}

PerformanceProfiler::PerformanceData PerformanceProfiler::getCurrentPerformanceData() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    PerformanceData data;
    
    if (!performanceHistory.empty()) {
        data = performanceHistory.back();
    }
    
    data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (profilerConfig.enableWallClock && isActive) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        data.wallClockTime = elapsed.count();
    }
    
    if (profilerConfig.enableCPUTime) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        data.cpuTime = usage.ru_utime.tv_sec * 1000.0 + usage.ru_utime.tv_usec / 1000.0 +
                       usage.ru_stime.tv_sec * 1000.0 + usage.ru_stime.tv_usec / 1000.0;
    }
    
    if (profilerConfig.enableMemoryProfiling) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        data.memoryUsage = usage.ru_maxrss * 1024.0; // KB to bytes
    }
    
    return data;
}

std::vector<PerformanceProfiler::PerformanceData> PerformanceProfiler::getPerformanceData(
    int64_t startTime, int64_t endTime) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    std::vector<PerformanceData> result;
    
    for (const auto& data : performanceHistory) {
        if (data.timestamp >= startTime && data.timestamp <= endTime) {
            result.push_back(data);
        }
    }
    
    return result;
}

std::string PerformanceProfiler::generateReport(const std::string& format) const {
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

bool PerformanceProfiler::exportData(const std::string& filename, const std::string& format) const {
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
    
    if (profilerConfig.verbose) {
        NLM_LOG_INFO("Performance data exported to: " + filename);
    }
    
    return true;
}

std::string PerformanceProfiler::getPerformanceStatistics() const {
    std::ostringstream oss;
    oss << "Performance Statistics for session: " << sessionName << "\n";
    oss << "==================================================\n\n";
    
    if (performanceHistory.empty()) {
        oss << "No performance data collected\n";
        return oss.str();
    }
    
    std::vector<double> wallClockTimes;
    std::vector<double> cpuTimes;
    std::vector<double> memoryUsages;
    
    for (const auto& data : performanceHistory) {
        wallClockTimes.push_back(data.wallClockTime);
        cpuTimes.push_back(data.cpuTime);
        memoryUsages.push_back(data.memoryUsage);
    }
    
    oss << "Collected " << performanceHistory.size() << " data points\n";
    oss << "Duration: " << getDuration() << "ms\n\n";
    
    if (!wallClockTimes.empty()) {
        double sum = std::accumulate(wallClockTimes.begin(), wallClockTimes.end(), 0.0);
        double avg = sum / wallClockTimes.size();
        double min = *std::min_element(wallClockTimes.begin(), wallClockTimes.end());
        double max = *std::max_element(wallClockTimes.begin(), wallClockTimes.end());
        
        oss << "Wall Clock Time:\n";
        oss << "  Average: " << avg << "ms\n";
        oss << "  Minimum: " << min << "ms\n";
        oss << "  Maximum: " << max << "ms\n\n";
    }
    
    if (!cpuTimes.empty()) {
        double sum = std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0.0);
        double avg = sum / cpuTimes.size();
        
        oss << "CPU Time:\n";
        oss << "  Average: " << avg << "ms\n\n";
    }
    
    if (!memoryUsages.empty()) {
        double sum = std::accumulate(memoryUsages.begin(), memoryUsages.end(), 0.0);
        double avg = sum / memoryUsages.size();
        double min = *std::min_element(memoryUsages.begin(), memoryUsages.end());
        double max = *std::max_element(memoryUsages.begin(), memoryUsages.end());
        
        oss << "Memory Usage:\n";
        oss << "  Average: " << avg / (1024 * 1024) << "MB\n";
        oss << "  Minimum: " << min / (1024 * 1024) << "MB\n";
        oss << "  Maximum: " << max / (1024 * 1024) << "MB\n\n";
    }
    
    return oss.str();
}

std::vector<std::string> PerformanceProfiler::detectBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    if (performanceHistory.size() < 2) {
        return bottlenecks;
    }
    
    // Check for memory bottlenecks
    std::vector<double> memoryUsages;
    for (const auto& data : performanceHistory) {
        memoryUsages.push_back(data.memoryUsage);
    }
    
    double avgMemory = std::accumulate(memoryUsages.begin(), memoryUsages.end(), 0.0) / memoryUsages.size();
    if (avgMemory > 1024 * 1024 * 1024) { // > 1GB
        bottlenecks.push_back("High memory usage detected (>1GB average)");
    }
    
    // Check for time bottlenecks
    int64_t duration = getDuration();
    if (duration > 60000) { // > 1 minute
        bottlenecks.push_back("Long execution time (>1 minute)");
    }
    
    // Check for performance degradation over time
    if (performanceHistory.size() >= 10) {
        size_t halfPoint = performanceHistory.size() / 2;
        double firstHalfTime = 0.0, secondHalfTime = 0.0;
        
        for (size_t i = 0; i < halfPoint; ++i) {
            firstHalfTime += performanceHistory[i].wallClockTime;
        }
        
        for (size_t i = halfPoint; i < performanceHistory.size(); ++i) {
            secondHalfTime += performanceHistory[i].wallClockTime;
        }
        
        if (secondHalfTime > firstHalfTime * 1.5) {
            bottlenecks.push_back("Performance degradation over time (second half > 1.5x first half)");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceProfiler::generateRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (performanceHistory.empty()) {
        return recommendations;
    }
    
    // Calculate performance metrics
    double totalWallClock = 0.0;
    double totalCpu = 0.0;
    double totalMemory = 0.0;
    
    for (const auto& data : performanceHistory) {
        totalWallClock += data.wallClockTime;
        totalCpu += data.cpuTime;
        totalMemory += data.memoryUsage;
    }
    
    size_t count = performanceHistory.size();
    double avgWallClock = totalWallClock / count;
    double avgCpu = totalCpu / count;
    double avgMemory = totalMemory / count;
    
    // Generate recommendations
    if (avgWallClock > 10000) { // > 10 seconds
        recommendations.push_back("Optimize wall-clock time - consider parallel processing");
    }
    
    if (avgCpu > avgWallClock * 0.9) { // CPU time is close to wall clock time
        recommendations.push_back("CPU time is close to wall clock - consider optimizing CPU-bound operations");
    }
    
    if (avgMemory > 512 * 1024 * 1024) { // > 512MB
        recommendations.push_back("Optimize memory usage - consider memory pooling or data structure optimization");
    }
    
    if (performanceHistory.size() > 100) {
        recommendations.push_back("Consider reducing sampling frequency to improve performance");
    }
    
    return recommendations;
}

std::string PerformanceProfiler::compareWithBaseline(
    const std::vector<PerformanceData>& baseline) const {
    std::ostringstream oss;
    oss << "Performance Comparison with Baseline\n";
    oss << "=====================================\n\n";
    
    if (baseline.empty()) {
        oss << "No baseline provided for comparison\n";
        return oss.str();
    }
    
    oss << "Baseline collected " << baseline.size() << " data points\n";
    oss << "Current session collected " << performanceHistory.size() << " data points\n\n";
    
    // Compare average metrics
    double baselineWallClock = 0.0, currentWallClock = 0.0;
    double baselineCpu = 0.0, currentCpu = 0.0;
    double baselineMemory = 0.0, currentMemory = 0.0;
    
    for (const auto& data : baseline) {
        baselineWallClock += data.wallClockTime;
        baselineCpu += data.cpuTime;
        baselineMemory += data.memoryUsage;
    }
    
    for (const auto& data : performanceHistory) {
        currentWallClock += data.wallClockTime;
        currentCpu += data.cpuTime;
        currentMemory += data.memoryUsage;
    }
    
    size_t baselineCount = baseline.size();
    size_t currentCount = performanceHistory.size();
    
    baselineWallClock /= baselineCount;
    currentWallClock /= currentCount;
    baselineCpu /= baselineCount;
    currentCpu /= currentCount;
    baselineMemory /= baselineCount;
    currentMemory /= currentCount;
    
    oss << "Average Metrics Comparison:\n";
    oss << "  Wall Clock Time: Baseline = " << baselineWallClock << "ms, Current = " << currentWallClock << "ms, Ratio = " << (currentWallClock / baselineWallClock) << "x\n";
    oss << "  CPU Time: Baseline = " << baselineCpu << "ms, Current = " << currentCpu << "ms, Ratio = " << (currentCpu / baselineCpu) << "x\n";
    oss << "  Memory Usage: Baseline = " << baselineMemory << " bytes, Current = " << currentMemory << " bytes, Ratio = " << (currentMemory / baselineMemory) << "x\n\n";
    
    // Determine improvement/regression
    if (currentWallClock < baselineWallClock * 0.9) {
        oss << "✓ Current performance is better than baseline (wall clock)" << std::endl;
    } else if (currentWallClock > baselineWallClock * 1.1) {
        oss << "⚠ Current performance is worse than baseline (wall clock)" << std::endl;
    }
    
    if (currentCpu < baselineCpu * 0.9) {
        oss << "✓ Current CPU performance is better than baseline" << std::endl;
    } else if (currentCpu > baselineCpu * 1.1) {
        oss << "⚠ Current CPU performance is worse than baseline" << std::endl;
    }
    
    if (currentMemory < baselineMemory * 0.9) {
        oss << "✓ Current memory usage is better than baseline" << std::endl;
    } else if (currentMemory > baselineMemory * 1.1) {
        oss << "⚠ Current memory usage is worse than baseline" << std::endl;
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeScalability(
    const std::vector<PerformanceData>& data) const {
    std::ostringstream oss;
    oss << "Scalability Analysis\n";
    oss << "===================\n\n";
    
    if (data.size() < 3) {
        oss << "Insufficient data for scalability analysis (need at least 3 data points)\n";
        return oss.str();
    }
    
    // Check for linear vs sublinear scaling
    std::vector<double> metrics;
    std::vector<double> timeScales;
    
    for (const auto& sample : data) {
        metrics.push_back(sample.wallClockTime);
        timeScales.push_back(sample.timestamp);
    }
    
    oss << "Collected " << data.size() << " data points over " << 
        (data.back().timestamp - data.front().timestamp) << " ms\n\n";
    
    // Simple scaling analysis (assuming time scales linearly)
    oss << "Scaling Analysis:\n";
    for (size_t i = 1; i < data.size(); ++i) {
        double timeRatio = data[i].wallClockTime / data[i-1].wallClockTime;
        double timeScale = data[i].timestamp / data[i-1].timestamp;
        
        oss << "  Point " << i << ": Time scale = " << timeScale << ", Performance ratio = " << timeRatio << "\n";
        
        if (timeRatio > timeScale * 1.5) {
            oss << "    → Sublinear scaling detected (performance degradation)\n";
        } else if (timeRatio < timeScale * 0.5) {
            oss << "    → Superlinear scaling detected (performance improvement)\n";
        } else {
            oss << "    → Linear scaling detected\n";
        }
    }
    
    return oss.str();
}

int64_t PerformanceProfiler::getDuration() const {
    if (!isActive) {
        if (endTime > startTime) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return duration.count();
        }
        return 0;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    return duration.count();
}

void PerformanceProfiler::monitoringFunction() {
    while (monitoringActive) {
        updatePerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(profilerConfig.samplingIntervalMs)));
    }
}

void PerformanceProfiler::updatePerformanceData() {
    if (!isActive) return;
    
    addPerformanceSample();
}

void PerformanceProfiler::addPerformanceSample() {
    PerformanceData data;
    
    data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (profilerConfig.enableWallClock) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        data.wallClockTime = elapsed.count();
    }
    
    if (profilerConfig.enableCPUTime) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        data.cpuTime = usage.ru_utime.tv_sec * 1000.0 + usage.ru_utime.tv_usec / 1000.0 +
                       usage.ru_stime.tv_sec * 1000.0 + usage.ru_stime.tv_usec / 1000.0;
    }
    
    if (profilerConfig.enableMemoryProfiling) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        data.memoryUsage = usage.ru_maxrss * 1024.0;
    }
    
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        performanceHistory.push_back(data);
        
        cleanupOldData();
        
        if (profilerConfig.verbose) {
            NLM_LOG_DEBUG("Performance sample recorded: " + std::to_string(data.wallClockTime) + "ms, " + 
                         std::to_string(data.memoryUsage / (1024 * 1024)) + "MB");
        }
    }
}

void PerformanceProfiler::cleanupOldData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Remove old samples (keep only recent ones)
    auto cutoffTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() - 
        profilerConfig.maxMemorySamples * profilerConfig.samplingIntervalMs;
    
    performanceHistory.erase(
        std::remove_if(performanceHistory.begin(), performanceHistory.end(),
            [cutoffTime](const PerformanceData& data) {
                return data.timestamp < cutoffTime;
            }),
        performanceHistory.end());
    
    // Limit the number of samples
    if (performanceHistory.size() > profilerConfig.maxMemorySamples) {
        size_t excess = performanceHistory.size() - profilerConfig.maxMemorySamples;
        performanceHistory.erase(performanceHistory.begin(), 
                               performanceHistory.begin() + excess);
    }
}

std::string PerformanceProfiler::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"sessionName\": \"" << sessionName << "\",\n";
    oss << "  \"durationMs\": " << getDuration() << ",\n";
    oss << "  \"isActive\": " << (isActive ? "true" : "false") << ",\n";
    oss << "  \"dataPoints\": " << performanceHistory.size() << ",\n";
    
    if (!performanceHistory.empty()) {
        oss << "  \"samples\": [\n";
        for (size_t i = 0; i < performanceHistory.size(); ++i) {
            const auto& data = performanceHistory[i];
            oss << "    {\n";
            oss << "      \"timestamp\": " << data.timestamp << ",\n";
            oss << "      \"wallClockTime\": " << data.wallClockTime << ",\n";
            oss << "      \"cpuTime\": " << data.cpuTime << ",\n";
            oss << "      \"memoryUsage\": " << data.memoryUsage << ",\n";
            oss << "      \"throughput\": " << (data.wallClockTime > 0 ? 
                (data.customMetrics.empty() ? 0.0 : 1.0) : 0.0) << ",\n";
            oss << "      \"metadata\": {\"session\": \"" << sessionName << "\"}\n";
            if (i < performanceHistory.size() - 1) {
                oss << "    },\n";
            } else {
                oss << "    }\n";
            }
        }
        oss << "  ],\n";
    }
    
    oss << "  \"customMetrics\": {\n";
    bool first = true;
    for (const auto& pair : customMetrics) {
        if (!first) oss << ",\n";
        oss << "    \"" << pair.first << "\": " << pair.second;
        first = false;
    }
    oss << "\n  }\n";
    oss << "}\n";
    
    return oss.str();
}

std::string PerformanceProfiler::generateCsvReport() const {
    std::ostringstream oss;
    oss << "timestamp,wallClockTime,CPUTime,memoryUsage,throughput\n";
    
    for (const auto& data : performanceHistory) {
        oss << data.timestamp << ","
            << data.wallClockTime << ","
            << data.cpuTime << ","
            << data.memoryUsage << ","
            << (data.wallClockTime > 0 ? 1.0 : 0.0) << "\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::generateTextReport() const {
    std::ostringstream oss;
    oss << "Performance Profiling Report\n";
    oss << "===============================\n\n";
    oss << "Session: " << sessionName << "\n";
    oss << "Duration: " << getDuration() << "ms\n";
    oss << "Status: " << (isActive ? "Active" : "Stopped") << "\n";
    oss << "Data Points: " << performanceHistory.size() << "\n\n";
    
    if (!performanceHistory.empty()) {
        oss << "Recent Samples:\n";
        size_t recentCount = std::min(size_t(10), performanceHistory.size());
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& data = performanceHistory[performanceHistory.size() - 1 - i];
            oss << "  " << data.timestamp << ": " << data.wallClockTime << "ms, "
                << data.cpuTime << "ms CPU, "
                << data.memoryUsage / (1024 * 1024) << "MB memory\n";
        }
    }
    
    oss << "\nBottlenecks:\n";
    auto bottlenecks = detectBottlenecks();
    if (bottlenecks.empty()) {
        oss << "  None detected\n";
    } else {
        for (const auto& bottleneck : bottlenecks) {
            oss << "  - " << bottleneck << "\n";
        }
    }
    
    oss << "\nRecommendations:\n";
    auto recommendations = generateRecommendations();
    if (recommendations.empty()) {
        oss << "  No recommendations\n";
    } else {
        for (const auto& recommendation : recommendations) {
            oss << "  - " << recommendation << "\n";
        }
    }
    
    return oss.str();
}

} // namespace nlm