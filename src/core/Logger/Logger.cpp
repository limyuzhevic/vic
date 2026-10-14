#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cmath>
#include <algorithm>

namespace nlm {

// Helper functions for log file operations

// Get the output file path for a ConsoleLogger
inline std::string getOutputFilePath(const ConsoleLogger& logger) {
    return logger.pImpl->currentFilePath;
}

// Write a header to the log file
void writeLogHeader(std::ofstream& file) {
    if (file.is_open()) {
        file << "=== NLM Log File ===\n";
        file << "Format: [LEVEL] YYYY-MM-DD HH:MM:SS Message\n";
        file << "Levels: DEBUG=0, INFO=1, WARNING=2, ERROR=3, CRITICAL=4\n\n";
        file.flush();
    }
}

// Filter logs by level
bool shouldLogLevel(LogLevel entryLevel, LogLevel minLevel) {
    return entryLevel >= minLevel;
}

// Performance monitoring implementation

void recordLogPerformance(double durationMs) {
    // Record performance data for analysis
    // In a real implementation, this would log to a performance monitoring system
    NLM_LOG_DEBUG("Log performance: " + std::to_string(durationMs) + "ms");
}

// Enhanced ConsoleLogger implementation with log rotation

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB default
    int maxFiles = 5;
    size_t logCount = 0;
    double totalLogTime = 0.0;
    std::string currentFilePath;
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
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Check file rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.seekp(0, std::ios::end);
        size_t currentSize = pImpl->fileOut.tellp();
        if (currentSize >= pImpl->maxFileSize) {
            rotateLogs();
        }
    }
    
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
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    recordLogPerformance(duration);
    
    pImpl->logCount++;
    pImpl->totalLogTime += duration;
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
    pImpl->currentFilePath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open() && pImpl->fileOut.tellp() == 0) {
        writeLogHeader(pImpl->fileOut);
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->currentFilePath.clear();
    }
}

// Get current output file path
std::string ConsoleLogger::getOutputFilePath() const {
    return pImpl->currentFilePath;
}

void ConsoleLogger::setMaxFileSize(size_t maxBytes) {
    pImpl->maxFileSize = maxBytes;
}

size_t ConsoleLogger::getMaxFileSize() const {
    return pImpl->maxFileSize;
}

void ConsoleLogger::setMaxFiles(int maxFiles) {
    pImpl->maxFiles = maxFiles;
}

int ConsoleLogger::getMaxFiles() const {
    return pImpl->maxFiles;
}

void ConsoleLogger::rotateLogs() {
    if (!pImpl->fileOut.is_open() || pImpl->currentFilePath.empty()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Remove old files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string oldPath = pImpl->currentFilePath;
        if (i > 0) {
            size_t dotPos = oldPath.find_last_of('.');
            if (dotPos != std::string::npos) {
                oldPath.replace(dotPos, 0, "_" + std::to_string(i));
            } else {
                oldPath += "_" + std::to_string(i);
            }
        }
        
        std::remove(oldPath.c_str());
    }
    
    // Rename current file to backup
    size_t dotPos = pImpl->currentFilePath.find_last_of('.');
    std::string backupPath = pImpl->currentFilePath;
    if (dotPos != std::string::npos) {
        backupPath.replace(dotPos, 0, "_1");
    } else {
        backupPath += "_1";
    }
    
    std::rename(pImpl->currentFilePath.c_str(), backupPath.c_str());
    
    // Reopen file for writing
    pImpl->fileOut.open(pImpl->currentFilePath, std::ios::trunc);
    writeLogHeader(pImpl->fileOut);
}

size_t ConsoleLogger::getLogCount() const {
    return pImpl->logCount;
}

double ConsoleLogger::getAverageLogTime() const {
    if (pImpl->logCount == 0) return 0.0;
    return pImpl->totalLogTime / pImpl->logCount;
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

// Enhanced Logger implementation with performance tracking

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogLevel maxLogLevel = LogLevel::Critical;
    bool enablePerformanceStats = true;
    size_t totalLogCount = 0;
    double totalLogTime = 0.0;
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
    pImpl->maxLogLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    if (level > pImpl->maxLogLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->totalLogCount++;
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

size_t Logger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t total = pImpl->totalLogCount;
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            total += consoleLogger->getLogCount();
        }
    }
    return total;
}

double Logger::getGlobalAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    double totalTime = pImpl->totalLogTime;
    size_t totalLogs = pImpl->totalLogCount;
    
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            totalTime += consoleLogger->getAverageLogTime() * consoleLogger->getLogCount();
            totalLogs += consoleLogger->getLogCount();
        }
    }
    
    if (totalLogs == 0) return 0.0;
    return totalTime / totalLogs;
}

void Logger::setGlobalMaxLogLevel(LogLevel level) {
    getGlobal().setLevel(level);
}

void Logger::setEnablePerformanceStats(bool enable) {
    pImpl->enablePerformanceStats = enable;
}

bool Logger::getEnablePerformanceStats() const {
    return pImpl->enablePerformanceStats;
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->closeOutputFile();
            // Reopen to ensure clean state
            if (!consoleLogger->getUseColors()) {
                consoleLogger->setOutputFile(consoleLogger->getOutputFilePath());
            }
        }
    }
}

// Note: getOutputFilePath() method would need to be added to ConsoleLogger class
// For now, this is a placeholder for demonstration

} // namespace nlm