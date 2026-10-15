// Test Performance Profiler Implementation
// Performance monitoring during test execution

#include "TestPerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <thread>

namespace nlm {
namespace tests {

struct TestPerformanceProfiler::Impl {
    // Active measurements
    std::map<std::string, PerformanceMeasurement> activeMeasurements;
    std::mutex measurementsMutex;
    
    // Measurement history
    std::vector<PerformanceMeasurement> measurementHistory;
    std::mutex historyMutex;
    
    // Metrics registry
    std::map<std::string, PerformanceMetricType> metricRegistry;
    std::mutex registryMutex;
    
    // Performance thresholds
    std::map<std::string, double> performanceThresholds;
    
    // Reporting configuration
    bool realTimeReportingEnabled;
    std::chrono::milliseconds reportingInterval;
    
    // Global state
    bool paused;
    std::chrono::system_clock::time_point sessionStartTime;
    
    Impl() {
        realTimeReportingEnabled = false;
        reportingInterval = std::chrono::milliseconds(1000);
        paused = false;
        sessionStartTime = std::chrono::high_resolution_clock::now();
    }
};

TestPerformanceProfiler::TestPerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}
TestPerformanceProfiler::~TestPerformanceProfiler() {
    clear();
}

bool TestPerformanceProfiler::registerMetric(const std::string& name, PerformanceMetricType type) {
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    // Check if metric already exists
    if (pImpl->metricRegistry.find(name) != pImpl->metricRegistry.end()) {
        return false;
    }
    
    pImpl->metricRegistry[name] = type;
    return true;
}

bool TestPerformanceProfiler::unregisterMetric(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->metricRegistry.find(name);
    if (it != pImpl->metricRegistry.end()) {
        pImpl->metricRegistry.erase(it);
        
        // Also remove from active measurements
        std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
        pImpl->activeMeasurements.erase(name);
        
        return true;
    }
    
    return false;
}

bool TestPerformanceProfiler::updateMetricParameters(const std::string& name, 
                                                      const std::map<std::string, double>& params) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->metricRegistry.find(name);
    if (it != pImpl->metricRegistry.end()) {
        // In real implementation, would update parameter storage
        return true;
    }
    
    return false;
}

bool TestPerformanceProfiler::startMeasurement(const std::string& metricName) {
    if (pImpl->paused) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = pImpl->metricRegistry.find(metricName);
    if (it == pImpl->metricRegistry.end()) {
        return false;
    }
    
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    // Check if measurement already active
    if (pImpl->activeMeasurements.find(metricName) != pImpl->activeMeasurements.end()) {
        if (pImpl->activeMeasurements[metricName].isActive) {
            return false; // Already active
        } else {
            // Resume existing measurement
            pImpl->activeMeasurements[metricName].isActive = true;
            pImpl->activeMeasurements[metricName].startTime = std::chrono::high_resolution_clock::now();
            return true;
        }
    }
    
    // Create new measurement
    pImpl->activeMeasurements[metricName] = PerformanceMeasurement(metricName, it->second);
    pImpl->activeMeasurements[metricName].isActive = true;
    pImpl->activeMeasurements[metricName].startTime = std::chrono::high_resolution_clock::now();
    
    return true;
}

bool TestPerformanceProfiler::stopMeasurement(const std::string& metricName) {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it == pImpl->activeMeasurements.end() || !it->second.isActive) {
        return false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second.startTime);
    
    it->second.endTime = endTime;
    it->second.duration = duration;
    it->second.isActive = false;
    
    // Add to history
    pImpl->measurementHistory.push_back(it->second);
    
    return true;
}

bool TestPerformanceProfiler::pauseMeasurement(const std::string& metricName) {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it == pImpl->activeMeasurements.end() || !it->second.isActive) {
        return false;
    }
    
    it->second.isActive = false;
    return true;
}

bool TestPerformanceProfiler::resumeMeasurement(const std::string& metricName) {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it == pImpl->activeMeasurements.end() || it->second.isActive) {
        return false;
    }
    
    // Update start time to compensate for pause
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.startTime);
    
    it->second.startTime = now;
    it->second.isActive = true;
    
    return true;
}

bool TestPerformanceProfiler::cancelMeasurement(const std::string& metricName) {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it == pImpl->activeMeasurements.end()) {
        return false;
    }
    
    // Cancel the measurement
    pImpl->activeMeasurements.erase(it);
    return true;
}

double TestPerformanceProfiler::getMeasurement(const std::string& metricName) const {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it != pImpl->activeMeasurements.end() && it->second.isActive) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second.startTime);
        return duration.count();
    }
    
    return 0.0;
}

std::chrono::milliseconds TestPerformanceProfiler::getTimeMeasurement(const std::string& metricName) const {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it != pImpl->activeMeasurements.end() && it->second.isActive) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second.startTime);
        return duration;
    }
    
    return std::chrono::milliseconds(0);
}

PerformanceMeasurement TestPerformanceProfiler::getCurrentMeasurement(const std::string& metricName) const {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    if (it != pImpl->activeMeasurements.end()) {
        return it->second;
    }
    
    return PerformanceMeasurement(metricName, PerformanceMetricType::TIME);
}

std::vector<PerformanceMeasurement> TestPerformanceProfiler::getAllMeasurements() const {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    
    std::vector<PerformanceMeasurement> allMeasurements;
    for (const auto& pair : pImpl->activeMeasurements) {
        allMeasurements.push_back(pair.second);
    }
    
    return allMeasurements;
}

PerformanceStatistics TestPerformanceProfiler::getStatistics(const std::string& metricName) const {
    PerformanceStatistics stats(metricName, PerformanceMetricType::TIME);
    
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex);
    
    std::vector<PerformanceMeasurement> relevantMeasurements;
    for (const auto& measurement : pImpl->measurementHistory) {
        if (measurement.metricName == metricName) {
            relevantMeasurements.push_back(measurement);
        }
    }
    
    if (relevantMeasurements.empty()) {
        return stats;
    }
    
    stats.measurementCount = relevantMeasurements.size();
    
    // Extract duration values
    std::vector<double> values;
    double minVal = std::numeric_limits<double>::max();
    double maxVal = 0.0;
    
    for (const auto& measurement : relevantMeasurements) {
        double value = measurement.duration.count();
        values.push_back(value);
        
        if (value < minVal) minVal = value;
        if (value > maxVal) maxVal = value;
    }
    
    stats.values = values;
    stats.minValue = minVal;
    stats.maxValue = maxVal;
    
    if (!values.empty()) {
        stats.averageValue = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        stats.medianValue = calculateMedian(values);
    }
    
    return stats;
}

std::map<std::string, PerformanceStatistics> TestPerformanceProfiler::getAllStatistics() const {
    std::map<std::string, PerformanceStatistics> allStats;
    
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex);
    
    // Collect all unique metric names
    std::set<std::string> metricNames;
    for (const auto& measurement : pImpl->measurementHistory) {
        metricNames.insert(measurement.metricName);
    }
    
    for (const auto& metricName : metricNames) {
        allStats[metricName] = getStatistics(metricName);
    }
    
    return allStats;
}

void TestPerformanceProfiler::profilePluginLoading(const std::string& pluginName) {
    std::string metricName = "plugin_loading_" + pluginName;
    
    if (!registerMetric(metricName, PerformanceMetricType::TIME)) {
        return; // Already registered
    }
    
    startMeasurement(metricName);
    
    // In real implementation, this would measure plugin loading time
    // For now, just simulate with a sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    stopMeasurement(metricName);
}

void TestPerformanceProfiler::profileSimulation(const std::string& simulationId) {
    std::string metricName = "simulation_" + simulationId;
    
    if (!registerMetric(metricName, PerformanceMetricType::TIME)) {
        return;
    }
    
    startMeasurement(metricName);
    
    // In real implementation, this would measure simulation performance
    // For now, just simulate with variable sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100 + 50));
    
    stopMeasurement(metricName);
}

void TestPerformanceProfiler::profileMemoryUsage(const std::string& memoryType) {
    std::string metricName = "memory_usage_" + memoryType;
    
    if (!registerMetric(metricName, PerformanceMetricType::MEMORY)) {
        return;
    }
    
    startMeasurement(metricName);
    
    // In real implementation, this would measure memory usage
    // For now, simulate with sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    stopMeasurement(metricName);
}

void TestPerformanceProfiler::profileNetworkScaling(const std::string& networkId) {
    std::string metricName = "network_scaling_" + networkId;
    
    if (!registerMetric(metricName, PerformanceMetricType::TIME)) {
        return;
    }
    
    startMeasurement(metricName);
    
    // In real implementation, this would measure network scaling performance
    // For now, simulate with variable sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200 + 100));
    
    stopMeasurement(metricName);
}

void TestPerformanceProfiler::profileExperimentExecution(const std::string& experimentName) {
    std::string metricName = "experiment_" + experimentName;
    
    if (!registerMetric(metricName, PerformanceMetricType::TIME)) {
        return;
    }
    
    startMeasurement(metricName);
    
    // In real implementation, this would measure experiment execution time
    // For now, simulate with sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 200));
    
    stopMeasurement(metricName);
}

std::string TestPerformanceProfiler::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "NLM Performance Profiling Report\n";
    ss << "==============================\n\n";
    
    auto allStats = getAllStatistics();
    
    if (allStats.empty()) {
        ss << "No performance measurements recorded.\n";
        return ss.str();
    }
    
    ss << "Performance Summary:\n";
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        ss << "  " << stats.metricName << ":\n";
        ss << "    Measurements: " << stats.measurementCount << "\n";
        ss << "    Average: " << formatMetricValue(stats.averageValue, stats.metricType) << "\n";
        ss << "    Min: " << formatMetricValue(stats.minValue, stats.metricType) << "\n";
        ss << "    Max: " << formatMetricValue(stats.maxValue, stats.metricType) << "\n";
        ss << "    Median: " << formatMetricValue(stats.medianValue, stats.metricType) << "\n";
    }
    
    return ss.str();
}

std::string TestPerformanceProfiler::generateDetailedPerformanceReport() const {
    std::stringstream ss;
    ss << "NLM Detailed Performance Report\n";
    ss << "===============================\n\n";
    
    auto allStats = getAllStatistics();
    
    ss << "All Performance Measurements:\n";
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        ss << "\nMetric: " << stats.metricName << " (" << getMetricTypeString(stats.metricType) << ")\n";
        ss << "  Count: " << stats.measurementCount << "\n";
        
        if (!stats.values.empty()) {
            ss << "  Values: ";
            for (size_t i = 0; i < std::min<size_t>(10, stats.values.size()); ++i) {
                ss << formatMetricValue(stats.values[i], stats.metricType);
                if (i < stats.values.size() - 1 && i < 9) ss << ", ";
            }
            if (stats.values.size() > 10) {
                ss << ", ... (total: " << stats.values.size() << ")";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string TestPerformanceProfiler::generateBenchmarkReport() const {
    std::stringstream ss;
    ss << "NLM Benchmark Performance Report\n";
    ss << "==============================\n\n";
    
    auto allStats = getAllStatistics();
    
    // Group by metric type
    std::map<PerformanceMetricType, std::vector<PerformanceStatistics>> statsByType;
    for (const auto& pair : allStats) {
        statsByType[pair.second.metricType].push_back(pair.second);
    }
    
    for (const auto& pair : statsByType) {
        const PerformanceMetricType& type = pair.first;
        const auto& statsList = pair.second;
        
        ss << getMetricTypeString(type) << " Metrics:\n";
        for (const auto& stats : statsList) {
            ss << "  " << stats.metricName << ": " << formatMetricValue(stats.averageValue, type) << " (" << stats.measurementCount << " measurements)\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestPerformanceProfiler::generateOptimizationReport() const {
    std::stringstream ss;
    ss << "NLM Optimization Opportunities Report\n";
    ss << "===================================\n\n";
    
    auto bottlenecks = analyzePerformanceBottlenecks();
    auto optimizations = identifyOptimizationOpportunities();
    
    ss << "Performance Bottlenecks:\n";
    ss << bottlenecks << "\n";
    
    ss << "Optimization Recommendations:\n";
    for (const auto& optimization : optimizations) {
        ss << "  - " << optimization << "\n";
    }
    
    ss << "\nOverall Performance Score: " << calculatePerformanceScore() << "/100\n";
    
    return ss.str();
}

bool TestPerformanceProfiler::exportPerformanceData(const std::string& filename, const std::string& format) {
    try {
        if (format == "csv") {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "MetricName,MetricType,AverageValue,MinValue,MaxValue,MeasurementCount\n";
            
            auto allStats = getAllStatistics();
            for (const auto& pair : allStats) {
                const auto& stats = pair.second;
                file << escapeCSV(stats.metricName) << "," << escapeCSV(getMetricTypeString(stats.metricType)) << "," 
                     << stats.averageValue << "," << stats.minValue << "," << stats.maxValue << "," 
                     << stats.measurementCount << "\n";
            }
            
            file.close();
            
        } else if (format == "json") {
            // JSON export implementation
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << generateBenchmarkReport();
            file.close();
            
        } else {
            return false;
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestPerformanceProfiler::exportStatistics(const std::string& filename, const std::string& format) {
    return exportPerformanceData(filename, format);
}

bool TestPerformanceProfiler::importPerformanceData(const std::string& filename) {
    // Implementation for importing performance data
    return false;
}

std::string TestPerformanceProfiler::analyzePerformanceBottlenecks() const {
    std::stringstream ss;
    
    auto allStats = getAllStatistics();
    
    if (allStats.empty()) {
        ss << "No performance data available for bottleneck analysis.\n";
        return ss.str();
    }
    
    // Find metrics with high variance
    std::vector<std::pair<std::string, double>> bottleneckCandidates;
    
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        
        if (stats.measurementCount < 3) {
            continue; // Not enough data
        }
        
        // Calculate coefficient of variation (std dev / mean)
        double cv = 0.0;
        if (stats.averageValue > 0) {
            cv = calculateStdDev(stats.values, stats.averageValue) / stats.averageValue;
        }
        
        if (cv > 1.0) { // High coefficient of variation indicates bottleneck
            bottleneckCandidates.push_back({stats.metricName, cv});
        }
    }
    
    // Sort by coefficient of variation (descending)
    std::sort(bottleneckCandidates.begin(), bottleneckCandidates.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (bottleneckCandidates.empty()) {
        ss << "No significant performance bottlenecks detected.\n";
        return ss.str();
    }
    
    ss << "Top Performance Bottlenecks:\n";
    for (size_t i = 0; i < std::min<size_t>(5, bottleneckCandidates.size()); ++i) {
        ss << "  " << (i + 1) << ". " << bottleneckCandidates[i].first 
           << " (coefficient of variation: " << bottleneckCandidates[i].second << ")\n";
    }
    
    return ss.str();
}

std::string TestPerformanceProfiler::identifyOptimizationOpportunities() const {
    std::stringstream ss;
    ss << "Potential Optimization Opportunities:\n";
    
    auto allStats = getAllStatistics();
    
    // Identify opportunities based on analysis
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        
        // Check if metric values are consistently high
        if (stats.averageValue > 1000.0) { // 1 second threshold
            ss << "  - " << stats.metricName << ": Average time is " << formatMetricValue(stats.averageValue, stats.metricType) 
               << ", consider optimization\n";
        }
        
        // Check for high variance
        double cv = 0.0;
        if (stats.averageValue > 0) {
            cv = calculateStdDev(stats.values, stats.averageValue) / stats.averageValue;
        }
        
        if (cv > 0.5) {
            ss << "  - " << stats.metricName << ": High variance (" << cv << "), consider stabilization\n";
        }
    }
    
    if (ss.str() == "Potential Optimization Opportunities:\n") {
        ss << "  No obvious optimization opportunities detected.\n";
    }
    
    return ss.str();
}

double TestPerformanceProfiler::calculatePerformanceScore() const {
    auto allStats = getAllStatistics();
    
    if (allStats.empty()) {
        return 0.0;
    }
    
    double totalScore = 0.0;
    size_t count = 0;
    
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        
        // Calculate individual metric score (inverted - faster is better)
        double metricScore = 100.0 / (1.0 + stats.averageValue / 1000.0); // Normalize to 1000ms
        
        totalScore += metricScore;
        count++;
    }
    
    return count > 0 ? totalScore / count : 0.0;
}

std::vector<std::string> TestPerformanceProfiler::getRecommendedOptimizations() const {
    std::vector<std::string> optimizations;
    
    auto allStats = getAllStatistics();
    
    for (const auto& pair : allStats) {
        const auto& stats = pair.second;
        
        // Generate optimization recommendations
        if (stats.averageValue > 2000.0) {
            optimizations.push_back("Optimize " + stats.metricName + " - average time is " + 
                                   formatMetricValue(stats.averageValue, stats.metricType));
        }
    }
    
    return optimizations;
}

void TestPerformanceProfiler::enableRealTimeReporting(bool enable) {
    pImpl->realTimeReportingEnabled = enable;
}

void TestPerformanceProfiler::setReportingInterval(std::chrono::milliseconds interval) {
    pImpl->reportingInterval = interval;
}

void TestPerformanceProfiler::setThresholds(const std::map<std::string, double>& thresholds) {
    pImpl->performanceThresholds = thresholds;
}

void TestPerformanceProfiler::reset() {
    std::lock_guard<std::mutex> measurementsLock(pImpl->measurementsMutex);
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex);
    
    pImpl->activeMeasurements.clear();
    pImpl->measurementHistory.clear();
    pImpl->performanceThresholds.clear();
}

void TestPerformanceProfiler::clear() {
    reset();
}

void TestPerformanceProfiler::pauseAllMeasurements() {
    std::lock_guard<std::mutex> lock(pImpl->measurementsMutex);
    pImpl->paused = true;
}

void TestPerformanceProfiler::resumeAllMeasurements() {
    std::lock_guard<std::mutex> lock(pImpl->measurementsMutex);
    pImpl->paused = false;
}

bool TestPerformanceProfiler::isMeasurementActive(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->measurementsMutex);
    
    auto it = pImpl->activeMeasurements.find(metricName);
    return it != pImpl->activeMeasurements.end() && it->second.isActive;
}

TestPerformanceProfiler& TestPerformanceProfiler::getInstance() {
    static TestPerformanceProfiler instance;
    return instance;
}

void TestPerformanceProfiler::initialize() {
    getInstance();
}

void TestPerformanceProfiler::shutdown() {
    getInstance().clear();
}

double TestPerformanceProfiler::calculateMedian(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
    } else {
        return sorted[n / 2];
    }
}

double TestPerformanceProfiler::calculateStdDev(const std::vector<double>& values, double mean) const {
    if (values.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / values.size());
}

std::string TestPerformanceProfiler::formatMetricValue(double value, PerformanceMetricType type) const {
    switch (type) {
        case PerformanceMetricType::TIME:
            return std::to_string(value) + " ms";
        case PerformanceMetricType::MEMORY:
            return std::to_string(value) + " MB";
        case PerformanceMetricType::CPU:
            return std::to_string(value) + "%";
        case PerformanceMetricType::BANDWIDTH:
            return std::to_string(value) + " Mbps";
        case PerformanceMetricType::THROUGHPUT:
            return std::to_string(value) + " ops/s";
        case PerformanceMetricType::LATENCY:
            return std::to_string(value) + " ms";
        case PerformanceMetricType::CUSTOM:
            return std::to_string(value);
        default:
            return std::to_string(value);
    }
}

std::string TestPerformanceProfiler::getMetricTypeString(PerformanceMetricType type) const {
    switch (type) {
        case PerformanceMetricType::TIME: return "Time";
        case PerformanceMetricType::MEMORY: return "Memory";
        case PerformanceMetricType::CPU: return "CPU";
        case PerformanceMetricType::BANDWIDTH: return "Bandwidth";
        case PerformanceMetricType::THROUGHPUT: return "Throughput";
        case PerformanceMetricType::LATENCY: return "Latency";
        case PerformanceMetricType::CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

std::string TestPerformanceProfiler::escapeCSV(const std::string& text) const {
    std::string escaped = text;
    if (escaped.find(',') != std::string::npos || escaped.find('"') != std::string::npos || escaped.find('\n') != std::string::npos) {
        std::replace(escaped.begin(), escaped.end(), '"', '"');
        escaped = '"' + escaped + '"';
    }
    return escaped;
}

} // namespace tests
} // namespace nlm
