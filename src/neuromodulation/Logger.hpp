#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <fstream>
#include <mutex>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Log severity levels
enum class LogSeverity {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// Simple color codes for console output
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
}

// Simple color mapping for log severity
inline std::string getColorForSeverity(LogSeverity severity) {
    switch (severity) {
        case LogSeverity::TRACE: return Color::CYAN;
        case LogSeverity::DEBUG: return Color::BLUE;
        case LogSeverity::INFO: return Color::GREEN;
        case LogSeverity::WARN: return Color::YELLOW;
        case LogSeverity::ERROR: return Color::RED;
        case LogSeverity::FATAL: return Color::BRIGHT_RED;
        default: return Color::WHITE;
    }
}

inline std::string getSeverityPrefix(LogSeverity severity) {
    switch (severity) {
        case LogSeverity::TRACE: return "T";
        case LogSeverity::DEBUG: return "D";
        case LogSeverity::INFO: return "I";
        case LogSeverity::WARN: return "W";
        case LogSeverity::ERROR: return "E";
        case LogSeverity::FATAL: return "F";
        default: return "U";
    }
}

// Log message structure
struct LogMessage {
    LogSeverity severity;
    std::string module;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    int lineNumber;
    std::string fileName;
    
    LogMessage(LogSeverity sev, const std::string& mod, const std::string& msg, 
               int line = 0, const std::string& file = "")
        : severity(sev), module(mod), message(msg), timestamp(std::chrono::system_clock::now()),
          lineNumber(line), fileName(file) {}
};

// Log sink interface
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogMessage& message) = 0;
};

// Console log sink
class ConsoleSink : public LogSink {
public:
    void write(const LogMessage& message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto time = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm tm_buf;
        localtime_r(&time, &tm_buf);
        
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << 
           "." << std::setfill('0') << std::setw(3) << 
           std::chrono::duration_cast<std::chrono::milliseconds>(
               message.timestamp.time_since_epoch()).count() % 1000;
        
        std::string timestamp = ss.str();
        
        std::string color = getColorForSeverity(message.severity);
        std::string prefix = getSeverityPrefix(message.severity);
        
        std::cout << color << "[" << timestamp << "] [" << prefix << "]";
        
        if (!message.module.empty()) {
            std::cout << " [" << message.module << "]";
        }
        
        if (!message.fileName.empty() && message.lineNumber > 0) {
            // Show relative file path
            size_t last_slash = message.fileName.find_last_of("/\\");
            std::string file_name = (last_slash != std::string::npos) ? 
                message.fileName.substr(last_slash + 1) : message.fileName;
            std::cout << " " << file_name << ":" << message.lineNumber;
        }
        
        std::cout << " " << message.message << Color::RESET << std::endl;
    }
    
private:
    std::mutex mutex_;
};

// File log sink
class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& filename) {
        file_.open(filename, std::ios::out | std::ios::app);
        if (!file_.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }
    
    void write(const LogMessage& message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!file_.is_open()) {
            return;
        }
        
        auto time = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm tm_buf;
        localtime_r(&time, &tm_buf);
        
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << 
           "." << std::setfill('0') << std::setw(3) << 
           std::chrono::duration_cast<std::chrono::milliseconds>(
               message.timestamp.time_since_epoch()).count() % 1000;
        
        std::string timestamp = ss.str();
        
        file_ << "[" << timestamp << "] [" << getSeverityPrefix(message.severity) << "]";
        
        if (!message.module.empty()) {
            file_ << " [" << message.module << "]";
        }
        
        if (!message.fileName.empty() && message.lineNumber > 0) {
            file_ << " " << message.fileName << ":" << message.lineNumber;
        }
        
        file_ << " " << message.message << std::endl;
        file_.flush();
    }
    
    ~FileSink() override {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
private:
    std::ofstream file_;
    std::mutex mutex_;
};

// Logger class providing the main interface
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    // Set the minimum log level
    void setLevel(LogSeverity level) {
        std::lock_guard<std::mutex> lock(mutex_);
        minLevel_ = level;
    }
    
    // Get the current minimum log level
    LogSeverity getLevel() const {
        return minLevel_;
    }
    
    // Add a sink to receive log messages
    void addSink(std::unique_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(mutex_);
        sinks_.push_back(std::move(sink));
    }
    
    // Add console sink
    void enableConsole(bool enable = true) {
        if (enable) {
            addSink(std::make_unique<ConsoleSink>());
        }
    }
    
    // Add file sink
    void enableFile(const std::string& filename) {
        addSink(std::make_unique<FileSink>(filename));
    }
    
    // Remove all sinks
    void clearSinks() {
        std::lock_guard<std::mutex> lock(mutex_);
        sinks_.clear();
    }
    
    // Log methods
    template<typename... Args>
    void trace(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::TRACE, module, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void debug(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::DEBUG, module, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::INFO, module, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warn(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::WARN, module, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::ERROR, module, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void fatal(const std::string& module, const std::string& format, Args&&... args) {
        log(LogSeverity::FATAL, module, format, std::forward<Args>(args)...);
    }
    
    // Simplified logging methods
    template<typename... Args>
    void log(const std::string& module, LogSeverity severity, const std::string& format, Args&&... args) {
        std::string message = formatMessage(format, std::forward<Args>(args)...);
        logInternal(severity, module, message);
    }
    
    template<typename... Args>
    void log(LogSeverity severity, const std::string& module, const std::string& format, Args&&... args) {
        std::string message = formatMessage(format, std::forward<Args>(args)...);
        logInternal(severity, module, message);
    }
    
private:
    Logger() : minLevel_(LogSeverity::TRACE) {
        enableConsole();
    }
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    template<typename... Args>
    std::string formatMessage(const std::string& format, Args&&... args) {
        if (sizeof...(args) == 0) {
            return format;
        }
        
        // Simple formatting - for production use, consider a proper formatting library
        size_t pos = 0;
        std::string result = format;
        std::string replacements[] = {std::string(args)...};
        
        for (const auto& replacement : replacements) {
            pos = result.find("{}");
            if (pos != std::string::npos) {
                result.replace(pos, 2, replacement);
            }
        }
        
        return result;
    }
    
    void logInternal(LogSeverity severity, const std::string& module, const std::string& message) {
        if (severity < minLevel_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& sink : sinks_) {
            sink->write(LogMessage(severity, module, message, 0, ""));
        }
    }
    
private:
    LogSeverity minLevel_;
    std::vector<std::unique_ptr<LogSink>> sinks_;
    mutable std::mutex mutex_;
};

// Convenience functions
inline void setLogLevel(LogSeverity level) {
    Logger::getInstance().setLevel(level);
}

inline void enableConsoleLogging(bool enable = true) {
    Logger::getInstance().enableConsole(enable);
}

inline void enableFileLogging(const std::string& filename) {
    Logger::getInstance().enableFile(filename);
}

inline void clearLogSinks() {
    Logger::getInstance().clearSinks();
}

// Simple macro-based logging for different modules
#define NLM_LOG_TRACE(module, ...) nlm::Logger::getInstance().trace(module, __VA_ARGS__)
#define NLM_LOG_DEBUG(module, ...) nlm::Logger::getInstance().debug(module, __VA_ARGS__)
#define NLM_LOG_INFO(module, ...) nlm::Logger::getInstance().info(module, __VA_ARGS__)
#define NLM_LOG_WARN(module, ...) nlm::Logger::getInstance().warn(module, __VA_ARGS__)
#define NLM_LOG_ERROR(module, ...) nlm::Logger::getInstance().error(module, __VA_ARGS__)
#define NLM_LOG_FATAL(module, ...) nlm::Logger::getInstance().fatal(module, __VA_ARGS__)

} // namespace nlm
