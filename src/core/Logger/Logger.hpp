#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Log levels for standardized logging
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

/**
 * Log entry structure
 */
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
    int lineNumber;
    std::string fileName;
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& src = "",
             int line = 0, const std::string& file = "")
        : level(lvl), message(msg), source(src), timestamp(std::chrono::system_clock::now()),
          lineNumber(line), fileName(file) {}
};

/**
 * Logger interface for standardized logging
 */
class Logger {
public:
    virtual ~Logger() = default;
    
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
};

/**
 * Console logger implementation
 */
class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level = LogLevel::Info) : level_(level) {
        format_ = "%T [%L] %M";
    }
    
    void log(const LogEntry& entry) override {
        if (entry.level < level_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string formatted = formatMessage(entry);
        output(formatted);
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    void setFormat(const std::string& format) override {
        std::lock_guard<std::mutex> lock(mutex_);
        format_ = format;
    }
    
    std::string getFormat() const override {
        return format_;
    }
    
protected:
    virtual void output(const std::string& message) {
        std::cout << message << std::endl;
    }
    
    std::string formatMessage(const LogEntry& entry) {
        std::ostringstream oss;
        
        // Simple formatting for now - can be extended
        std::time_t timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm tm = *std::localtime(&timestamp);
        
        oss << std::put_time(&tm, "%H:%M:%S");
        oss << " [";
        
        switch (entry.level) {
            case LogLevel::Debug: oss << "DEBUG"; break;
            case LogLevel::Info: oss << "INFO"; break;
            case LogLevel::Warning: oss << "WARNING"; break;
            case LogLevel::Error: oss << "ERROR"; break;
            case LogLevel::Critical: oss << "CRITICAL"; break;
        }
        
        oss << "] ";
        oss << entry.message;
        
        if (!entry.source.empty()) {
            oss << " (" << entry.source << ")";
        }
        
        return oss.str();
    }
    
private:
    LogLevel level_;
    std::string format_;
    mutable std::mutex mutex_;
};

/**
 * File logger implementation
 */
class FileLogger : public Logger {
public:
    FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info) : level_(level) {
        format_ = "%T [%L] %M %F:%L";
        openFile(filepath);
    }
    
    ~FileLogger() override {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    void log(const LogEntry& entry) override {
        if (entry.level < level_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string formatted = formatMessage(entry);
        file_ << formatted << std::endl;
        file_.flush();
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    void setFormat(const std::string& format) override {
        std::lock_guard<std::mutex> lock(mutex_);
        format_ = format;
    }
    
    std::string getFormat() const override {
        return format_;
    }
    
    void openFile(const std::string& filepath) {
        file_.open(filepath, std::ios::app);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filepath);
        }
    }
    
protected:
    std::string formatMessage(const LogEntry& entry) override {
        std::ostringstream oss;
        
        std::time_t timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm tm = *std::localtime(&timestamp);
        
        oss << std::put_time(&tm, "%H:%M:%S");
        oss << " [";
        
        switch (entry.level) {
            case LogLevel::Debug: oss << "DEBUG"; break;
            case LogLevel::Info: oss << "INFO"; break;
            case LogLevel::Warning: oss << "WARNING"; break;
            case LogLevel::Error: oss << "ERROR"; break;
            case LogLevel::Critical: oss << "CRITICAL"; break;
        }
        
        oss << "] ";
        oss << entry.message;
        
        if (!entry.source.empty()) {
            oss << " (" << entry.source << ")";
        }
        
        oss << " " << entry.fileName << ":" << entry.lineNumber;
        
        return oss.str();
    }
    
private:
    LogLevel level_;
    std::string format_;
    std::ofstream file_;
    mutable std::mutex mutex_;
};

/**
 * Global logger manager
 */
class LoggerManager {
public:
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }
    
    void addLogger(std::shared_ptr<Logger> logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.push_back(logger);
    }
    
    void removeLogger(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.erase(std::remove_if(loggers_.begin(), loggers_.end(),
                                     [&name](const std::shared_ptr<Logger>& logger) {
                                         return dynamic_cast<ConsoleLogger*>(logger.get()) != nullptr;
                                     }),
                       loggers_.end());
    }
    
    void log(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->log(entry);
        }
    }
    
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->setLevel(level);
        }
    }
    
    LogLevel getLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!loggers_.empty()) {
            return loggers_[0]->getLevel();
        }
        return LogLevel::Info;
    }
    
    void setFormat(const std::string& format) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->setFormat(format);
        }
    }
    
    std::string getFormat() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!loggers_.empty()) {
            return loggers_[0]->getFormat();
        }
        return "%T [%L] %M";
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.clear();
    }
    
private:
    LoggerManager() = default;
    ~LoggerManager() = default;
    
    std::vector<std::shared_ptr<Logger>> loggers_;
    mutable std::mutex mutex_;
};

/**
 * Convenience functions for logging
 */
inline void logMessage(LogLevel level, const std::string& message,
                      const std::string& source = "",
                      int lineNumber = 0, const std::string& fileName = "") {
    LoggerManager::getInstance().log(LogEntry(level, message, source, lineNumber, fileName));
}

inline void NLM_LOG_DEBUG(const std::string& message, const std::string& source = "") {
    logMessage(LogLevel::Debug, message, source, 0, "");
}

inline void NLM_LOG_INFO(const std::string& message, const std::string& source = "") {
    logMessage(LogLevel::Info, message, source, 0, "");
}

inline void NLM_LOG_WARNING(const std::string& message, const std::string& source = "") {
    logMessage(LogLevel::Warning, message, source, 0, "");
}

inline void NLM_LOG_ERROR(const std::string& message, const std::string& source = "") {
    logMessage(LogLevel::Error, message, source, 0, "");
}

inline void NLM_LOG_CRITICAL(const std::string& message, const std::string& source = "") {
    logMessage(LogLevel::Critical, message, source, 0, "");
}

inline void NLM_LOG(const std::string& message, const std::string& source = "") {
    NLM_LOG_INFO(message, source);
}

/**
 * Standardized logging macros
 */
#define NLM_LOG_DEBUG_IMPL(msg) NLM_LOG_DEBUG(msg, __func__)
#define NLM_LOG_INFO_IMPL(msg) NLM_LOG_INFO(msg, __func__)
#define NLM_LOG_WARNING_IMPL(msg) NLM_LOG_WARNING(msg, __func__)
#define NLM_LOG_ERROR_IMPL(msg) NLM_LOG_ERROR(msg, __func__)
#define NLM_LOG_CRITICAL_IMPL(msg) NLM_LOG_CRITICAL(msg, __func__)

} // namespace nlm
