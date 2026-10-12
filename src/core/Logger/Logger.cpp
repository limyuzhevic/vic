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
    // Validate entry
    if (!entry.message.empty()) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        try {
            if (entry.level < pImpl->level) {
                return;
            }
            
            std::ostringstream oss;
            oss << "[" << levelToString(entry.level) << "] ";
            
            // Add timestamp
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            
            oss << " " << entry.message;
            
            std::string output = oss.str();
            
            // Validate output before writing
            if (output.empty()) {
                return;
            }
            
            // Console output with exception safety
            try {
                if (pImpl->useColors) {
                    std::cout << levelToColor(entry.level) << output << "\033[0m\n";
                } else {
                    std::cout << output << "\n";
                }
            } catch (const std::exception& e) {
                // Fallback to basic output if color formatting fails
                std::cout << "[ERROR] Log output failed: " << e.what() << std::endl;
            }
            
            // File output if set with exception safety
            if (pImpl->fileOut.is_open()) {
                try {
                    pImpl->fileOut << output << "\n";
                    pImpl->fileOut.flush();
                } catch (const std::exception& e) {
                    // If file logging fails, continue without it
                    // Don't throw to preserve existing error handling semantics
                }
            }
            
        } catch (const std::exception& e) {
            // Log logger internal error to stderr as fallback
            std::cerr << "[CRITICAL] ConsoleLogger internal error: " << e.what() << std::endl;
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
    try {
        // Validate filepath
        if (filepath.empty()) {
            Logger::getGlobal().warning("ConsoleLogger::setOutputFile: Empty filepath provided");
            return;
        }
        
        // Close existing file if open
        closeOutputFile();
        
        // Open new file with exception safety
        pImpl->fileOut.open(filepath, std::ios::app);
        
        if (!pImpl->fileOut.is_open()) {
            Logger::getGlobal().error("ConsoleLogger::setOutputFile: Failed to open file for writing: " + filepath);
        }
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("ConsoleLogger::setOutputFile: Exception opening file " + filepath + ": " + std::string(e.what()));
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
    try {
        // Validate input parameters
        if (message.empty()) {
            Logger::getGlobal().warning("Logger::log: Empty message provided");
            return;
        }
        
        // Validate level
        if (static_cast<int>(level) < 0 || static_cast<int>(level) > 4) {
            Logger::getGlobal().error("Logger::log: Invalid log level: " + std::to_string(static_cast<int>(level)));
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file.empty() ? "unknown" : file;
        entry.line = line >= 0 ? line : 0;
        entry.function = function.empty() ? "unknown" : function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Validate log entry
        if (entry.file.empty() || entry.function.empty()) {
            Logger::getGlobal().warning("Logger::log: Invalid log entry fields");
            return;
        }
        
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                Logger::getGlobal().error("Logger::log: Failed to log to logger: " + std::string(e.what()));
            }
        }
        
    } catch (const std::exception& e) {
        // Last resort error logging to stderr
        std::cerr << "[CRITICAL] Logger::log internal error: " << e.what() << std::endl;
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
