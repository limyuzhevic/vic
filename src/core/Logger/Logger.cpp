#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <exception>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t fileSize = 0;
    bool logRotation = false;
    std::string backupPrefix = "backup_";
    bool writeErrorsToStderr = true;
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
    
    // Add timestamp with milliseconds
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm_time;
    std::localtime_s(&tm_time, &time);
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S"); 
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    oss << " " << entry.message;
    
    // Add file and line info if available
    if (!entry.file.empty()) {
        oss << " (" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        oss << ")";
    }
    
    // Add function info if available
    if (!entry.function.empty()) {
        oss << " [" << entry.function << "]";
    }
    
    std::string output = oss.str();
    
    // Handle file output with rotation
    if (pImpl->fileOut.is_open()) {
        // Check if we need to rotate the file
        if (pImpl->logRotation && (pImpl->fileSize + output.size() + 1 > pImpl->maxFileSize)) {
            pImpl->fileOut.close();
            // Rotate file
            std::string backupPath = pImpl->backupPrefix + std::to_string(time) + ".log";
            std::rename("logger.log", backupPath.c_str());
            pImpl->fileOut.open("logger.log", std::ios::app);
            pImpl->fileSize = 0;
        }
        
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
            pImpl->fileSize += output.size() + 1;
        } catch (const std::exception& e) {
            if (pImpl->writeErrorsToStderr) {
                std::cerr << "LOG ERROR: Failed to write to log file: " << e.what() << std::endl;
            }
        }
    }
    
    // Console output
    try {
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "LOG ERROR: Failed to write to console: " << e.what() << std::endl;
        std::cerr << "Original message: " << entry.message << std::endl;
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
    try {
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            if (pImpl->writeErrorsToStderr) {
                std::cerr << "WARNING: Failed to open log file: " << filepath << std::endl;
            }
        }
    } catch (const std::exception& e) {
        if (pImpl->writeErrorsToStderr) {
            std::cerr << "ERROR: Failed to open log file " << filepath << ": " << e.what() << std::endl;
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut.close();
        } catch (const std::exception& e) {
            if (pImpl->writeErrorsToStderr) {
                std::cerr << "ERROR: Failed to close log file: " << e.what() << std::endl;
            }
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
    std::vector<LogEntry> recentLogs;  // Store recent log entries for debugging
    size_t maxRecentLogs = 1000;
    std::chrono::system_clock::time_point lastCleanup;
    bool exceptionSafetyEnabled = true;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastCleanup = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        error("Attempted to add null logger");
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
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Check if this level is enabled before creating log entry (performance optimization)
    if (!isEnabled(level)) {
        return;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Store recent log for debugging
        pImpl->recentLogs.push_back(entry);
        if (pImpl->recentLogs.size() > pImpl->maxRecentLogs) {
            pImpl->recentLogs.erase(pImpl->recentLogs.begin());
        }
        
        // Cleanup old logs periodically
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::minutes>(now - pImpl->lastCleanup).count() > 5) {
            pImpl->recentLogs.clear();
            pImpl->lastCleanup = now;
        }
        
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                // If a logger fails, log the error to stderr as fallback
                std::cerr << "CRITICAL: Logger failed to log message: " << e.what() << std::endl;
                std::cerr << "Failed message: " << message << std::endl;
                std::cerr << "Level: " << static_cast<int>(level) << std::endl;
                if (!file.empty()) std::cerr << "File: " << file << std::endl;
                if (line > 0) std::cerr << "Line: " << line << std::endl;
                if (!function.empty()) std::cerr << "Function: " << function << std::endl;
            }
        }
    } catch (const std::exception& e) {
        // If there's an exception during logging, try to log it directly to stderr
        std::cerr << "CRITICAL: Exception in logging system: " << e.what() << std::endl;
        std::cerr << "Failed message: " << message << std::endl;
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Debug)) {
        log(LogLevel::Debug, message, file, line, function);
    }
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Info)) {
        log(LogLevel::Info, message, file, line, function);
    }
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Warning)) {
        log(LogLevel::Warning, message, file, line, function);
    }
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Error)) {
        // Enhance error messages with context
        std::string enhancedMessage = message;
        if (!file.empty()) {
            enhancedMessage += " [" + file;
            if (line > 0) enhancedMessage += ":" + std::to_string(line);
            enhancedMessage += "]";
        }
        if (!function.empty()) {
            enhancedMessage += " [" + function + "]";
        }
        
        log(LogLevel::Error, enhancedMessage, file, line, function);
    }
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Critical)) {
        // For critical errors, ensure they're always logged even if other loggers fail
        std::string enhancedMessage = "CRITICAL: " + message;
        if (!file.empty()) {
            enhancedMessage += " [" + file;
            if (line > 0) enhancedMessage += ":" + std::to_string(line);
            enhancedMessage += "]";
        }
        if (!function.empty()) {
            enhancedMessage += " [" + function + "]";
        }
        enhancedMessage += " - SYSTEM FAILURE IMMINENT";
        
        try {
            // Log directly to stderr first (guaranteed to work)
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::cerr << "[" << levelToString(LogLevel::Critical) << "] ";
            std::cerr << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            std::cerr << " " << enhancedMessage << std::endl;
            
            // Then log through normal system
            log(LogLevel::Critical, enhancedMessage, file, line, function);
        } catch (...) {
            // Ultimate fallback - nothing should fail to log a critical error
            std::cerr << "CRITICAL: " << message << " (Failed to log via normal system)" << std::endl;
        }
    }
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
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "FATAL: LogStream destructor failed: " << e.what() << std::endl;
        std::cerr << "Failed stream content: " << oss_.str() << std::endl;
    }
}

} // namespace nlm