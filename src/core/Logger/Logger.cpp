#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

/**
 * @brief Implementation details for ConsoleLogger
 * 
 * This implementation uses the Pimpl idiom to hide implementation details
 * and provide better encapsulation. The actual implementation is stored
 * in a separate structure to reduce compilation dependencies and improve
 * compilation times.
 * 
 * The ConsoleLogger provides:
 * - Colored console output for different log levels
 * - Optional file logging in addition to console output
 * - Thread-safe logging operations
 * - Configurable log level filtering
 * - Timestamp formatting for log entries
 * 
 * The implementation uses RAII (Resource Acquisition Is Initialization)
 * principles for resource management, particularly for the output file stream.
 */
struct ConsoleLogger::Impl {
    /**
     * @brief Minimum log level for this logger
     * 
     * Only log messages at this level or higher (more severe) are output.
     * For example, if level is LogLevel::Warning, only Warning, Error, and
     * Critical messages are displayed.
     */
    LogLevel level = LogLevel::Info;
    
    /**
     * @brief Whether to use ANSI color codes for console output
     * 
     * When enabled, different log levels are displayed in different colors
     * to help users quickly identify message severity. Colors are disabled
     * by default for compatibility with systems that don't support ANSI colors.
     */
    bool useColors = true;
    
    /**
     * @brief Output file stream for additional log storage
     * 
     * If set, all log messages are also written to this file in addition to
     * the console output. This provides persistent logging for debugging
     * and auditing purposes.
     */
    std::ofstream fileOut;
    
    /**
     * @brief Mutex for thread-safe logging operations
     * 
     * Ensures that concurrent logging operations from multiple threads
     * don't interfere with each other, preventing corrupted output and
     * data races.
     */
    std::mutex mutex;
};

/**
 * @brief Constructor with default settings
 * 
 * Creates a ConsoleLogger with default configuration (LogLevel::Info, colors enabled).
 * The logger is ready to use immediately after construction.
 */
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Constructor with specified log level
 * 
 * Creates a ConsoleLogger with the specified minimum log level. This allows
 * users to filter log output based on severity.
 * 
 * @param level Minimum log level for this logger
 */
ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

/**
 * @brief Destructor - cleans up logger resources
 * 
 * The destructor is defaulted as the implementation uses smart pointers
 * for resource management. The file stream will be automatically closed
 * when the logger is destroyed.
 */
ConsoleLogger::~ConsoleLogger() = default;

/**
 * @brief Log a message to the console and/or file
 * 
 * This method implements the core logging functionality. It:
 * 1. Filters messages based on the logger's minimum level
 * 2. Formats the message with timestamp and log level
 * 3. Outputs to console (with optional colors)
 * 4. Optionally writes to the output file
 * 
 * The method is thread-safe due to the mutex lock.
 * 
 * @param entry The log entry containing message and metadata
 */
void ConsoleLogger::log(const LogEntry& entry) {
    // Skip messages below the minimum log level
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp in a readable format
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with optional colors
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if output file is configured
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

/**
 * @brief Set the minimum log level
 * 
 * Configures the logger to only output messages at or above the specified level.
 * This is useful for reducing log noise in production environments.
 * 
 * @param level Minimum log level (Debug, Info, Warning, Error, or Critical)
 */
void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

/**
 * @brief Get the current minimum log level
 * 
 * @return Current minimum log level
 */
LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

/**
 * @brief Enable or disable ANSI color output
 * 
 * Colors make log messages more readable by allowing quick visual
 * identification of message severity. However, they may not work correctly
 * on all terminals and can cause issues in some logging systems.
 * 
 * @param useColors true to enable colors, false to disable
 */
void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

/**
 * @brief Check if colors are enabled
 * 
 * @return true if colors are enabled, false otherwise
 */
bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

/**
 * @brief Set the output file for log messages
 * 
 * Redirects log output to the specified file in addition to (or instead of)
 * console output. The file is opened in append mode, so multiple runs will
 * add to the same log file.
 * 
 * @param filepath Path to the log file
 */
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

/**
 * @brief Close the output file
 * 
 * Closes the log file stream. After calling this method, log messages will
 * only go to the console (unless another file is set).
 */
void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

/**
 * @brief Convert log level to human-readable string
 * 
 * Provides short, consistent abbreviations for log levels suitable for
 * concise display. This is used in the log message formatting.
 * 
 * @param level The log level to convert
 * @return Human-readable string representation
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
 * @brief Convert log level to ANSI color code
 * 
 * Returns the appropriate ANSI escape sequence for coloring console output
 * based on log level. Colors are chosen for good visibility and contrast.
 * 
 * @param level The log level to get color for
 * @return ANSI color escape sequence or empty string if colors disabled
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
 * @brief Logger implementation details
 * 
 * This structure stores the implementation details of the main Logger class,
 * including the collection of logger sinks and thread synchronization primitives.
 * 
 * The Logger class acts as a facade that manages multiple logger implementations
 * (such as ConsoleLogger) and provides a unified interface for logging operations.
 * This allows for flexible logging configurations where different parts of
 * the application can use different logging strategies.
 */
struct Logger::Impl {
    /**
     * @brief Collection of logger sinks
     * 
     * Each logger sink is responsible for handling log messages in a specific
     * way (console output, file output, network logging, etc.). The main
     * Logger class distributes log messages to all registered sinks.
     */
    std::vector<std::shared_ptr<ILogger>> loggers;
    
    /**
     * @brief Mutex for thread-safe logger management
     * 
     * Protects concurrent access to the loggers vector to prevent race conditions
     * when adding, removing, or modifying loggers from multiple threads.
     */
    std::mutex mutex;
};

/**
 * @brief Constructor with default initialization
 * 
 * Creates a Logger instance with no sinks. Loggers must be added before
 * logging can occur. This allows for flexible logger configuration.
 */
Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

/**
 * @brief Destructor - cleans up logger resources
 * 
 * The destructor is defaulted as the implementation uses smart pointers
 * for resource management.
 */
Logger::~Logger() = default;

/**
 * @brief Add a logger sink to the collection
 * 
 * Registers a new logger implementation that will receive all log messages
 * sent through this Logger. This allows for multiple logging destinations
 * (console, file, network, etc.) from a single point of configuration.
 * 
 * @param logger Shared pointer to the logger implementation to add
 */
void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

/**
 * @brief Remove all logger sinks
 * 
 * Clears the collection of logger sinks, effectively disabling all logging
 * through this Logger instance. This is useful for temporarily disabling
 * logging or cleaning up before reinitialization.
 */
void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

/**
 * @brief Set the minimum log level for all loggers
 * 
 * Configures all registered logger sinks to only output messages at or above
 * the specified level. This provides a global filter for log verbosity.
 * 
 * @param level Minimum log level for all loggers
 */
void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

/**
 * @brief Log a message to all registered loggers
 * 
 * This is the core logging method that creates a LogEntry with timestamp
 * and file/function information, then distributes it to all registered
 * logger sinks. This ensures consistent logging across all configured
 * destinations.
 * 
 * @param level The severity level of the log message
 * @param message The actual log message content
 * @param file Source file name (optional, defaults to empty string)
 * @param line Line number in source file (optional, defaults to 0)
 * @param function Function name (optional, defaults to empty string)
 */
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

/**
 * @brief Convenience method for debug-level logging
 * 
 * @param message The debug message
 * @param file Source file name (optional)
 * @param line Line number (optional)
 * @param function Function name (optional)
 */
void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

/**
 * @brief Convenience method for info-level logging
 * 
 * @param message The info message
 * @param file Source file name (optional)
 * @param line Line number (optional)
 * @param function Function name (optional)
 */
void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

/**
 * @brief Convenience method for warning-level logging
 * 
 * @param message The warning message
 * @param file Source file name (optional)
 * @param line Line number (optional)
 * @param function Function name (optional)
 */
void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

/**
 * @brief Convenience method for error-level logging
 * 
 * @param message The error message
 * @param file Source file name (optional)
 * @param line Line number (optional)
 * @param function Function name (optional)
 */
void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

/**
 * @brief Convenience method for critical-level logging
 * 
 * @param message The critical message
 * @param file Source file name (optional)
 * @param line Line number (optional)
 * @param function Function name (optional)
 */
void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

/**
 * @brief Check if a log level is enabled
 * 
 * Returns true if any registered logger is configured to handle messages
 * at or above the specified level. This is useful for conditional logging
 * to avoid unnecessary string formatting for messages that won't be output.
 * 
 * @param level The log level to check
 * @return true if the level is enabled, false otherwise
 */
bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Get the global logger instance
 * 
 * Returns a static global logger instance that is used by default for
 * logging throughout the application. This provides a convenient singleton
 * for applications that want centralized logging without explicitly
 * configuring loggers.
 * 
 * @return Reference to the global logger instance
 */
Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

/**
 * @brief Set the global logger
 * 
 * This method provides a way to replace the default global logger instance
 * with a custom implementation. However, the current implementation is
 * a placeholder and primarily serves documentation purposes. In practice,
 * getGlobal() is used directly to access the logger.
 * 
 * @param logger The logger instance to set as global
 */
void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

/**
 * @brief LogStream constructor
 * 
 * Initializes a LogStream with log level, file information, and other metadata.
 * The stream is initialized but doesn't output anything until the destructor
 * is called, which then performs the actual logging operation.
 * 
 * This class enables stream-style logging with the NLM_LOG_STREAM macro,
 * allowing for natural C++ stream operations like `NLM_LOG_STREAM(WARNING) << "Error: " << errorCode`.
 * 
 * @param level The log level for this stream
 * @param file Source file name
 * @param line Line number in source file
 * @param function Function name
 */
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

/**
 * @brief LogStream destructor - performs the logging operation
 * 
 * This destructor converts the accumulated stream content into a log message
 * and sends it through the global logger. This ensures that stream-style
 * logging happens at the end of the scope where the LogStream was created.
 */
LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
