#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <cstdio>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::mutex mutex;
    
    // Log rotation support
    RotationMode rotationMode = RotationMode::None;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    std::chrono::system_clock::duration rotationPeriod = std::chrono::hours(24);  // 24 hours
    size_t maxFiles = 5;
    std::string backupSuffix = ".bak";
    
    std::string filePath;
    std::ofstream fileOut;
    std::chrono::system_clock::time_point lastRotation;
    size_t currentFileSize = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if rotation is needed
    rotateIfNeeded();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
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
        pImpl->currentFileSize += output.size() + 1;  // +1 for newline
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
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
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filePath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = pImpl->fileOut.is_open() ? 
        std::filesystem::file_size(filepath) : 0;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->filePath.clear();
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setRotationMode(RotationMode mode) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationMode = mode;
}

void ConsoleLogger::setRotationConfig(const RotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationMode = config.mode;
    pImpl->maxFileSize = config.maxFileSize;
    pImpl->rotationPeriod = config.rotationPeriod;
    pImpl->maxFiles = config.maxFiles;
    pImpl->backupSuffix = config.backupSuffix;
}

ConsoleLogger::RotationConfig ConsoleLogger::getRotationConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    RotationConfig config;
    config.mode = pImpl->rotationMode;
    config.maxFileSize = pImpl->maxFileSize;
    config.rotationPeriod = pImpl->rotationPeriod;
    config.maxFiles = pImpl->maxFiles;
    config.backupSuffix = pImpl->backupSuffix;
    return config;
}

void ConsoleLogger::rotate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        
        // Rotate existing files
        for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
            if (i == 0) {
                std::string rotatedPath = getRotatedFileName(pImpl->filePath, 1);
                std::filesystem::rename(pImpl->filePath, rotatedPath);
            } else {
                std::string from = getRotatedFileName(pImpl->filePath, i);
                std::string to = getRotatedFileName(pImpl->filePath, i + 1);
                if (std::filesystem::exists(from)) {
                    std::filesystem::rename(from, to);
                }
            }
        }
        
        // Reopen file for new logs
        pImpl->fileOut.open(pImpl->filePath, std::ios::app);
        pImpl->currentFileSize = 0;
        pImpl->lastRotation = std::chrono::system_clock::now();
    }
}

size_t ConsoleLogger::getLogFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->filePath.empty() || !std::filesystem::exists(pImpl->filePath)) {
        return 0;
    }
    return std::filesystem::file_size(pImpl->filePath);
}

std::string ConsoleLogger::getLogFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->filePath;
}

void ConsoleLogger::rotateIfNeeded() {
    if (pImpl->rotationMode == RotationMode::None) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    
    // Check time-based rotation
    if (pImpl->rotationMode == RotationMode::ByTime && 
        std::chrono::duration_cast<std::chrono::system_clock::duration>(now - pImpl->lastRotation) >= pImpl->rotationPeriod) {
        rotate();
        return;
    }
    
    // Check size-based rotation
    if (pImpl->rotationMode == RotationMode::BySize && pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
        return;
    }
    
    // Check both time and size
    if (pImpl->rotationMode == RotationMode::ByTime && 
        std::chrono::duration_cast<std::chrono::system_clock::duration>(now - pImpl->lastRotation) >= pImpl->rotationPeriod &&
        pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
    }
}

std::string ConsoleLogger::getRotatedFileName(const std::string& baseName, int index) const {
    if (index == 0) {
        return baseName + pImpl->backupSuffix;
    }
    
    std::string::size_type dotPos = baseName.find_last_of('.');
    if (dotPos == std::string::npos) {
        return baseName + "." + std::to_string(index);
    }
    
    std::string stem = baseName.substr(0, dotPos);
    std::string ext = baseName.substr(dotPos);
    return stem + "." + std::to_string(index) + ext;
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

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
