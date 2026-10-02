#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

    // Enhanced log severity levels with more granularity
    enum class LogLevel {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Notice = 3,
        Warning = 4,
        Error = 5,
        Critical = 6,
        Alert = 7,
        Emergency = 8
    };

    // Log categories for better organization
    enum class LogCategory {
        System = 0,
        Neural,
        Memory,
        Plasticity,
        Neuromodulation,
        Cognition,
        Development,
        Prediction,
        Sensory,
        Motor,
        Environment,
        Agent,
        Experiment,
        Performance,
        Configuration
    };

    // Enhanced log entry structure with category and performance tracking
    struct LogEntry {
        LogLevel level;
        LogCategory category;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        
        // Performance tracking
        std::chrono::microseconds duration;
        
        // Context information
        uint64_t threadId;
        uint64_t processId;
        std::string operation;
        double simulationTime;
    };

    // Logger interface
    class ILogger {
    public:
        virtual ~ILogger() = default;
        virtual void log(const LogEntry& entry) = 0;
        virtual void setLevel(LogLevel level) = 0;
        virtual LogLevel getLevel() const = 0;
        virtual void setCategoryLevel(LogCategory category, LogLevel level) = 0;
        virtual LogLevel getCategoryLevel(LogCategory category) const = 0;
        
        // Performance logging
        virtual void startPerformanceTimer(const std::string& operation) = 0;
        virtual void endPerformanceTimer(const std::string& operation) = 0;
        
        // Memory tracking
        virtual void logMemoryUsage(size_t allocated, size_t peak, size_t available) = 0;
    };

    // Enhanced log entry structure with category and performance tracking
    struct LogEntry {
        LogLevel level;
        LogCategory category;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        
        // Performance tracking
        std::chrono::microseconds duration;
        
        // Context information
        uint64_t threadId;
        uint64_t processId;
        std::string operation;
        double simulationTime;
    };

    // Logger interface
    class ILogger {
    public:
        virtual ~ILogger() = default;
        virtual void log(const LogEntry& entry) = 0;
        virtual void setLevel(LogLevel level) = 0;
        virtual LogLevel getLevel() const = 0;
        virtual void setCategoryLevel(LogCategory category, LogLevel level) = 0;
        virtual LogLevel getCategoryLevel(LogCategory category) const = 0;
        
        // Performance logging
        virtual void startPerformanceTimer(const std::string& operation) = 0;
        virtual void endPerformanceTimer(const std::string& operation) = 0;
        
        // Memory tracking
        virtual void logMemoryUsage(size_t allocated, size_t peak, size_t available) = 0;
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
