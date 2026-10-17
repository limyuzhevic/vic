#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
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

// Enhanced logging with structured data
void Logger::logWithContext(LogLevel level, const std::string& message,
                            const std::unordered_map<std::string, std::string>& context,
                            const std::string& file, int line, const std::string& function) {
    std::ostringstream oss;
    oss << message;
    
    // Add context data
    if (!context.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& pair : context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "]";
    }
    
    log(level, oss.str(), file, line, function);
}

// Performance-aware logging (throttled)
void Logger::logThrottled(LogLevel level, const std::string& message,
                           double intervalSeconds,
                           const std::string& file, int line, const std::string& function) {
    static std::unordered_map<std::string, std::chrono::system_clock::time_point> lastLogTimes;
    static std::mutex throttleMutex;
    
    std::string key = file + ":" + std::to_string(line) + ":" + message;
    
    std::lock_guard<std::mutex> lock(throttleMutex);
    auto now = std::chrono::system_clock::now();
    auto lastTime = lastLogTimes[key];
    
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastTime).count();
    
    if (duration >= intervalSeconds) {
        lastLogTimes[key] = now;
        log(level, message, file, line, function);
    }
}

// Batch logging for performance
void Logger::batchLog(const std::vector<LogEntry>& entries) {
    if (entries.empty()) return;
    
    // For performance, we could implement batch processing
    // For now, we just log each entry individually
    for (const auto& entry : entries) {
        // Note: This doesn't bypass the level filtering, but for batch operations
        // we might want to have different behavior
        log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }
}

// Get recent log entries
std::vector<LogEntry> Logger::getRecentLogs(size_t count) const {
    std::vector<LogEntry> recentLogs;
    
    // This is a placeholder implementation
    // In a real implementation, we would have a log ring buffer
    // or persistent storage where we can retrieve recent logs
    
    // For now, return an empty vector as the core logging system
    // doesn't have built-in log storage capabilities
    return recentLogs;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Add file-level logging support to ConsoleLogger
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

// Add a static method to get log file output for testing
std::ofstream& ConsoleLogger::getFileOutput() {
    return pImpl->fileOut;
}

// Add JSON logging support
void ConsoleLogger::logWithJsonFormat(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "{\"timestamp\": \"";
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "\", \"level\": \"" << levelToString(entry.level) << "\", \"message\": \"";
    
    // Escape special characters in message
    std::string escapedMessage = entry.message;
    size_t pos = 0;
    while ((pos = escapedMessage.find('\\', pos)) != std::string::npos) {
        escapedMessage.replace(pos, 1, "\\\\");
        pos += 2;
    }
    while ((pos = escapedMessage.find('"', pos)) != std::string::npos) {
        escapedMessage.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    oss << escapedMessage << "\"";
    if (!entry.file.empty()) {
        oss << ", \"file\": \"" << entry.file << "\"";
    }
    if (entry.line > 0) {
        oss << ", \"line\": " << entry.line;
    }
    if (!entry.function.empty()) {
        oss << ", \"function\": \"" << entry.function << "\"";
    }
    oss << "}\n";
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m";
    } else {
        std::cout << output;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output;
        pImpl->fileOut.flush();
    }
}
