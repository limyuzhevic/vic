#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <ctime>

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
    
    // Helper to get formatted timestamp
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Get formatted log line
    virtual std::string formatEntry(const LogEntry& entry) const {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        oss << entry.getFormattedTimestamp() << " ";
        oss << entry.message;
        if (!entry.file.empty()) {
            oss << " (" << entry.file << ":" << entry.line << ")";
            if (!entry.function.empty()) {
                oss << " in " << entry.function;
            }
        }
        return oss.str();
    }
    
protected:
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
};

// Console logger implementation with file rotation
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
    
    // File rotation support
    void setMaxFileSize(size_t maxSizeBytes);
    void setMaxFiles(int maxFiles);
    size_t getFileSize() const;
    bool needsRotation() const;
    
    // Log formatting options
    void setUseTimestamps(bool useTimestamps);
    bool getUseTimestamps() const;
    void setUseFileInfo(bool useFileInfo);
    bool getUseFileInfo() const;
    
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
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger (replaces current global logger)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
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
    
    // Get current configuration
    LogLevel getGlobalLevel() const;
    
    // Log stream support
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
    
#define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().logStream(level, __FILE__, __LINE__, __func__)
    
    // Get global log stream (for streaming logging)
    LogStream logStream(LogLevel level, const std::string& file, int line, const std::string& function);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global logging functions (convenience macros)
void logGlobal(LogLevel level, const std::string& message, 
               const std::string& file = "", int line = 0, 
               const std::string& function = "");

#define NLM_LOG_DEBUG(msg) nlm::logGlobal(nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::logGlobal(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::logGlobal(nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::logGlobal(nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::logGlobal(nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)

} // namespace nlm

} // namespace nlm
