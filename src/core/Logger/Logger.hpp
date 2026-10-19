#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Comprehensive logging system for NLM
    // Provides thread-safe logging with multiple sinks, severity levels, and formatting
    // Supports console, file, and custom logger implementations
    // Includes utilities for performance-critical logging with minimal overhead

    // Log severity levels - higher values indicate more severe issues
    enum class LogLevel {
        Debug = 0,      // Detailed debug information (spammy)
        Info = 1,       // General information about execution
        Warning = 2,    // Warning conditions that don't prevent operation
        Error = 3,      // Error conditions that may affect functionality
        Critical = 4    // Critical errors that prevent operation
    };

    // Standardized log entry containing all information needed for logging
    // Used internally by the logging system, can be extended for custom sinks
    struct LogEntry {
        LogLevel level;                                   // Severity of the log message
        std::string message;                              // Log message text
        std::string file;                                 // Source file where log occurred
        int line;                                         // Line number in source file
        std::string function;                            // Function name where log occurred
        std::chrono::system_clock::time_point timestamp; // When the log occurred
        
        // Human-readable string representation of the log level
        std::string getLevelString() const {
            switch (level) {
                case LogLevel::Debug: return "DEBUG";
                case LogLevel::Info: return "INFO";
                case LogLevel::Warning: return "WARNING";
                case LogLevel::Error: return "ERROR";
                case LogLevel::Critical: return "CRITICAL";
                default: return "UNKNOWN";
            }
        }
    };

    // Abstract interface for logger implementations
    // Users can implement this to create custom logging sinks (e.g., database, network, etc.)
    class ILogger {
    public:
        virtual ~ILogger() = default;
        
        // Log a message entry
        // Implementations should handle thread safety and formatting as needed
        virtual void log(const LogEntry& entry) = 0;
        
        // Set the minimum log level for this logger
        // Messages below this level will not be logged
        virtual void setLevel(LogLevel level) = 0;
        
        // Get the current minimum log level
        virtual LogLevel getLevel() const = 0;
    };

    // Console logger implementation
    // Outputs logs to stdout with optional colors and file output
    // Thread-safe with mutex protection for concurrent access
    class ConsoleLogger : public ILogger {
    public:
        ConsoleLogger();
        explicit ConsoleLogger(LogLevel level);  // Set initial log level
        ~ConsoleLogger() override;
        
        void log(const LogEntry& entry) override;
        void setLevel(LogLevel level) override;
        LogLevel getLevel() const override;
        
        // Enable/disable colored output for different log levels
        void setUseColors(bool useColors);
        bool getUseColors() const;
        
        // Set additional file output in addition to console
        void setOutputFile(const std::string& filepath);
        void closeOutputFile();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        // Convert log level to human-readable string
        std::string levelToString(LogLevel level) const;
        
        // Get ANSI color code for log level (returns empty string if colors disabled)
        std::string levelToColor(LogLevel level) const;
    };

    // Main Logger class - composite logger managing multiple sinks
    // Thread-safe with internal mutex for concurrent logging from multiple threads
    // Manages a collection of logger implementations (sinks) and distributes logs to all
    class Logger {
    public:
        Logger();
        ~Logger();
        
        // Disable copying to prevent multiple access to same logger
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        
        // Add a logger sink to receive log messages
        void addLogger(std::shared_ptr<ILogger> logger);
        
        // Remove all logger sinks (disables logging)
        void clearLoggers();
        
        // Set minimum log level for all registered loggers
        void setLevel(LogLevel level);
        
        // Log a message with file/line/function information
        void log(LogLevel level, const std::string& message, 
                 const std::string& file = "", int line = 0, 
                 const std::string& function = "");
        
        // Convenience methods with automatic file/line/function injection
        void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
        void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
        void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
        void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
        void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
        
        // Check if a log level is enabled (would produce output)
        bool isEnabled(LogLevel level) const;
        
        // Get the global logger instance (singleton)
        // Most NLM components use this global logger
        static Logger& getGlobal();
        
        // Set the global logger instance
        // Advanced users can replace the global logger with custom implementations
        static void setGlobal(std::shared_ptr<Logger> logger);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Convenience macro for file/line/function info
    #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

    // Stream-style logging helper for performance-critical code
    // Avoids string concatenation overhead - use like std::cout
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };

    // Macro for stream-style logging
    #define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
