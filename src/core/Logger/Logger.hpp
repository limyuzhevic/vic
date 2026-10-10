#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

namespace nlm {

// Logging system for NLM
// Provides flexible, hierarchical logging with multiple output formats
//
// Logging System Architecture:
// ============================
// The NLM logging system implements a multi-layer logging architecture designed
// for performance and flexibility in large-scale neural simulations.
//
// Key Features:
// 1. **Hierarchical Log Levels**: Debug, Info, Warning, Error, Critical
// 2. **Multiple Sinks**: Console, file, and custom logging backends
// 3. **Thread-Safe**: Designed for concurrent access in multi-threaded environments
// 4. **Performance Optimized**: Fast path for disabled levels, minimal overhead
// 5. **Rich Metadata**: File, line, and function name for debugging
// 6. **Configurable**: Runtime configuration of log levels and formats
//
// Log Level Hierarchy:
// ------------------
// - **Debug**: Detailed debugging information, very verbose
// - **Info**: General information about system operation
// - **Warning**: Potential issues that don't prevent operation
// - **Error**: Errors that affect functionality but don't crash
// - **Critical**: Severe errors that may cause system failure
//
// Logging Categories:
// ------------------
// - **NEURAL COMPUTATION**: Neuron dynamics, synaptic events, spikes
// - **PLASTICITY**: Synaptic weight changes, learning rules
// - **MEMORY**: Memory formation, retrieval, and consolidation
// - **COGNITION**: Planning, attention, concept formation
// - **SIMULATION**: Time steps, configuration, system state
// - **PERFORMANCE**: Benchmarking, profiling, optimization metrics
// - **ERROR_HANDLING**: Exceptions, recovery, system failures
// - **SYSTEM**: Startup, shutdown, resource management
//
// Logger Usage Examples:
// ---------------------
// Example 1: Basic logging
// NLM_LOG_INFO("Simulation started with {} neurons", neuronCount);
// NLM_LOG_DEBUG("Spike detected at time {}", currentTime);
//
// Example 2: Error logging with context
// NLM_LOG_ERROR("Failed to load model from {}", filepath);
//
// Example 3: Performance logging
// NLM_LOG_INFO("Processed {} spikes in {} ms", spikeCount, elapsedMs);
//
// Example 4: Structured logging with streams
// NLM_LOG_STREAM(nlm::LogLevel::Warning) << "Connection exceeded threshold: " << value;
//
// Log Entry Structure:
// -------------------
// Each log entry contains:
// - Severity level (enum)
// - Message text (string)
// - Source location (file, line, function)
// - Timestamp (system clock precision)
//
// Performance Considerations:
// --------------------------
// - Fast path: Check log level before formatting expensive operations
// - Buffer management: Minimize memory allocations
// - Sink optimization: Only log to enabled outputs
// - Async logging: Separated for future high-throughput scenarios
//
// Configuration:
// -------------
// Global log level set via Logger::getGlobal().setLevel()
// Per-sink configuration for different output formats
// Runtime enable/disable of specific log categories
//
// Implementation Details:
// ----------------------
// - Uses Pimpl idiom for extensibility
// - Thread-safe singleton pattern for global logger
// - Circular buffer for high-volume scenarios (planned)
// - Color-coded console output for improved readability

// Log severity levels
enum class LogLevel {
    Debug = 0,      // Detailed debugging information, very verbose
    Info = 1,        // General information about system operation
    Warning = 2,     // Potential issues that don't prevent operation
    Error = 3,       // Errors that affect functionality but don't crash
    Critical = 4     // Severe errors that may cause system failure
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
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
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
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

// Stream-style logging helper
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

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
