#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>

namespace nlm {

// Enhanced log severity levels with more granularity
enum class LogLevel {
    Trace = 0,           // Very detailed debugging information
    Debug = 1,           // General debugging information
    Info = 2,            // General information
    Notice = 3,          // Important events that are not errors
    Warning = 4,         // Warning conditions
    Error = 5,           // Error conditions
    Critical = 6,        // Critical conditions that may lead to failure
    Alert = 7,           // Action must be taken immediately
    Emergency = 8        // System is unusable
};

// Enhanced log entry with additional context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced context information
    std::string category;                    // Log category (e.g., "neuron", "plasticity", "network")
    std::string subcategory;                 // Sub-category for more specific filtering
    std::unordered_map<std::string, std::string> context;  // Key-value pairs for context
    std::optional<int> neuronId;              // Optional neuron ID for neural-related logs
    std::optional<int> synapseId;             // Optional synapse ID for synaptic-related logs
    std::optional<int> regionId;              // Optional region ID for regional logs
    
    // Performance metrics
    std::optional<std::chrono::microseconds> processingTime;
    std::optional<size_t> memoryUsage;
    
    // Error specific fields
    std::string errorCode;
    std::string errorMessage;
    bool isRecoverable;
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced filtering capabilities
    virtual void addFilter(LogLevel minLevel, const std::string& category = "") = 0;
    virtual void removeFilter(const std::string& category = "") = 0;
    
    // Pattern matching for log filtering
    virtual void addPatternFilter(const std::string& pattern) = 0;
    
    // Output formatting options
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string formatEntry(const LogEntry& entry) const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced console options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Enhanced output file options
    void setOutputFile(const std::string& filepath, bool append = true);
    void closeOutputFile();
    
    // Enhanced formatting
    void setTimestampFormat(const std::string& format);
    void setIncludeFileInfo(bool include);
    void setIncludeFunctionInfo(bool include);
    
    // Log level specific thresholds
    void setLevelThreshold(const std::string& category, LogLevel level);
    LogLevel getLevelThreshold(const std::string& category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

// Enhanced File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // File rotation and management
    void setFilepath(const std::string& filepath);
    void setMaxFileSize(size_t maxSize);
    void setMaxFileCount(size_t maxCount);
    void setCompression(bool enable);
    
    // Log file management
    void rotate();
    bool isFileOpen() const;
    std::string getCurrentFilepath() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced asynchronous logger for performance
class AsyncLogger : public ILogger {
public:
    AsyncLogger();
    explicit AsyncLogger(LogLevel level);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Buffer management
    void setBufferSize(size_t size);
    void flush();
    void waitForFlush();
    
    // Thread control
    void start();
    void stop();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Logger class with advanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logging methods with context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Enhanced context-aware logging
    void logWithContext(LogLevel level, const std::string& message,
                       const std::unordered_map<std::string, std::string>& context,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Convenience methods with context
    void debugWithContext(const std::string& message,
                         const std::unordered_map<std::string, std::string>& context,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    void infoWithContext(const std::string& message,
                        const std::unordered_map<std::string, std::string>& context,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    void warningWithContext(const std::string& message,
                           const std::unordered_map<std::string, std::string>& context,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    void errorWithContext(const std::string& message,
                         const std::unordered_map<std::string, std::string>& context,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Performance timing helpers
    void timeOperation(LogLevel level, const std::string& operation,
                      std::function<void()> func,
                      const std::string& category = "");
    
    // Performance metrics logging
    void logPerformance(const std::string& operation,
                       const std::chrono::microseconds& duration,
                       size_t memoryUsage = 0,
                       const std::unordered_map<std::string, std::string>& context = {});
    
    // Enhanced error logging with recovery information
    void logError(const std::string& errorCode, const std::string& errorMessage,
                 bool isRecoverable = false,
                 const std::string& recoveryAction = "",
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration integration
    void loadFromConfig(const std::string& section = "logging");
    void saveToConfig(std::string& section) const;
    
    // Log analysis and statistics
    size_t getLogCount(LogLevel level = LogLevel::Info) const;
    std::string getLogStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros with context
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Notice, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Alert, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Emergency, msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add context to log entry
    LogStream& addContext(const std::string& key, const std::string& value);
    LogStream& addContext(const std::string& key, int value);
    LogStream& addContext(const std::string& key, float value);
    
    // Add specific neural context
    LogStream& withNeuron(int neuronId);
    LogStream& withSynapse(int synapseId);
    LogStream& withRegion(int regionId);
    LogStream& withCategory(const std::string& category);
    LogStream& withSubcategory(const std::string& subcategory);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> context_;
    std::optional<int> neuronId_;
    std::optional<int> synapseId_;
    std::optional<int> regionId_;
    std::string category_;
    std::string subcategory_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Error handling helper
class LogError {
public:
    LogError(const std::string& errorCode, const std::string& errorMessage,
             bool isRecoverable = false, const std::string& recoveryAction = "");
    
    template<typename T>
    LogError& operator<<(const T& value) {
        message_ << value;
        return *this;
    }
    
    void log();
    void logWithRecovery();
    
private:
    std::string errorCode_;
    std::string errorMessage_;
    bool isRecoverable_;
    std::string recoveryAction_;
    std::ostringstream message_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_ERROR_HELPER(errorCode, errorMessage) nlm::LogError(errorCode, errorMessage, __FILE__, __LINE__, __func__)

// Logging categories enum for better organization
enum class LogCategory {
    System,           // Core system initialization and shutdown
    Memory,           // Memory allocation and deallocation
    Performance,      // Performance metrics and timing
    Network,          // Neural network operations
    Neuron,           // Individual neuron operations
    Synapse,          // Synapse operations and plasticity
    Plasticity,       // Plasticity mechanisms (STDP, Hebbian)
    Development,      // Developmental processes
    Learning,         // Learning experiments
    Sensory,          // Sensory processing
    Motor,            // Motor output
    Environment,      // Environment simulation
    World,            // World simulation
    Visualization,    // Visualization and output
    Configuration,    // Configuration loading and parsing
    FileIO,           // File operations
    NetworkIO,        // Network operations
    Error,            // Error conditions
    Warning,          // Warning conditions
    Debug,            // Debug information
    Custom            // Custom category
};

// Helper function to get category string
const char* GetLogCategoryString(LogCategory category);

} // namespace nlm
