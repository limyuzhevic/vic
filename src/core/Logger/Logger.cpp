#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool includeTimestamp = true;
    bool includeThreadId = false;
    bool includeFileLocation = true;
    std::string logFormat = "[%level] %timestamp %thread %file:%line %function: %message";
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
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
    
    // Format the log message based on the log format
    std::string formattedMessage = pImpl->logFormat;
    
    // Replace placeholders
    if (pImpl->includeTimestamp) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timeBuffer[64];
        std::strftime(timeBuffer, sizeof(timeBuffer), pImpl->timestampFormat.c_str(), std::localtime(&time));
        replaceAll(formattedMessage, "%timestamp", std::string(timeBuffer));
    }
    
    replaceAll(formattedMessage, "%level", levelToString(entry.level));
    
    if (pImpl->includeThreadId) {
        // Thread ID would be implementation-specific
        replaceAll(formattedMessage, "%thread", "[main]");
    }
    
    if (pImpl->includeFileLocation) {
        std::string location = entry.file;
        if (!entry.file.empty() && entry.line > 0) {
            location += ":" + std::to_string(entry.line);
        }
        replaceAll(formattedMessage, "%file:%line", location);
        replaceAll(formattedMessage, "%function", entry.function);
    } else {
        replaceAll(formattedMessage, "%file:%line %function", "");
    }
    
    replaceAll(formattedMessage, "%message", entry.message);
    
    // Add thread ID placeholder replacement if needed
    replaceAll(formattedMessage, "%thread", "[main]");
    
    // Handle colors
    std::string output;
    if (pImpl->useColors) {
        output = levelToColor(entry.level) + formattedMessage + "\033[0m\n";
    } else {
        output = formattedMessage + "\n";
    }
    
    std::cout << output;
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
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

void ConsoleLogger::setIncludeTimestamp(bool include) {
    pImpl->includeTimestamp = include;
}

void ConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

void ConsoleLogger::setIncludeFileLocation(bool include) {
    pImpl->includeFileLocation = include;
}

void ConsoleLogger::setLogFormat(const std::string& format) {
    pImpl->logFormat = format;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
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

// Utility function to replace all occurrences
void ConsoleLogger::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
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
