#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string outputFilepath;
    bool fileOutputEnabled = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotation;
    
    Impl() : fileOutputEnabled(false), lastRotation(std::chrono::system_clock::now()) {}
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
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    oss << " [" << entry.file << ":" << entry.line << "] ";
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (pImpl->fileOutputEnabled && pImpl->fileOut.is_open()) {
        // Check file size and rotate if needed
        if (pImpl->currentFileSize + output.size() + 1 > pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1;
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
    // Close current file if open
    if (pImpl->fileOutputEnabled && pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->fileOutputEnabled = false;
        pImpl->currentFileSize = 0;
    }
    
    if (!filepath.empty()) {
        // Try to create the directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::path parentDir = path.parent_path();
        
        try {
            if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
                std::filesystem::create_directories(parentDir);
            }
            
            pImpl->fileOut.open(filepath, std::ios::app);
            if (pImpl->fileOut.is_open()) {
                pImpl->fileOutputEnabled = true;
                pImpl->outputFilepath = filepath;
                
                // Get current file size
                pImpl->currentFileSize = std::filesystem::file_size(filepath);
                
                NLM_LOG_INFO("Log output file set to: " + filepath);
            } else {
                NLM_LOG_ERROR("Failed to open log file: " + filepath);
            }
        } catch (const std::filesystem::filesystem_error& e) {
            NLM_LOG_ERROR(std::string("Filesystem error creating log directory: ") + e.what());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error opening log file: ") + e.what());
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOutputEnabled && pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->fileOutputEnabled = false;
        pImpl->currentFileSize = 0;
        NLM_LOG_INFO("Log output file closed");
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOutputEnabled) return;
    
    // Close current file
    pImpl->fileOut.close();
    
    // Create new file with timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream filename;
    filename << pImpl->outputFilepath << "." << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".log";
    
    // Reopen with new name
    pImpl->fileOut.open(filename.str(), std::ios::trunc);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = 0;
        NLM_LOG_INFO("Log file rotated to: " + filename.str());
    } else {
        // If rotation fails, try to reopen original file
        pImpl->fileOut.open(pImpl->outputFilepath, std::ios::app);
        NLM_LOG_WARNING("Failed to rotate log file, reopened original");
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
    if (!pImpl->useColors) return "";
    
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
    std::string lastError;
    size_t maxLogEntries;
    std::vector<LogEntry> recentLogs;
    bool enableRecentLogs;
    
    Impl() : lastError(), maxLogEntries(1000), enableRecentLogs(false) {}
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        pImpl->lastError = "Cannot add null logger";
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    
    // Remove oldest logger if we have too many
    if (pImpl->loggers.size() > 10) {
        pImpl->loggers.erase(pImpl->loggers.begin());
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->recentLogs.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Send to all loggers
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                pImpl->lastError = std::string("Logger error: ") + e.what();
            }
        }
        
        // Store in recent logs if enabled
        if (pImpl->enableRecentLogs) {
            pImpl->recentLogs.push_back(entry);
            
            // Limit recent logs size
            if (pImpl->recentLogs.size() > pImpl->maxLogEntries) {
                pImpl->recentLogs.erase(pImpl->recentLogs.begin());
            }
        }
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Error logging: ") + e.what();
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
        std::lock_guard<std::mutex> lock(getGlobal().pImpl->mutex);
        getGlobal().pImpl->loggers.clear();
        getGlobal().pImpl->loggers.push_back(logger);
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "LogStream error: " << e.what() << std::endl;
    }
}

} // namespace nlm
