#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <deque>
#include <condition_variable>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct LogRotationConfig {
    uint64_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    uint32_t maxFiles = 5;
    bool compressOldLogs = false;
};

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Log rotation configuration
    LogRotationConfig rotationConfig;
    uint64_t currentFileSize = 0;
    uint32_t fileIndex = 1;
    std::string baseFilename;
    
    // Log level filtering optimization
    bool debugEnabled = false;
    bool infoEnabled = true;
    bool warningEnabled = true;
    bool errorEnabled = true;
    bool criticalEnabled = true;
    
    // Performance optimization
    size_t maxCachedMessages = 1000;
    std::deque<LogEntry> messageCache;
    
    // Thread safety improvements
    std::mutex cacheMutex;
    std::condition_variable cacheCondition;
    bool stopBackgroundThread = false;
    std::thread backgroundThread;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize performance optimization flags
    pImpl->debugEnabled = false;
    pImpl->infoEnabled = true;
    pImpl->warningEnabled = true;
    pImpl->errorEnabled = true;
    pImpl->criticalEnabled = true;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->debugEnabled = (level <= LogLevel::Debug);
    pImpl->infoEnabled = (level <= LogLevel::Info);
    pImpl->warningEnabled = (level <= LogLevel::Warning);
    pImpl->errorEnabled = (level <= LogLevel::Error);
    pImpl->criticalEnabled = (level <= LogLevel::Critical);
}

ConsoleLogger::~ConsoleLogger() {
    // Stop background thread if running
    if (pImpl->backgroundThread.joinable()) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->stopBackgroundThread = true;
        }
        pImpl->cacheCondition.notify_one();
        pImpl->backgroundThread.join();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Fast path: check if level is enabled before acquiring mutex
    switch (entry.level) {
        case LogLevel::Debug:
            if (!pImpl->debugEnabled) return;
            break;
        case LogLevel::Info:
            if (!pImpl->infoEnabled) return;
            break;
        case LogLevel::Warning:
            if (!pImpl->warningEnabled) return;
            break;
        case LogLevel::Error:
            if (!pImpl->errorEnabled) return;
            break;
        case LogLevel::Critical:
            if (!pImpl->criticalEnabled) return;
            break;
    }
    
    // Acquire mutex for file operations
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with milliseconds for better precision
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // Update file size and handle log rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize += output.size() + 1;  // +1 for newline
        
        // Check if file needs rotation
        if (pImpl->currentFileSize >= pImpl->rotationConfig.maxFileSize) {
            rotateLogFile();
        }
        
        // Write to file
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        
        // Update size after writing
        pImpl->currentFileSize += 1;  // newline
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->level = level;
    pImpl->debugEnabled = (level <= LogLevel::Debug);
    pImpl->infoEnabled = (level <= LogLevel::Info);
    pImpl->warningEnabled = (level <= LogLevel::Warning);
    pImpl->errorEnabled = (level <= LogLevel::Error);
    pImpl->criticalEnabled = (level <= LogLevel::Critical);
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Set base filename for rotation
    pImpl->baseFilename = filepath;
    pImpl->currentFileSize = 0;
    pImpl->fileIndex = 1;
    
    // Ensure directory exists
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
        } catch (const std::filesystem::filesystem_error&) {
            // If directory creation fails, just continue without file output
            return;
        }
    }
    
    // Open file with append mode and handle errors gracefully
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        // Log error to console (this is a simplified error handling)
        std::cerr << "Failed to open log file: " << filepath << std::endl;
        return;
    }
    
    // Check file size for potential rotation
    try {
        auto fileSize = std::filesystem::file_size(filepath);
        pImpl->currentFileSize = static_cast<uint64_t>(fileSize);
        
        if (pImpl->currentFileSize >= pImpl->rotationConfig.maxFileSize) {
            rotateLogFile();
        }
    } catch (const std::filesystem::filesystem_error&) {
        // If we can't get file size, assume it's empty
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open()) return;
    
    // Close current file
    pImpl->fileOut.close();
    
    // Close any existing rotated files beyond maxFiles
    for (uint32_t i = pImpl->rotationConfig.maxFiles; i < pImpl->fileIndex; ++i) {
        std::string rotatedPath = pImpl->baseFilename + "." + std::to_string(i);
        try {
            std::filesystem::remove(rotatedPath);
        } catch (const std::filesystem::filesystem_error&) {
            // Ignore errors when removing old files
        }
    }
    
    // Rename current file to rotated version
    if (pImpl->fileIndex <= pImpl->rotationConfig.maxFiles) {
        std::string currentPath = pImpl->baseFilename;
        std::string rotatedPath = pImpl->baseFilename + "." + std::to_string(pImpl->fileIndex);
        
        try {
            std::filesystem::rename(currentPath, rotatedPath);
            
            // If compression is enabled, compress the rotated file
            if (pImpl->rotationConfig.compressOldLogs) {
                // Simple compression simulation - in real implementation would use gzip
                // For now, just note that compression would occur
            }
            
            pImpl->fileIndex++;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Failed to rotate log file: " << e.what() << std::endl;
            // Reopen the original file
            pImpl->fileOut.open(pImpl->baseFilename, std::ios::app);
            return;
        }
    }
    
    // Open new log file
    pImpl->fileOut.open(pImpl->baseFilename, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Failed to reopen log file after rotation" << std::endl;
    }
    
    pImpl->currentFileSize = 0;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:    return "\033[36m";    // Cyan
        case LogLevel::Info:     return "\033[32m";      // Green
        case LogLevel::Warning:  return "\033[33m";   // Yellow
        case LogLevel::Error:    return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
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
                const std::string& file, int line, const std::string& function) {
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

Logger& Logger::getInstance() {
    // Legacy compatibility alias for getGlobal()
    return getGlobal();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
