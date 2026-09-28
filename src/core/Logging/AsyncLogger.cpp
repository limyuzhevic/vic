#pragma once

#include <nlohmann/json.hpp>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <filesystem>
#include <ctime>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

struct JsonLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::string format = "json";
    std::ofstream fileOut;
    std::mutex mutex;
    uint64_t correlationId = 0;
    uint64_t lastCorrelationId = 0;
    bool trackPerformance = false;
    bool trackMemory = false;
    bool trackCpu = false;
    std::string processId;
    uint64_t lastMemoryUsage = 0;
    double lastCpuUsage = 0.0;
};

JsonLogger::JsonLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->processId = getCurrentProcessId();
}

JsonLogger::JsonLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->processId = getCurrentProcessId();
}

JsonLogger::~JsonLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void JsonLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update performance metrics if enabled
    if (pImpl->trackPerformance) {
        updatePerformanceMetrics(const_cast<LogEntry&>(entry));
    }
    if (pImpl->trackMemory) {
        updateMemoryUsage(const_cast<LogEntry&>(entry));
    }
    if (pImpl->trackCpu) {
        updateCpuUsage(const_cast<LogEntry&>(entry));
    }
    
    // Generate correlation ID if not set
    uint64_t correlationId = entry.correlationId;
    if (correlationId == 0) {
        correlationId = ++pImpl->lastCorrelationId;
    }
    
    // Create JSON entry
    json logJson = logEntryToJson(entry);
    logJson["correlation_id"] = correlationId;
    logJson["thread_id"] = getCurrentThreadId();
    logJson["process_id"] = pImpl->processId;
    
    // Add tags and metrics
    json tagsJson = json::array();
    for (const auto& tag : entry.tags) {
        tagsJson.push_back(json::object({{"key", tag.first}, {"value", tag.second}}));
    }
    logJson["tags"] = tagsJson;
    
    json metricsJson = json::array();
    for (const auto& metric : entry.metrics) {
        metricsJson.push_back(json::object({{"key", metric.first}, {"value", metric.second}}));
    }
    logJson["metrics"] = metricsJson;
    
    std::string output;
    if (pImpl->format == "json") {
        output = logJson.dump(2);
    } else {
        output = logEntryToJson(entry).dump(2);
    }
    
    // Output based on format
    if (pImpl->format == "json") {
        std::cout << output << std::endl;
    } else {
        // Text output
        std::cout << "[" << levelToString(entry.level) << "] "
                  << entry.message << std::endl;
    }
    
    // Write to file if set
    if (pImpl->fileOut.is_open()) {
        writeToJsonFile(entry);
    }
}

void JsonLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel JsonLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void JsonLogger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

void JsonLogger::setOutputFile(const std::string& filepath, bool rotate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());
    
    // Open file in append mode
    pImpl->fileOut.open(filepath, std::ios::app);
}

void JsonLogger::setMaxFileSize(size_t maxBytes) {
    // Implementation would handle file size checking
}

void JsonLogger::setMaxFiles(int maxCount) {
    // Implementation would handle file count management
}

void JsonLogger::setTrackPerformance(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackPerformance = track;
}

void JsonLogger::setTrackMemory(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackMemory = track;
}

void JsonLogger::setTrackCpu(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackCpu = track;
}

uint64_t JsonLogger::getNextCorrelationId() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return ++pImpl->lastCorrelationId;
}

void JsonLogger::setCorrelationId(uint64_t id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->correlationId = id;
}

void JsonLogger::writeToJsonFile(const LogEntry& entry) {
    json logJson = logEntryToJson(entry);
    pImpl->fileOut << logJson.dump(2) << std::endl;
    pImpl->fileOut.flush();
}

void JsonLogger::rotateFiles() {
    // Implementation of file rotation
}

void JsonLogger::cleanupOldFiles() {
    // Implementation of old file cleanup
}

void JsonLogger::updatePerformanceMetrics(LogEntry& entry) {
    // Update performance metrics
}

void JsonLogger::updateMemoryUsage(LogEntry& entry) {
    // Update memory usage
}

void JsonLogger::updateCpuUsage(LogEntry& entry) {
    // Update CPU usage
}

std::string JsonLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

json JsonLogger::logEntryToJson(const LogEntry& entry) const {
    json j;
    
    // Convert time_point to timestamp
    auto timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
    
    j = {
        {"level", static_cast<int>(entry.level)},
        {"message", entry.message},
        {"file", entry.file},
        {"line", entry.line},
        {"function", entry.function},
        {"timestamp", ss.str()},
        {"cpu_usage", entry.cpuUsage},
        {"memory_usage", entry.memoryUsage},
        {"processing_time_ns", entry.processingTime.count()},
        {"elapsed_since_last_ns", entry.elapsedSinceLast.count()},
        {"module", entry.module},
        {"severity", entry.severity},
        {"memory_percent", entry.memoryPercent},
        {"network_bytes", entry.networkBytes},
        {"disk_bytes", entry.diskBytes},
        {"trace_id", entry.traceId},
        {"span_id", entry.spanId},
        {"is_performance_event", entry.isPerformanceEvent}
    };
    
    return j;
}

std::string JsonLogger::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string JsonLogger::getCurrentProcessId() const {
    std::stringstream ss;
    ss << getpid();
    return ss.str();
}

uint64_t JsonLogger::getCurrentThreadId() const {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}
}