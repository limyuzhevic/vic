#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // File rotation settings
    std::string logFile;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    int maxFiles = 5;
    size_t currentFileSize = 0;
    
    // Log formatting
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    std::string colorFormat = true;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

std::string ConsoleLogger::getTimestamp(const std::chrono::system_clock::time_point& timestamp, const std::string& format) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    
    std::tm tm_time;
    std::tm tm_time_local;
    
    // Thread-safe localtime conversion
    std::istringstream ss(std::to_string(time));
    if (ss >> std::get_time(&tm_time_local, "%Y-%m-%d %H:%M:%S")) {
        tm_time = tm_time_local;
    } else {
        // Fallback to localtime with thread safety
        std::time_t time_t_val = time;
        if (std::localtime_r(&time_t_val, &tm_time) == nullptr) {
            // If localtime fails, use empty struct
            std::memset(&tm_time, 0, sizeof(tm_time));
        }
    }
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, format.c_str());
    return oss.str();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    // Format timestamp
    std::string timestamp = getTimestamp(entry.timestamp, pImpl->timestampFormat);
    
    // Add thread ID for multi-threaded debugging
    std::ostringstream threadOss;
    threadOss << std::this_thread::get_id();
    
    // Structured logging format
    oss << "[" << levelToString(entry.level) << "] "
        << timestamp << " " << threadOss.str() << " "
        << entry.message;
    
    // Add file info if provided
    if (!entry.file.empty()) {
        oss << " [" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        oss << "]";
    }
    
    // Add function info if provided
    if (!entry.function.empty()) {
        oss << " <" << entry.function << ">";
    }
    
    std::string output = oss.str();
    
    // File rotation check
    if (!pImpl->logFile.empty() && !pImpl->fileOut.is_open()) {
        pImpl->fileOut.open(pImpl->logFile, std::ios::app);
    }
    
    // Check file size and rotate if needed
    if (pImpl->fileOut.is_open() && pImpl->currentFileSize >= pImpl->maxFileSize) {
        pImpl->fileOut.close();
        
        // Rotate existing files
        for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
            std::string from = pImpl->logFile;
            std::string to = pImpl->logFile;
            if (i < pImpl->maxFiles - 1) {
                to = pImpl->logFile + "." + std::to_string(i + 1);
            }
            
            if (i == pImpl->maxFiles - 1) {
                // Delete the oldest file
                std::remove(to.c_str());
            } else {
                // Rename the next file to current position
                std::rename(to.c_str(), (to + ".bak").c_str());
            }
            
            if (i > 0) {
                // Rename the file backward
                std::string nextFile = pImpl->logFile + "." + std::to_string(i);
                std::rename(nextFile.c_str(), to.c_str());
            }
        }
        
        // Create new file
        pImpl->fileOut.open(pImpl->logFile, std::ios::trunc);
        pImpl->currentFileSize = 0;
    }
    
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
        pImpl->currentFileSize += output.size() + 1;  // +1 for newline
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
    pImpl->logFile = filepath;
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Open file for appending
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        // Get current file size
        pImpl->fileOut.seekp(0, std::ios::end);
        pImpl->currentFileSize = pImpl->fileOut.tellp();
    }
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
