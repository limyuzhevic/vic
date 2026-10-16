#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unordered_map>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance metrics collection
    std::unordered_map<std::string, std::vector<PerformanceMetrics>> performanceMetrics;
    std::mutex metricsMutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    // Add timing if present
    if (entry.isTimed) {
        oss << " [" << entry.duration.count() << "s]";
    }
    
    // Add structured data if present
    if (!entry.structuredData.empty()) {
        oss << " {" << entry.structuredData.dump() << "}";
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatStructuredLog(const LogEntry& entry) const {
    nlohmann::json logJson;
    logJson["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
    logJson["level"] = static_cast<int>(entry.level);
    logJson["message"] = entry.message;
    logJson["file"] = entry.file;
    logJson["line"] = entry.line;
    logJson["function"] = entry.function;
    logJson["is_timed"] = entry.isTimed;
    logJson["duration"] = entry.duration.count();
    
    if (!entry.structuredData.empty()) {
        logJson["structured_data"] = entry.structuredData;
    }
    
    return logJson.dump(2);
}

void ConsoleLogger::logStructured(LogLevel level, const std::string& message,
                                  const nlohmann::json& data,
                                  const std::string& file, int line,
                                  const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    entry.structuredData = data;
    log(entry);
}

void ConsoleLogger::logPerformance(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->performanceMetrics[metrics.operation].push_back(metrics);
    
    // Log performance metrics
    std::ostringstream oss;
    oss << "[PERF] " << metrics.operation << " took " 
        << metrics.duration.count() << "s, memory: " << metrics.memoryUsage << " bytes";
    
    for (const auto& [key, value] : metrics.metadata) {
        oss << ", " << key << ": " << value;
    }
    
    std::cout << levelToColor(LogLevel::Info) << oss.str() << "\033[0m\n";
}

std::unique_ptr<ScopedTimer> ConsoleLogger::startTiming(const std::string& operation) {
    return std::make_unique<ScopedTimer>(operation, this);
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Performance metrics and system monitoring
    std::chrono::steady_clock::time_point sessionStart;
    std::unordered_map<std::string, size_t> operationCounts;
    std::unordered_map<std::string, double> totalOperationTimes;
    
    Impl() {
        sessionStart = std::chrono::steady_clock::now();
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
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
                 const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    log(entry);
}

void Logger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update operation statistics
    if (!entry.structuredData.empty()) {
        pImpl->operationCounts[entry.message]++;
        if (entry.isTimed) {
            pImpl->totalOperationTimes[entry.message] += entry.duration.count();
        }
    }
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

void Logger::logStructured(LogLevel level, const std::string& message,
                          const nlohmann::json& data,
                          const std::string& file, int line,
                          const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    entry.structuredData = data;
    log(entry);
}

void Logger::logPerformance(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->logPerformance(metrics);
    }
}

std::unique_ptr<ScopedTimer> Logger::startTiming(const std::string& operation) {
    // Find a logger that supports timing
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        auto timer = logger->startTiming(operation);
        if (timer) {
            return timer;
        }
    }
    return nullptr;
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

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// Performance monitoring helpers

double Logger::getSystemMemoryUsage() {
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        return static_cast<double>(memInfo.totalram) / (1024.0 * 1024.0); // MB
    }
    return 0.0;
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time;
    gmtime_r(&time, &tm_time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace nlm