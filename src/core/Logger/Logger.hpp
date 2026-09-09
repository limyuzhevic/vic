#pragma once
/**
 * @file Logger.hpp
 * @brief Thread-safe logging system for NLM
 * 
 * This file provides a comprehensive logging infrastructure for the NLM neural
 * simulation system. It includes:
 * - Severity-based log levels (Debug, Info, Warning, Error, Critical)
 * - Multiple logger sinks (console, file, custom implementations)
 * - Thread-safe global logging functions
 * - Rich log entry metadata (file, line, function, timestamp)
 * - Convenience macros for simple logging
 * - Stream-style logging interface
 * 
 * The logging system is designed for performance and flexibility, allowing
 * multiple log destinations and level-based filtering.
 * 
 * @author NLM Development Team
 * @version 1.0
 * @date 2026
 * 
 * Usage examples:
 * @code
 * // Basic logging
 * nlm::Logger::getGlobal().info("Simulation started");
 * 
 * // Using macros (recommended)
 * NLM_LOG_INFO("Processing step " << step);
 * 
 * // Stream-style logging
 * NLM_LOG_DEBUG("Neuron " << id << " has membrane potential " << potential);
 * @endcode
 */

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Simulation Library namespace
 * 
 * Contains all types, classes, and functions for the Neural Simulation Library,
 * including the logging infrastructure.
 */

/**
 * @brief Log severity levels for filtering and categorization
 * 
 * These levels determine the importance and urgency of log messages. They also
 * define which messages are actually logged based on the current logging level.
 * The ordering is from most verbose (Debug) to most severe (Critical).
 */
enum class LogLevel {
    /** Detailed debug information for troubleshooting */
    Debug = 0,
    /** General information about system operation */
    Info = 1,
    /** Warning about potential issues that don't prevent operation */
    Warning = 2,
    /** Error that prevents certain operations but allows continuation */
    Error = 3,
    /** Critical error that may prevent system operation */
    Critical = 4
};

/**
 * @struct LogEntry
 * @brief Container for a single log message with metadata
 * 
 * Contains the actual log message along with context information that helps
 * with debugging and analysis. Each log entry includes source location and
 * timestamp for better traceability.
 */
struct LogEntry {
    /** Severity level of the log message */
    LogLevel level;
    /** The actual log message */
    std::string message;
    /** Source file where the log was generated */
    std::string file;
    /** Line number in the source file */
    int line;
    /** Function name where the log was generated */
    std::string function;
    /** Timestamp when the log was created */
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @class ILogger
 * @brief Abstract interface for log implementations
 * 
 * This is the interface that all concrete logger implementations must satisfy.
 * The Logger class maintains a list of sinks that implement this interface.
 * 
 * @note Subclasses must be thread-safe if they will be used concurrently.
 * @see ConsoleLogger, Logger, NLM_LOG_* macros
 */
class ILogger {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ILogger() = default;
    
    /**
     * @brief Log a message with full metadata
     * 
     * @param entry Complete log entry with metadata
     */
    virtual void log(const LogEntry& entry) = 0;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * Messages with lower severity than the set level will be ignored.
     * 
     * @param level Minimum severity level to log
     */
    virtual void setLevel(LogLevel level) = 0;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum severity level
     */
    virtual LogLevel getLevel() const = 0;
};

/**
 * @class ConsoleLogger
 * @brief Console-based logger implementation
 * 
 * Logs messages to standard output (or error output for higher severity levels).
 * Supports colored output for better readability in terminal environments.
 * Can also write to an additional file while maintaining console output.
 * 
 * @note Thread-safe implementation suitable for concurrent use.
 * @see ILogger
 */
class ConsoleLogger : public ILogger {
public:
    /**
     * @brief Constructs a console logger with default settings
     * 
     * Default log level is Info, colors are enabled, and logging is to console only.
     */
    ConsoleLogger();
    
    /**
     * @brief Constructs a console logger with specified log level
     * 
     * @param level Minimum severity level to log (default: Info)
     */
    explicit ConsoleLogger(LogLevel level);
    
    /**
     * @brief Destructor
     * 
     * Flushes and closes any open file handles.
     */
    ~ConsoleLogger() override;
    
    /**
     * @brief Log a message with full metadata
     * 
     * @param entry Complete log entry with metadata
     */
    void log(const LogEntry& entry) override;
    
    /**
     * @brief Set the minimum log level for this logger
     * 
     * @param level Minimum severity level to log
     */
    void setLevel(LogLevel level) override;
    
    /**
     * @brief Get the current minimum log level
     * 
     * @return Current minimum severity level
     */
    LogLevel getLevel() const override;
    
    /**
     * @brief Enable or disable colored output
     * 
     * Colors are only supported in terminal environments that support them.
     * When disabled, all messages are output in plain text.
     * 
     * @param useColors Whether to enable colored output (default: true)
     */
    void setUseColors(bool useColors);
    
    /**
     * @brief Check if colored output is enabled
     * 
     * @return true if colors are enabled, false otherwise
     */
    bool getUseColors() const;
    
    /**
     * @brief Set additional output file
     * 
     * The logger will write to both console and this file simultaneously.
     * Pass an empty string to disable file output.
     * 
     * @param filepath Path to file for additional logging (default: "")
     */
    void setOutputFile(const std::string& filepath);
    
    /**
     * @brief Close the output file
     * 
     * Closes the additional file handle (if any) used for logging.
     */
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert LogLevel enum to string representation
     * 
     * @param level LogLevel enum value
     * @return std::string String representation (e.g., "INFO")
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * @brief Get ANSI color code for a log level
     * 
     * @param level LogLevel enum value
     * @return std::string ANSI escape code or empty string if no color
     */
    std::string levelToColor(LogLevel level) const;
};

/**
 * @class Logger
 * @brief Centralized logging manager for NLM
 * 
 * Provides a global logging interface that can have multiple sinks (ConsoleLogger,
 * file loggers, etc.) attached. Offers both object-oriented and macro-based
 * logging interfaces. Thread-safe for concurrent use from multiple threads.
 * 
 * The Logger class uses the singleton pattern for the global logger instance
 * accessible via getGlobal().
 * 
 * @note This class is not copyable or assignable (deleted).
 * @see ILogger, ConsoleLogger, NLM_LOG_* macros
 */
class Logger {
public:
    /**
     * @brief Constructs a new Logger with default configuration
     * 
     * Initializes an empty logger with no sinks. Use addLogger() to add
     * sinks for actual logging.
     */
    Logger();
    
    /**
     * @brief Destructor
     * 
     * Cleans up all attached logger sinks.
     */
    ~Logger();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Logger objects cannot be copied due to internal implementation details.
     */
    Logger(const Logger&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Logger objects cannot be assigned due to internal implementation details.
     */
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink to this logger
     * 
     * Appends the given logger to the list of sinks. Messages will be sent
     * to all attached sinks.
     * 
     * @param logger Shared pointer to logger sink to add
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Clears the list of attached sinks, effectively disabling all logging.
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all sinks
     * 
     * Sets the level filter for all attached logger sinks. Messages with
     * severity lower than this level will be ignored by all sinks.
     * 
     * @param level Minimum severity level to log
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with metadata
     * 
     * Creates a LogEntry with the provided metadata and sends it to all
     * attached logger sinks that meet the level criteria.
     * 
     * @param level Severity level of the message
     * @param message The log message
     * @param file Source file (default: "")
     * @param line Line number in source file (default: 0)
     * @param function Function name (default: "")
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    /**
     * @brief Convenience method for debug-level logging
     * 
     * Logs at Debug level. Uses file/line/function macro if called from
     * macro-generated context.
     * 
     * @param message The log message
     * @param file Source file (default: "")
     * @param line Line number (default: 0)
     * @param function Function name (default: "")
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for info-level logging
     */
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for warning-level logging
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for error-level logging
     */
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Convenience method for critical-level logging
     */
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Check if a log level is enabled
     * 
     * Tests whether messages at the given severity level would actually be
     * logged based on the current configuration.
     * 
     * @param level Log level to test
     * @return true if level would be logged, false otherwise
     */
    bool isEnabled(LogLevel level) const;
    
    /**
     * @brief Get the global logger instance (singleton)
     * 
     * Returns a reference to the global logger instance. This is the primary
     * logging interface for NLM components. All macro-based logging (NLM_LOG_*)
     * uses this global instance.
     * 
     * @return Logger& Reference to the global logger instance
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * Replaces the default global logger with a custom one. This can be used
     * to redirect all logging to a custom implementation or for testing.
     * 
     * @param logger Shared pointer to logger to set as global
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Macro for debug-level logging with automatic metadata
 * 
 * Expands to a call to the global logger's debug() method with __FILE__,
 * __LINE__, and __func__ predefined by the C++ compiler. This provides
 * full context for debugging purposes.
 * 
 * @param msg Message to log
 * 
 * @see NLM_LOG_INFO, NLM_LOG_WARNING, NLM_LOG_ERROR, NLM_LOG_CRITICAL
 * @see LogStream
 */
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Macro for info-level logging with automatic metadata
 * 
 * @param msg Message to log
 */
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Macro for warning-level logging with automatic metadata
 * 
 * @param msg Message to log
 */
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Macro for error-level logging with automatic metadata
 * 
 * @param msg Message to log
 */
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)

/**
 * @brief Macro for critical-level logging with automatic metadata
 * 
 * @param msg Message to log
 */
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

/**
 * @class LogStream
 * @brief Stream-style logging helper
 * 
 * Provides an ostream-like interface for constructing complex log messages
 * using operator<<. This allows for formatted logging similar to iostream.
 * 
 * Example usage:
 * @code
 * NLM_LOG_STREAM(nlm::LogLevel::Info) << "Processing " << id << " with weight " << weight;
 * @endcode
 * 
 * @see NLM_LOG_STREAM macro
 */
class LogStream {
public:
    /**
     * @brief Constructs a stream for logging
     * 
     * @param level Severity level for this log message
     * @param file Source file where logging occurred
     * @param line Line number in source file
     * @param function Function name where logging occurred
     */
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
    /**
     * @brief Destructor - automatically logs the accumulated message
     */
    ~LogStream();
    
    /**
     * @brief Stream insertion operator for any type
     * 
     * @tparam T Any type that can be streamed to std::ostringstream
     * @param value Value to add to the log message
     * @return LogStream& Reference to self for chaining
     */
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    /** Log level for this message */
    LogLevel level_;
    /** String stream containing the formatted message */
    std::ostringstream oss_;
    /** Source file path */
    std::string file_;
    /** Line number in source file */
    int line_;
    /** Function name where logging occurred */
    std::string function_;
};

/**
 * @brief Macro for stream-style logging
 * 
 * Creates a temporary LogStream object with the specified level and
 * automatically deletes it when the expression ends, logging the
 * accumulated message.
 * 
 * @param level Log level for this message
 * 
 * @see LogStream
 */
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

/**
 * @namespace std
 * @brief Extend std namespace with NLM-specific stream operators
 * 
 * This forward declaration allows NLM to add stream operators to std::ostream
 * for custom types without polluting the global namespace.
 * 
 * @see PlasticityFlags, NeuronId, SynapseId
 */

/**
 * @brief Stream output operator for PlasticityFlags
 * 
 * Provides human-readable output for PlasticityFlags bit field.
 * 
 * @param os Output stream
 * @param flags PlasticityFlags to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::PlasticityFlags& flags);

/**
 * @brief Stream output operator for NeuronId
 * 
 * Provides formatted output for NeuronId.
 * 
 * @param os Output stream
 * @param id NeuronId to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::NeuronId& id);

/**
 * @brief Stream output operator for SynapseId
 * 
 * Provides formatted output for SynapseId.
 * 
 * @param os Output stream
 * @param id SynapseId to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::SynapseId& id);

/**
 * @brief Stream output operator for RegionId
 * 
 * Provides formatted output for RegionId.
 * 
 * @param os Output stream
 * @param id RegionId to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::RegionId& id);

/**
 * @brief Stream output operator for PopulationId
 * 
 * Provides formatted output for PopulationId.
 * 
 * @param os Output stream
 * @param id PopulationId to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::PopulationId& id);

/**
 * @brief Stream output operator for NeuronType enum
 * 
 * Provides string representation for NeuronType enum values.
 * 
 * @param os Output stream
 * @param type NeuronType to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::NeuronType& type);

/**
 * @brief Stream output operator for SynapseType enum
 * 
 * Provides string representation for SynapseType enum values.
 * 
 * @param os Output stream
 * @param type SynapseType to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::SynapseType& type);

/**
 * @brief Stream output operator for DevelopmentalStage enum
 * 
 * Provides string representation for DevelopmentalStage enum values.
 * 
 * @param os Output stream
 * @param stage DevelopmentalStage to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::DevelopmentalStage& stage);

/**
 * @brief Stream output operator for FiringState enum
 * 
 * Provides string representation for FiringState enum values.
 * 
 * @param os Output stream
 * @param state FiringState to output
 * @return std::ostream& Reference to output stream for chaining
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::FiringState& state);
