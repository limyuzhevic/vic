#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct StructuredLogger::Impl {
    LogLevel minimumLevel = LogLevel::Info;
    std::unordered_map<LogCategory, bool> enabledCategories;
    std::mutex mutex;
    std::string currentLogFile;
    std::ofstream logFile;
    size_t maxFileSizeMB = 10;
    int maxBackupFiles = 5;
    std::vector<PerformanceMetric> recentMetrics;
    std::string lastCorrelationId;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeTraces;
    std::unordered_map<std::string, std::vector<std::string>> correlationMetadata;
    size_t metricHistoryLimit = 1000;
};

StructuredLogger::StructuredLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize all categories to enabled
    for (int i = 0; i < 9; ++i) {
        pImpl->enabledCategories[static_cast<LogCategory>(i)] = true;
    }
}

StructuredLogger::~StructuredLogger() {
    flushLogs();
}

void StructuredLogger::logInternal(const StructuredLogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check level
    LogLevel entryLevel;
    if (entry.level == "Trace") entryLevel = LogLevel::Trace;
    else if (entry.level == "Debug") entryLevel = LogLevel::Debug;
    else if (entry.level == "Info") entryLevel = LogLevel::Info;
    else if (entry.level == "Warning") entryLevel = LogLevel::Warning;
    else if (entry.level == "Error") entryLevel = LogLevel::Error;
    else if (entry.level == "Critical") entryLevel = LogLevel::Critical;
    else entryLevel = LogLevel::Info;
    
    if (entryLevel < pImpl->minimumLevel) {
        return;
    }
    
    // Check category
    LogCategory category;
    if (entry.category == "System") category = LogCategory::System;
    else if (entry.category == "Simulation") category = LogCategory::Simulation;
    else if (entry.category == "Neural") category = LogCategory::Neural;
    else if (entry.category == "Memory") category = LogCategory::Memory;
    else if (entry.category == "Performance") category = LogCategory::Performance;
    else if (entry.category == "Network") category = LogCategory::Network;
    else if (entry.category == "GUI") category = LogCategory::GUI;
    else if (entry.category == "Error") category = LogCategory::Error;
    else if (entry.category == "Audit") category = LogCategory::Audit;
    else category = LogCategory::System;
    
    if (!pImpl->enabledCategories[category]) {
        return;
    }
    
    // Format the log entry
    std::string formatted = formatStructuredLog(entry);
    
    // Output to console
    std::cout << formatted << std::endl;
    
    // Output to file if set
    if (!pImpl->currentLogFile.empty()) {
        if (!pImpl->logFile.is_open()) {
            pImpl->logFile.open(pImpl->currentLogFile, std::ios::app);
        }
        if (pImpl->logFile.is_open()) {
            pImpl->logFile << formatted << std::endl;
            pImpl->logFile.flush();
            
            // Check file size and rotate if needed
            if (pImpl->logFile.tellp() > pImpl->maxFileSizeMB * 1024 * 1024) {
                rotateLogFiles();
            }
        }
    }
}

void StructuredLogger::trace(const std::string& message, const std::string& category,
                            const std::string& correlationId,
                            const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Trace";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::debug(const std::string& message, const std::string& category,
                            const std::string& correlationId,
                            const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Debug";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::info(const std::string& message, const std::string& category,
                           const std::string& correlationId,
                           const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Info";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::warning(const std::string& message, const std::string& category,
                               const std::string& correlationId,
                               const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Warning";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::error(const std::string& message, const std::string& category,
                            const std::string& correlationId,
                            const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Error";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::critical(const std::string& message, const std::string& category,
                               const std::string& correlationId,
                               const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Critical";
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::logPerformance(const std::string& metricName, double value,
                                      const std::string& unit,
                                      const std::string& category,
                                      const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetric metric;
    metric.name = metricName;
    metric.value = value;
    metric.unit = unit;
    metric.timestamp = std::chrono::system_clock::now();
    metric.tags = tags;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    registerPerformanceMetric(metric);
    
    StructuredLogEntry entry;
    entry.correlationId = pImpl->lastCorrelationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Info";
    entry.message = "Performance metric: " + metricName + " = " + std::to_string(value) + " " + unit;
    entry.metrics["value"] = std::to_string(value);
    entry.metrics["unit"] = unit;
    for (const auto& tag : tags) {
        entry.metrics["tag_" + tag.first] = tag.second;
    }
    
    logInternal(entry);
}

void StructuredLogger::logContext(const std::string& category, const std::string& correlationId,
                                  LogLevel level, const std::string& message,
                                  const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    
    switch (level) {
        case LogLevel::Trace: entry.level = "Trace"; break;
        case LogLevel::Debug: entry.level = "Debug"; break;
        case LogLevel::Info: entry.level = "Info"; break;
        case LogLevel::Warning: entry.level = "Warning"; break;
        case LogLevel::Error: entry.level = "Error"; break;
        case LogLevel::Critical: entry.level = "Critical"; break;
    }
    
    entry.message = message;
    entry.metadata = metadata;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    
    logInternal(entry);
}

void StructuredLogger::logMemoryAllocation(const std::string& pointerId, size_t size,
                                          const std::string& category,
                                          const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Debug";
    entry.message = "Memory allocated: " + pointerId + " (" + std::to_string(size) + " bytes)";
    entry.metrics["size"] = std::to_string(size);
    entry.metrics["allocation"] = "true";
    
    logInternal(entry);
}

void StructuredLogger::logMemoryDeallocation(const std::string& pointerId,
                                            const std::string& category,
                                            const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Debug";
    entry.message = "Memory deallocated: " + pointerId;
    entry.metrics["deallocation"] = "true";
    
    logInternal(entry);
}

void StructuredLogger::logCPUProfile(const std::string& functionName, double cpuTime,
                                    int threadId,
                                    const std::string& category,
                                    const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.correlationId = pImpl->lastCorrelationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Debug";
    entry.message = "CPU profile: " + functionName + " took " + std::to_string(cpuTime) + " ms";
    entry.metrics["cpu_time"] = std::to_string(cpuTime);
    entry.metrics["thread_id"] = std::to_string(threadId);
    entry.metadata = metadata;
    
    logInternal(entry);
}

void StructuredLogger::logNetworkActivity(const std::string& operation, const std::string& endpoint,
                                         double latency, size_t bytesSent, size_t bytesReceived,
                                         const std::string& category,
                                         const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Info";
    entry.message = "Network: " + operation + " to " + endpoint + " (latency: " + std::to_string(latency) + "ms)";
    entry.metrics["latency"] = std::to_string(latency);
    entry.metrics["bytes_sent"] = std::to_string(bytesSent);
    entry.metrics["bytes_received"] = std::to_string(bytesReceived);
    
    logInternal(entry);
}

void StructuredLogger::logGUIClick(const std::string& elementId, const std::string& elementType,
                                  const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "GUI";
    entry.level = "Debug";
    entry.message = "GUI click: " + elementType + " (id: " + elementId + ")";
    
    logInternal(entry);
}

void StructuredLogger::logGUIUpdate(const std::string& elementId, const std::string& updateType,
                                   const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "GUI";
    entry.level = "Debug";
    entry.message = "GUI update: " + updateType + " (element: " + elementId + ")";
    
    logInternal(entry);
}

void StructuredLogger::logErrorRecovery(const std::string& errorType, const std::string& recoveryAction,
                                       bool success, const std::string& correlationId,
                                       const std::unordered_map<std::string, std::string>& context) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "Error";
    entry.level = success ? "Info" : "Error";
    entry.message = "Error recovery: " + errorType + " -> " + recoveryAction + " (" + (success ? "success" : "failed") + ")";
    entry.metadata = context;
    entry.metrics["success"] = success ? "true" : "false";
    
    logInternal(entry);
}

void StructuredLogger::logAuditEvent(const std::string& eventType, const std::string& userId,
                                    const std::string& action, const std::string& correlationId,
                                    const std::unordered_map<std::string, std::string>& details) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "Audit";
    entry.level = "Info";
    entry.message = "Audit: " + eventType + " by " + userId + " - " + action;
    entry.metadata = details;
    
    logInternal(entry);
}

void StructuredLogger::logSimulationStep(int step, double simulationTime, int activeNeurons,
                                        int totalNeurons, const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "Simulation";
    entry.level = "Info";
    entry.message = "Simulation step: " + std::to_string(step) + " (" + std::to_string(simulationTime) + "ms, " + std::to_string(activeNeurons) + "/" + std::to_string(totalNeurons) + " neurons)";
    entry.metrics["step"] = std::to_string(step);
    entry.metrics["simulation_time"] = std::to_string(simulationTime);
    entry.metrics["active_neurons"] = std::to_string(activeNeurons);
    entry.metrics["total_neurons"] = std::to_string(totalNeurons);
    
    logInternal(entry);
}

void StructuredLogger::logBenchmark(const std::string& benchmarkName, double executionTime,
                                   int iterations, const std::string& category) {
    StructuredLogEntry entry;
    entry.correlationId = pImpl->lastCorrelationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.level = "Info";
    entry.message = "Benchmark: " + benchmarkName + " completed in " + std::to_string(executionTime) + "ms (" + std::to_string(iterations) + " iterations)";
    entry.metrics["execution_time"] = std::to_string(executionTime);
    entry.metrics["iterations"] = std::to_string(iterations);
    
    logInternal(entry);
}

void StructuredLogger::logSystemHealth(double cpuUsage, double memoryUsage,
                                      double diskUsage, const std::string& correlationId) {
    StructuredLogEntry entry;
    entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "System";
    entry.level = "Info";
    entry.message = "System health: CPU=" + std::to_string(cpuUsage) + "%, Memory=" + std::to_string(memoryUsage) + "%, Disk=" + std::to_string(diskUsage) + "%";
    entry.metrics["cpu_usage"] = std::to_string(cpuUsage);
    entry.metrics["memory_usage"] = std::to_string(memoryUsage);
    entry.metrics["disk_usage"] = std::to_string(diskUsage);
    
    logInternal(entry);
}

void StructuredLogger::setDebugBreakpoint(bool condition, const std::string& message,
                                        const std::string& correlationId) {
    if (condition) {
        StructuredLogEntry entry;
        entry.correlationId = correlationId.empty() ? pImpl->lastCorrelationId : correlationId;
        entry.timestamp = std::chrono::system_clock::now();
        entry.category = "Debug";
        entry.level = "Critical";
        entry.message = "BREAKPOINT: " + message + " (condition met)";
        
        logInternal(entry);
    }
}

void StructuredLogger::registerPerformanceMetric(const PerformanceMetric& metric) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recentMetrics.push_back(metric);
    
    // Limit history size
    if (pImpl->recentMetrics.size() > pImpl->metricHistoryLimit) {
        pImpl->recentMetrics.erase(pImpl->recentMetrics.begin());
    }
}

std::vector<PerformanceMetric> StructuredLogger::getRecentMetrics(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (limit > pImpl->recentMetrics.size()) {
        limit = pImpl->recentMetrics.size();
    }
    
    std::vector<PerformanceMetric> result(pImpl->recentMetrics.end() - limit, pImpl->recentMetrics.end());
    return result;
}

void StructuredLogger::rotateLogFiles(size_t maxFileSizeMB, int maxBackupFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
    
    if (std::filesystem::exists(pImpl->currentLogFile)) {
        std::string filename = pImpl->currentLogFile;
        int backupNum = 1;
        
        while (backupNum <= pImpl->maxBackupFiles) {
            std::string backupFile = filename + "." + std::to_string(backupNum);
            if (std::filesystem::exists(backupFile)) {
                std::filesystem::rename(backupFile, filename + "." + std::to_string(backupNum + 1));
            }
            backupNum++;
        }
        
        std::filesystem::rename(filename, filename + ".1");
    }
    
    pImpl->logFile.open(pImpl->currentLogFile, std::ios::trunc);
    pImpl->maxFileSizeMB = maxFileSizeMB;
    pImpl->maxBackupFiles = maxBackupFiles;
}

void StructuredLogger::flushLogs() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.flush();
    }
}

std::string StructuredLogger::generateCorrelationId() const {
    static const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string correlationId;
    std::srand(std::time(nullptr));
    
    for (int i = 0; i < 16; ++i) {
        correlationId += characters[std::rand() % characters.size()];
    }
    
    return correlationId;
}

void StructuredLogger::startCorrelationTrace(const std::string& correlationId,
                                           const std::string& operation,
                                           const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lastCorrelationId = correlationId;
    pImpl->activeTraces[correlationId] = std::chrono::system_clock::now();
    pImpl->correlationMetadata[correlationId] = metadata;
    
    StructuredLogEntry entry;
    entry.correlationId = correlationId;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "System";
    entry.level = "Info";
    entry.message = "Trace started: " + operation;
    entry.metadata = metadata;
    
    logInternal(entry);
}

void StructuredLogger::endCorrelationTrace(const std::string& correlationId,
                                         bool success, const std::string& result) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTraces.find(correlationId);
    if (it != pImpl->activeTraces.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - it->second);
        
        StructuredLogEntry entry;
        entry.correlationId = correlationId;
        entry.timestamp = std::chrono::system_clock::now();
        entry.category = "System";
        entry.level = success ? "Info" : "Error";
        entry.message = "Trace completed: " + (success ? "success" : "failed") + " (duration: " + std::to_string(duration.count()) + "ms)";
        entry.metadata["result"] = result;
        entry.metadata["success"] = success ? "true" : "false";
        entry.metadata["duration_ms"] = std::to_string(duration.count());
        
        logInternal(entry);
        
        pImpl->activeTraces.erase(it);
        pImpl->correlationMetadata.erase(correlationId);
    }
}

StructuredLogger& StructuredLogger::getInstance() {
    static StructuredLogger instance;
    return instance;
}

void StructuredLogger::setGlobalStructuredLogger(std::shared_ptr<StructuredLogger> logger) {
    static std::shared_ptr<StructuredLogger> globalLogger = logger;
}

void StructuredLogger::setMinimumLevel(LogLevel level) {
    pImpl->minimumLevel = level;
}

void StructuredLogger::setMinimumCategory(LogCategory category, bool enabled) {
    pImpl->enabledCategories[category] = enabled;
}

void StructuredLogger::setLogFile(const std::string& filepath) {
    flushLogs();
    pImpl->currentLogFile = filepath;
    pImpl->maxFileSizeMB = 10;
    pImpl->maxBackupFiles = 5;
}

std::string StructuredLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string StructuredLogger::categoryToString(LogCategory category) const {
    switch (category) {
        case LogCategory::System: return "System";
        case LogCategory::Simulation: return "Simulation";
        case LogCategory::Neural: return "Neural";
        case LogCategory::Memory: return "Memory";
        case LogCategory::Performance: return "Performance";
        case LogCategory::Network: return "Network";
        case LogCategory::GUI: return "GUI";
        case LogCategory::Error: return "Error";
        case LogCategory::Audit: return "Audit";
        default: return "Unknown";
    }
}

std::string StructuredLogger::formatStructuredLog(const StructuredLogEntry& entry) const {
    std::ostringstream oss;
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F");
    
    // Correlation ID
    oss << " [CID:" << entry.correlationId << "]";
    
    // Category and level
    oss << " [" << entry.category << "/" << entry.level << "]";
    
    // Message
    oss << " " << entry.message;
    
    // Metadata
    if (!entry.metadata.empty()) {
        oss << " {meta:";
        bool first = true;
        for (const auto& pair : entry.metadata) {
            if (!first) oss << ",";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    // Metrics
    if (!entry.metrics.empty()) {
        oss << " {metrics:";
        bool first = true;
        for (const auto& pair : entry.metrics) {
            if (!first) oss << ",";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

PerformanceScope::PerformanceScope(const std::string& function, const std::string& file, int line, const std::string& name)
    : function_(function), file_(file), line_(line), name_(name), started_(false) {}

PerformanceScope::~PerformanceScope() {
    if (started_) {
        end();
    }
}

void PerformanceScope::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
    started_ = true;
}

void PerformanceScope::end(const std::unordered_map<std::string, std::string>& metadata) {
    if (!started_) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    
    double durationMs = duration.count() / 1000.0;
    
    nlm::StructuredLogger::getInstance().logPerformance(name_, durationMs, "ms", "Performance", metadata);
    started_ = false;
}