#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    // Category filtering
    std::unordered_map<LogCategory, bool> categoryEnabled;
    // Statistics
    LogStatistics stats;
    // Buffering
    std::vector<LogEntry> logBuffer;
    size_t maxBufferSize = 1000;
    // Formatters
    std::function<std::string(const LogEntry&)> messageFormatter;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters
    for (int i = 0; i < static_cast<int>(LogCategory::Debug); ++i) {
        pImpl->categoryEnabled[static_cast<LogCategory>(i)] = true;
    }
    pImpl->categoryEnabled[LogCategory::Debug] = false; // Debug disabled by default
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Initialize category filters
    for (int i = 0; i < static_cast<int>(LogCategory::Debug); ++i) {
        pImpl->categoryEnabled[static_cast<LogCategory>(i)] = true;
    }
    pImpl->categoryEnabled[LogCategory::Debug] = false; // Debug disabled by default
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level || !pImpl->categoryEnabled[entry.category]) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update statistics
    auto now = std::chrono::system_clock::now();
    if (pImpl->stats.firstLog.time_since_epoch().count() == 0) {
        pImpl->stats.firstLog = now;
    }
    pImpl->stats.lastLog = now;
    pImpl->stats.totalCount++;
    
    switch (entry.level) {
        case LogLevel::Debug: pImpl->stats.debugCount++; break;
        case LogLevel::Info: pImpl->stats.infoCount++; break;
        case LogLevel::Warning: pImpl->stats.warningCount++; break;
        case LogLevel::Error: pImpl->stats.errorCount++; break;
        case LogLevel::Critical: pImpl->stats.criticalCount++; break;
    }
    
    if (entry.duration) {
        pImpl->stats.totalDuration += *entry.duration;
    }
    
    // Format message
    std::ostringstream oss;
    
    // Apply custom formatter if set, otherwise use default
    std::string formattedMessage;
    if (pImpl->messageFormatter) {
        formattedMessage = pImpl->messageFormatter(entry);
    } else {
        // Default formatting
        oss << "[" << levelToString(entry.level) << "] ";
        oss << formatTimestamp(entry.timestamp) << " ";
        oss << entry.message;
        
        // Add structured data
        if (!entry.metadata.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "}";
        }
        
        if (!entry.file.empty()) {
            oss << " [" << entry.file;
            if (entry.line > 0) oss << ":" << entry.line;
            oss << "]";
        }
        
        formattedMessage = oss.str();
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->fileOut.flush();
    }
    
    // Buffer management
    pImpl->logBuffer.push_back(entry);
    if (pImpl->logBuffer.size() > pImpl->maxBufferSize) {
        pImpl->logBuffer.erase(pImpl->logBuffer.begin());
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setCategoryFilter(LogCategory category, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryEnabled[category] = enabled;
}

bool ConsoleLogger::isCategoryEnabled(LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->categoryEnabled.find(category);
    return it != pImpl->categoryEnabled.end() ? it->second : false;
}

LogStatistics ConsoleLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ConsoleLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.reset();
}

void ConsoleLogger::setMessageFormatter(std::function<std::string(const LogEntry&)> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->messageFormatter = std::move(formatter);
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timestampFormat = format;
}

void ConsoleLogger::setMaxBufferSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxBufferSize = maxSize;
}

size_t ConsoleLogger::getMaxBufferSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxBufferSize;
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

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    if (pImpl->timestampFormat == "%Y-%m-%d %H:%M:%S") {
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    } else {
        oss << std::put_time(&tm_buf, "%H:%M:%S");
    }
    return oss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    // Category filtering for Logger itself
    std::unordered_map<LogCategory, bool> categoryFilters;
    // Global statistics
    LogStatistics globalStats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters for Logger
    for (int i = 0; i < static_cast<int>(LogCategory::Debug); ++i) {
        pImpl->categoryFilters[static_cast<LogCategory>(i)] = true;
    }
    pImpl->categoryFilters[LogCategory::Debug] = false; // Debug disabled by default
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

void Logger::setCategoryFilter(LogCategory category, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilters[category] = enabled;
}

bool Logger::isCategoryEnabled(LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->categoryFilters.find(category);
    return it != pImpl->categoryFilters.end() ? it->second : false;
}

void Logger::log(LogLevel level, const std::string& message, LogCategory category,
                 const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Update global statistics
    auto now = entry.timestamp;
    if (pImpl->globalStats.firstLog.time_since_epoch().count() == 0) {
        pImpl->globalStats.firstLog = now;
    }
    pImpl->globalStats.lastLog = now;
    pImpl->globalStats.totalCount++;
    
    switch (level) {
        case LogLevel::Debug: pImpl->globalStats.debugCount++; break;
        case LogLevel::Info: pImpl->globalStats.infoCount++; break;
        case LogLevel::Warning: pImpl->globalStats.warningCount++; break;
        case LogLevel::Error: pImpl->globalStats.errorCount++; break;
        case LogLevel::Critical: pImpl->globalStats.criticalCount++; break;
    }
    
    // Pass entry to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, LogCategory::Debug, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, LogCategory::All, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, LogCategory::All, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, LogCategory::All, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, LogCategory::All, file, line, function);
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

LogStatistics Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalStats;
}

void Logger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalStats.reset();
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), LogCategory::All, file_, line_, function_);
}

} // namespace nlm