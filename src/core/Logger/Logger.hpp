#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Standardized logging levels
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Log message structure
struct LogMessage {
    std::chrono::steady_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string module;
};

// Abstract log handler interface
class LogHandler {
public:
    virtual ~LogHandler() = default;
    virtual void handle(const LogMessage& message) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console log handler
class ConsoleLogHandler : public LogHandler {
public:
    ConsoleLogHandler(LogLevel level = LogLevel::Info) : level_(level) {}
    
    void handle(const LogMessage& message) override {
        if (message.level < level_) return;
        
        std::string levelStr;
        switch (message.level) {
            case LogLevel::Trace: levelStr = "TRACE"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Critical: levelStr = "CRITICAL"; break;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - message.timestamp);
        
        std::cout << "[" << duration.count() << "ms] "
                  << "[" << levelStr << "] "
                  << "[" << message.module << "] "
                  << message.message << std::endl;
    }
    
    void setLevel(LogLevel level) override {
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
private:
    LogLevel level_;
};

// File log handler
class FileLogHandler : public LogHandler {
public:
    FileLogHandler(const std::string& filename, LogLevel level = LogLevel::Info) : level_(level) {
        file_.open(filename);
        if (!file_.is_open()) {
            throw std::runtime_error("Cannot open log file: " + filename);
        }
    }
    
    ~FileLogHandler() override {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    void handle(const LogMessage& message) override {
        if (message.level < level_) return;
        
        std::string levelStr;
        switch (message.level) {
            case LogLevel::Trace: levelStr = "TRACE"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Critical: levelStr = "CRITICAL"; break;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - message.timestamp);
        
        file_ << "[" << duration.count() << "ms] "
              << "[" << levelStr << "] "
              << "[" << message.module << "] "
              << message.message << std::endl;
        file_.flush();
        
        // Also output to console for real-time monitoring
        std::cout << "[FILE] " << message.message << std::endl;
    }
    
    void setLevel(LogLevel level) override {
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    void flush() {
        file_.flush();
    }
    
private:
    std::ofstream file_;
    LogLevel level_;
};

// Global logger class
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
        for (auto& handler : handlers_) {
            handler->setLevel(level);
        }
    }
    
    LogLevel getLevel() const {
        return level_;
    }
    
    void addHandler(std::unique_ptr<LogHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(std::move(handler));
    }
    
    void log(LogLevel level, const std::string& module, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (level < level_) return;
        
        LogMessage logMessage {
            std::chrono::steady_clock::now(),
            level,
            message,
            module
        };
        
        for (auto& handler : handlers_) {
            handler->handle(logMessage);
        }
    }
    
    void trace(const std::string& module, const std::string& message) {
        log(LogLevel::Trace, module, message);
    }
    
    void debug(const std::string& module, const std::string& message) {
        log(LogLevel::Debug, module, message);
    }
    
    void info(const std::string& module, const std::string& message) {
        log(LogLevel::Info, module, message);
    }
    
    void warning(const std::string& module, const std::string& message) {
        log(LogLevel::Warning, module, message);
    }
    
    void error(const std::string& module, const std::string& message) {
        log(LogLevel::Error, module, message);
    }
    
    void critical(const std::string& module, const std::string& message) {
        log(LogLevel::Critical, module, message);
    }
    
    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& handler : handlers_) {
            if (auto fileHandler = dynamic_cast<FileLogHandler*>(handler.get())) {
                fileHandler->flush();
            }
        }
    }
    
private:
    Logger() {
        addHandler(std::make_unique<ConsoleLogHandler>(LogLevel::Info));
    }
    
    ~Logger() {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.clear();
    }
    
    std::mutex mutex_;
    LogLevel level_ = LogLevel::Info;
    std::vector<std::unique_ptr<LogHandler>> handlers_;
};

// Convenience macros
#define NLM_LOG_TRACE(module, message) Logger::getInstance().trace(module, message)
#define NLM_LOG_DEBUG(module, message) Logger::getInstance().debug(module, message)
#define NLM_LOG_INFO(module, message) Logger::getInstance().info(module, message)
#define NLM_LOG_WARNING(module, message) Logger::getInstance().warning(module, message)
#define NLM_LOG_ERROR(module, message) Logger::getInstance().error(module, message)
#define NLM_LOG_CRITICAL(module, message) Logger::getInstance().critical(module, message)

// Helper functions for common logging patterns
class LoggerHelper {
public:
    template<typename... Args>
    static void info(const std::string& module, const std::string& format, const Args&... args) {
        std::stringstream ss;
        ss << std::vformat(format, std::make_format_args(args...));
        NLM_LOG_INFO(module, ss.str());
    }
    
    template<typename... Args>
    static void warning(const std::string& module, const std::string& format, const Args&... args) {
        std::stringstream ss;
        ss << std::vformat(format, std::make_format_args(args...));
        NLM_LOG_WARNING(module, ss.str());
    }
    
    template<typename... Args>
    static void error(const std::string& module, const std::string& format, const Args&... args) {
        std::stringstream ss;
        ss << std::vformat(format, std::make_format_args(args...));
        NLM_LOG_ERROR(module, ss.str());
    }
};

} // namespace nlm