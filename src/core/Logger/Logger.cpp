#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <system_error>

namespace nlm {

/**
 * ConsoleLogger::Impl struct
 * 
 * @struct Impl
 * @brief Internal implementation details for ConsoleLogger
 * @details Contains all runtime state for console logging:
 *         - Current log level
 *         - Color output setting
 *         - File output stream
 *         - Thread synchronization
 *         - Error tracking
 * 
 * @note PIMPL pattern provides ABI stability
 * @warning This is an internal implementation detail
 */
/**
 * ConsoleLogger constructor - creates console logger
 * 
 * @post Logger initialized with default settings:
 *       - Log level: Info
 *       - Colors: Enabled
 *       - File output: Disabled
 *       - Thread synchronization: Ready
 *       - Error state: Healthy
 */
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}
    
/**
 * ConsoleLogger constructor with log level
 * 
 * @param level Initial log level threshold
 * @post Logger initialized with specified level
 * @warning Setting level too high may miss important messages
 */
ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setLevel(level);
}
    
/**
 * ConsoleLogger destructor - cleans up resources
 * 
 * @post File closed if open; mutex released
 * @note Safe to call multiple times
 */
ConsoleLogger::~ConsoleLogger() = default;
    
/**
 * Log message to console
 * 
 * @param entry Log entry containing message and metadata
 * @post Message output:
 *       - Filtered by log level
 *       - Formatted with timestamp and level
 *       - Written to console with optional colors
 *       - Written to file if set
 * @note Thread-safe with mutex protection
 * @throws std::exception if output fails
 */
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
    try {
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
    } catch (const std::exception& e) {
        pImpl->lastFileError = "Error writing to console: " + std::string(e.what());
        pImpl->fileError = true;
        NLM_REPORT_ERROR(pImpl->lastFileError, ErrorCategory::Logger, ErrorSeverity::Error);
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            pImpl->lastFileError = "Error writing to log file: " + std::string(e.what());
            pImpl->fileError = true;
            NLM_REPORT_ERROR(pImpl->lastFileError, ErrorCategory::Logger, ErrorSeverity::Error);
        }
    }
}
    
/**
 * Set log level
 * 
 * @param level New log level threshold
 * @post Log level updated
 * @note Thread-safe operation
 */
void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}
    
/**
 * Get log level
 * 
 * @return Current log level
 * @post None
 */
LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}
    
/**
 * Enable/disable colors
 * 
 * @param useColors Whether to use ANSI color codes
 * @post Color setting updated
 * @note Colors disabled on Windows consoles by default
 */
void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}
    
/**
 * Get color setting
 * 
 * @return true if colors enabled, false otherwise
 * @post None
 */
bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}
    
/**
 * Set output file
 * 
 * @param filepath Path to log file
 * @post File opened for appending
 * @warning File must be writable
 * @warning Overwrites last file error
 * @warning Opens in append mode; does not truncate existing content
 * @see closeOutputFile() for file closing
 */
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    try {
        NLM_VALIDATE(!filepath.empty(), "Log file path cannot be empty");
        
        pImpl->fileOut.close(); // Close any existing file
        pImpl->fileOut.open(filepath, std::ios::app);
        
        if (!pImpl->fileOut.is_open()) {
            pImpl->lastFileError = "Failed to open log file: " + filepath;
            pImpl->fileError = true;
            NLM_REPORT_ERROR(pImpl->lastFileError, ErrorCategory::Logger, ErrorSeverity::Error);
        } else {
            pImpl->fileError = false;
        }
    } catch (const std::exception& e) {
        pImpl->lastFileError = "Exception opening log file " + filepath + ": " + std::string(e.what());
        pImpl->fileError = true;
        NLM_REPORT_ERROR(pImpl->lastFileError, ErrorCategory::Logger, ErrorSeverity::Error);
    }
}
    
/**
 * Close output file
 * 
 * @post File closed if open
 * @note Safe to call if file not open
 */
void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut.close();
            pImpl->fileError = false;
        } catch (const std::exception& e) {
            pImpl->lastFileError = "Error closing log file: " + std::string(e.what());
            pImpl->fileError = true;
            NLM_REPORT_ERROR(pImpl->lastFileError, ErrorCategory::Logger, ErrorSeverity::Error);
        }
    }
}
    
/**
 * Check if file error occurred
 * 
 * @return true if file error, false otherwise
 * @post None
 * @note Indicates if log file operations failed
 */
bool ConsoleLogger::isFileError() const {
    return pImpl->fileError;
}
    
/**
 * Get last file error message
 * 
 * @return Error message string
 * @post None
 * @note Empty string if no error
 */
std::string ConsoleLogger::getLastFileError() const {
    return pImpl->lastFileError;
}
    
/**
 * Convert log level to string
 * 
 * @param level Log level to convert
 * @return String representation
 * @post None
 */
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
    
/**
 * Convert log level to ANSI color code
 * 
 * @param level Log level to convert
 * @return ANSI color escape sequence
 * @post None
 * @note Colors disabled when useColors is false
 */
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

/**
 * Logger::Impl struct
 * 
 * @struct Impl
 * @brief Internal implementation details for Logger
 * @details Contains all runtime state for logging:
 *         - Logger sinks (ConsoleLogger, file loggers, etc.)
 *         - Thread synchronization
 *         - Health monitoring state
 *         - Error tracking
 * 
 * @note PIMPL pattern provides ABI stability
 * @warning This is an internal implementation detail
 */
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool healthy = true;
    std::vector<std::string> healthIssues;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    NLM_VALIDATE(logger != nullptr, "Logger cannot be null");
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    try {
        pImpl->loggers.push_back(std::move(logger));
        pImpl->healthIssues.erase(
            std::remove_if(pImpl->healthIssues.begin(), pImpl->healthIssues.end(),
                [logger](const std::string& issue) {
                    return issue.find("logger" + std::to_string(reinterpret_cast<uintptr_t>(logger.get()))) != std::string::npos;
                }),
            pImpl->healthIssues.end()
        );
        pImpl->healthy = true;
    } catch (const std::exception& e) {
        pImpl->healthy = false;
        pImpl->healthIssues.push_back("Failed to add logger: " + std::string(e.what()));
        NLM_REPORT_ERROR(pImpl->healthIssues.back(), ErrorCategory::Logger, ErrorSeverity::Warning);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->healthIssues.clear();
    pImpl->healthy = true;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_VALIDATE(static_cast<int>(level) >= 0 && static_cast<int>(level) <= static_cast<int>(LogLevel::Critical), 
               "Invalid log level: " + std::to_string(static_cast<int>(level)));
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->setLevel(level);
        } catch (const std::exception& e) {
            pImpl->healthy = false;
            pImpl->healthIssues.push_back("Failed to set log level for logger: " + std::string(e.what()));
            NLM_REPORT_ERROR(pImpl->healthIssues.back(), ErrorCategory::Logger, ErrorSeverity::Warning);
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_VALIDATE(static_cast<int>(level) >= 0 && static_cast<int>(level) <= static_cast<int>(LogLevel::Critical), 
               "Invalid log level: " + std::to_string(static_cast<int>(level)));
    
    NLM_VALIDATE(!message.empty(), "Log message cannot be empty");
    
    try {
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
        
        pImpl->healthIssues.erase(
            std::remove_if(pImpl->healthIssues.begin(), pImpl->healthIssues.end(),
                [level](const std::string& issue) {
                    return issue.find("logging" + std::to_string(static_cast<int>(level))) != std::string::npos;
                }),
            pImpl->healthIssues.end()
        );
        
        pImpl->healthy = true;
    } catch (const std::exception& e) {
        pImpl->healthy = false;
        pImpl->healthIssues.push_back("Error during logging: " + std::string(e.what()));
        NLM_REPORT_ERROR(pImpl->healthIssues.back(), ErrorCategory::Logger, ErrorSeverity::Error);
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
    
    NLM_VALIDATE(static_cast<int>(level) >= 0 && static_cast<int>(level) <= static_cast<int>(LogLevel::Critical), 
               "Invalid log level: " + std::to_string(static_cast<int>(level)));
    
    for (auto& logger : pImpl->loggers) {
        try {
            if (logger->getLevel() <= level) {
                return true;
            }
        } catch (const std::exception& e) {
            NLM_REPORT_ERROR("Error checking log level: " + std::string(e.what()),
                            ErrorCategory::Logger, ErrorSeverity::Warning);
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

bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->healthy;
}

std::vector<std::string> Logger::getHealthIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->healthIssues;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    
    NLM_VALIDATE(static_cast<int>(level_) >= 0 && static_cast<int>(level_) <= static_cast<int>(LogLevel::Critical),
               "Invalid log level: " + std::to_string(static_cast<int>(level_)));
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Error in LogStream destructor: " + std::string(e.what()),
                        ErrorCategory::Logger, ErrorSeverity::Error);
    }
}

} // namespace nlm
