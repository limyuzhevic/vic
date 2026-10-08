#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <filesystem>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level;
    bool useColors;
    std::ofstream outputFile;
    std::string filepath;
    
    Impl() : level(LogLevel::Info), useColors(true) {}
    explicit Impl(LogLevel l) : level(l), useColors(true) {}
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>(level)) {}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check if this level is enabled
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::string timestamp = "[" + std::to_string(std::chrono::system_clock::to_time_t(entry.timestamp)) + "]";
    std::string levelStr = levelToString(entry.level);
    std::string color = levelToColor(entry.level);
    
    std::string reset = "\033[0m";
    
    // Build message
    std::ostringstream oss;
    oss << color << "[" << timestamp << "]" << reset << " "
        << "[" << levelStr << "]" << reset << " "
        << entry.message;
    
    if (!entry.file.empty()) {
        oss << " " << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    
    std::string finalMessage = oss.str();
    
    // Output to console
    std::cout << finalMessage << std::endl;
    
    // Output to file if enabled
    if (!pImpl->filepath.empty() && pImpl->outputFile.is_open()) {
        pImpl->outputFile << finalMessage << std::endl;
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
    pImpl->filepath = filepath;
    pImpl->outputFile.open(filepath, std::ios::app);
    if (!pImpl->outputFile.is_open()) {
        // Try to create directory if it doesn't exist
        size_t pos = filepath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string dir = filepath.substr(0, pos);
            std::filesystem::create_directories(dir);
            pImpl->outputFile.open(filepath, std::ios::app);
        }
    }
}

void ConsoleLogger::closeOutputFile() {
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
        case LogLevel::Info: return "\033[32m";   // Green
        case LogLevel::Warning: return "\033[33m"; // Yellow
        case LogLevel::Error: return "\033[31m";   // Red
        case LogLevel::Critical: return "\033[41m\033[37m"; // Red background, white text
        default: return "";
    }
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    LogLevel globalLevel;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->globalLevel = LogLevel::Info;
    // Add default console logger
    pImpl->loggers.push_back(std::make_shared<ConsoleLogger>(LogLevel::Info));
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (logger) {
        pImpl->loggers.push_back(logger);
    }
}

void Logger::clearLoggers() {
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    pImpl->globalLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (level < pImpl->globalLevel) {
        return;
    }
    
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
    return level >= pImpl->globalLevel;
}

Logger& Logger::getGlobal() {
    static std::shared_ptr<Logger> globalLogger = std::make_shared<Logger>();
    return *globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        Logger::globalLogger_ = logger;
    }
}

std::shared_ptr<Logger> Logger::globalLogger_ = std::make_shared<Logger>();

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    Logger::getGlobal().log(level, "", file, line, function);
}

LogStream::~LogStream() {
    // Auto-end the stream by destructing
}

} // namespace nlm
