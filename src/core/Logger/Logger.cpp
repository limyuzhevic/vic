#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <memory>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <zlib.h>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace nlm {

// Simple JSON-like formatter for structured logging
class SimpleJSONFormatter {
public:
    static std::string format(const LogEntry& entry) {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"level\": \"" << levelToString(entry.level) << "\",\n";
        oss << "  \"timestamp\": \"" << timestampToString(entry.timestamp) << "\",\n";
        oss << "  \"message\": \"" << escapeJsonString(entry.message) << "\",\n";
        oss << "  \"file\": \"" << entry.file << "\",\n";
        oss << "  \"line\": " << entry.line << ",\n";
        oss << "  \"function\": \"" << entry.function << "\",\n";
        oss << "  \"thread_id\": \"" << entry.threadId << "\",\n";
        oss << "  \"process_id\": \"" << entry.processId << "\",\n";
        oss << "  \"elapsed_time\": " << entry.elapsedTime << ",\n";
        oss << "  \"memory_usage\": " << entry.memoryUsage << ",\n";
        
        if (!entry.metadata.empty()) {
            oss << "  \"metadata\": {\n";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) oss << ",\n";
                oss << "    \"" << escapeJsonString(pair.first) << "\": \"" << escapeJsonString(pair.second) << "\"";
                first = false;
            }
            oss << "\n  },\n";
        }
        
        if (!entry.context.empty()) {
            oss << "  \"context\": {\n";
            bool first = true;
            for (const auto& pair : entry.context) {
                if (!first) oss << ",\n";
                oss << "    \"" << escapeJsonString(pair.first) << "\": \"" << escapeJsonString(pair.second) << "\"";
                first = false;
            }
            oss << "\n  },\n";
        }
        
        oss << "}";
        return oss.str();
    }
    
private:
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Trace: return "TRACE";
            default: return "UNKNOWN";
        }
    }
    
    static std::string timestampToString(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%fZ");
        return ss.str();
    }
    
    static std::string escapeJsonString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                default:
                    if (c < ' ' || c > '~') {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }
};

// Async logger with queue support
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000, size_t maxThreads = 4);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    void start();
    void stop();
    bool isRunning() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File logger with rotation and compression
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    void setRotationSize(size_t maxFileSize);
    void setRotationCount(size_t maxFiles);
    void setCompressionEnabled(bool enabled);
    void enableIncrementalBackup(bool enabled);
    void setErrorEscalation(bool enabled);
    void setEscalationThreshold(LogLevel threshold);
    void enablePerformanceTracking(bool enabled);
    void setPerformanceLogInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance tracking
class PerformanceTracker {
public:
    PerformanceTracker();
    ~PerformanceTracker();
    
    void recordTime(const std::string& operation, double duration);
    void recordMemoryUsage(uint64_t memoryBytes);
    double getAverageTime(const std::string& operation) const;
    uint64_t getAverageMemory() const;
    bool isEnabled() const;
    void enable(bool enabled);
    void setLogInterval(double interval);
    double getLogInterval() const;
    std::string getPerformanceSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration system
class LogConfig {
public:
    LogConfig();
    ~LogConfig();
    
    void setComponentLevel(const std::string& component, LogLevel level);
    LogLevel getComponentLevel(const std::string& component) const;
    bool isFilteringEnabled() const;
    void enableFiltering(bool enabled);
    bool isMaskingEnabled() const;
    void enableMasking(bool enabled);
    void setMaskedFields(const std::vector<std::string>& fields);
    const std::string& getMaskPattern(const std::string& field) const;
    bool isAsyncEnabled() const;
    void enableAsync(bool enabled);
    bool isEscalationEnabled() const;
    void enableEscalation(bool enabled);
    LogLevel getEscalationThreshold() const;
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logger implementation
class Logger::Impl {
public:
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::map<std::string, LogLevel> componentLevels;
    bool filteringEnabled = false;
    bool maskingEnabled = false;
    std::vector<std::string> maskedFields;
    std::map<std::string, double> performanceMetrics;
    uint64_t memoryUsage = 0;
    std::chrono::system_clock::time_point lastPerfLog;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastPerfLog = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(std::move(logger));
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function,
                 const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.processId = std::to_string(getpid());
    entry.metadata = metadata;
    
    // Apply masking if enabled
    if (pImpl->maskingEnabled) {
        maskSensitiveData(entry.metadata);
    }
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                   const std::string& function,
                   const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Debug, message, file, line, function, metadata);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                  const std::string& function,
                  const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Info, message, file, line, function, metadata);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function,
                     const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Warning, message, file, line, function, metadata);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                   const std::string& function,
                   const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Error, message, file, line, function, metadata);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function,
                      const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Critical, message, file, line, function, metadata);
}

void Logger::logWithContext(LogLevel level, const std::string& message,
                            const std::map<std::string, std::string>& context,
                            const std::string& file, int line,
                            const std::string& function) {
    std::map<std::string, std::string> metadata = context;
    metadata["context_source"] = "log_with_context";
    log(level, message, file, line, function, metadata);
}

void Logger::logWithPerformance(LogLevel level, const std::string& message,
                                const std::string& operation,
                                const std::string& file, int line,
                                const std::string& function) {
    double avgTime = getAveragePerformanceTime(operation);
    uint64_t avgMemory = getAverageMemoryUsage();
    
    std::map<std::string, std::string> metadata;
    metadata["performance_operation"] = operation;
    metadata["performance_time_ms"] = std::to_string(avgTime * 1000.0);
    metadata["performance_memory_bytes"] = std::to_string(avgMemory);
    
    log(level, message, file, line, function, metadata);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

void Logger::maskSensitiveData(std::map<std::string, std::string>& metadata) const {
    for (auto& pair : metadata) {
        for (const auto& maskedField : pImpl->maskedFields) {
            if (pair.first.find(maskedField) != std::string::npos) {
                pair.second = "[REDACTED]";
            }
        }
    }
}

double Logger::getAveragePerformanceTime(const std::string& operation) const {
    auto it = pImpl->performanceMetrics.find("time_" + operation);
    if (it != pImpl->performanceMetrics.end()) {
        return it->second;
    }
    return 0.0;
}

uint64_t Logger::getAverageMemoryUsage() const {
    return pImpl->memoryUsage;
}

// Helper functions
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Trace: return "TRACE";
        default: return "UNKNOWN";
    }
}

} // namespace nlm