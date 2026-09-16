#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cassert>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string outputFilepath;
    bool fileError = false;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()), valid_(true) {
    if (!validateParameters()) {
        valid_ = false;
        pImpl->level = LogLevel::Error;
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()), valid_(true) {
    setLevel(level);
    if (!validateParameters()) {
        valid_ = false;
        pImpl->level = LogLevel::Error;
    }
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

bool ConsoleLogger::validateParameters() const {
    return pImpl != nullptr;
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!valid_ || !pImpl) {
        return;
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Validate entry parameters
    if (entry.message.empty()) {
        return; // Skip empty messages
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with error handling
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::tm tm_buf;
    if (std::localtime_s(&tm_buf, &time) == 0) {
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    } else {
        oss << "[TIMESTAMP_ERROR]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with error handling
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with error handling
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            pImpl->fileError = true;
            // Log to stderr if file writing fails
            std::cerr << "[LOGGER ERROR] Failed to write to log file: " << e.what() << std::endl;
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    if (!valid_ || !pImpl) return;
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl ? pImpl->level : LogLevel::Error;
}

void ConsoleLogger::setUseColors(bool useColors) {
    if (!valid_ || !pImpl) return;
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl ? pImpl->useColors : false;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    if (!valid_ || !pImpl) return;
    
    // Validate file path
    if (!isValidFilePath(filepath)) {
        std::cerr << "[LOGGER ERROR] Invalid file path: " << filepath << std::endl;
        return;
    }
    
    closeOutputFile(); // Close existing file if open
    
    try {
        pImpl->fileOut.open(filepath, std::ios::app);
        pImpl->outputFilepath = filepath;
        
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "[LOGGER ERROR] Failed to open log file: " << filepath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER ERROR] Exception opening log file " << filepath << ": " << e.what() << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl && pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut.close();
            pImpl->outputFilepath.clear();
        } catch (const std::exception& e) {
            std::cerr << "[LOGGER ERROR] Exception closing log file: " << e.what() << std::endl;
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
    size_t logCount = 0;
    size_t errorCount = 0;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()), valid_(true) {
    // Create default console logger
    auto defaultLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    if (addLogger(defaultLogger)) {
        pImpl->loggers.push_back(defaultLogger);
    } else {
        valid_ = false;
    }
}

Logger::~Logger() {
    clearLoggers();
}

Logger::Logger(Logger&& other) noexcept : pImpl(std::move(other.pImpl)), valid_(other.valid_) {
    other.valid_ = false;
}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        clearLoggers();
        pImpl = std::move(other.pImpl);
        valid_ = other.valid_;
        other.valid_ = false;
    }
    return *this;
}

bool Logger::validateLogParameters(LogLevel level, const std::string& message) {
    return level != LogLevel::Critical || !message.empty();
}

std::string Logger::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    std::ostringstream oss;
    
    if (std::localtime_s(&tm_buf, &time) == 0) {
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    } else {
        oss << "[TIMESTAMP_ERROR]";
    }
    
    return oss.str();
}

bool Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!valid_ || !logger || !pImpl) {
        return false;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
        pImpl->logCount++;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER ERROR] Failed to add logger: " << e.what() << std::endl;
        return false;
    }
}

void Logger::clearLoggers() {
    if (!valid_ || !pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    if (!valid_ || !pImpl) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    if (!valid_ || !validateLogParameters(level, message) || !pImpl) return;
    
    LogEntry entry(level, message, file, line, function);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
            if (level >= LogLevel::Error) {
                pImpl->errorCount++;
            }
        } catch (const std::exception& e) {
            std::cerr << "[LOGGER ERROR] Exception in logger: " << e.what() << std::endl;
        }
    }
    
    pImpl->logCount++;
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
    if (!valid_ || !pImpl) return false;
    
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
    if (logger) {
        // In practice, we can't easily replace a static object
        // but we provide the interface for completeness
        std::cerr << "[LOGGER WARNING] setGlobal() has limited effect in current implementation" << std::endl;
    }
}

size_t Logger::getLoggerCount() const {
    return pImpl ? pImpl->logCount : 0;
}

void Logger::printStats() const {
    if (!pImpl) return;
    
    std::cout << "=== Logger Statistics ===" << std::endl;
    std::cout << "Total loggers: " << getLoggerCount() << std::endl;
    std::cout << "Total log entries: " << pImpl->logCount << std::endl;
    std::cout << "Total errors: " << pImpl->errorCount << std::endl;
    
    std::cout << "Individual loggers:" << std::endl;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (size_t i = 0; i < pImpl->loggers.size(); ++i) {
        std::cout << "  " << i << ": " << (pImpl->loggers[i] ? "valid" : "invalid") << std::endl;
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), valid_(true) {
    
    // Validate parameters
    if (!nlm::isValidFilePath(file) || line < 0) {
        valid_ = false;
    }
}

LogStream::~LogStream() {
    if (valid_) {
        try {
            Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
        } catch (const std::exception& e) {
            std::cerr << "[LOGGER ERROR] Exception in LogStream destructor: " << e.what() << std::endl;
        }
    }
}

} // namespace nlm
