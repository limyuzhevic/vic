#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

// Clean up duplicate implementations
    // Remove the duplicate levelToColor method at line 100-120
    // Also fix the remaining implementations
    
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
    
    void ConsoleLogger::addSourceFilter(const std::string& pattern) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->sourceFilters.push_back(pattern);
    }
    
    void ConsoleLogger::clearSourceFilters() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->sourceFilters.clear();
    }
    
    void ConsoleLogger::setLogRotation(size_t maxSize, size_t maxFiles) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->maxLogFileSize = maxSize;
        pImpl->maxLogFiles = maxFiles;
        if (maxSize > 0 && pImpl->fileOut.is_open()) {
            // Rotate if file is too large
            pImpl->fileOut.close();
            std::string currentPath = "console.log";
            // Simple rotation: if file exists, rename to backup
            // In a real implementation, you'd want more sophisticated rotation
            currentPath = "console.log";
            for (size_t i = maxFiles - 1; i > 0; --i) {
                std::string src = "console_" + std::to_string(i) + ".log";
                std::string dst = "console_" + std::to_string(i + 1) + ".log";
                std::rename(src.c_str(), dst.c_str());
            }
            std::rename(currentPath.c_str(), "console_1.log");
            pImpl->fileOut.open(currentPath, std::ios::trunc);
        }
    }
    
    ConsoleLogger::LogStats ConsoleLogger::getStats() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->stats;
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
    
    static std::shared_ptr<Logger>& globalLoggerPtr() {
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
        : level_(level), file_(file), line_(line), function_(function) {}
    
    LogStream::~LogStream() {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    }
    
} // namespace nlm
