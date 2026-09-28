#pragma once

#include "AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <ctime>

namespace nlm {

struct StructuredLogger::Impl {
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
    std::chrono::system_clock::time_point lastUpdate;
};

StructuredLogger::StructuredLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->processId = getCurrentProcessId();
    pImpl->lastUpdate = std::chrono::system_clock::now();
}

StructuredLogger::~StructuredLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void StructuredLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update performance metrics if enabled
    if (pImpl->trackPerformance) {
        // Update performance metrics
    }
    if (pImpl->trackMemory) {
        // Update memory usage
    }
    if (pImpl->trackCpu) {
        // Update CPU usage
    }
    
    // Generate correlation ID if not set
    uint64_t correlationId = entry.correlationId;
    if (correlationId == 0) {
        correlationId = ++pImpl->lastCorrelationId;
    }
    
    // Output based on format
    if (pImpl->format == "json") {
        json logJson = logEntryToJson(entry);
        logJson["correlation_id"] = correlationId;
        logJson["thread_id"] = getCurrentThreadId();
        logJson["process_id"] = pImpl->processId;
        
        std::cout << logJson.dump(2) << std::endl;
    } else {
        // Text output
        std::cout << "[" << levelToString(entry.level) << "] "
                  << entry.message << std::endl;
    }
    
    // Write to file if set
    if (pImpl->fileOut.is_open()) {
        std::ofstream file(pImpl->fileOut);
        if (file.is_open()) {
            json logJson = logEntryToJson(entry);
            logJson["correlation_id"] = correlationId;
            file << logJson.dump(2) << std::endl;
            file.flush();
        }
    }
}

void StructuredLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel StructuredLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void StructuredLogger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

void StructuredLogger::setOutputFile(const std::string& filepath, bool rotate) {
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

void StructuredLogger::setMaxFileSize(size_t maxBytes) {
    // Implementation would handle file size checking
}

void StructuredLogger::setMaxFiles(int maxCount) {
    // Implementation would handle file count management
}

void StructuredLogger::setTrackPerformance(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackPerformance = track;
}

void StructuredLogger::setTrackMemory(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackMemory = track;
}

void StructuredLogger::setTrackCpu(bool track) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trackCpu = track;
}

uint64_t StructuredLogger::getNextCorrelationId() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return ++pImpl->lastCorrelationId;
}

void StructuredLogger::setCorrelationId(uint64_t id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->correlationId = id;
}

void StructuredLogger::logEvent(const StructuredLogEntry& event) {
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = event.eventType;
    entry.module = event.module;
    entry.addTag("entity_id", std::to_string(event.entityId));
    entry.addTag("entity_type", event.entityType);
    
    // Add data to message
    std::stringstream ss;
    ss << event.eventType << ": ";
    if (!event.data.empty()) {
        ss << event.data.dump();
    }
    if (event.value != 0.0) {
        ss << " (value: " << event.value << " " << event.unit << ")";
    }
    entry.message = ss.str();
    
    log(entry);
}

void StructuredLogger::logNeuronEvent(NeuronId neuronId, const std::string& eventType, const json& data) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "neuron";
    entry.entityId = neuronId;
    entry.entityType = "neuron";
    entry.data = data;
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

void StructuredLogger::logSynapseEvent(SynapseId synapseId, const std::string& eventType, const json& data) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "synapse";
    entry.entityId = synapseId;
    entry.entityType = "synapse";
    entry.data = data;
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

void StructuredLogger::logMemoryEvent(const std::string& eventType, uint64_t bytes, const std::string& unit) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "memory";
    entry.entityId = 0;
    entry.entityType = "system";
    entry.value = static_cast<double>(bytes);
    entry.unit = unit;
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

void StructuredLogger::logPerformanceEvent(const std::string& eventType, double value, const std::string& unit) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "performance";
    entry.entityId = 0;
    entry.entityType = "system";
    entry.value = value;
    entry.unit = unit;
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

void StructuredLogger::logNetworkEvent(const std::string& eventType, uint64_t bytes, const std::string& direction) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "network";
    entry.entityId = 0;
    entry.entityType = "system";
    entry.value = static_cast<double>(bytes);
    entry.unit = direction + "_bytes";
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

void StructuredLogger::logDiskEvent(const std::string& eventType, uint64_t bytes, const std::string& operation) {
    StructuredLogEntry entry;
    entry.eventType = eventType;
    entry.module = "disk";
    entry.entityId = 0;
    entry.entityType = "system";
    entry.value = static_cast<double>(bytes);
    entry.unit = operation + "_bytes";
    entry.timestamp = std::chrono::system_clock::now();
    
    logEvent(entry);
}

json StructuredLogger::logEntryToJson(const LogEntry& entry) const {
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

std::string StructuredLogger::getCurrentProcessId() const {
    std::stringstream ss;
    ss << getpid();
    return ss.str();
}

uint64_t StructuredLogger::getCurrentThreadId() const {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}
}