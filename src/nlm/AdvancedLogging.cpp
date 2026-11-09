// Implementation of AdvancedLogger

#include "AdvancedLogging.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace nlm {

AdvancedLogger::AdvancedLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default values
    pImpl->consoleLoggingEnabled = true;
    pImpl->consoleLogLevel = LogLevel::Info;
    pImpl->coloredOutput = true;
    pImpl->fileLoggingEnabled = false;
}

AdvancedLogger::~AdvancedLogger() {
    flush();
}

void AdvancedLogger::enableFileLogging(const std::string& filepath, bool append) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    pImpl->logFilePath = filepath;
    std::ios_base::openmode mode = std::ios::out;
    if (append) {
        mode |= std::ios::app;
    } else {
        mode |= std::ios::trunc;
    }
    
    pImpl->logFile.open(filepath, mode);
    if (pImpl->logFile.is_open()) {
        pImpl->fileLoggingEnabled = true;
        logEvent(LogLevel::Info, "AdvancedLogger", "File logging enabled: " + filepath);
    } else {
        logEvent(LogLevel::Error, "AdvancedLogger", "Failed to open log file: " + filepath);
    }
}

void AdvancedLogger::disableFileLogging() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
    pImpl->fileLoggingEnabled = false;
}

void AdvancedLogger::setConsoleLogging(bool enabled) {
    pImpl->consoleLoggingEnabled = enabled;
}

void AdvancedLogger::setConsoleLogLevel(LogLevel level) {
    pImpl->consoleLogLevel = level;
}

void AdvancedLogger::enableColoredOutput(bool enabled) {
    pImpl->coloredOutput = enabled;
}

void AdvancedLogger::setSourceLogLevel(const std::string& source, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->sourceLogLevels[source] = level;
}

void AdvancedLogger::includeSource(const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->includedSources.push_back(source);
}

void AdvancedLogger::excludeSource(const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->excludedSources.push_back(source);
}

void AdvancedLogger::setPerformanceThreshold(PerformanceMetric metric, double min, double max) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    pImpl->performanceThresholds[metric] = {min, max};
}

void AdvancedLogger::logEvent(LogLevel level, const std::string& source, const std::string& message,
                              const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    // Check if source is excluded
    bool isExcluded = std::find(pImpl->excludedSources.begin(), pImpl->excludedSources.end(), source) != pImpl->excludedSources.end();
    if (isExcluded) {
        return;
    }
    
    // Check if source is explicitly included
    bool isExplicitlyIncluded = pImpl->includedSources.empty() ||
                                std::find(pImpl->includedSources.begin(), pImpl->includedSources.end(), source) != pImpl->includedSources.end();
    
    // Check source-specific log level
    bool shouldLog = true;
    if (pImpl->sourceLogLevels.count(source)) {
        shouldLog = level >= pImpl->sourceLogLevels[source];
    } else if (!isExplicitlyIncluded) {
        shouldLog = false;
    }
    
    if (!shouldLog || level < pImpl->consoleLogLevel) {
        return;
    }
    
    // Create event log
    EventLog event;
    event.timestamp = std::chrono::steady_clock::now();
    event.level = level;
    event.source = source;
    event.message = message;
    event.metadata = metadata;
    
    // Add to event log
    pImpl->eventLog.push_back(event);
    
    // Write to file if enabled
    if (pImpl->fileLoggingEnabled && pImpl->logFile.is_open()) {
        pImpl->logFile << formatEventLog(event) << std::endl;
    }
    
    // Output to console
    if (pImpl->consoleLoggingEnabled) {
        std::cout << formatConsoleOutput(event) << std::endl;
    }
    
    // Call event callbacks
    for (const auto& callback : pImpl->eventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            std::cerr << "Error in event callback: " << e.what() << std::endl;
        }
    }
}

void AdvancedLogger::logTrace(const std::string& source, const std::string& message,
                              const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Trace, source, message, metadata);
}

void AdvancedLogger::logDebug(const std::string& source, const std::string& message,
                              const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Debug, source, message, metadata);
}

void AdvancedLogger::logInfo(const std::string& source, const std::string& message,
                             const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Info, source, message, metadata);
}

void AdvancedLogger::logWarning(const std::string& source, const std::string& message,
                                const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Warning, source, message, metadata);
}

void AdvancedLogger::logError(const std::string& source, const std::string& message,
                              const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Error, source, message, metadata);
}

void AdvancedLogger::logFatal(const std::string& source, const std::string& message,
                              const std::map<std::string, std::string>& metadata) {
    logEvent(LogLevel::Fatal, source, message, metadata);
}

void AdvancedLogger::recordPerformanceMetric(PerformanceMetric metric, double value,
                                             const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::steady_clock::now();
    snapshot.metrics[metric] = value;
    snapshot.brainState = context;
    
    // Check for performance threshold violations
    if (pImpl->performanceThresholds.count(metric)) {
        auto& threshold = pImpl->performanceThresholds[metric];
        if (value < threshold.first || value > threshold.second) {
            logEvent(LogLevel::Warning, "PerformanceMonitor", 
                    "Performance threshold violation: " + std::to_string(value) + " (threshold: " +
                    std::to_string(threshold.first) + " - " + std::to_string(threshold.second) + ")",
                    {{metric, std::to_string(value)}});
        }
    }
    
    pImpl->performanceHistory.push_back(snapshot);
    
    // Call performance callback
    if (pImpl->performanceCallback) {
        try {
            pImpl->performanceCallback(snapshot);
        } catch (const std::exception& e) {
            std::cerr << "Error in performance callback: " << e.what() << std::endl;
        }
    }
}

void AdvancedLogger::updatePerformance(const std::map<PerformanceMetric, double>& metrics,
                                       uint64_t step, float simulationTime) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::steady_clock::now();
    snapshot.simulationStep = step;
    snapshot.simulationTime = simulationTime;
    snapshot.metrics = metrics;
    
    pImpl->performanceHistory.push_back(snapshot);
}

void AdvancedLogger::recordSpike() {
    pImpl->totalSpikes.fetch_add(1);
    recordPerformanceMetric(PerformanceMetric::SpikeCount, 
                           static_cast<double>(pImpl->totalSpikes.load()));
}

void AdvancedLogger::recordStep() {
    pImpl->totalSteps.fetch_add(1);
    recordPerformanceMetric(PerformanceMetric::SimulationSteps,
                           static_cast<double>(pImpl->totalSteps.load()));
}

void AdvancedLogger::addEventCallback(const std::function<void(const EventLog&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->eventCallbacks.push_back(callback);
}

void AdvancedLogger::setPerformanceCallback(const std::function<void(const PerformanceSnapshot&)>& callback) {
    pImpl->performanceCallback = callback;
}

std::vector<EventLog> AdvancedLogger::getEvents(const std::string& source,
                                                LogLevel minLevel,
                                                const std::chrono::steady_clock::time_point& since) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    std::vector<EventLog> filteredEvents;
    filteredEvents.reserve(pImpl->eventLog.size());
    
    for (const auto& event : pImpl->eventLog) {
        // Filter by source
        if (!source.empty() && event.source != source) {
            continue;
        }
        
        // Filter by level
        if (event.level < minLevel) {
            continue;
        }
        
        // Filter by time
        if (!since.empty() && event.timestamp < since) {
            continue;
        }
        
        filteredEvents.push_back(event);
    }
    
    return filteredEvents;
}

std::vector<PerformanceSnapshot> AdvancedLogger::getPerformanceHistory(
    const std::chrono::steady_clock::time_point& since) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    
    std::vector<PerformanceSnapshot> filteredSnapshots;
    filteredSnapshots.reserve(pImpl->performanceHistory.size());
    
    for (const auto& snapshot : pImpl->performanceHistory) {
        if (!since.empty() && snapshot.timestamp < since) {
            continue;
        }
        filteredSnapshots.push_back(snapshot);
    }
    
    return filteredSnapshots;
}

std::string AdvancedLogger::formatEventLog(const EventLog& event) const {
    std::ostringstream oss;
    
    // Format timestamp
    auto time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << 
        "." << std::setfill('0') << std::setw(6) << 
        std::chrono::duration_cast<std::chrono::milliseconds>(
            event.timestamp.time_since_epoch()).count() % 1000;
    
    // Format level
    const char* levelStr = "UNKNOWN";
    switch (event.level) {
        case LogLevel::Trace: levelStr = "TRACE"; break;
        case LogLevel::Debug: levelStr = "DEBUG"; break;
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARN"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
        case LogLevel::Fatal: levelStr = "FATAL"; break;
    }
    
    oss << " [" << levelStr << "] " << event.source << ": " << event.message;
    
    // Add metadata
    if (!event.metadata.empty()) {
        oss << " (";
        bool first = true;
        for (const auto& pair : event.metadata) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << ")";
    }
    
    return oss.str();
}

std::string AdvancedLogger::formatConsoleOutput(const EventLog& event) const {
    std::ostringstream oss;
    
    if (pImpl->coloredOutput) {
        // Add color codes
        const char* colorCode = "\033[0m"; // Default
        switch (event.level) {
            case LogLevel::Trace: colorCode = "\033[37m"; break; // White
            case LogLevel::Debug: colorCode = "\033[36m"; break; // Cyan
            case LogLevel::Info: colorCode = "\033[32m"; break; // Green
            case LogLevel::Warning: colorCode = "\033[33m"; break; // Yellow
            case LogLevel::Error: colorCode = "\033[31m"; break; // Red
            case LogLevel::Fatal: colorCode = "\033[41m"; break; // Red background
        }
        
        oss << colorCode;
    }
    
    // Format level and source
    const char* levelStr = "UNKNOWN";
    switch (event.level) {
        case LogLevel::Trace: levelStr = "TRACE"; break;
        case LogLevel::Debug: levelStr = "DEBUG"; break;
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARN"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
        case LogLevel::Fatal: levelStr = "FATAL"; break;
    }
    
    oss << "[" << levelStr << "] " << event.source << ": " << event.message;
    
    if (pImpl->coloredOutput) {
        oss << "\033[0m"; // Reset color
    }
    
    return oss.str();
}

std::string AdvancedLogger::generatePerformanceReport() const {
    std::ostringstream oss;
    
    oss << "=== NLM Performance Report ===" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << "Total Steps: " << pImpl->totalSteps.load() << std::endl;
    oss << "Total Spikes: " << pImpl->totalSpikes.load() << std::endl;
    
    if (!pImpl->performanceHistory.empty()) {
        oss << "\nPerformance Metrics:" << std::endl;
        oss << std::left << std::setw(30) << "Metric" << std::setw(15) << "Min" << 
            std::setw(15) << "Max" << std::setw(15) << "Avg" << std::setw(15) << "Current" << 
            std::endl;
        oss << std::string(90, '-') << std::endl;
        
        for (const auto& metricPair : pImpl->performanceThresholds) {
            PerformanceMetric metric = metricPair.first;
            const auto& thresholds = metricPair.second;
            
            // Calculate statistics
            double min = 0.0, max = 0.0, avg = 0.0;
            double current = 0.0;
            size_t count = 0;
            
            for (const auto& snapshot : pImpl->performanceHistory) {
                auto it = snapshot.metrics.find(metric);
                if (it != snapshot.metrics.end()) {
                    double value = it->second;
                    if (count == 0) {
                        min = max = avg = current = value;
                    } else {
                        min = std::min(min, value);
                        max = std::max(max, value);
                        avg += value;
                    }
                    count++;
                    current = value;
                }
            }
            
            if (count > 0) {
                avg /= count;
                oss << std::left << std::setw(30) << getPerformanceMetricName(metric);
                oss << std::setw(15) << min;
                oss << std::setw(15) << max;
                oss << std::setw(15) << avg;
                oss << std::setw(15) << current << std::endl;
            }
        }
    }
    
    return oss.str();
}

std::string AdvancedLogger::generateEventReport(const std::string& source) const {
    std::ostringstream oss;
    
    oss << "=== NLM Event Report ===" << std::endl;
    
    std::vector<EventLog> filteredEvents = getEvents(source);
    
    oss << "Total Events: " << filteredEvents.size() << std::endl;
    
    // Count events by level
    std::map<LogLevel, int> levelCounts;
    for (const auto& event : filteredEvents) {
        levelCounts[event.level]++;
    }
    
    oss << "\nEvents by Level:" << std::endl;
    for (const auto& pair : levelCounts) {
        const char* levelStr = "UNKNOWN";
        switch (pair.first) {
            case LogLevel::Trace: levelStr = "TRACE"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Warning: levelStr = "WARN"; break;
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Fatal: levelStr = "FATAL"; break;
        }
        oss << "  " << levelStr << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

std::map<std::string, double> AdvancedLogger::analyzePerformanceTrends() const {
    std::map<std::string, double> trends;
    
    if (pImpl->performanceHistory.size() < 2) {
        return trends;
    }
    
    // Calculate simple linear trends for key metrics
    auto firstSnapshot = pImpl->performanceHistory.front();
    auto lastSnapshot = pImpl->performanceHistory.back();
    
    // Calculate step rate
    auto firstTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        firstSnapshot.timestamp.time_since_epoch()).count();
    auto lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        lastSnapshot.timestamp.time_since_epoch()).count();
    
    if (lastTime > firstTime) {
        double timeDiff = (lastTime - firstTime) / 1000.0; // seconds
        double stepDiff = lastSnapshot.simulationStep - firstSnapshot.simulationStep;
        trends["steps_per_second"] = stepDiff / timeDiff;
    }
    
    // Calculate spike rate
    double spikeDiff = lastSnapshot.metrics.count(PerformanceMetric::SpikeCount) ? 
                       lastSnapshot.metrics.at(PerformanceMetric::SpikeCount) : 0.0 - 
                       (firstSnapshot.metrics.count(PerformanceMetric::SpikeCount) ? 
                        firstSnapshot.metrics.at(PerformanceMetric::SpikeCount) : 0.0);
    
    if (timeDiff > 0.0) {
        trends["spikes_per_second"] = spikeDiff / timeDiff;
    }
    
    return trends;
}

bool AdvancedLogger::exportLogs(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Export events
        file << "=== Event Log ===" << std::endl;
        for (const auto& event : pImpl->eventLog) {
            file << formatEventLog(event) << std::endl;
        }
        
        // Export performance data
        file << "\n=== Performance History ===" << std::endl;
        for (const auto& snapshot : pImpl->performanceHistory) {
            file << "Step: " << snapshot.simulationStep << 
                 " Time: " << snapshot.simulationTime << "s" << std::endl;
            for (const auto& metricPair : snapshot.metrics) {
                file << "  " << getPerformanceMetricName(metricPair.first) << 
                     ": " << metricPair.second << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting logs: " << e.what() << std::endl;
        return false;
    }
}

bool AdvancedLogger::importLogs(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        EventLog currentEvent;
        bool inEventSection = false;
        
        while (std::getline(file, line)) {
            if (line.find("=== Event Log ===") != std::string::npos) {
                inEventSection = true;
                continue;
            } else if (line.find("=== Performance History ===") != std::string::npos) {
                inEventSection = false;
                continue;
            } else if (line.empty()) {
                continue;
            }
            
            if (inEventSection) {
                // Parse event line (simplified)
                // In a real implementation, this would properly parse the format
                // For now, just add the line as a message
                if (!currentEvent.message.empty()) {
                    currentEvent.message += "\n" + line;
                } else {
                    currentEvent.message = line;
                }
            } else {
                // Parse performance data (simplified)
                // In a real implementation, this would properly parse the format
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing logs: " << e.what() << std::endl;
        return false;
    }
}

void AdvancedLogger::printStatus() const {
    std::cout << "=== NLM Advanced Logger Status ===" << std::endl;
    std::cout << "File Logging: " << (pImpl->fileLoggingEnabled ? "Enabled" : "Disabled") << std::endl;
    if (pImpl->fileLoggingEnabled) {
        std::cout << "Log File: " << pImpl->logFilePath << std::endl;
    }
    std::cout << "Console Logging: " << (pImpl->consoleLoggingEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Console Log Level: " << getLogLevelName(pImpl->consoleLogLevel) << std::endl;
    std::cout << "Colored Output: " << (pImpl->coloredOutput ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Total Events: " << pImpl->eventLog.size() << std::endl;
    std::cout << "Total Performance Snapshots: " << pImpl->performanceHistory.size() << std::endl;
    std::cout << "Total Callbacks: " << pImpl->eventCallbacks.size() << std::endl;
    std::cout << "Performance Thresholds: " << pImpl->performanceThresholds.size() << std::endl;
}

bool AdvancedLogger::isHealthy() const {
    // Check if the logger is responsive
    try {
        // Try to log a test message
        logEvent(LogLevel::Info, "HealthCheck", "Health check test");
        
        // Check if files are still open
        if (pImpl->fileLoggingEnabled && pImpl->logFile.is_open()) {
            return true;
        } else if (!pImpl->fileLoggingEnabled) {
            return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

void AdvancedLogger::clearHistory(size_t maxEvents, size_t maxSnapshots) {
    std::lock_guard<std::mutex> lock1(pImpl->logMutex);
    std::lock_guard<std::mutex> lock2(pImpl->performanceMutex);
    
    if (pImpl->eventLog.size() > maxEvents) {
        auto start = pImpl->eventLog.end() - maxEvents;
        pImpl->eventLog.erase(pImpl->eventLog.begin(), start);
    }
    
    if (pImpl->performanceHistory.size() > maxSnapshots) {
        auto start = pImpl->performanceHistory.end() - maxSnapshots;
        pImpl->performanceHistory.erase(pImpl->performanceHistory.begin(), start);
    }
}

void AdvancedLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    if (pImpl->fileLoggingEnabled && pImpl->logFile.is_open()) {
        pImpl->logFile.flush();
    }
}

std::string AdvancedLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

const char* AdvancedLogger::getPerformanceMetricName(PerformanceMetric metric) const {
    switch (metric) {
        case PerformanceMetric::SimulationSteps: return "Simulation Steps";
        case PerformanceMetric::SpikeCount: return "Spike Count";
        case PerformanceMetric::FiringRate: return "Firing Rate (Hz)";
        case PerformanceMetric::MemoryUsage: return "Memory Usage (MB)";
        case PerformanceMetric::CPUUsage: return "CPU Usage (%)";
        case PerformanceMetric::CheckpointTime: return "Checkpoint Time (s)";
        case PerformanceMetric::NetworkLatency: return "Network Latency (ms)";
        case PerformanceMetric::PlasticityEvents: return "Plasticity Events";
        case PerformanceMetric::MemoryOperations: return "Memory Operations";
        default: return "Unknown";
    }
}

const char* AdvancedLogger::getLogLevelName(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Global instance
AdvancedLogger g_advancedLogger;

} // namespace nlm
