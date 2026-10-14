#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <mutex>
#include <thread>
#include <deque>
#include <condition_variable>
#include <atomic>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level;
    bool useColors;
    std::ofstream outputFile;
    std::string filepath;
    std::mutex mutex;
    
    ConsoleLogger::Impl() : level(LogLevel::Info), useColors(true) {
        // Check if terminal supports colors
        if (isatty(fileno(stdout))) {
            useColors = true;
        } else {
            useColors = false;
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if entry level meets or exceeds logger level
    if (entry.level < pImpl->level && entry.level != LogLevel::Error && entry.level != LogLevel::Critical) {
        return;
    }
    
    // Format log message
    std::stringstream ss;
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
    
    // Add level
    ss << "[" << levelToString(entry.level) << "] ";
    
    // Add message
    ss << entry.message;
    
    // Write to output
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << ss.str() << std::endl;
    } else {
        std::cout << ss.str() << std::endl;
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
    pImpl->filepath = filepath;
    pImpl->outputFile.open(filepath, std::ios::out | std::ios::app);
    if (!pImpl->outputFile.is_open()) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m"; // Cyan
        case LogLevel::Info: return "\033[32m";  // Green
        case LogLevel::Warning: return "\033[33m"; // Yellow
        case LogLevel::Error: return "\033[31m";  // Red
        case LogLevel::Critical: return "\033[1;31m"; // Bold Red
        default: return "";
    }
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    LogLevel minLevel;
    mutable std::mutex mutex;
    
    Impl() : minLevel(LogLevel::Info) {}
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
    pImpl->minLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line,
                const std::string& function) {
    // Check if level is enabled globally
    if (!isEnabled(level)) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "Error in logger: " << e.what() << std::endl;
        }
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
    return level >= pImpl->minLevel;
}

Logger& Logger::getGlobal() {
    static std::shared_ptr<Logger> globalLogger = std::make_shared<Logger>();
    return *globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    static std::shared_ptr<Logger> globalLogger = logger;
}

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "Error logging: " << e.what() << std::endl;
    }
}

} // namespace nlm