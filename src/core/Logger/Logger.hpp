#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels - defines the hierarchy of message importance
// Lower values represent more verbose/detailed messages
// Used for filtering messages at runtime
enum class LogLevel {
    Debug = 0,      // Detailed debugging information for development
    Info = 1,        // General information about system operation
    Warning = 2,     // Warnings about potential issues or unexpected behavior
    Error = 3,       // Errors that affect functionality but don't crash the system
    Critical = 4     // Critical errors that indicate system failure or data corruption
};

// Log entry structure - represents a single log message with metadata
// Contains the message content, severity level, and source location information
// Used internally by the logging system to pass log messages between components
struct LogEntry {
    LogLevel level;                       // Severity level of the message
    std::string message;                   // The actual log message text
    std::string file;                      // Source file where log was generated
    int line;                              // Line number in source file
    std::string function;                  // Function name where log was generated
    std::chrono::system_clock::time_point timestamp;  // When the log was created
};

// Logger interface - abstract base class for all logger implementations
// Defines the contract that all loggers must implement
// Enables multiple logging backends (console, file, network, etc.)
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Log a message entry
    // Parameters: entry - LogEntry containing message and metadata
    // Note: Implementers should handle formatting and output
    virtual void log(const LogEntry& entry) = 0;
    
    // Set the minimum severity level for this logger
    // Messages below this level will be filtered out
    // Parameters: level - Minimum LogLevel to process
    virtual void setLevel(LogLevel level) = 0;
    
    // Get the current minimum severity level
    // Returns: Current LogLevel threshold
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation - writes log messages to standard output
// Supports colored output for different log levels
// Maintains thread safety for concurrent logging
// Can also write to a file in addition to console output
class ConsoleLogger : public ILogger {
public:
    // Default constructor creates a console logger with Info level
    ConsoleLogger();
    
    // Constructor with custom severity level
    // Parameters: level - Initial minimum severity level
    explicit ConsoleLogger(LogLevel level);
    
    ~ConsoleLogger() override;
    
    // Log a message to console
    void log(const LogEntry& entry) override;
    
    // Set severity level for this logger
    void setLevel(LogLevel level) override;
    
    // Get current severity level
    LogLevel getLevel() const override;
    
    // Enable or disable colored output for different log levels
    // Parameters: useColors - If true, add ANSI color codes to output
    void setUseColors(bool useColors);
    
    // Check if colors are enabled
    // Returns: Current color setting
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    // Parameters: filepath - Path to file for log output (appends by default)
    void setOutputFile(const std::string& filepath);
    
    // Close the log output file
    // File handle is closed and subsequent logs go to console only
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Convert log level to human-readable string
    // Parameters: level - LogLevel to convert
    // Returns: String representation (e.g., "DEBUG", "INFO")
    std::string levelToString(LogLevel level) const;
    
    // Get ANSI color code for log level
    // Parameters: level - LogLevel to get color for
    // Returns: ANSI escape sequence for color, empty string if no colors
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class - facade for logging system
// Provides a unified interface for adding loggers and logging messages
// Maintains thread safety for concurrent access
// Supports both simple and stream-style logging interfaces
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    // Logger contains internal state (mutexes, shared pointers) that shouldn't be copied
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    // Parameters: logger - Shared pointer to logger implementation
    // Note: Multiple loggers can be added for different outputs (console, file, etc.)
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    // Clears the list of logging destinations
    void clearLoggers();
    
    // Set minimum level for all loggers
    // Parameters: level - Minimum LogLevel to process across all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions - static methods for easy logging
    
    // Core logging function - logs a message with metadata
    // Parameters: level - Severity level, message - Text to log
    // Optional: file, line, function - source location (defaults empty)
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods - typed logging for common severity levels
    
    // Log debug information
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Log general information
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Log warnings
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Log errors
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Log critical errors
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    // Returns: true if any configured logger will process messages at this level
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    // Returns: Reference to the singleton global logger
    // Note: The global logger is created on first access and persists for the program
    static Logger& getGlobal();
    
    // Set global logger
    // Parameters: logger - New global logger instance
    // Note: This replaces the default singleton logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info - logging with source location
// These macros automatically capture __FILE__, __LINE__, and __func__
// Usage examples:
//   NLM_LOG_DEBUG("Starting neuron simulation");
//   NLM_LOG_INFO("Spiked at time " << current_time);
//   NLM_LOG_ERROR("Failed to load configuration: " << error_msg);
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper - formatted logging similar to iostream
// Enables easy logging with arbitrary data types
// Usage examples:
//   NLM_LOG_STREAM(nlm::LogLevel::Debug) << "Current value: " << value << " spikes/sec";
//   NLM_LOG_STREAM(nlm::LogLevel::Info) << "Connected " << neuron_count << " neurons";
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Stream insertion operator - forwards to logger
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;    // Internal string stream for formatted output
    std::string file_;          // Source file for error reporting
    int line_;                  // Line number for error reporting
    std::string function_;      // Function name for error reporting
};

// Macro for stream-style logging
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
