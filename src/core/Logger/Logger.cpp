// Enhanced Logger Implementation with Advanced Features

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    bool useFile = false;
    std::string logFile;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t fileRotationCount = 5;
    std::ofstream fileOut;
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotation;
    std::mutex mutex;
    
    // Log level filtering
    bool enableLevelDebug = true;
    bool enableLevelInfo = true;
    bool enableLevelWarning = true;
    bool enableLevelError = true;
    bool enableLevelCritical = true;
    
    // Performance statistics
    uint64_t totalLogs = 0;
    uint64_t debugLogs = 0;
    uint64_t infoLogs = 0;
    uint64_t warningLogs = 0;
    uint64_t errorLogs = 0;
    uint64_t criticalLogs = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize defaults
    pImpl->useColors = true;
    pImpl->useTimestamps = true;
    pImpl->useFile = false;
    pImpl->fileRotationCount = 5;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->useColors = true;
    pImpl->useTimestamps = true;
    pImpl->useFile = false;
    pImpl->fileRotationCount = 5;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check level filtering
    if (!shouldLogLevel(entry.level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "]";
    
    // Add timestamp if enabled
    if (pImpl->useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << " [" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "]";
    }
    
    // Add file and line info if available
    if (!entry.file.empty() && entry.line > 0) {
        oss << " [" << entry.file << ":" << entry.line << "]";
    }
    
    // Add function name if available
    if (!entry.function.empty()) {
        oss << " [" << entry.function << "]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation if enabled
    if (pImpl->useFile && pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        pImpl->fileOut.flush();
        
        // Check if file needs rotation
        checkFileRotation();
    }
    
    // Update statistics
    pImpl->totalLogs++;
    switch (entry.level) {
        case LogLevel::Debug: pImpl->debugLogs++; break;
        case LogLevel::Info: pImpl->infoLogs++; break;
        case LogLevel::Warning: pImpl->warningLogs++; break;
        case LogLevel::Error: pImpl->errorLogs++; break;
        case LogLevel::Critical: pImpl->criticalLogs++; break;
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

void ConsoleLogger::setFileOutput(const std::string& filepath, size_t maxSizeMB, size_t rotationCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->logFile = filepath;
    pImpl->maxFileSize = maxSizeMB * 1024 * 1024;
    pImpl->fileRotationCount = rotationCount;
    pImpl->useFile = true;
    pImpl->currentFileSize = 0;
    pImpl->lastRotation = std::chrono::system_clock::now();
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
        pImpl->useFile = false;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->useFile = false;
        pImpl->logFile.clear();
    }
}

void ConsoleLogger::setLogLevelFilter(bool debug, bool info, bool warning, bool error, bool critical) {
    pImpl->enableLevelDebug = debug;
    pImpl->enableLevelInfo = info;
    pImpl->enableLevelWarning = warning;
    pImpl->enableLevelError = error;
    pImpl->enableLevelCritical = critical;
}

bool ConsoleLogger::shouldLogLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return pImpl->enableLevelDebug;
        case LogLevel::Info: return pImpl->enableLevelInfo;
        case LogLevel::Warning: return pImpl->enableLevelWarning;
        case LogLevel::Error: return pImpl->enableLevelError;
        case LogLevel::Critical: return pImpl->enableLevelCritical;
        default: return true;
    }
}

void ConsoleLogger::setPerformanceTracking(bool enable) {
    // Performance tracking is always enabled now
}

struct LogStats {
    uint64_t totalLogs = 0;
    uint64_t debugLogs = 0;
    uint64_t infoLogs = 0;
    uint64_t warningLogs = 0;
    uint64_t errorLogs = 0;
    uint64_t criticalLogs = 0;
};

LogStats ConsoleLogger::getStatistics() const {
    LogStats stats;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    stats.totalLogs = pImpl->totalLogs;
    stats.debugLogs = pImpl->debugLogs;
    stats.infoLogs = pImpl->infoLogs;
    stats.warningLogs = pImpl->warningLogs;
    stats.errorLogs = pImpl->errorLogs;
    stats.criticalLogs = pImpl->criticalLogs;
    return stats;
}

std::string ConsoleLogger::getLogFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->fileOut.is_open()) {
        return "0 bytes";
    }
    
    auto fileSize = pImpl->fileOut.tellp();
    if (fileSize == -1) {
        return "unknown";
    }
    
    size_t bytes = static_cast<size_t>(fileSize);
    if (bytes < 1024) {
        return std::to_string(bytes) + " bytes";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    }
}

void ConsoleLogger::checkFileRotation() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->useFile || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Check file size
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotateFile();
    }
    
    // Check if it's time to rotate based on time (optional)
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation).count();
    if (age >= 24) { // Rotate daily if file not already rotated by size
        rotateFile();
    }
}

void ConsoleLogger::rotateFile() {
    if (!pImpl->useFile || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rename existing files
    for (int i = pImpl->fileRotationCount - 1; i >= 0; --i) {
        std::string oldName = (i == pImpl->fileRotationCount - 1) ? pImpl->logFile : pImpl->logFile + "." + std::to_string(i);
        std::string newName = pImpl->logFile + "." + std::to_string(i + 1);
        
        if (std::rename(oldName.c_str(), newName.c_str()) != 0) {
            // File doesn't exist, that's okay
        }
    }
    
    // Rename current file to .0
    if (std::rename(pImpl->logFile.c_str(), (pImpl->logFile + ".0").c_str()) == 0) {
        // Create new empty log file
        pImpl->fileOut.open(pImpl->logFile, std::ios::trunc);
        pImpl->currentFileSize = 0;
        pImpl->lastRotation = std::chrono::system_clock::now();
        
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "Failed to reopen log file after rotation: " << pImpl->logFile << std::endl;
            pImpl->useFile = false;
        }
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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::map<LogLevel, uint64_t> levelCounts;
    std::chrono::system_clock::time_point startTime;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
}

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
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Update level statistics
    pImpl->levelCounts[level]++;
    
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

std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm