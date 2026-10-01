#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <map>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

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
    // Rotation modes
    enum class RotationMode {
        None,
        BySize,
        ByTime
    };
    
    // Rotation configuration
    struct RotationConfig {
        RotationMode mode = RotationMode::None;
        size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
        std::chrono::system_clock::duration rotationPeriod = std::chrono::hours(24);  // 24 hours
        size_t maxFiles = 5;
        std::string backupSuffix = ".bak";
    };
    
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
    
    // Log rotation configuration
    void setRotationMode(RotationMode mode);
    void setRotationConfig(const RotationConfig& config);
    RotationConfig getRotationConfig() const;
    
    // Force rotation
    void rotate();
    
    // Get current log file size
    size_t getLogFileSize() const;
    
    // Get log file path
    std::string getLogFilePath() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void rotateIfNeeded();
    std::string getRotatedFileName(const std::string& baseName, int index) const;
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
#define NLM_LOG_DEBUG(msg) do { nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__); } while (false)
#define NLM_LOG_INFO(msg) do { nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__); } while (false)
#define NLM_LOG_WARNING(msg) do { nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__); } while (false)
#define NLM_LOG_ERROR(msg) do { nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__); } while (false)
#define NLM_LOG_CRITICAL(msg) do { nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__); } while (false)

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
