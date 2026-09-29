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
    
    // File output with improved error handling
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to write to log file: " + std::string(e.what()));
            // Attempt to recover by closing and reopening the file
            pImpl->fileOut.close();
            // Note: In a real implementation, you might want to retry opening the file
        }
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
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid file path: filepath is empty");
        return;
    }
    
    try {
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Open file with append mode and error checking
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            NLM_LOG_ERROR("Failed to open log file for writing: " + filepath);
            return;
        }
        
        // Log successful file open
        NLM_LOG_INFO("Opened log file: " + filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception opening log file " + filepath + ": " + std::string(e.what()));
    }
}

void ConsoleLogger::closeOutputFile() {
    try {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.flush();
            pImpl->fileOut.close();
            NLM_LOG_INFO("Closed log file");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception closing log file: " + std::string(e.what()));
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
    bool initialized;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

Logger::~Logger() {
    try {
        // Clean up all loggers
        clearLoggers();
        pImpl->initialized = false;
    } catch (const std::exception& e) {
        // Log the exception during destruction
        std::cerr << "Exception during Logger destruction: " << e.what() << std::endl;
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Attempted to add null logger");
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        pImpl->loggers.push_back(logger);
        pImpl->initialized = true;
        NLM_LOG_INFO("Added logger to global logger");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception adding logger: " + std::string(e.what()));
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Close any file handles in loggers
        for (auto& logger : pImpl->loggers) {
            if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
                consoleLogger->closeOutputFile();
            }
        }
        
        pImpl->loggers.clear();
        pImpl->initialized = false;
        NLM_LOG_INFO("Cleared all loggers");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception clearing loggers: " + std::string(e.what()));
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        for (auto& logger : pImpl->loggers) {
            if (logger) {
                logger->setLevel(level);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception setting logger level: " + std::string(e.what()));
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Validate log entry
        if (entry.message.empty()) {
            NLM_LOG_WARNING("Empty log message in " + function);
        }
        
        for (auto& logger : pImpl->loggers) {
            if (logger) {
                logger->log(entry);
            } else {
                NLM_LOG_ERROR("Null logger encountered in global logger");
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in global logger log: " + std::string(e.what()));
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
    
    try {
        for (auto& logger : pImpl->loggers) {
            if (logger && logger->getLevel() <= level) {
                return true;
            }
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception checking if logger is enabled: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
