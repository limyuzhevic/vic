#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool enableTimestamps = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxLogSize = 10 * 1024 * 1024; // 10MB
    size_t currentLogSize = 0;
    bool autoRollOver = true;
    std::string logFilePath;
    
    Impl() = default;
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
    
    try {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        if (pImpl->enableTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str()) << " ";
        }
        
        oss << entry.message;
        
        if (!entry.file.empty() && entry.line > 0) {
            oss << " [" << entry.file << ":" << entry.line << "]";
        }
        
        if (!entry.function.empty()) {
            oss << " (" << entry.function << ")";
        }
        
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
            pImpl->currentLogSize += output.length() + 1; // +1 for newline
            
            // Simple log rotation
            if (pImpl->autoRollOver && pImpl->currentLogSize > pImpl->maxLogSize) {
                rollOverLogFile();
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in logging: " << e.what() << std::endl;
        std::cerr << "Failed to process log entry: " << entry.message << std::endl;
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

void ConsoleLogger::setEnableTimestamps(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableTimestamps = enable;
}

bool ConsoleLogger::getEnableTimestamps() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enableTimestamps;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timestampFormat = format;
}

void ConsoleLogger::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logFilePath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.seekp(0, std::ios::end);
        pImpl->currentLogSize = pImpl->fileOut.tellp();
    }
}

void ConsoleLogger::closeLogFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setMaxLogSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxLogSize = maxSize;
}

void ConsoleLogger::rollOverLogFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    if (!pImpl->logFilePath.empty()) {
        std::string backupPath = pImpl->logFilePath + "." + 
            std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        if (std::filesystem::exists(pImpl->logFilePath)) {
            std::filesystem::rename(pImpl->logFilePath, backupPath);
        }
        
        pImpl->fileOut.open(pImpl->logFilePath, std::ios::app);
        pImpl->currentLogSize = 0;
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
    LogLevel globalLevel = LogLevel::Info;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& logger : loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (level < pImpl->globalLevel) {
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
    
    for (auto& logger : loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Failed to log message: " << e.what() << std::endl;
            std::cerr << "Message: " << message << std::endl;
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
    return level <= pImpl->globalLevel;
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
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "FATAL: Failed to log message in destructor: " << e.what() << std::endl;
        std::cerr << "Message: " << oss_.str() << std::endl;
    }
}

} // namespace nlm

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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
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
