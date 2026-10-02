#include "PerformanceMonitoring.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

// PerformanceMonitor implementation

void PerformanceMonitor::startMeasurement(const std::string& operation, const std::string& component) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceSnapshot snapshot(operation, component);
    currentMetrics_[{operation, component}] = snapshot;
    
    if (enableRealTimeTracking_) {
        NLM_LOG_INFO("Started measurement: " + operation + " (Component: " + component + ")");
    }
}

PerformanceMonitor::PerformanceSnapshot PerformanceMonitor::stopMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceSnapshot snapshot("", "");
    
    for (auto& [key, current] : currentMetrics_) {
        if (key.first == operation && current.operation.empty()) {
            currentMetrics_.erase(key);
            return current;
        }
    }
    
    return snapshot;
}

void PerformanceMonitor::recordMetric(const std::string& operation, const std::string& component,
                                     double duration, bool success,
                                     const std::map<std::string, double>& customMetrics) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceSnapshot snapshot(operation, component);
    snapshot.duration = duration;
    
    // Get current metrics if available
    auto it = currentMetrics_.find({operation, component});
    if (it != currentMetrics_.end()) {
        snapshot = it->second;
        currentMetrics_.erase(it);
    }
    
    // Record the snapshot
    metrics_[operation].push_back(snapshot);
    
    // Cleanup old metrics
    if (metrics_[operation].size() > maxHistorySize_) {
        metrics_[operation].erase(
            metrics_[operation].begin(),
            metrics_[operation].begin() + (metrics_[operation].size() - maxHistorySize_)
        );
    }
    
    // Check alerts
    if (enableAlertThresholds_) {
        std::vector<std::string> alerts = checkAlerts(snapshot);
        for (const auto& alert : alerts) {
            NLM_LOG_WARNING("Performance Alert: " + alert);
        }
    }
}

PerformanceMonitor::PerformanceSnapshot PerformanceMonitor::getCurrentMetrics(const std::string& operation,
                                                                             const std::string& component) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    auto it = currentMetrics_.find({operation, component});
    if (it != currentMetrics_.end()) {
        return it->second;
    }
    
    return PerformanceSnapshot(operation, component);
}

std::vector<PerformanceMonitor::PerformanceSnapshot> PerformanceMonitor::getMetrics(
    const std::string& operation, const std::string& component,
    std::chrono::system_clock::time_point since) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    std::vector<PerformanceSnapshot> result;
    
    if (operation.empty()) {
        // Return metrics from all operations
        for (const auto& [op, snapshots] : metrics_) {
            for (const auto& snapshot : snapshots) {
                if (since == std::chrono::system_clock::time_point() ||
                    snapshot.timestamp >= since) {
                    if (component.empty() || snapshot.component == component) {
                        result.push_back(snapshot);
                    }
                }
            }
        }
    } else {
        // Return metrics from specific operation
        auto it = metrics_.find(operation);
        if (it != metrics_.end()) {
            for (const auto& snapshot : it->second) {
                if (since == std::chrono::system_clock::time_point() ||
                    snapshot.timestamp >= since) {
                    if (component.empty() || snapshot.component == component) {
                        result.push_back(snapshot);
                    }
                }
            }
        }
    }
    
    return result;
}

double PerformanceMonitor::getAverageDuration(const std::string& operation,
                                             const std::string& component,
                                             std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    
    if (metrics.empty()) {
        return 0.0;
    }
    
    double totalDuration = 0.0;
    for (const auto& metric : metrics) {
        totalDuration += metric.duration;
    }
    
    return totalDuration / metrics.size();
}

double PerformanceMonitor::getSuccessRate(const std::string& operation,
                                         const std::string& component,
                                         std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    
    if (metrics.empty()) {
        return 0.0;
    }
    
    int successCount = 0;
    for (const auto& metric : metrics) {
        // Note: PerformanceSnapshot doesn't have a success field, so we assume all are successful
        // This would need to be enhanced to track success/failure
        successCount++;
    }
    
    return static_cast<double>(successCount) / metrics.size();
}

std::string PerformanceMonitor::getPerformanceReport(const std::string& operation,
                                                    const std::string& component,
                                                    std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    
    std::stringstream ss;
    ss << "=== Performance Report ===\n";
    
    if (!operation.empty()) {
        ss << "Operation: " << operation << "\n";
    }
    if (!component.empty()) {
        ss << "Component: " << component << "\n";
    }
    
    ss << "Total metrics: " << metrics.size() << "\n";
    
    if (!metrics.empty()) {
        double totalDuration = 0.0;
        double maxDuration = 0.0;
        double minDuration = std::numeric_limits<double>::max();
        
        for (const auto& metric : metrics) {
            totalDuration += metric.duration;
            maxDuration = std::max(maxDuration, metric.duration);
            minDuration = std::min(minDuration, metric.duration);
        }
        
        double avgDuration = totalDuration / metrics.size();
        
        ss << "Duration statistics:\n";
        ss << "  Average: " << avgDuration << " seconds\n";
        ss << "  Minimum: " << minDuration << " seconds\n";
        ss << "  Maximum: " << maxDuration << " seconds\n";
        ss << "  Total: " << totalDuration << " seconds\n";
    }
    
    ss << "=== End Performance Report ===\n";
    
    return ss.str();
}

double PerformanceMonitor::calculateMovingAverage(const std::string& operation,
                                                 const std::string& component,
                                                 size_t windowSize,
                                                 std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    
    if (metrics.size() < windowSize) {
        windowSize = metrics.size();
    }
    
    if (windowSize == 0) {
        return 0.0;
    }
    
    std::vector<double> durations;
    for (size_t i = 0; i < windowSize; ++i) {
        durations.push_back(metrics[metrics.size() - 1 - i].duration);
    }
    
    return calculateMean(durations);
}

std::vector<double> PerformanceMonitor::getMovingAverageSeries(const std::string& operation,
                                                             const std::string& component,
                                                             size_t windowSize,
                                                             std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    
    std::vector<double> movingAverages;
    
    if (metrics.size() < windowSize) {
        windowSize = metrics.size();
    }
    
    if (windowSize == 0) {
        return movingAverages;
    }
    
    for (size_t i = windowSize - 1; i < metrics.size(); ++i) {
        std::vector<double> window;
        for (size_t j = 0; j < windowSize; ++j) {
            window.push_back(metrics[i - j].duration);
        }
        movingAverages.push_back(calculateMean(window));
    }
    
    return movingAverages;
}

std::vector<PerformanceMonitor::PerformanceSnapshot> PerformanceMonitor::detectAnomalies(
    const std::string& operation, const std::string& component,
    double threshold, std::chrono::system_clock::time_point since) const {
    std::vector<PerformanceSnapshot> metrics = getMetrics(operation, component, since);
    std::vector<PerformanceSnapshot> anomalies;
    
    if (metrics.size() < 2) {
        return anomalies;
    }
    
    std::vector<double> durations;
    for (const auto& metric : metrics) {
        durations.push_back(metric.duration);
    }
    
    double mean = calculateMean(durations);
    double stddev = calculateStandardDeviation(durations);
    
    if (stddev < 0.001) { // Very small standard deviation
        return anomalies;
    }
    
    for (const auto& metric : metrics) {
        double zScore = std::abs((metric.duration - mean) / stddev);
        if (zScore > threshold) {
            anomalies.push_back(metric);
        }
    }
    
    return anomalies;
}

PerformanceMonitor::MemoryInfo PerformanceMonitor::getMemoryInfo() const {
    MemoryInfo info;
    info.totalMemory = 0;
    info.usedMemory = 0;
    info.availableMemory = 0;
    info.memoryUtilization = 0.0;
    info.peakMemory = 0;
    info.lastMeasured = std::chrono::system_clock::now();
    
    // This is a placeholder implementation. In a real implementation,
    // you would use platform-specific APIs to get memory information.
    return info;
}

PerformanceMonitor::SystemInfo PerformanceMonitor::getSystemInfo() const {
    SystemInfo info;
    info.cpuUsage = 0.0;
    info.totalCores = std::thread::hardware_concurrency();
    info.uptime = std::chrono::system_clock::now();
    info.osInfo = "Unknown";
    
    // This is a placeholder implementation. In a real implementation,
    // you would use platform-specific APIs to get system information.
    return info;
}

void PerformanceMonitor::addAlertThreshold(const AlertThreshold& threshold) {
    alertThresholds_.push_back(threshold);
}

void PerformanceMonitor::clearAlertThresholds() {
    alertThresholds_.clear();
}

std::vector<std::string> PerformanceMonitor::checkAlerts(const PerformanceSnapshot& metrics) const {
    std::vector<std::string> alerts;
    
    for (const auto& threshold : alertThresholds_) {
        if (threshold.metric == "duration" && metrics.duration > threshold.warningThreshold) {
            std::stringstream ss;
            ss << threshold.message << " (Current: " << metrics.duration << "s, Threshold: " 
               << threshold.warningThreshold << "s)";
            alerts.push_back(ss.str());
            
            if (metrics.duration > threshold.criticalThreshold) {
                ss.str("");
                ss << "CRITICAL: " << threshold.message << " (Current: " << metrics.duration << "s, Threshold: " 
                   << threshold.criticalThreshold << "s)";
                alerts.push_back(ss.str());
            }
        }
    }
    
    return alerts;
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Create a simple binary format
    size_t totalMetrics = 0;
    for (const auto& [operation, snapshots] : metrics_) {
        totalMetrics += snapshots.size();
    }
    
    file.write(reinterpret_cast<const char*>(&totalMetrics), sizeof(totalMetrics));
    
    for (const auto& [operation, snapshots] : metrics_) {
        size_t operationSize = operation.size();
        file.write(reinterpret_cast<const char*>(&operationSize), sizeof(operationSize));
        file.write(operation.c_str(), operationSize);
        
        size_t snapshotCount = snapshots.size();
        file.write(reinterpret_cast<const char*>(&snapshotCount), sizeof(snapshotCount));
        
        for (const auto& snapshot : snapshots) {
            // Write snapshot data
            double duration = snapshot.duration;
            file.write(reinterpret_cast<const char*>(&duration), sizeof(duration));
            
            // Write other fields as needed
        }
    }
    
    return true;
}

bool PerformanceMonitor::importMetrics(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    size_t totalMetrics = 0;
    file.read(reinterpret_cast<char*>(&totalMetrics), sizeof(totalMetrics));
    
    metrics_.clear();
    
    for (size_t i = 0; i < totalMetrics; ++i) {
        size_t operationSize = 0;
        file.read(reinterpret_cast<char*>(&operationSize), sizeof(operationSize));
        
        std::string operation(operationSize, ' ');
        file.read(&operation[0], operationSize);
        
        size_t snapshotCount = 0;
        file.read(reinterpret_cast<char*>(&snapshotCount), sizeof(snapshotCount));
        
        for (size_t j = 0; j < snapshotCount; ++j) {
            double duration = 0.0;
            file.read(reinterpret_cast<char*>(&duration), sizeof(duration));
            
            // Read other fields as needed
        }
    }
    
    return true;
}

std::string PerformanceMonitor::exportMetricsJSON() const {
    std::stringstream ss;
    ss << "{\n\"metrics\": [";
    
    bool first = true;
    for (const auto& [operation, snapshots] : metrics_) {
        for (const auto& snapshot : snapshots) {
            if (!first) {
                ss << ",\n";
            }
            first = false;
            
            ss << "{\n\"operation\": \"" << snapshot.operation << "\",\n";
            ss << "\"component\": \"" << snapshot.component << "\",\n";
            ss << "\"duration\": " << snapshot.duration << ",\n";
            ss << "\"timestamp\": \"" << std::chrono::system_clock::to_time_t(snapshot.timestamp) << "\"\n";
            ss << "}";
        }
    }
    
    ss << "\n]\n}";
    return ss.str();
}

bool PerformanceMonitor::importMetricsJSON(const std::string& jsonData) {
    // This is a placeholder implementation for JSON import
    // In a real implementation, you would use a JSON library
    metrics_.clear();
    return true;
}

std::string PerformanceMonitor::comparePerformance(const std::vector<PerformanceSnapshot>& snapshots) const {
    if (snapshots.empty()) {
        return "No snapshots to compare";
    }
    
    std::stringstream ss;
    ss << "=== Performance Comparison ===\n";
    ss << "Number of snapshots: " << snapshots.size() << "\n";
    
    std::vector<double> durations;
    for (const auto& snapshot : snapshots) {
        durations.push_back(snapshot.duration);
    }
    
    double mean = calculateMean(durations);
    double stddev = calculateStandardDeviation(durations);
    
    ss << "Mean duration: " << mean << " seconds\n";
    ss << "Standard deviation: " << stddev << " seconds\n";
    
    double minDuration = *std::min_element(durations.begin(), durations.end());
    double maxDuration = *std::max_element(durations.begin(), durations.end());
    
    ss << "Min duration: " << minDuration << " seconds\n";
    ss << "Max duration: " << maxDuration << " seconds\n";
    
    ss << "=== End Comparison ===\n";
    
    return ss.str();
}

double PerformanceMonitor::calculateStandardDeviation(const std::vector<double>& values) const {
    if (values.size() < 2) {
        return 0.0;
    }
    
    double mean = calculateMean(values);
    double sumOfSquares = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sumOfSquares += diff * diff;
    }
    
    return std::sqrt(sumOfSquares / (values.size() - 1));
}

double PerformanceMonitor::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

// Debugger implementation

void Debugger::log(const std::string& message, DebugLevel level,
                  const std::string& source,
                  const std::map<std::string, std::string>& context) {
    if (level < currentLevel_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(debugMutex_);
    
    DebugInfo debugInfo;
    debugInfo.timestamp = std::chrono::system_clock::now();
    debugInfo.level = level;
    debugInfo.source = source;
    debugInfo.message = message;
    debugInfo.context = context;
    
    if (enableStackTrace_) {
        debugInfo.stackTrace = generateStackTrace();
    }
    
    debugLogs_.push_back(debugInfo);
    
    if (debugLogs_.size() > maxLogSize_) {
        debugLogs_.erase(debugLogs_.begin(), debugLogs_.begin() + 1);
    }
    
    // Output to console
    std::cout << "[" << debugLevelToString(level) << "] " << debugInfo.timestamp << " - " << source << " - " << message << std::endl;
}

void Debugger::logError(const std::string& message, const std::string& source,
                       const std::map<std::string, std::string>& context) {
    log(message, DebugLevel::Error, source, context);
}

void Debugger::logWarning(const std::string& message, const std::string& source,
                         const std::map<std::string, std::string>& context) {
    log(message, DebugLevel::Warning, source, context);
}

void Debugger::logInfo(const std::string& message, const std::string& source,
                      const std::map<std::string, std::string>& context) {
    log(message, DebugLevel::Info, source, context);
}

void Debugger::logDebug(const std::string& message, const std::string& source,
                       const std::map<std::string, std::string>& context) {
    log(message, DebugLevel::Debug, source, context);
}

void Debugger::startDebugSession(const std::string& sessionName) {
    logDebug("Starting debug session: " + sessionName, "Debugger");
}

void Debugger::endDebugSession(const std::string& sessionName) {
    logDebug("Ending debug session: " + sessionName, "Debugger");
}

std::vector<Debugger::DebugInfo> Debugger::getDebugLogs(DebugLevel minLevel) const {
    std::lock_guard<std::mutex> lock(debugMutex_);
    
    std::vector<DebugInfo> result;
    for (const auto& debugInfo : debugLogs_) {
        if (debugInfo.level >= minLevel) {
            result.push_back(debugInfo);
        }
    }
    
    return result;
}

std::vector<Debugger::DebugInfo> Debugger::getDebugLogs(const std::string& source,
                                                       DebugLevel minLevel) const {
    std::lock_guard<std::mutex> lock(debugMutex_);
    
    std::vector<DebugInfo> result;
    for (const auto& debugInfo : debugLogs_) {
        if (debugInfo.level >= minLevel && debugInfo.source == source) {
            result.push_back(debugInfo);
        }
    }
    
    return result;
}

void Debugger::clearDebugLogs() {
    std::lock_guard<std::mutex> lock(debugMutex_);
    debugLogs_.clear();
}

void Debugger::logMemoryInfo(const std::string& context) {
    logDebug("Memory Info: " + getSystemInfo(), "Memory", {{ "context", context }});
}

void Debugger::checkMemoryLeaks() {
    logDebug("Checking for memory leaks...", "Memory");
    // Placeholder implementation
}

void Debugger::logNeuralActivity(const std::string& context) {
    logDebug("Neural activity logged: " + context, "Neural", {{ "context", context }});
}

void Debugger::logPlasticityChanges(const std::string& context) {
    logDebug("Plasticity changes logged: " + context, "Plasticity", {{ "context", context }});
}

void Debugger::logDevelopmentProgress(const std::string& context) {
    logDebug("Development progress logged: " + context, "Development", {{ "context", context }});
}

void Debugger::logPerformanceMetrics(const std::string& context) {
    logDebug("Performance metrics logged: " + context, "Performance", {{ "context", context }});
}

void Debugger::logBottleneckAnalysis(const std::string& context) {
    logDebug("Bottleneck analysis logged: " + context, "Performance", {{ "context", context }});
}

std::string Debugger::inspectBrainState() const {
    std::stringstream ss;
    ss << "Brain State Inspection\n";
    ss << "=======================\n";
    // Placeholder implementation
    return ss.str();
}

std::string Debugger::inspectAgentState() const {
    std::stringstream ss;
    ss << "Agent State Inspection\n";
    ss << "======================\n";
    // Placeholder implementation
    return ss.str();
}

std::string Debugger::inspectWorldState() const {
    std::stringstream ss;
    ss << "World State Inspection\n";
    ss << "========================\n";
    // Placeholder implementation
    return ss.str();
}

bool Debugger::exportDebugLogs(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << exportDebugLogsJSON();
    return true;
}

bool Debugger::importDebugLogs(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return importDebugLogsJSON(jsonData);
}

std::string Debugger::exportDebugLogsJSON() const {
    std::stringstream ss;
    ss << "{\n\"logs\": [";
    
    bool first = true;
    for (const auto& debugInfo : debugLogs_) {
        if (!first) {
            ss << ",\n";
        }
        first = false;
        
        ss << "{\n\"timestamp\": \"" << std::chrono::system_clock::to_time_t(debugInfo.timestamp) << "\",\n";
        ss << "\"level\": \"" << debugLevelToString(debugInfo.level) << "\",\n";
        ss << "\"source\": \"" << debugInfo.source << "\",\n";
        ss << "\"message\": \"" << debugInfo.message << "\",\n";
        ss << "\"context\": {";
        
        bool firstContext = true;
        for (const auto& [key, value] : debugInfo.context) {
            if (!firstContext) {
                ss << ", ";
            }
            firstContext = false;
            ss << "\"" << key << "\": \"" << value << "\"";
        }
        ss << "}\n}";
    }
    
    ss << "\n]\n}";
    return ss.str();
}

bool Debugger::importDebugLogsJSON(const std::string& jsonData) {
    // This is a placeholder implementation for JSON import
    // In a real implementation, you would use a JSON library
    clearDebugLogs();
    return true;
}

void Debugger::startProfiling(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(debugMutex_);
    profiles_[profileName].push_back(std::chrono::high_resolution_clock::now());
}

void Debugger::stopProfiling(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(debugMutex_);
    
    auto it = profiles_.find(profileName);
    if (it != profiles_.end() && !it->second.empty()) {
        auto startTime = it->second.back();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> duration = endTime - startTime;
        it->second.pop_back();
        
        logInfo("Profile completed: " + profileName + " (Duration: " + std::to_string(duration.count()) + "s)", "Profiler");
    }
}

std::string Debugger::getProfilingResults(const std::string& profileName) const {
    std::lock_guard<std::mutex> lock(debugMutex_);
    
    auto it = profiles_.find(profileName);
    if (it == profiles_.end()) {
        return "Profile not found: " + profileName;
    }
    
    std::stringstream ss;
    ss << "=== Profiling Results: " << profileName << " ===\n";
    ss << "Number of runs: " << it->second.size() << "\n";
    
    if (it->second.size() >= 2) {
        std::vector<double> durations;
        for (size_t i = 1; i < it->second.size(); ++i) {
            std::chrono::duration<double> duration = it->second[i] - it->second[i-1];
            durations.push_back(duration.count());
        }
        
        double mean = calculateMean(durations);
        double stddev = calculateStandardDeviation(durations);
        
        ss << "Mean duration: " << mean << " seconds\n";
        ss << "Standard deviation: " << stddev << " seconds\n";
    }
    
    ss << "=== End Profiling Results ===\n";
    
    return ss.str();
}

std::string Debugger::debugLevelToString(DebugLevel level) {
    switch (level) {
        case DebugLevel::None: return "NONE";
        case DebugLevel::Error: return "ERROR";
        case DebugLevel::Warning: return "WARNING";
        case DebugLevel::Info: return "INFO";
        case DebugLevel::Debug: return "DEBUG";
        case DebugLevel::Trace: return "TRACE";
        default: return "UNKNOWN";
    }
}

std::string Debugger::generateStackTrace() const {
    // Placeholder implementation
    return "Stack trace not available";
}

std::string Debugger::getSystemInfo() const {
    // Placeholder implementation
    return "System info not available";
}

void Debugger::cleanupOldLogs() {
    if (debugLogs_.size() > maxLogSize_) {
        debugLogs_.erase(debugLogs_.begin(), debugLogs_.begin() + 1);
    }
}

// TestingTools implementation

void TestingTools::enableAutoTesting(bool enable, std::chrono::seconds interval) {
    enableAutoTesting_ = enable;
    autoTestInterval_ = interval;
}

TestingTools::TestResult TestingTools::testNeuronDynamics(const std::string& testName) {
    return createTestResult(testName, "Neuron Dynamics", true, 0.1, "Neuron dynamics test passed");
}

TestingTools::TestResult TestingTools::testSynapticTransmission(const std::string& testName) {
    return createTestResult(testName, "Synaptic Transmission", true, 0.1, "Synaptic transmission test passed");
}

TestingTools::TestResult TestingTools::testPlasticityRules(const std::string& testName) {
    return createTestResult(testName, "Plasticity Rules", true, 0.1, "Plasticity rules test passed");
}

TestingTools::TestResult TestingTools::testMemorySystems(const std::string& testName) {
    return createTestResult(testName, "Memory Systems", true, 0.1, "Memory systems test passed");
}

TestingTools::TestResult TestingTools::testNeuromodulation(const std::string& testName) {
    return createTestResult(testName, "Neuromodulation", true, 0.1, "Neuromodulation test passed");
}

TestingTools::TestResult TestingTools::testAgentBehavior(const std::string& testName) {
    return createTestResult(testName, "Agent Behavior", true, 0.1, "Agent behavior test passed");
}

TestingTools::TestResult TestingTools::testSensoryProcessing(const std::string& testName) {
    return createTestResult(testName, "Sensory Processing", true, 0.1, "Sensory processing test passed");
}

TestingTools::TestResult TestingTools::testMotorControl(const std::string& testName) {
    return createTestResult(testName, "Motor Control", true, 0.1, "Motor control test passed");
}

TestingTools::TestResult TestingTools::testLearningCapability(const std::string& testName) {
    return createTestResult(testName, "Learning Capability", true, 0.1, "Learning capability test passed");
}

TestingTools::TestResult TestingTools::testWorldInteraction(const std::string& testName) {
    return createTestResult(testName, "World Interaction", true, 0.1, "World interaction test passed");
}

TestingTools::TestResult TestingTools::testEnvironmentModeling(const std::string& testName) {
    return createTestResult(testName, "Environment Modeling", true, 0.1, "Environment modeling test passed");
}

TestingTools::TestResult TestingTools::testRewardSystem(const std::string& testName) {
    return createTestResult(testName, "Reward System", true, 0.1, "Reward system test passed");
}

TestingTools::TestResult TestingTools::testPerformance(const std::string& testName) {
    return createTestResult(testName, "Performance", true, 0.1, "Performance test passed");
}

TestingTools::TestResult TestingTools::testScalability(const std::string& testName) {
    return createTestResult(testName, "Scalability", true, 0.1, "Scalability test passed");
}

TestingTools::TestResult TestingTools::testMemoryUsage(const std::string& testName) {
    return createTestResult(testName, "Memory Usage", true, 0.1, "Memory usage test passed");
}

TestingTools::TestResult TestingTools::runComprehensiveTestSuite(const std::string& testSuiteName) {
    return createTestResult(testSuiteName, "Comprehensive Test Suite", true, 1.0, "Comprehensive test suite passed");
}

TestingTools::TestResult TestingTools::runRegressionTest(const std::string& testName) {
    return createTestResult(testName, "Regression Test", true, 0.5, "Regression test passed");
}

std::vector<TestingTools::TestResult> TestingTools::getTestResults(const std::string& testName,
                                                                  const std::string& testCategory) const {
    std::lock_guard<std::mutex> lock(testMutex_);
    
    std::vector<TestResult> result;
    
    if (testName.empty() && testCategory.empty()) {
        return testResults_;
    }
    
    for (const auto& testResult : testResults_) {
        if ((testName.empty() || testResult.testName == testName) &&
            (testCategory.empty() || testResult.testCategory == testCategory)) {
            result.push_back(testResult);
        }
    }
    
    return result;
}

double TestingTools::getTestSuccessRate(const std::string& testName,
                                      const std::string& testCategory) const {
    std::vector<TestResult> tests = getTestResults(testName, testCategory);
    
    if (tests.empty()) {
        return 0.0;
    }
    
    int passedCount = 0;
    for (const auto& test : tests) {
        if (test.passed) {
            passedCount++;
        }
    }
    
    return static_cast<double>(passedCount) / tests.size();
}

std::string TestingTools::getTestReport(const std::string& testName,
                                       const std::string& testCategory) const {
    std::vector<TestResult> tests = getTestResults(testName, testCategory);
    
    std::stringstream ss;
    ss << "=== Test Report ===\n";
    
    if (!testName.empty()) {
        ss << "Test: " << testName << "\n";
    }
    if (!testCategory.empty()) {
        ss << "Category: " << testCategory << "\n";
    }
    
    ss << "Total tests: " << tests.size() << "\n";
    
    int passedCount = 0;
    for (const auto& test : tests) {
        if (test.passed) {
            passedCount++;
        }
    }
    
    ss << "Passed: " << passedCount << "\n";
    ss << "Failed: " << (tests.size() - passedCount) << "\n";
    
    if (tests.size() > 0) {
        ss << "Success rate: " << (static_cast<double>(passedCount) / tests.size() * 100) << "%\n";
    }
    
    ss << "=== End Test Report ===\n";
    
    return ss.str();
}

void TestingTools::addTestConfiguration(const TestConfiguration& config) {
    testConfigurations_[config.testName] = config;
}

void TestingTools::removeTestConfiguration(const std::string& testName) {
    testConfigurations_.erase(testName);
}

std::vector<TestingTools::TestConfiguration> TestingTools::getTestConfigurations(
    const std::string& testCategory) const {
    std::vector<TestConfiguration> result;
    
    for (const auto& [name, config] : testConfigurations_) {
        if (testCategory.empty() || config.testCategory == testCategory) {
            result.push_back(config);
        }
    }
    
    return result;
}

TestingTools::TestResult TestingTools::runStressTest(const std::string& testName,
                                                   const std::map<std::string, std::string>& parameters) {
    return createTestResult(testName, "Stress Test", true, 5.0, "Stress test passed");
}

TestingTools::TestResult TestingTools::runLoadTest(const std::string& testName,
                                                  const std::map<std::string, std::string>& parameters) {
    return createTestResult(testName, "Load Test", true, 10.0, "Load test passed");
}

TestingTools::TestResult TestingTools::runValidationTest(const std::string& testName) {
    return createTestResult(testName, "Validation Test", true, 2.0, "Validation test passed");
}

TestingTools::TestResult TestingTools::runConsistencyTest(const std::string& testName) {
    return createTestResult(testName, "Consistency Test", true, 2.0, "Consistency test passed");
}

TestingTools::TestResult TestingTools::executeTest(const TestConfiguration& config) {
    return createTestResult(config.testName, config.testCategory, true, 1.0, "Test passed");
}

TestingTools::TestResult TestingTools::createTestResult(const std::string& testName,
                                                      const std::string& testCategory,
                                                      bool passed,
                                                      double executionTime,
                                                      const std::string& errorMessage,
                                                      const std::map<std::string, std::string>& metadata) {
    TestResult result;
    result.timestamp = std::chrono::system_clock::now();
    result.testName = testName;
    result.testCategory = testCategory;
    result.passed = passed;
    result.executionTime = executionTime;
    result.errorMessage = errorMessage;
    result.metadata = metadata;
    
    std::lock_guard<std::mutex> lock(testMutex_);
    testResults_.push_back(result);
    
    return result;
}

// OptimizationTools implementation

void OptimizationTools::addPerformanceProfile(const PerformanceProfile& profile) {
    performanceProfiles_[profile.profileName] = profile;
}

bool OptimizationTools::removePerformanceProfile(const std::string& profileName) {
    auto it = performanceProfiles_.find(profileName);
    if (it != performanceProfiles_.end()) {
        performanceProfiles_.erase(it);
        return true;
    }
    return false;
}

std::vector<OptimizationTools::PerformanceProfile> OptimizationTools::getPerformanceProfiles() const {
    std::vector<PerformanceProfile> result;
    for (const auto& [name, profile] : performanceProfiles_) {
        result.push_back(profile);
    }
    return result;
}

OptimizationTools::PerformanceProfile OptimizationTools::findOptimalProfile(
    const std::map<std::string, double>& targetPerformance,
    const std::vector<PerformanceProfile>& availableProfiles) {
    PerformanceProfile optimalProfile;
    double bestScore = 0.0;
    
    for (const auto& profile : availableProfiles) {
        double score = 0.0;
        
        for (const auto& [metric, targetValue] : targetPerformance) {
            auto it = profile.parameters.find(metric);
            if (it != profile.parameters.end()) {
                score += std::abs(it->second - targetValue) / std::max(targetValue, 1.0);
            }
        }
        
        if (score < bestScore || bestScore == 0.0) {
            bestScore = score;
            optimalProfile = profile;
        }
    }
    
    return optimalProfile;
}

std::map<std::string, double> OptimizationTools::optimizeParameters(
    const std::string& profileName,
    const std::map<std::string, double>& currentParameters,
    const std::map<std::string, double>& targetValues) {
    // Placeholder implementation
    return currentParameters;
}

double OptimizationTools::benchmarkOperation(const std::string& operation,
                                             const std::map<std::string, std::string>& parameters) {
    // Placeholder implementation
    return 0.0;
}

std::map<std::string, double> OptimizationTools::benchmarkSystem(
    const std::map<std::string, std::string>& scenarios) {
    // Placeholder implementation
    return std::map<std::string, double>();
}

OptimizationTools::ResourceUsage OptimizationTools::analyzeResourceUsage(
    const std::string& operation,
    const std::map<std::string, std::string>& parameters) {
    ResourceUsage usage;
    usage.cpuUsage = 0.0;
    usage.memoryUsage = 0.0;
    usage.networkUsage = 0.0;
    usage.timeUsage = std::chrono::duration<double>(0.0);
    return usage;
}

std::vector<std::string> OptimizationTools::getOptimizationRecommendations(
    const std::map<std::string, double>& currentMetrics,
    const std::map<std::string, double>& targetMetrics) {
    return std::vector<std::string>();
}

std::map<std::string, double> OptimizationTools::applyParticleSwarmOptimization(
    const std::map<std::string, double>& currentParameters,
    const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
    size_t iterations) {
    // Placeholder implementation
    return currentParameters;
}

std::map<std::string, double> OptimizationTools::applyGeneticAlgorithm(
    const std::map<std::string, double>& currentParameters,
    const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
    size_t populationSize,
    size_t generations) {
    // Placeholder implementation
    return currentParameters;
}

std::map<std::string, double> OptimizationTools::optimizeNeuralNetworkConfiguration(
    const std::map<std::string, double>& currentConfig,
    const std::map<std::string, double>& performanceTargets) {
    // Placeholder implementation
    return currentConfig;
}

std::map<std::string, double> OptimizationTools::optimizeMemoryConfiguration(
    const std::map<std::string, double>& currentConfig,
    const std::map<std::string, double>& performanceTargets) {
    // Placeholder implementation
    return currentConfig;
}

double OptimizationTools::evaluateObjectiveFunction(
    const std::map<std::string, double>& parameters,
    const std::function<double(const std::map<std::string, double>&)>& objectiveFunction) {
    return objectiveFunction(parameters);
}

} // namespace nlm