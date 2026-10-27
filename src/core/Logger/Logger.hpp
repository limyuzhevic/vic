#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
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

// Structured log entry for advanced logging
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    LogLevel level;
    std::string category;
    std::string message;
    std::map<std::string, std::string> fields;
    
    StructuredLogEntry() : timestamp(), level(LogLevel::Info), category("general") {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Additional interface methods
    virtual void setUseColors(bool useColors) = 0;
    virtual bool getUseColors() const = 0;
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    virtual std::string getTimestampFormat() const = 0;
    virtual void setTimestampFormat(const std::string& format) = 0;
    virtual size_t getMaxFileSize() const = 0;
    virtual void setMaxFileSize(size_t maxSize) = 0;
    virtual int getMaxLogFiles() const = 0;
    virtual void setMaxLogFiles(int maxFiles) = 0;
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
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    // Set output file with rotation
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    
    // File rotation and formatting
    size_t getMaxFileSize() const override;
    void setMaxFileSize(size_t maxSize) override;
    int getMaxLogFiles() const override;
    void setMaxLogFiles(int maxFiles) override;
    
    // Timestamp formatting
    std::string getTimestampFormat() const override;
    void setTimestampFormat(const std::string& format) override;
    
    // Thread-safe timestamp generation
    static std::string getTimestamp(const std::chrono::system_clock::time_point& timestamp, const std::string& format = "%Y-%m-%d %H:%M:%S");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class with advanced features
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
    
    // Advanced configuration
    void setGlobalLogLevel(LogLevel level);
    LogLevel getGlobalLogLevel() const;
    
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
    
    // Stream-style logging helper with structured support
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Set category for structured logging
        LogStream& category(const std::string& category);
        
        // Add field for structured logging
        template<typename T>
        LogStream& field(const std::string& name, const T& value) {
            oss_ << " [" << name << "=" << value << "]";
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::string category_;
    };
    
    // Structured logging convenience functions
    void logStructured(LogLevel level, const std::string& category, const std::string& message);
    template<typename T>
    void logStructured(LogLevel level, const std::string& category, const std::string& message, const std::string& fieldName, const T& value) {
        std::ostringstream oss;
        oss << message << " [" << fieldName << "=" << value << "]";
        log(level, oss.str(), "", 0, "");
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log file statistics
    std::string getLogStatistics() const;
    
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

// Stream-style logging macro
#define NLM_LOG_STREAM(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_STRUCTURED_INFO(category, msg) nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Info, category, msg)
#define NLM_LOG_STRUCTURED_ERROR(category, msg) nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Error, category, msg)

} // namespace nlm
