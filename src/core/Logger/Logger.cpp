#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <map>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::map<std::string, std::string> contextData;
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
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";
    
    // Add log level with enhanced formatting
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add thread ID for debugging
    oss << "[thread:" << std::this_thread::get_id() << "] ";
    
    // Add file and line information if provided
    if (!entry.file.empty()) {
        oss << "[" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        oss << "] ";
    }
    
    // Add function information if provided
    if (!entry.function.empty()) {
        oss << "[" << entry.function << "] ";
    }
    
    // Add context data
    for (const auto& context : pImpl->contextData) {
        oss << "[" << context.first << ":" << context.second << "] ";
    }
    
    // Add the message
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Console output with enhanced formatting
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

void ConsoleLogger::setContext(const std::string& key, const std::string& value) {
    pImpl->contextData[key] = value;
}

void ConsoleLogger::clearContext(const std::string& key) {
    pImpl->contextData.erase(key);
}

void ConsoleLogger::clearAllContext() {
    pImpl->contextData.clear();
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

// Logger class implementation with enhanced features
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::map<std::string, std::string> globalContext;
    std::vector<LogEntry> recentLogs;
    size_t maxRecentLogs;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxRecentLogs = 1000;
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

void Logger::setGlobalContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext[key] = value;
}

void Logger::clearGlobalContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext.erase(key);
}

void Logger::clearAllGlobalContext() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext.clear();
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
    
    // Add global context to message
    std::string contextMessage = message;
    if (!pImpl->globalContext.empty()) {
        contextMessage += " [context:";
        bool first = true;
        for (const auto& context : pImpl->globalContext) {
            if (!first) contextMessage += ", ";
            contextMessage += context.first + "=" + context.second;
            first = false;
        }
        contextMessage += "]";
    }
    
    entry.message = contextMessage;
    
    // Store recent log for debugging
    pImpl->recentLogs.push_back(entry);
    if (pImpl->recentLogs.size() > pImpl->maxRecentLogs) {
        pImpl->recentLogs.erase(pImpl->recentLogs.begin());
    }
    
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

std::vector<LogEntry> Logger::getRecentLogs(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (count == 0 || count > pImpl->recentLogs.size()) {
        return pImpl->recentLogs;
    }
    std::vector<LogEntry> recent;
    recent.reserve(count);
    auto start = pImpl->recentLogs.end() - count;
    for (auto it = start; it != pImpl->recentLogs.end(); ++it) {
        recent.push_back(*it);
    }
    return recent;
}

void Logger::clearRecentLogs() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recentLogs.clear();
}

// LogStream implementation with enhanced features
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm