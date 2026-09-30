#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <algorithm>
#include <map>
#include <fmt/core.h>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::shared_ptr<ILogFormatter> formatter;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxFiles = 5;
    bool logRotationEnabled = false;
    std::string filepath;
    size_t currentFileSize = 0;
    int fileNumber = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->formatter = std::make_shared<TextFormatter>();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->formatter = std::make_shared<TextFormatter>();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Rotate log file if needed
    if (pImpl->logRotationEnabled) {
        rotateLogFileIfNeeded();
    }
    
    // Format the log entry
    std::string formattedMessage = pImpl->formatter->format(entry);
    
    // Console output
    if (pImpl->useColors) {
        // Add color codes for different log levels
        std::string colorCode;
        switch (entry.level) {
            case LogLevel::Debug: colorCode = "\033[36m"; break;    // Cyan
            case LogLevel::Info: colorCode = "\033[32m"; break;     // Green
            case LogLevel::Warning: colorCode = "\033[33m"; break;  // Yellow
            case LogLevel::Error: colorCode = "\033[31m"; break;    // Red
            case LogLevel::Critical: colorCode = "\033[35m"; break; // Magenta
            default: colorCode = "\033[0m"; break;
        }
        std::cout << colorCode << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += formattedMessage.size() + 1; // +1 for newline
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
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filepath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    
    if (!pImpl->fileOut.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFormatter(std::shared_ptr<ILogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatter = formatter;
}

std::shared_ptr<ILogFormatter> ConsoleLogger::getFormatter() const {
    return pImpl->formatter;
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setMaxFiles(int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFiles = maxFiles;
}

void ConsoleLogger::enableLogRotation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logRotationEnabled = enable;
}

void ConsoleLogger::rotateLogFileIfNeeded() {
    if (!pImpl->fileOut.is_open() || pImpl->currentFileSize < pImpl->maxFileSize) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rotate files
    std::string basePath = pImpl->filepath;
    std::string extension = "";
    
    // Find existing files and rotate them
    for (int i = pImpl->maxFiles - 1; i > 0; --i) {
        std::string src = basePath;
        if (i == pImpl->maxFiles - 1) src += ".1"; // Current log is .1
        else src += "." + std::to_string(i);
        
        std::string dst = basePath;
        dst += "." + std::to_string(i + 1);
        
        if (std::filesystem::exists(src)) {
            std::filesystem::rename(src, dst);
        }
    }
    
    // Rename current log file
    if (std::filesystem::exists(basePath)) {
        std::filesystem::rename(basePath, basePath + ".1");
    }
    
    // Reopen log file
    pImpl->fileOut.open(basePath, std::ios::trunc);
    pImpl->currentFileSize = 0;
    pImpl->fileNumber = 1;
}

// Rotating file logger implementation
struct RotatingFileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    std::shared_ptr<ILogFormatter> formatter;
    std::string filepath;
    size_t maxFileSize;
    int maxFiles;
    size_t currentFileSize = 0;
    int fileNumber = 0;
};

RotatingFileLogger::RotatingFileLogger(const std::string& filepath, size_t maxSize, int maxFiles)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->filepath = filepath;
    pImpl->maxFileSize = maxSize;
    pImpl->maxFiles = maxFiles;
    pImpl->formatter = std::make_shared<TextFormatter>();
    
    // Open file in truncate mode
    pImpl->fileOut.open(filepath, std::ios::trunc);
    if (!pImpl->fileOut.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filepath);
    }
}

RotatingFileLogger::~RotatingFileLogger() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void RotatingFileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the log entry
    std::string formattedMessage = pImpl->formatter->format(entry);
    
    // Write to file
    pImpl->fileOut << formattedMessage << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += formattedMessage.size() + 1;
    
    // Rotate if needed
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
    }
}

void RotatingFileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel RotatingFileLogger::getLevel() const {
    return pImpl->level;
}

void RotatingFileLogger::setFormatter(std::shared_ptr<ILogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatter = formatter;
}

std::shared_ptr<ILogFormatter> RotatingFileLogger::getFormatter() const {
    return pImpl->formatter;
}

void RotatingFileLogger::rotate() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rotate existing files
    std::string basePath = pImpl->filepath;
    for (int i = pImpl->maxFiles - 1; i > 0; --i) {
        std::string src = basePath;
        if (i == pImpl->maxFiles - 1) src += ".1";
        else src += "." + std::to_string(i);
        
        std::string dst = basePath;
        dst += "." + std::to_string(i + 1);
        
        if (std::filesystem::exists(src)) {
            std::filesystem::rename(src, dst);
        }
    }
    
    // Rename current log file
    if (std::filesystem::exists(basePath)) {
        std::filesystem::rename(basePath, basePath + ".1");
    }
    
    // Reopen log file
    pImpl->fileOut.open(basePath, std::ios::trunc);
    pImpl->currentFileSize = 0;
    pImpl->fileNumber = 1;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogStatistics stats;
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
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->stats.totalLogs++;
    switch (level) {
        case LogLevel::Debug: pImpl->stats.debugLogs++; break;
        case LogLevel::Info: pImpl->stats.infoLogs++; break;
        case LogLevel::Warning: pImpl->stats.warningLogs++; break;
        case LogLevel::Error: pImpl->stats.errorLogs++; break;
        case LogLevel::Critical: pImpl->stats.criticalLogs++; break;
    }
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                             const std::map<std::string, std::string>& metadata,
                             const std::string& file, int line,
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->stats.totalLogs++;
    switch (level) {
        case LogLevel::Debug: pImpl->stats.debugLogs++; break;
        case LogLevel::Info: pImpl->stats.infoLogs++; break;
        case LogLevel::Warning: pImpl->stats.warningLogs++; break;
        case LogLevel::Error: pImpl->stats.errorLogs++; break;
        case LogLevel::Critical: pImpl->stats.criticalLogs++; break;
    }
}

void Logger::logWithMetrics(LogLevel level, const std::string& message,
                            const std::map<std::string, double>& metrics,
                            const std::string& file, int line,
                            const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metrics = metrics;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->stats.totalLogs++;
    switch (level) {
        case LogLevel::Debug: pImpl->stats.debugLogs++; break;
        case LogLevel::Info: pImpl->stats.infoLogs++; break;
        case LogLevel::Warning: pImpl->stats.warningLogs++; break;
        case LogLevel::Error: pImpl->stats.errorLogs++; break;
        case LogLevel::Critical: pImpl->stats.criticalLogs++; break;
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

void Logger::configureGlobalLogger(const std::string& configFile) {
    if (!configFile.empty()) {
        // TODO: Load logger configuration from file
        // For now, just set up basic configuration
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        
        auto fileLogger = std::make_shared<RotatingFileLogger>("logs/nlm.log", 10 * 1024 * 1024, 5);
        
        clearLoggers();
        addLogger(consoleLogger);
        addLogger(fileLogger);
    }
}

Logger::LogStatistics Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogStatistics stats = pImpl->stats;
    
    // Calculate average log rate (logs per second)
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
    if (duration.count() > 0) {
        stats.averageLogRate = static_cast<double>(stats.totalLogs) / duration.count();
    }
    
    return stats;
}

void Logger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = LogStatistics();
    pImpl->startTime = std::chrono::system_clock::now();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::withMetric(const std::string& key, double value) {
    metrics_[key] = value;
    return *this;
}

} // namespace nlm
