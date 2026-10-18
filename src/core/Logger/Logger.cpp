#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>

namespace nlm {

// Validation and error handling helper functions
namespace LoggerValidation {
    
    bool isValidLogLevel(LogLevel level) {
        return level >= LogLevel::Debug && level <= LogLevel::Critical;
    }
    
    bool isValidTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto now = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::hours>(now - timestamp);
        return std::abs(static_cast<long long>(diff.count())) < 24 * 365 * 100; // Within 100 years
    }
    
    bool isValidMessage(const std::string& message) {
        return !message.empty() && message.length() <= 10000;
    }
    
    bool isValidFileName(const std::string& filename) {
        return !filename.empty() && filename.find('/') != std::string::npos;
    }
    
} // namespace LoggerValidation

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool initialized = false;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    initialize();
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() {
    // Ensure file is closed before destruction
    closeOutputFile();
}

void ConsoleLogger::initialize() {
    pImpl->initialized = true;
    // Initialize with default values
    pImpl->level = LogLevel::Info;
    pImpl->useColors = true;
    pImpl->fileOut.open(); // Open default file stream
}

bool ConsoleLogger::validateLogEntry(const LogEntry& entry) const {
    // Validate entry
    if (!LoggerValidation::isValidLogLevel(entry.level)) {
        return false;
    }
    
    if (!LoggerValidation::isValidTimestamp(entry.timestamp)) {
        return false;
    }
    
    if (!LoggerValidation::isValidMessage(entry.message)) {
        return false;
    }
    
    return true;
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Validate entry
    if (!validateLogEntry(entry)) {
        // Log error using stderr to avoid recursion
        std::cerr << "[INVALID LOG ENTRY] " << entry.message << std::endl;
        return;
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with error handling
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    char timeStr[100];
    if (std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&time))) {
        oss << timeStr;
    } else {
        oss << "[timestamp error]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with error handling
    try {
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error writing to console: " << e.what() << std::endl;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            std::cerr << "Error writing to log file: " << e.what() << std::endl;
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    if (!LoggerValidation::isValidLogLevel(level)) {
        NLM_LOG_ERROR("Invalid log level requested: " + std::to_string(static_cast<int>(level)));
        return;
    }
    
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
    if (!LoggerValidation::isValidFileName(filepath)) {
        NLM_LOG_ERROR("Invalid file path: " + filepath);
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file
    closeOutputFile();
    
    // Open new file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("Cannot open log file: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
    bool initialized = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

void Logger::initialize() {
    pImpl->initialized = true;
    // Add default console logger
    addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Attempted to add null logger");
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    if (!LoggerValidation::isValidLogLevel(level)) {
        NLM_LOG_ERROR("Invalid log level: " + std::to_string(static_cast<int>(level)));
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (!LoggerValidation::isValidLogLevel(level)) {
        return;
    }
    
    if (!LoggerValidation::isValidMessage(message)) {
        NLM_LOG_ERROR("Invalid log message (too long or empty): " + message.substr(0, 100));
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
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "Error logging to logger: " << e.what() << std::endl;
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
    if (!LoggerValidation::isValidLogLevel(level)) {
        return false;
    }
    
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
        // Replace global logger
        // For now, just log the change
        NLM_LOG_INFO("Global logger set to new instance");
    }
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    
    if (!LoggerValidation::isValidLogLevel(level)) {
        NLM_LOG_ERROR("Invalid log level in LogStream: " + std::to_string(static_cast<int>(level)));
    }
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "Error in LogStream destructor: " << e.what() << std::endl;
    }
}

} // namespace nlm