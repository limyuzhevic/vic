#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <ctime>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filename;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t fileSize = 0;
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
    
    // Add timestamp with improved formatting
    auto timePoint = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    
    std::tm tm_time;
    localtime_s(&tm_time, &timePoint);
    
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    oss << " " << entry.message;
    
    if (!entry.file.empty()) {
        oss << " [" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        oss << "]";
    }
    
    if (!entry.function.empty()) {
        oss << " [" << entry.function << "]";
    }
    
    std::string output = oss.str();
    
    // Console output with enhanced formatting
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with size management
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileSize += output.size() + 1; // +1 for newline
        
        // Rotate file if it gets too large
        if (pImpl->fileSize >= pImpl->maxFileSize) {
            pImpl->fileOut.close();
            pImpl->fileOut.open(pImpl->filename, std::ios::app);
            pImpl->fileSize = 0;
        }
        
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Open new file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->filename = filepath;
        pImpl->fileSize = 0;
        NLM_LOG_INFO("Logger output file set to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to open log file for writing: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->filename.clear();
        pImpl->fileSize = 0;
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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogLevel minimumLevel;
    bool includeTimestamps;
    bool includeSourceInfo;
    std::string logPattern;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->minimumLevel = LogLevel::Info;
    pImpl->includeTimestamps = true;
    pImpl->includeSourceInfo = true;
    pImpl->logPattern = "[LEVEL] TIMESTAMP MESSAGE [FILE:LINE] [FUNCTION]";
    
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    addLogger(consoleLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_WARNING("Attempted to add null logger");
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minimumLevel = level;
    
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Check if this level should be logged
    if (level < pImpl->minimumLevel) {
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
            // Don't let logging failures break the application
            std::cerr << "Logging error: " << e.what() << std::endl;
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
    return level >= pImpl->minimumLevel;
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
