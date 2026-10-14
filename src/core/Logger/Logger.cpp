#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <filesystem>
#include <cstring>

namespace nlm {

// Exception class for logger errors
class LoggerException : public std::runtime_error {
public:
    LoggerException(const std::string& message, LogLevel level = LogLevel::Error)
        : std::runtime_error(message), level_(level) {}
    
    LogLevel getLevel() const { return level_; }
    
private:
    LogLevel level_;
};

// Log entry implementation
LogEntry::LogEntry(LogLevel level, const std::string& message, 
                   const std::string& file, int line, const std::string& function)
    : level(level), message(message), file(file), line(line), function(function) {
    timestamp = std::chrono::system_clock::now();
}

std::string LogEntry::getFormattedTimestamp() const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ConsoleLogger implementation with enhanced features
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    bool useFileNames = true;
    bool useFunctionNames = true;
    size_t maxMessageLength = 1024;
    std::ofstream fileOut;
    std::mutex mutex;
    std::chrono::steady_clock::time_point lastMessageTime;
    std::map<LogLevel, size_t> messageCounts;
    std::string logFilePath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSettings();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    initializeDefaultSettings();
}

void ConsoleLogger::initializeDefaultSettings() {
    pImpl->useTimestamps = true;
    pImpl->useFileNames = true;
    pImpl->useFunctionNames = true;
    pImpl->maxMessageLength = 1024;
    pImpl->messageCounts.clear();
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Check if this log level is enabled
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Enforce maximum message length
    std::string message = entry.message;
    if (message.length() > pImpl->maxMessageLength) {
        message = message.substr(0, pImpl->maxMessageLength - 3) + "...";
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update message statistics
    pImpl->messageCounts[entry.level]++;
    pImpl->lastMessageTime = std::chrono::steady_clock::now();
    
    // Build the log message
    std::ostringstream oss;
    
    // Add level indicator with color if enabled
    if (pImpl->useColors) {
        oss << levelToColor(entry.level);
    }
    
    // Add timestamp
    if (pImpl->useTimestamps) {
        oss << "[" << entry.getFormattedTimestamp() << "] ";
    }
    
    // Add level
    oss << "[" << levelToString(entry.level) << "]";
    
    // Add file and line number
    if (pImpl->useFileNames && !entry.file.empty()) {
        oss << " " << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    
    // Add function name
    if (pImpl->useFunctionNames && !entry.function.empty()) {
        oss << " @ " << entry.function;
    }
    
    // Add message
    oss << " " << message;
    
    // Reset colors if used
    if (pImpl->useColors) {
        oss << "\033[0m";
    }
    
    std::string output = oss.str();
    
    // Output to console
    std::cout << output << std::endl;
    
    // Output to file if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << std::endl;
        pImpl->fileOut.flush();
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

void ConsoleLogger::setUseFileNames(bool useFileNames) {
    pImpl->useFileNames = useFileNames;
}

bool ConsoleLogger::getUseFileNames() const {
    return pImpl->useFileNames;
}

void ConsoleLogger::setUseFunctionNames(bool useFunctionNames) {
    pImpl->useFunctionNames = useFunctionNames;
}

bool ConsoleLogger::getUseFunctionNames() const {
    return pImpl->useFunctionNames;
}

void ConsoleLogger::setMaxMessageLength(size_t maxLength) {
    pImpl->maxMessageLength = maxLength;
}

size_t ConsoleLogger::getMaxMessageLength() const {
    return pImpl->maxMessageLength;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    // Close existing file if open
    closeOutputFile();
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    std::filesystem::path dir = path.parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
        } catch (const std::filesystem::filesystem_error& e) {
            throw LoggerException("Failed to create log directory " + dir.string() + ": " + e.what());
        }
    }
    
    // Open file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        throw LoggerException("Failed to open log file for writing: " + filepath);
    }
    
    pImpl->logFilePath = filepath;
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->logFilePath.clear();
    }
}

bool ConsoleLogger::isFileOpen() const {
    return pImpl->fileOut.is_open();
}

std::string ConsoleLogger::getLogFilePath() const {
    return pImpl->logFilePath;
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
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::map<LogLevel, size_t> ConsoleLogger::getMessageCounts() const {
    return pImpl->messageCounts;
}

std::chrono::steady_clock::time_point ConsoleLogger::getLastMessageTime() const {
    return pImpl->lastMessageTime;
}

std::chrono::duration<double> ConsoleLogger::getTimeSinceLastMessage() const {
    return std::chrono::steady_clock::now() - pImpl->lastMessageTime;
}

// Global Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<ILogger> defaultLogger;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Create default console logger
    pImpl->defaultLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    pImpl->loggers.push_back(pImpl->defaultLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::remove(pImpl->loggers.begin(), pImpl->loggers.end(), logger);
    pImpl->loggers.erase(it, pImpl->loggers.end());
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
    if (message.empty()) {
        throw std::invalid_argument("Log message cannot be empty");
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
            // Don't let one logger failure break the entire logging operation
            std::cerr << "Logger error: " << e.what() << std::endl;
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
        // This is a placeholder for advanced global logger management
        // In practice, we just use getGlobal() which is already static
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    oss_ << "[" << getLogLevelString(level_) << "] ";
    if (!file_.empty()) {
        oss_ << file_;
        if (line_ > 0) {
            oss_ << ":" << line_;
        }
        oss_ << " @ " << function_ << " ";
    }
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "Fatal logging error in destructor: " << e.what() << std::endl;
    }
}

LogStream& LogStream::operator<<(const std::string& str) {
    oss_ << str;
    return *this;
}

LogStream& LogStream::operator<<(int value) {
    oss_ << value;
    return *this;
}

LogStream& LogStream::operator<<(double value) {
    oss_ << value;
    return *this;
}

LogStream& LogStream::operator<<(bool value) {
    oss_ << (value ? "true" : "false");
    return *this;
}

LogStream& LogStream::operator<<(char value) {
    oss_ << value;
    return *this;
}

LogStream& LogStream::operator<<(const char* str) {
    oss_ << str;
    return *this;
}

LogLevel LogStream::getLogLevel() const {
    return level_;
}

std::string LogStream::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

} // namespace nlm

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
