#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <ctime>
#include <codecvt>
#include <locale>

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
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& func = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(func),
          timestamp(std::chrono::system_clock::now()) {}
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) const = 0;
};

// Simple text formatter
class TextFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) const override {
        std::ostringstream oss;
        
        // Timestamp with milliseconds
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()) % 1000;
        
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << " [" << levelToString(entry.level) << "] ";
        
        if (!entry.file.empty() && entry.line > 0) {
            oss << entry.file << ":" << entry.line << " ";
        }
        
        oss << entry.message;
        
        return oss.str();
    }
    
private:
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

// Colored text formatter for console
class ColoredFormatter : public TextFormatter {
public:
    std::string format(const LogEntry& entry) const override {
        std::string formatted = TextFormatter::format(entry);
        
        // Add ANSI color codes if supported
        if (shouldUseColors()) {
            std::string colorCode = getColorCode(entry.level);
            if (!colorCode.empty()) {
                formatted = colorCode + formatted + "\033[0m";
            }
        }
        
        return formatted;
    }
    
private:
    bool shouldUseColors() const {
        // Check if output is a terminal
        return isatty(fileno(stdout)) != 0;
    }
    
    std::string getColorCode(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    int fileno(FILE* stream) {
        return _fileno(stream);
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormatter(std::shared_ptr<LogFormatter> formatter) = 0;
    virtual std::shared_ptr<LogFormatter> getFormatter() const = 0;
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
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void ensureFileOpen();
    void writeToConsole(const std::string& formattedMessage);
    void writeToFile(const std::string& formattedMessage);
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // Rotate file when it reaches max size
    void setMaxFileSize(size_t maxSizeBytes);
    void setMaxFiles(int maxFiles);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string getCurrentLogFile() const;
    void rotateIfNeeded();
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
    
    // Set formatter for all loggers
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    
    // Global logging functions with file/line/function info
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
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
    
    // Convenience macro for file/line/function info
    #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
    
    // Stream-style macro
    #define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
    
    // Get global logger instance (thread-safe singleton)
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
