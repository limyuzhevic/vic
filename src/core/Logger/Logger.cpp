#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <unordered_map>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::string format = "[%level] [%time] [%category] %message";
    bool structuredLogging = false;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Log filtering
    std::unordered_map<std::string, LogLevel> categoryFilters;
    
    // Performance logging
    bool performanceLogging = false;
    
    // Memory tracking
    bool memoryTracking = false;
    
    // Statistics
    size_t totalLogs = 0;
    double totalLogSize = 0.0;
    double lastLogTime = 0.0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->totalLogs++;
    pImpl->totalLogSize += entry.message.size();
    pImpl->lastLogTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    std::string output = formatMessage(entry);
    
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
    
    // Performance logging if enabled
    if (pImpl->performanceLogging) {
        // Log performance metrics to separate category
        LogEntry perfEntry;
        perfEntry.level = LogLevel::Info;
        perfEntry.message = "Performance metric logged";
        perfEntry.category = "performance";
        // In a real implementation, we would log the actual performance data
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::enableStructuredLogging(bool enable) {
    pImpl->structuredLogging = enable;
}

bool ConsoleLogger::isStructuredLoggingEnabled() const {
    return pImpl->structuredLogging;
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

void ConsoleLogger::addLogFilter(const std::string& category, LogLevel level) {
    pImpl->categoryFilters[category] = level;
}

void ConsoleLogger::removeLogFilter(const std::string& category) {
    pImpl->categoryFilters.erase(category);
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Emergency: return "EMERGENCY";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Verbose: return "VERBOSE";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Emergency: return "\033[35m";    // Magenta
        case LogLevel::Alert: return "\033[1;35m";     // Bold Magenta
        case LogLevel::Critical: return "\033[31m";    // Red
        case LogLevel::Error: return "\033[31m";       // Red
        case LogLevel::Warning: return "\033[33m";     // Yellow
        case LogLevel::Notice: return "\033[36m";     // Cyan
        case LogLevel::Info: return "\033[32m";       // Green
        case LogLevel::Debug: return "\033[34m";       // Blue
        case LogLevel::Trace: return "\033[90m";      // Bright Black
        case LogLevel::Verbose: return "\033[37m";    // White
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Replace format placeholders
    std::string format = pImpl->format;
    size_t pos;
    
    while ((pos = format.find("[%level]")) != std::string::npos) {
        format.replace(pos, 8, levelToString(entry.level));
    }
    
    while ((pos = format.find("[%time]")) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        format.replace(pos, 8, oss.str());
        oss.str("");
    }
    
    while ((pos = format.find("[%category]")) != std::string::npos) {
        format.replace(pos, 11, entry.category.empty() ? "" : entry.category);
    }
    
    while ((pos = format.find("%message")) != std::string::npos) {
        format.replace(pos, 9, entry.message);
    }
    
    // Add metadata if structured logging is enabled
    if (pImpl->structuredLogging) {
        std::string metadata = formatMetadata(entry);
        if (!metadata.empty()) {
            format += " " + metadata;
        }
    }
    
    return format;
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level filter
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Check category filter
    if (!entry.category.empty() && pImpl->categoryFilters.count(entry.category)) {
        if (entry.level < pImpl->categoryFilters[entry.category]) {
            return false;
        }
    }
    
    return true;
}

std::string ConsoleLogger::formatMetadata(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    
    for (const auto& pair : entry.metadata) {
        if (!first) oss << ", ";
        oss << pair.first << "=\"" << pair.second << "\"";
        first = false;
    }
    
    for (const auto& pair : entry.numericMetadata) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    
    for (const auto& pair : entry.intMetadata) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    
    oss << "}";
    return oss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Global settings
    LogLevel globalLevel = LogLevel::Info;
    std::string globalFormat = "[%level] [%time] [%category] %message";
    bool structuredLogging = false;
    
    // Log filtering
    std::unordered_map<std::string, LogLevel> categoryFilters;
    
    // Performance monitoring
    bool performanceLogging = false;
    bool memoryTracking = false;
    
    // Statistics
    size_t totalLogs = 0;
    double totalLogSize = 0.0;
    double lastLogTime = 0.0;
    size_t activeLoggers = 0;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    pImpl->activeLoggers++;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->activeLoggers = 0;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFormat = format;
    for (auto& logger : pImpl->loggers) {
        logger->setFormat(format);
    }
}

void Logger::enableStructuredLogging(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->structuredLogging = enable;
    for (auto& logger : pImpl->loggers) {
        logger->enableStructuredLogging(enable);
    }
}

void Logger::addLogFilter(const std::string& category, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilters[category] = level;
    for (auto& logger : pImpl->loggers) {
        logger->addLogFilter(category, level);
    }
}

void Logger::removeLogFilter(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilters.erase(category);
    for (auto& logger : pImpl->loggers) {
        logger->removeLogFilter(category);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->totalLogs++;
    pImpl->totalLogSize += message.size();
    pImpl->lastLogTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                            const std::unordered_map<std::string, std::string>& metadata,
                            const std::string& file, int line,
                            const std::string& function, const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Copy metadata
    entry.metadata = metadata;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->totalLogs++;
    pImpl->totalLogSize += message.size();
    pImpl->lastLogTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                 const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

void Logger::emergency(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& category) {
    log(LogLevel::Emergency, message, file, line, function, category);
}

void Logger::alert(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Alert, message, file, line, function, category);
}

void Logger::notice(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category) {
    log(LogLevel::Notice, message, file, line, function, category);
}

void Logger::trace(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Trace, message, file, line, function, category);
}

void Logger::verbose(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& category) {
    log(LogLevel::Verbose, message, file, line, function, category);
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

void Logger::enablePerformanceLogging(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceLogging = enable;
}

bool Logger::isPerformanceLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceLogging;
}

void Logger::logPerformanceMetric(const std::string& name, double value,
                                 const std::string& category) {
    if (!pImpl->performanceLogging) {
        return;
    }
    
    std::ostringstream oss;
    oss << "Performance metric: " << name << " = " << value;
    
    logWithMetadata(LogLevel::Info, oss.str(), {{}}, "", 0, "", category);
}

void Logger::enableMemoryTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryTracking = enable;
}

bool Logger::isMemoryTrackingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryTracking;
}

void Logger::logMemoryUsage(const std::string& category) {
    if (!pImpl->memoryTracking) {
        return;
    }
    
    // In a real implementation, we would use platform-specific APIs
    // to get memory usage information
    std::ostringstream oss;
    oss << "Memory usage: (not implemented in logging wrapper)";
    
    logWithMetadata(LogLevel::Info, oss.str(), {{}}, "", 0, "", category);
}

Logger::LoggerStats Logger::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LoggerStats stats;
    stats.totalLogs = pImpl->totalLogs;
    stats.totalLogSize = pImpl->totalLogSize;
    stats.lastLogTime = pImpl->lastLogTime;
    stats.activeLoggers = pImpl->activeLoggers;
    
    if (pImpl->totalLogs > 0) {
        stats.averageLogSize = pImpl->totalLogSize / pImpl->totalLogs;
    }
    
    return stats;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line,
                   const std::string& function, const std::string& category)
    : level_(level), file_(file), line_(line), function_(function), category_(category) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_);
}

} // namespace nlm
