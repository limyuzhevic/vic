#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string outputFilepath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize with current time
    NLM_LOG_DEBUG("ConsoleLogger initialized");
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    NLM_LOG_DEBUG("ConsoleLogger initialized with level: " + levelToString(level));
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
    NLM_LOG_DEBUG("ConsoleLogger destroyed");
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp with better formatting
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::tm tm_buf;
        localtime_r(&time_t_now, &tm_buf);
        
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") 
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << " " << entry.message;
        
        std::string output = oss.str();
        
        // Console output with better error handling
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        // File output with error handling
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        }
        
    } catch (const std::exception& e) {
        // Fallback to simple logging if anything goes wrong
        std::cerr << "[LOGGER_ERROR] Failed to log entry: " << e.what() << std::endl;
        std::cerr << "[ORIGINAL_ENTRY] " << entry.message << std::endl;
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    try {
        pImpl->level = level;
        NLM_LOG_DEBUG("ConsoleLogger level set to: " + levelToString(level));
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to set level: " << e.what() << std::endl;
    }
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    try {
        pImpl->useColors = useColors;
        NLM_LOG_DEBUG("ConsoleLogger colors " + std::string(useColors ? "enabled" : "disabled"));
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to set colors: " << e.what() << std::endl;
    }
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    try {
        // Close existing file if open
        closeOutputFile();
        
        // Set new file
        pImpl->fileOut.open(filepath, std::ios::app);
        pImpl->outputFilepath = filepath;
        
        if (pImpl->fileOut.is_open()) {
            NLM_LOG_INFO("ConsoleLogger output file set to: " + filepath);
        } else {
            NLM_LOG_WARNING("ConsoleLogger failed to open file: " + filepath);
        }
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to set output file: " << e.what() << std::endl;
        closeOutputFile();
    }
}

void ConsoleLogger::closeOutputFile() {
    try {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
            NLM_LOG_DEBUG("ConsoleLogger output file closed");
        }
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to close output file: " << e.what() << std::endl;
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
    try {
        if (!logger) {
            std::cerr << "[LOGGER_ERROR] Cannot add null logger" << std::endl;
            return;
        }
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
        NLM_LOG_DEBUG("Logger added successfully");
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to add logger: " << e.what() << std::endl;
    }
}

void Logger::clearLoggers() {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.clear();
        NLM_LOG_DEBUG("All loggers cleared");
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to clear loggers: " << e.what() << std::endl;
    }
}

void Logger::setLevel(LogLevel level) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            logger->setLevel(level);
        }
        NLM_LOG_DEBUG("Logger level set to: " + std::to_string(static_cast<int>(level)));
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to set logger level: " << e.what() << std::endl;
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
        
        for (auto& logger : pImpl->loggers) {
            logger->log(entry);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to log message: " << e.what() << std::endl;
        std::cerr << "[MESSAGE] " << message << std::endl;
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
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            if (logger->getLevel() <= level) {
                return true;
            }
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER_ERROR] Failed to check if level is enabled: " << e.what() << std::endl;
        return false;
    }
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
        std::cerr << "[LOGGER_ERROR] Failed to log stream message: " << e.what() << std::endl;
        std::cerr << "[MESSAGE] " << oss_.str() << std::endl;
    }
}

} // namespace nlm
