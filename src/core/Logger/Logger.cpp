#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <ctime>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool useThreadIds = true;
    bool useFileLocations = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced logging features
    std::vector<std::string> tagFilters;      // Only log messages with these tags
    std::vector<LogLevel> levelFilters;       // Only log messages at or above these levels
    std::unordered_map<std::string, LogLevel> categoryLevels; // Different levels for different categories
    
    // Performance: buffer for batch writing
    std::vector<std::string> logBuffer;
    size_t bufferSize = 100;
    bool asyncEnabled = false;
    std::mutex bufferMutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default filters
    pImpl->levelFilters = {LogLevel::Debug, LogLevel::Info, LogLevel::Warning, LogLevel::Error, LogLevel::Critical};
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->levelFilters = {LogLevel::Debug, LogLevel::Info, LogLevel::Warning, LogLevel::Error, LogLevel::Critical};
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check filters
    if (!shouldLog(entry)) {
        return;
    }
    
    // Generate enhanced log message
    std::string message = formatLogEntry(entry);
    
    // Handle buffering for performance
    if (pImpl->asyncEnabled) {
        std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
        pImpl->logBuffer.push_back(message);
        if (pImpl->logBuffer.size() >= pImpl->bufferSize) {
            flushBuffer();
        }
    } else {
        // Immediate output
        outputLogMessage(message);
    }
}

void ConsoleLogger::flushBuffer() {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    for (const auto& msg : pImpl->logBuffer) {
        outputLogMessage(msg);
    }
    pImpl->logBuffer.clear();
}

void ConsoleLogger::outputLogMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Console output
    if (pImpl->useColors) {
        // Extract level from message for coloring (simplified)
        if (message.find("[ERROR]") != std::string::npos || message.find("[CRIT]") != std::string::npos) {
            std::cout << "\033[31m"; // Red
        } else if (message.find("[WARN]") != std::string::npos) {
            std::cout << "\033[33m"; // Yellow
        } else if (message.find("[INFO]") != std::string::npos) {
            std::cout << "\033[32m"; // Green
        } else if (message.find("[DEBUG]") != std::string::npos) {
            std::cout << "\033[36m"; // Cyan
        }
        std::cout << message << "\033[0m\n";
    } else {
        std::cout << message << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << message << "\n";
        pImpl->fileOut.flush();
    }
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check global level
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Check category-level filters
    std::string category = extractCategory(entry.message);
    auto it = pImpl->categoryLevels.find(category);
    if (it != pImpl->categoryLevels.end() && entry.level < it->second) {
        return false;
    }
    
    // Check level filters
    bool levelMatches = std::any_of(pImpl->levelFilters.begin(), pImpl->levelFilters.end(),
        [entry](LogLevel filter) { return entry.level >= filter; });
    if (!levelMatches) {
        return false;
    }
    
    // Check tag filters
    std::vector<std::string> tags = extractTags(entry.message);
    if (!pImpl->tagFilters.empty()) {
        bool tagMatches = false;
        for (const auto& tag : tags) {
            if (std::find(pImpl->tagFilters.begin(), pImpl->tagFilters.end(), tag) != pImpl->tagFilters.end()) {
                tagMatches = true;
                break;
            }
        }
        if (!tagMatches) {
            return false;
        }
    }
    
    return true;
}

std::string ConsoleLogger::formatLogEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Add timestamp if enabled
    if (pImpl->useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str());
        oss << " ";
    }
    
    // Add thread ID if enabled
    if (pImpl->useThreadIds) {
        oss << "[" << std::this_thread::get_id() << "] ";
    }
    
    // Add level
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add category/file if enabled
    if (pImpl->useFileLocations && !entry.file.empty()) {
        // Extract filename from path
        size_t pos = entry.file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? entry.file.substr(pos + 1) : entry.file;
        oss << "[" << filename;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        oss << "] ";
    }
    
    // Add function name if enabled
    if (!entry.function.empty()) {
        oss << "[" << entry.function << "] ";
    }
    
    // Add message
    oss << entry.message;
    
    return oss.str();
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

void ConsoleLogger::setUseTimestamps(bool useTimestamps) {
    pImpl->useTimestamps = useTimestamps;
}

bool ConsoleLogger::getUseTimestamps() const {
    return pImpl->useTimestamps;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

std::string ConsoleLogger::getTimestampFormat() const {
    return pImpl->timestampFormat;
}

void ConsoleLogger::setUseThreadIds(bool useThreadIds) {
    pImpl->useThreadIds = useThreadIds;
}

bool ConsoleLogger::getUseThreadIds() const {
    return pImpl->useThreadIds;
}

void ConsoleLogger::setUseFileLocations(bool useFileLocations) {
    pImpl->useFileLocations = useFileLocations;
}

bool ConsoleLogger::getUseFileLocations() const {
    return pImpl->useFileLocations;
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

void ConsoleLogger::setTagFilters(const std::vector<std::string>& tags) {
    pImpl->tagFilters = tags;
}

std::vector<std::string> ConsoleLogger::getTagFilters() const {
    return pImpl->tagFilters;
}

void ConsoleLogger::setLevelFilters(const std::vector<LogLevel>& levels) {
    pImpl->levelFilters = levels;
}

std::vector<LogLevel> ConsoleLogger::getLevelFilters() const {
    return pImpl->levelFilters;
}

void ConsoleLogger::setCategoryLevel(const std::string& category, LogLevel level) {
    pImpl->categoryLevels[category] = level;
}

LogLevel ConsoleLogger::getCategoryLevel(const std::string& category) const {
    auto it = pImpl->categoryLevels.find(category);
    return (it != pImpl->categoryLevels.end()) ? it->second : LogLevel::Info;
}

void ConsoleLogger::setAsyncEnabled(bool enabled, size_t bufferSize) {
    pImpl->asyncEnabled = enabled;
    pImpl->bufferSize = bufferSize;
    if (enabled) {
        pImpl->logBuffer.reserve(bufferSize);
    }
}

bool ConsoleLogger::isAsyncEnabled() const {
    return pImpl->asyncEnabled;
}

size_t ConsoleLogger::getBufferSize() const {
    return pImpl->bufferSize;
}

void ConsoleLogger::flush() {
    if (pImpl->asyncEnabled) {
        flushBuffer();
    }
}

// Helper methods
std::string ConsoleLogger::extractCategory(const std::string& message) const {
    // Simple category extraction: first word or word after colon
    size_t pos = message.find(':');
    if (pos != std::string::npos) {
        std::string afterColon = message.substr(pos + 1);
        // Find first word
        pos = afterColon.find_first_not_of(" \t\n\r");
        if (pos != std::string::npos) {
            size_t end = afterColon.find_first_of(" \t\n\r");
            return (end != std::string::npos) ? afterColon.substr(0, end) : afterColon;
        }
    }
    return "general";
}

std::vector<std::string> ConsoleLogger::extractTags(const std::string& message) const {
    std::vector<std::string> tags;
    size_t pos = 0;
    
    while (pos < message.size()) {
        // Find next word
        while (pos < message.size() && std::isspace(message[pos])) {
            pos++;
        }
        
        if (pos >= message.size()) break;
        
        size_t start = pos;
        while (pos < message.size() && !std::isspace(message[pos])) {
            pos++;
        }
        
        std::string word = message.substr(start, pos - start);
        
        // Check if word looks like a tag (starts with # or contains :)
        if (!word.empty() && (word[0] == '#' || word.find(':') != std::string::npos)) {
            // Remove leading # if present
            if (word[0] == '#') {
                word = word.substr(1);
            }
            tags.push_back(word);
        }
    }
    
    return tags;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool enabled = true;
    std::string name;
    std::chrono::system_clock::time_point startTime;
    
    // Performance statistics
    struct LogStats {
        size_t totalMessages = 0;
        size_t messagesByLevel[5] = {0}; // Debug, Info, Warning, Error, Critical
        std::chrono::steady_clock::time_point lastLogTime;
    } stats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
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
                const std::string& file, int line, const std::string& function) {
    if (!pImpl->enabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Update statistics
    pImpl->stats.totalMessages++;
    pImpl->stats.messagesByLevel[static_cast<size_t>(level)]++;
    pImpl->stats.lastLogTime = std::chrono::steady_clock::now();
    
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

// Helper functions for enhanced logging
void enableStructuredLogging(bool enable) {
    Logger::getGlobal().info("Structured logging " + std::string(enable ? "enabled" : "disabled"));
}

void setLogFile(const std::string& filepath) {
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setOutputFile(filepath);
    Logger::getGlobal().addLogger(consoleLogger);
}

std::string getLogStatistics() {
    std::ostringstream oss;
    oss << "Log Statistics:\n";
    oss << "  Total messages logged: " << Logger::getGlobal().pImpl->stats.totalMessages << "\n";
    oss << "  Debug messages: " << Logger::getGlobal().pImpl->stats.messagesByLevel[0] << "\n";
    oss << "  Info messages: " << Logger::getGlobal().pImpl->stats.messagesByLevel[1] << "\n";
    oss << "  Warning messages: " << Logger::getGlobal().pImpl->stats.messagesByLevel[2] << "\n";
    oss << "  Error messages: " << Logger::getGlobal().pImpl->stats.messagesByLevel[3] << "\n";
    oss << "  Critical messages: " << Logger::getGlobal().pImpl->stats.messagesByLevel[4] << "\n";
    return oss.str();
}

} // namespace nlm