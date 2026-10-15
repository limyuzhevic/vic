#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <mutex>

namespace nlm {

// Global log instance
Logger* Logger::globalLogger = nullptr;
std::mutex Logger::loggerMutex;

// Singleton instance management
Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(loggerMutex);
    if (!globalLogger) {
        globalLogger = new ConsoleLogger(LogLevel::Info);
    }
    return *globalLogger;
}

void Logger::setGlobal(Logger* logger) {
    std::lock_guard<std::mutex> lock(loggerMutex);
    if (globalLogger) {
        delete globalLogger;
    }
    globalLogger = logger;
}

Logger::Logger() : level(LogLevel::Info) {}

Logger::~Logger() = default;

void Logger::setLevel(LogLevel logLevel) {
    std::lock_guard<std::mutex> lock(mutex);
    level = logLevel;
}

LogLevel Logger::getLevel() const {
    std::lock_guard<std::mutex> lock(mutex);
    return level;
}

void Logger::log(const std::string& message, LogLevel logLevel) {
    if (logLevel < level) {
        return; // Below minimum log level
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    std::string formatted = formatMessage(message, logLevel);
    output(formatted);
}

std::string Logger::formatMessage(const std::string& message, LogLevel logLevel) const {
    std::stringstream ss;
    
    // Add timestamp
    ss << formatTimestamp();
    ss << " [";
    
    // Add log level
    ss << formatLogLevel(logLevel);
    ss << "] ";
    
    // Add message
    ss << message;
    
    return ss.str();
}

std::string Logger::formatTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string Logger::formatLogLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:               return "UNKNOWN";
    }
}

// Global logging macros
#define NLM_LOG_DEBUG(msg) Logger::getInstance().log(msg, LogLevel::Debug)
#define NLM_LOG_INFO(msg)  Logger::getInstance().log(msg, LogLevel::Info)
#define NLM_LOG_WARNING(msg) Logger::getInstance().log(msg, LogLevel::Warning)
#define NLM_LOG_ERROR(msg) Logger::getInstance().log(msg, LogLevel::Error)
#define NLM_LOG_FATAL(msg) Logger::getInstance().log(msg, LogLevel::Fatal)

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setLevel(level);
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const std::string& message, LogLevel level) {
    if (level < getLevel()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::string formatted = formatMessage(message, level);
    output(formatted);
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::output(const std::string& message) {
    std::cout << message << std::endl;
}

// FileLogger implementation
struct FileLogger::Impl {
    std::ofstream file;
    LogLevel level;
    std::mutex mutex;
    bool isOpen;
};

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setLevel(level);
    pImpl->file.open(filepath, std::ios::app);
    pImpl->isOpen = pImpl->file.is_open();
    
    if (!pImpl->isOpen) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
    }
}

FileLogger::~FileLogger() {
    flush();
}

void FileLogger::log(const std::string& message, LogLevel level) {
    if (level < getLevel()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::string formatted = formatMessage(message, level);
    output(formatted);
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::output(const std::string& message) {
    if (pImpl->isOpen) {
        pImpl->file << message << std::endl;
    } else {
        std::cerr << "[FILE LOG ERROR] " << message << std::endl;
    }
}

void FileLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->isOpen) {
        pImpl->file.flush();
    }
}

} // namespace nlm