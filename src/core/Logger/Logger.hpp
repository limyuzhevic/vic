#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <stdexcept>

namespace nlm {

/**
 * @brief Log severity levels for the NLM system
 */
enum class LogLevel {
public:
    Debug = 0,     ///< Debug-level logging for detailed diagnostic information
    Info = 1,       ///< Informational messages about normal system operation
    Warning = 2,    ///< Warning messages about potential issues or unexpected behavior
    Error = 3,      ///< Error messages indicating system failures or errors
    Critical = 4    ///< Critical error messages indicating system-wide failures
};

/**
 * @brief Log entry structure containing all logging information
 */
struct LogEntry {
    LogLevel level;                                      ///< Log level of the entry
    std::string message;                                  ///< The actual log message
    std::string file;                                     ///< Source file where log was generated
    int line;                                             ///< Line number in source file
    std::string function;                                 ///< Function name where log was generated
    std::chrono::system_clock::time_point timestamp;      ///< Time when log was generated
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    /**
     * @brief Add a logger sink to the system
     * 
     * @param logger Shared pointer to a logger implementation
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks from the system
     */
    void clearLoggers();
    
    /**
     * @brief Set minimum log level for all loggers
     * 
     * @param level Minimum log level to output
     */
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods for different log levels
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if a specific log level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance (singleton pattern)
    static Logger& getGlobal();
    
    // Set global logger instance
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get configuration summary
    std::string getConfigSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper with formatted output
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    // Stream operator for easy formatting
    template<typename T>
    LogStream& operator<<(const T& value);
    
    // Stream operator for strings with special handling
    LogStream& operator<<(const std::string& value);
    LogStream& operator<<(const char* value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point startTime_;
};

// Enhanced log stream macro with automatic timing
define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

/**
 * @brief Exception class for NLM-specific errors
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    explicit NLMException(const std::string& message, const std::string& source) 
        : std::runtime_error("[" + source + "] " + message) {}
};

/**
 * @brief Validation utilities for NLM parameters
 */
namespace Validation {
    /**
     * @brief Validate that a value is within specified bounds
     * 
     * @tparam T Numeric type
     * @param value Value to validate
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @param name Name of parameter for error messages
     * @throws NLMException if value is out of bounds
     */
    template<typename T>
    void range(T value, T min, T max, const std::string& name = "value");
    
    /**
     * @brief Validate that a pointer is not null
     * 
     * @param ptr Pointer to validate
     * @param name Name of parameter for error messages
     * @throws NLMException if pointer is null
     */
    template<typename T>
    void notNull(T* ptr, const std::string& name = "pointer");
    
    /**
     * @brief Validate that a configuration key exists
     * 
     * @param config Configuration object
     * @param key Key to check
     * @throws NLMException if key doesn't exist
     */
    void hasKey(const Config& config, const std::string& key);
}

} // namespace nlm
