/**
 * @file Logger.hpp
 * @brief Comprehensive logging system for the NLM brain system
 * 
 * The NLM (Neural Learning Machine) logging system provides a flexible, thread-safe
 * approach to logging diagnostic information, performance metrics, and system status
 * for the brain simulation environment. This system is essential for understanding,
 * debugging, and monitoring the complex neural network dynamics that occur during
 * brain simulation runs.
 * 
 * ## Why Logging is Important for the NLM Brain System
 * 
 * Logging serves multiple critical functions in the NLM brain system:
 * 
 * 1. **Debugging and Troubleshooting**: Brain simulations are inherently complex,
 *    involving millions of neurons with intricate synaptic connections. When unexpected
 *    behavior occurs, detailed logs help identify the root cause, whether it's a bug
 *    in the neuron dynamics, synaptic plasticity rules, or network initialization.
 * 
 * 2. **Performance Monitoring**: Large-scale brain simulations require significant
 *    computational resources. Logging helps track simulation progress, identify
 *    performance bottlenecks, and measure the efficiency of different algorithms
 *    and data structures.
 * 
 * 3. **System Health Monitoring**: The brain system can experience various states
 *    (development, learning, equilibrium) and external influences. Logging provides
 *    visibility into system state transitions, learning progress, and overall health.
 * 
 * 4. **Research and Analysis**: For scientific research and experimentation, logging
 *    provides a complete record of simulation runs, enabling reproducibility and
 *    post-simulation analysis of neural activity patterns and learning outcomes.
 * 
 * 5. **Production Support**: When deployed in production environments, logs help
 *    operations teams understand system behavior, diagnose issues quickly, and
 *    maintain system reliability.
 * 
 * ## How Different Components Use Logging
 * 
 * The logging system is integrated throughout the NLM brain system:
 * 
 * - **Neuron Class**: Uses logging for state changes, spike events, plasticity
 *   rule applications, and development stage transitions. Neurons log internal
 *   dynamics, learning progress, and interaction events.
 * 
 * - **Synapse Class**: Logs synaptic transmission events, weight updates,
 *   plasticity rule applications, and connection establishment/breakdown.
 * 
 * - **Brain System**: Logs high-level operations like region initialization,
 *   population formation, network synchronization, and simulation step progress.
 * 
 * - **Learning Modules**: Logs learning algorithm progress, reward signals,
 *   and adaptation rule applications.
 * 
 * - **Config System**: Logs configuration loading, validation errors, and
 *   runtime parameter changes.
 * 
 * - **Simulation Core**: Logs step execution, timing information, and system
 *   resource utilization.
 * 
 * ## Best Practices for NLM Logging
 * 
 * 1. **Use Appropriate Severity Levels**: Match log levels to the nature of
 *    the information being logged. Use Debug for development details, Info for
 *    operational monitoring, Warning for potential issues, Error for failures,
 *    and Critical for system-threatening conditions.
 * 
 * 2. **Include Source Location**: Always include file, line, and function
 *    information in log messages to aid in debugging. Use the convenience
 *    macros (NLM_LOG_DEBUG, etc.) to automate this.
 * 
 * 3. **Log at the Right Level**: Avoid excessive debug logging in production
 *    systems. Use isEnabled() checks before constructing expensive log messages.
 * 
 * 4. **Consistent Formatting**: Use a consistent format for all log messages
 *    across the system. Include timestamps, severity levels, and relevant context.
 * 
 * 5. **Thread Safety**: All logging operations should be thread-safe to support
 *    concurrent access from multiple simulation threads.
 * 
 * 6. **Performance Considerations**: Minimize string construction overhead for
 *    log messages by using stream-style logging (NLM_LOG_STREAM) when possible.
 * 
 * ## Logging Approaches in NLM
 * 
 * The NLM logging system provides three complementary approaches:
 * 
 * ### 1. Interface-Based Logging (ILogger)
 * 
 * The ILogger abstract base class defines the interface for all logging
 * implementations. This allows for different output destinations:
 * - **ConsoleLogger**: Outputs to stderr with color support
 * - **FileLogger**: Writes to files for persistent storage
 * - **NetworkLogger**: Sends logs to remote monitoring systems
 * - **CustomLogger**: User-defined implementations for specialized needs
 * 
 * ### 2. Chain-Based Logging (Logger Class)
 * 
 * The Logger class maintains a chain of logger implementations. Each log message
 * is processed by all registered loggers in sequence. This enables multiple
 * output destinations simultaneously (e.g., console + file + network).
 * 
 * ### 3. Convenience APIs
 * 
 * The system provides multiple convenience interfaces:
 * - **Method-based**: debug(), info(), warning(), error(), critical() methods
 * - **Macro-based**: NLM_LOG_DEBUG, NLM_LOG_INFO, etc. macros
 * - **Stream-based**: NLM_LOG_STREAM macro for formatted output
 * 
 * ## Thread Safety and Lifecycle Considerations
 * 
 * The logging system is designed to be thread-safe:
 * 
 * - **AddLogger()**: Safe for concurrent calls from different threads
 * - **setLevel()**: Thread-safe implementation with appropriate synchronization
 * - **log()**: Thread-safe with minimal contention
 * - **getGlobal()**: Returns a reference to a singleton that is safe for concurrent use
 * 
 * Lifecycle considerations:
 * 
 * - Loggers should be added before first log operation
 * - The global logger is initialized on first use (lazy initialization)
 * - Call clearLoggers() to temporarily disable all logging
 * - File handles are properly closed on logger destruction
 * - Memory usage is managed through RAII with smart pointers
 * 
 * ## Configuration and Customization
 * 
 * The logging system can be customized at runtime:
 * 
 * - **Log Level Thresholds**: Set minimum severity levels for filtering
 * - **Output Destinations**: Add/remove logger implementations dynamically
 * - **Color Output**: Enable/disable console colors
 * - **File Output**: Configure additional log files
 * - **Global Logger**: Replace the default logger with custom implementations
 * 
 * ## Integration Guidelines
 * 
 * When integrating logging into new NLM components:
 * 
 * 1. Use the convenience macros for standard logging needs
 * 2. For specialized logging needs, implement ILogger
 * 3. Always include source location information
 * 4. Check isEnabled() before constructing expensive log messages
 * 5. Use appropriate severity levels for different types of information
 * 6. Consider thread safety in concurrent environments
 * 
 * This logging system provides the foundation for comprehensive monitoring,
 * debugging, and analysis of the NLM brain simulation environment, ensuring
 * that the system remains observable and maintainable throughout its lifecycle.
 */
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @brief Log severity levels for controlling logging output granularity
 * 
 * The LogLevel enum defines the severity hierarchy for log messages, allowing
 * filtering and categorization of logging output. Each level represents a
 * different severity tier, with Debug being the most verbose and Critical
 * being the most severe. This hierarchy enables selective logging based on
 * debugging needs, operational monitoring, or error tracking.
 * 
 * @note LogLevel values are ordered by severity (Debug < Info < Warning < Error < Critical)
 *       allowing for threshold-based filtering (e.g., only log Error and Critical).
 * @note Thread safety: LogLevel values are immutable and can be safely shared
 *       across threads without synchronization.
 */
enum class LogLevel {
    ///@brief Detailed debug information for development and troubleshooting
    ///@note Includes low-level implementation details, variable values, and
    ///      step-by-step execution flow. Useful for identifying root causes
    ///      of complex issues during development.
    Debug = 0,
    
    ///@brief General operational information for normal system monitoring
    ///@note Provides insight into system state, key events, and regular operations
    ///      without overwhelming detail. Essential for production monitoring
    ///      and operational awareness.
    Info = 1,
    
    ///@brief Warning conditions that may indicate potential problems
    ///@note Signals situations that could lead to errors if not addressed,
    ///      but do not prevent normal operation. Useful for proactive maintenance
    ///      and issue prevention.
    Warning = 2,
    
    ///@brief Error conditions that prevented normal operation
    ///@note Indicates failures that affect system functionality, require
    ///      immediate attention, and should be investigated and resolved.
    ///      Critical for incident response and system reliability.
    Error = 3,
    
    ///@brief Severe conditions that may cause system instability or failure
    ///@note Represents critical system failures, catastrophic errors, or
    ///      situations that may lead to complete system failure if not addressed
    ///      immediately. Requires urgent intervention.
    Critical = 4
};

// Log entry structure
struct LogEntry {
    ///@brief The severity level of this log message
    LogLevel level;
    
    ///@brief The actual log message content
    std::string message;
    
    ///@brief Source file where the log entry was generated
    std::string file;
    
    ///@brief Line number in the source file where the log entry was generated
    int line;
    
    ///@brief Function name where the log entry was generated
    std::string function;
    
    ///@brief Timestamp when the log entry was created
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
class ILogger {
public:
    /**
     * @brief Virtual destructor ensures proper cleanup of derived logger implementations
     * 
     * @note Must be virtual to ensure proper cleanup of concrete logger instances
     *       when ILogger pointers/references are used.
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a formatted entry to the output sink
     * 
     * @param entry Complete log entry containing level, message, metadata
     * 
     * @note This is the primary method for log output. Concrete implementations
     *       are responsible for formatting and outputting the entry according to
     *       their specific requirements (console, file, network, etc.).
     * @note Thread safety: Implementations should be thread-safe to support
     *       concurrent logging from multiple threads.
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level threshold for this logger
     * 
     * @param level Minimum severity level to log (entries below this level are ignored)
     * 
     * @note Log entries with severity below this threshold will not be processed.
     *       For example, if level is set to Warning, only Warning, Error, and Critical
     *       entries will be logged.
     * @note Thread safety: Implementations should handle concurrent setLevel calls
     *       appropriately.
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level threshold
     * 
     * @return LogLevel The current minimum severity level for logging
     * 
     * @note Returns the threshold level. Log entries with lower severity are filtered out.
     * @note Thread safety: Const method, should be safe for concurrent access with setLevel.
     */
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Default constructor creates a logger with Info level and colors enabled
     * 
     * @note Creates an output stream to stderr with color support enabled by default.
     *       The logger will output to the standard error stream, making it suitable
     *       for both user output and debugging.
     */
    ConsoleLogger();
    
    /**
     * @brief Constructor with explicit log level configuration
     * 
     * @param level Initial minimum log level threshold for this logger
     * 
     * @note Allows customization of the logging threshold from construction.
     *       Useful for creating loggers with different verbosity levels in different
     *       parts of the application.
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor ensures proper cleanup of resources
     * 
     * @note Safely closes any open output files and cleans up allocated resources.
     *       The implementation handles graceful shutdown even if an error occurs
     *       during cleanup.
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a formatted entry to the console output
     * 
     * @param entry Complete log entry containing level, message, metadata
     * 
     * @note Formats the entry with colored output based on severity level and
     *       outputs to stderr. Includes timestamp, level, and source location
     *       information in the formatted output.
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level threshold for this logger
     * 
     * @param level Minimum severity level to log (entries below this level are ignored)
     * 
     * @note Color output is adjusted based on the new threshold level for visual consistency.
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level threshold
     * 
     * @return LogLevel The current minimum severity level for logging
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable color output for log messages
     * 
     * @param useColors Whether to use ANSI color codes for console output
     * 
     * @note Colors enhance readability by associating severity levels with visual cues
     *       (e.g., red for errors, green for success). Note that colors may not work
     *       properly on all terminals or when output is redirected to a file.
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if color output is enabled
     * 
     * @return bool true if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set an additional output file for log messages
     * 
     * @param filepath Path to the file where log messages will also be written
     * 
     * @note Logs are written to both the console (stderr) and the specified file.
     *       The file is opened in append mode. Call closeOutputFile() to disable
     *       file output while keeping the logger active.
     * @note File output is disabled by default. Enable with setOutputFile().
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the additional output file
     * 
     * @note Disables file output while keeping the logger active. Does nothing if
     *       no file is currently open. After calling this, log messages go only
     *       to the console.
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert log level enum to human-readable string
     * 
     * @param level Log level to convert
     * @return std::string Human-readable representation of the log level
     * 
     * @note Used for formatting log entries in the output stream.
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for a log level
     * 
     * @param level Log level to get color for
     * @return std::string ANSI escape sequence for the color, empty string if colors disabled
     * 
     * @note Used for colored console output. Returns empty string when colors are disabled
     *       or when the terminal doesn't support color output.
     */
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
class Logger {
public:
    /**
     * @brief Default constructor creates a logger with a ConsoleLogger sink
     * 
     * @note Creates the global logger instance with default settings:
     *       - Log level: Info
     *       - Colors: enabled
     *       - File output: disabled
     *       - Console output: enabled
     *       This logger is registered as the global logger instance.
     */
    Logger();
    
    /**
     * @brief Destructor ensures cleanup of all registered logger sinks
     * 
     * @note Properly cleans up all registered logger instances and resets
     *       the global logger state. Ensures no dangling resources or memory leaks.
     */
    ~Logger();
    
    // Disable copying, enable moving for efficient resource management
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger implementation to the output chain
     * 
     * @param logger Shared pointer to a logger implementation (ConsoleLogger, file logger, etc.)
     * 
     * @note Loggers are processed in the order they are added. Each logger receives
     *       a copy of the log entry. Multiple loggers can be added to send the same
     *       log message to multiple destinations (console, files, network, etc.).
     * @note Thread safety: Safe for concurrent calls from different threads.
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all registered logger sinks
     * 
     * @note Clears the logger chain, preventing further output. Useful for
     *       temporarily disabling logging or cleaning up resources.
     * @note After calling clearLoggers(), no output will be produced until
     *       new loggers are added.
     */
    void clearLoggers();
    
    /**
     * @brief Set the minimum log level threshold for all registered loggers
     * 
     * @param level Minimum severity level to log (entries below this level are ignored)
     * 
     * @note This sets the threshold for all loggers in the system. Individual
     *       loggers can have their own thresholds that may be more restrictive.
     * @note Thread safety: Safe for concurrent calls from different threads.
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a formatted entry using the configured logger sinks
     * 
     * @param level Severity level of the log message
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note Creates a LogEntry with timestamp and source location information,
     *       then passes it to all registered loggers. The entry is filtered by
     *       each logger's individual threshold.
     * @note Source location parameters (file, line, function) are typically omitted
     *       when using convenience macros like NLM_LOG_DEBUG, which automatically
     *       include these parameters.
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Convenience method for debug level logging
     * 
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note This is a convenience method that calls log() with LogLevel::Debug.
     *       Often used with the NLM_LOG_DEBUG macro for streamlined debugging code.
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for info level logging
     * 
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note This is a convenience method that calls log() with LogLevel::Info.
     *       Useful for operational monitoring and system status reporting.
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for warning level logging
     * 
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note This is a convenience method that calls log() with LogLevel::Warning.
     *       Used for potential issues that don't prevent normal operation.
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for error level logging
     * 
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note This is a convenience method that calls log() with LogLevel::Error.
     *       Used for recoverable errors that affect system functionality.
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for critical level logging
     * 
     * @param message The log message content
     * @param file Source file where the log entry was generated (defaults to empty string)
     * @param line Line number in the source file (defaults to 0)
     * @param function Function name where the log entry was generated (defaults to empty string)
     * 
     * @note This is a convenience method that calls log() with LogLevel::Critical.
     *       Used for severe conditions that may cause system instability or failure.
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled based on the current threshold
     * 
     * @param level Log level to check
     * @return bool true if the level is enabled for logging, false otherwise
     * 
     * @note This allows callers to conditionally log messages without incurring
     *       the overhead of string formatting when the message won't be logged.
     *       Useful for performance-critical logging where format string construction
     *       is expensive.
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance (singleton pattern)
     * 
     * @return Logger& Reference to the global logger instance
     * 
     * @note This provides access to the default logger used throughout the NLM system.
     *       The global logger is typically initialized with default settings and
     *       is the primary interface for logging in the brain system.
     * @note Thread safety: Returns a reference to a singleton that should be
     *       treated as immutable after initialization. Multiple threads can safely
     *       call methods on the returned reference.
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * @param logger Shared pointer to the logger to set as global
     * 
     * @note Replaces the default global logger with a custom implementation.
     *       Useful for testing, custom logging destinations, or runtime configuration
     *       changes. This affects all calls to getGlobal() from this point forward.
     * @note The previous global logger is not destroyed and remains accessible
     *       through any existing references.
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for debug level logging with automatic source location
 * 
 * @param msg Log message string (can contain C++ stream-style formatting)
 * 
 * @note Automatically includes source file (__FILE__), line number (__LINE__), and
 *       function name (__func__) from the preprocessor. This provides complete
 *       debugging context for log messages without requiring manual parameters.
 * @note This macro is thread-safe and can be used in any context where logging
 *       is appropriate. The message is only formatted and logged if debug level
 *       logging is enabled.
 * @note Usage:
 * @code
 * NLM_LOG_DEBUG("Processing neuron " << neuronId << " with voltage " << voltage);
 * @endcode
 */

#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for info level logging with automatic source location
 * 
 * @param msg Log message string (can contain C++ stream-style formatting)
 * 
 * @note Similar to NLM_LOG_DEBUG but for operational information messages.
 *       Use for system status, normal operation reporting, and monitoring information.
 */

#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for warning level logging with automatic source location
 * 
 * @param msg Log message string (can contain C++ stream-style formatting)
 * 
 * @note Used for potential problems or unusual conditions that don't prevent
 *       normal operation. Includes source location for troubleshooting.
 */

#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for error level logging with automatic source location
 * 
 * @param msg Log message string (can contain C++ stream-style formatting)
 * 
 * @note Used for errors that affect system functionality. Includes complete
 *       context information for rapid diagnosis and resolution.
 */

#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Convenience macro for critical level logging with automatic source location
 * 
 * @param msg Log message string (can contain C++ stream-style formatting)
 * 
 * @note Used for severe conditions that may cause system instability or failure.
 *       This level should be used sparingly and indicates situations requiring
 *       immediate attention and intervention.
 */

// Stream-style logging helper
class LogStream {
public:
    /**
     * @brief Construct a stream logger with automatic source location
     * 
     * @param level Log level for this stream
     * @param file Source file where the log entry was generated
     * @param line Line number in the source file
     * @param function Function name where the log entry was generated
     * 
     * @note Creates a stream that will automatically log its contents when destroyed.
     *       This allows for C++ style stream formatting (e.g., NLM_LOG_STREAM(Debug) << "Value: " << value << "\n").
     *       The stream is designed to be used in a single statement and should be
     *       assigned to a temporary variable or used directly in an expression.
     * @note Thread safety: Safe for concurrent use from multiple threads as it doesn't
     *       maintain persistent state between uses.
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor logs the accumulated stream content
     * 
     * @note Automatically logs the stream content with the configured log level
     *       and source location information. If the log level is not enabled, this
     *       destructor performs no operations, providing efficient filtering at the
     *       last possible moment.
     */
    ~LogStream();
    
    /**
     * @brief Stream output operator for arbitrary types
     * 
     * @tparam T Type of the value to stream
     * @param value Value to add to the stream
     * @return LogStream& Reference to this stream for chaining
     * 
     * @note Uses the stream's internal ostringstream to accumulate output.
     *       Supports all types that can be streamed with std::operator<<.
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    ///@brief Log level for this stream message
    LogLevel level_;
    ///@brief String stream for accumulating output
    std::ostringstream oss_;
    ///@brief Source file where the log entry was generated
    std::string file_;
    ///@brief Line number in the source file
    int line_;
    ///@brief Function name where the log entry was generated
    std::string function_;
};

/**
 * @brief Convenience macro for stream-style logging with automatic source location
 * 
 * @param level Log level for the stream (e.g., NLM_LOG_STREAM(Debug))
 * 
 * @note Creates a LogStream that can be used with C++ stream operator<< for
 *       formatted logging. The stream is destroyed at the end of the statement,
 *       automatically logging its contents.
 * @note Usage:
 * @code
 * NLM_LOG_STREAM(Debug) << "Processing neuron " << neuronId << " with voltage " << voltage << "\n";
 * @endcode
 * @note This approach is efficient for debugging complex log messages that require
 *       multiple values to be formatted before logging. It reduces string copying
 *       and formatting overhead compared to constructing the message manually.
 */

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
