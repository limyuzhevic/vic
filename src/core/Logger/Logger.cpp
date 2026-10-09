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
    
    // Add timestamp (using gmtime for thread safety)
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    
    // Get current time in UTC format for thread-safe logging
    struct tm* timeInfo = std::gmtime(&time);
    
    // Ensure thread-safe timestamp conversion and check for null
    if (timeInfo != nullptr) {
        oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    } else {
        // Fallback to epoch timestamp if conversion fails
        oss << "1970-01-01 00:00:00";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set (with comprehensive error handling and fallback)
    if (pImpl->fileOut.is_open()) {
        try {
            // Attempt to write to file with error checking
            pImpl->fileOut << output << "\n";
            
            // Verify write succeeded
            if (pImpl->fileOut.fail()) {
                std::cerr << "[WARNING] Failed to write to log file\n";
                return;  // Exit to prevent further operations on failed stream
            }
            
            // Attempt to flush with error checking
            pImpl->fileOut.flush();
            
            // Verify flush succeeded
            if (pImpl->fileOut.fail()) {
                std::cerr << "[WARNING] Failed to flush log file\n";
            }
        } catch (const std::exception& e) {
            // Log file write errors to console as fallback (ensuring logging continues)
            std::cerr << "[ERROR] Exception writing to log file: " << e.what() << std::endl;
        } catch (...) {
            // Catch-all for unknown exceptions during file operations
            std::cerr << "[ERROR] Unknown exception writing to log file\n";
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Attempt to open the output file in append mode
        pImpl->fileOut.open(filepath, std::ios::app);
        
        // Verify file opened successfully
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "[ERROR] Failed to open log file: " << filepath << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions that might occur during file operations
        std::cerr << "[ERROR] Exception while opening log file '" << filepath 
                  << "': " << e.what() << std::endl;
    } catch (...) {
        // Handle any unknown exceptions
        std::cerr << "[ERROR] Unknown exception while opening log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
            
            if (pImpl->fileOut.is_open()) {
                std::cerr << "[WARNING] Failed to close log file\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while closing log file: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR] Unknown exception while closing log file\n";
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

// Constructor
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    // Initialize with default log level
    pImpl->level = level;  // Set minimum log level for filtering
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Process log entry with comprehensive error handling
    // First filter by log level
    if (entry.level < pImpl->level) {
        return;  // Skip if below minimum log level
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp (using gmtime for thread safety)
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    
    // Get current time in UTC format for thread-safe logging
    struct tm* timeInfo = std::gmtime(&time);
    
    // Ensure thread-safe timestamp conversion and check for null
    if (timeInfo != nullptr) {
        oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    } else {
        // Fallback to epoch timestamp if conversion fails
        oss << "1970-01-01 00:00:00";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with color support
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set (with comprehensive error handling and fallback)
    if (pImpl->fileOut.is_open()) {
        try {
            // Attempt to write to file with error checking
            pImpl->fileOut << output << "\n";
            
            // Verify write succeeded
            if (pImpl->fileOut.fail()) {
                std::cerr << "[WARNING] Failed to write to log file\n";
                return;  // Exit to prevent further operations on failed stream
            }
            
            // Attempt to flush with error checking
            pImpl->fileOut.flush();
            
            // Verify flush succeeded
            if (pImpl->fileOut.fail()) {
                std::cerr << "[WARNING] Failed to flush log file\n";
            }
        } catch (const std::exception& e) {
            // Log file write errors to console as fallback (ensuring logging continues)
            std::cerr << "[ERROR] Exception writing to log file: " << e.what() << std::endl;
        } catch (...) {
            // Catch-all for unknown exceptions during file operations
            std::cerr << "[ERROR] Unknown exception writing to log file\n";
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    // Set minimum log level for this logger instance
    // Messages below this level will be filtered out
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    // Get current minimum log level
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    // Enable or disable colored console output
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    // Check if colors are enabled
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    // Set output file for log messages (in addition to console)
    // Opens file in append mode, creates if doesn't exist
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Attempt to open the output file in append mode
        pImpl->fileOut.open(filepath, std::ios::app);
        
        // Verify file opened successfully
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "[ERROR] Failed to open log file: " << filepath << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        // Handle any exceptions that might occur during file operations
        std::cerr << "[ERROR] Exception while opening log file '" << filepath 
                  << "': " << e.what() << std::endl;
    } catch (...) {
        // Handle any unknown exceptions
        std::cerr << "[ERROR] Unknown exception while opening log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    // Close the output file if open
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
            
            if (pImpl->fileOut.is_open()) {
                std::cerr << "[WARNING] Failed to close log file\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while closing log file: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR] Unknown exception while closing log file\n";
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    // Convert log level enum to human-readable string
    // Used for console and file output formatting
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
    // Convert log level enum to ANSI color code for console output
    // Empty string for no color
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
    // Add a logger instance to the logging system
    // The logger will receive all log messages passed to Logger::log()
    // Thread-safe operation using mutex protection
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    // Remove all logger instances from the logging system
    // Thread-safe operation using mutex protection
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    // Set minimum log level for all registered loggers
    // All loggers will filter messages at or above this level
    // Thread-safe operation using mutex protection
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Process and forward log entry to all registered loggers
    // Creates LogEntry with timestamp and metadata, then delegates to each logger
    // Thread-safe operation using mutex protection
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;                    // Log severity level
    entry.message = message;               // Log message text
    entry.file = file;                     // Source file path
    entry.line = line;                     // Line number in source file
    entry.function = function;             // Function name where log occurred
    entry.timestamp = std::chrono::system_clock::now();  // Log entry timestamp
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    // Log debug level message with location information
    // Convenience method for debug logging with file/line/function metadata
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    // Log informational message with location information
    // Convenience method for info logging with file/line/function metadata
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    // Log warning message with location information
    // Convenience method for warning logging with file/line/function metadata
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    // Log error message with location information
    // Convenience method for error logging with file/line/function metadata
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    // Log critical message with location information
    // Convenience method for critical logging with file/line/function metadata
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    // Check if any registered logger will output messages at given level
    // Returns true if at least one logger has level <= specified level
    // Thread-safe operation using mutex protection
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    // Get reference to global logger instance
    // Static instance ensures single global logger throughout application
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
