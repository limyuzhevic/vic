// NLM (熙然) - Neural Learning Machine
// Enhanced Logger System with advanced features
// Improved logging system with structured logging, error handling, and performance tracking

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <map>
#include <iomanip>
#include <ctime>

namespace nlm {

// Log levels with enhanced hierarchy
enum class LogLevel {
    Trace = 0,      // Detailed debug information
    Debug = 1,       // Debug information
    Info = 2,        // General information
    Warn = 3,        // Warning conditions
    Error = 4,       // Error conditions
    Fatal = 5,       // Fatal errors (system shutdown)
    Off = 6          // No logging
};

// Log message with structured data
struct LogMessage {
    LogLevel level;
    std::string message;
    std::string timestamp;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    std::chrono::high_resolution_clock::time_point timePoint;
    
    LogMessage(LogLevel lvl, const std::string& msg) 
        : level(lvl), message(msg), timePoint(std::chrono::high_resolution_clock::now()) {
        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestamp = ss.str();
        
        // Generate thread ID (simplified)
        std::stringstream tid;
        tid << std::this_thread::get_id();
        threadId = tid.str();
    }
};

// Log formatter for different output formats
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogMessage& message) = 0;
    virtual std::string getFormatName() const = 0;
};

// Plain text formatter
class PlainTextFormatter : public LogFormatter {
public:
    std::string format(const LogMessage& message) override {
        std::stringstream ss;
        
        // Format level
        std::string levelStr;
        switch (message.level) {
            case LogLevel::Trace: levelStr = "TRACE"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Warn: levelStr = "WARN"; break;
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Fatal: levelStr = "FATAL"; break;
            case LogLevel::Off: levelStr = "OFF"; break;
        }
        
        ss << "[" << message.timestamp << "] "
           << "[" << levelStr << "] "
           << message.message;
        
        // Add thread ID if not empty
        if (!message.threadId.empty()) {
            ss << " [" << message.threadId << "]";
        }
        
        // Add metadata
        if (!message.metadata.empty()) {
            ss << " {";
            bool first = true;
            for (const auto& pair : message.metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
            ss << "}";
        }
        
        return ss.str();
    }
    
    std::string getFormatName() const override { return "PlainText"; }
};

// JSON formatter
class JSONFormatter : public LogFormatter {
public:
    std::string format(const LogMessage& message) override {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"timestamp\": \"" << message.timestamp << "\",\n";
        ss << "  \"level\": \"" << getLevelString(message.level) << "\",\n";
        ss << "  \"message\": \"" << escapeJSON(message.message) << "\",\n";
        ss << "  \"thread_id\": \"" << message.threadId << "\",\n";
        
        if (!message.metadata.empty()) {
            ss << "  \"metadata\": {\n";
            bool first = true;
            for (const auto& pair : message.metadata) {
                if (!first) ss << ",\n";
                ss << "    \"" << pair.first << "\": \"" << escapeJSON(pair.second) << "\"";
                first = false;
            }
            ss << "\n  }";
        }
        
        ss << "\n}";
        return ss.str();
    }
    
    std::string getFormatName() const override { return "JSON"; }
    
private:
    std::string getLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warn: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            case LogLevel::Off: return "OFF";
            default: return "UNKNOWN";
        }
    }
    
    std::string escapeJSON(const std::string& input) const {
        std::stringstream ss;
        for (char c : input) {
            switch (c) {
                case '\"': ss << "\\\""; break;
                case '\\': ss << "\\\\"; break;
                case '\b': ss << "\\b"; break;
                case '\f': ss << "\\f"; break;
                case '\n': ss << "\\n"; break;
                case '\r': ss << "\\r"; break;
                case '\t': ss << "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                    } else {
                        ss << c;
                    }
                    break;
            }
        }
        return ss.str();
    }
};

// CSV formatter
class CSVFormatter : public LogFormatter {
public:
    std::string format(const LogMessage& message) override {
        std::stringstream ss;
        ss << message.timestamp << ",";
        ss << getLevelString(message.level) << ",";
        ss << "\"" << message.message << "\"".replace(message.message.find('\"'), 1, "\\\"") << ",";
        ss << message.threadId << ",";
        ss << formatMetadata(message.metadata);
        return ss.str();
    }
    
    std::string getFormatName() const override { return "CSV"; }
    
private:
    std::string getLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warn: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            case LogLevel::Off: return "OFF";
            default: return "UNKNOWN";
        }
    }
    
    std::string formatMetadata(const std::map<std::string, std::string>& metadata) const {
        std::stringstream ss;
        bool first = true;
        for (const auto& pair : metadata) {
            if (!first) ss << "|";
            ss << pair.first << "=" << pair.second;
            first = false;
        }
        return ss.str();
    }
};

// Logger interface
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(LogLevel level, const std::string& message) = 0;
    virtual void logWithMetadata(LogLevel level, const std::string& message, 
                                const std::map<std::string, std::string>& metadata) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual std::string getName() const = 0;
};

// Console logger with formatting support
class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level, const std::shared_ptr<LogFormatter>& formatter = nullptr)
        : level_(level), formatter_(formatter ? formatter : std::make_shared<PlainTextFormatter>()) {}
    
    void log(LogLevel level, const std::string& message) override {
        if (level < this->level_) return;
        
        LogMessage logMsg(level, message);
        std::cout << formatter_->format(logMsg) << std::endl;
    }
    
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& metadata) override {
        if (level < this->level_) return;
        
        LogMessage logMsg(level, message);
        logMsg.metadata = metadata;
        std::cout << formatter_->format(logMsg) << std::endl;
    }
    
    LogLevel getLevel() const override { return level_; }
    void setLevel(LogLevel level) override { level_ = level; }
    std::string getName() const override { return "ConsoleLogger"; }
    
    void setFormatter(const std::shared_ptr<LogFormatter>& formatter) {
        formatter_ = formatter ? formatter : std::make_shared<PlainTextFormatter>();
    }
    
    std::string getFormatterName() const {
        return formatter_ ? formatter_->getFormatName() : "Unknown";
    }
    
private:
    LogLevel level_;
    std::shared_ptr<LogFormatter> formatter_;
};

// File logger
class FileLogger : public Logger {
public:
    FileLogger(const std::string& filename, LogLevel level, 
               const std::shared_ptr<LogFormatter>& formatter = nullptr)
        : level_(level), formatter_(formatter ? formatter : std::make_shared<PlainTextFormatter>()) {
        fileStream_.open(filename, std::ios::app);
        if (!fileStream_.is_open()) {
            throw std::runtime_error("Cannot open log file: " + filename);
        }
    }
    
    ~FileLogger() {
        if (fileStream_.is_open()) {
            fileStream_.close();
        }
    }
    
    void log(LogLevel level, const std::string& message) override {
        if (level < this->level_) return;
        
        LogMessage logMsg(level, message);
        fileStream_ << formatter_->format(logMsg) << std::endl;
        fileStream_.flush();
    }
    
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& metadata) override {
        if (level < this->level_) return;
        
        LogMessage logMsg(level, message);
        logMsg.metadata = metadata;
        fileStream_ << formatter_->format(logMsg) << std::endl;
        fileStream_.flush();
    }
    
    LogLevel getLevel() const override { return level_; }
    void setLevel(LogLevel level) override { level_ = level; }
    std::string getName() const override { return "FileLogger"; }
    
    void setFormatter(const std::shared_ptr<LogFormatter>& formatter) {
        formatter_ = formatter ? formatter : std::make_shared<PlainTextFormatter>();
    }
    
    std::string getFormatterName() const {
        return formatter_ ? formatter_->getFormatName() : "Unknown";
    }
    
private:
    LogLevel level_;
    std::shared_ptr<LogFormatter> formatter_;
    std::ofstream fileStream_;
};

// Global logger manager
class LoggerManager {
public:
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }
    
    void addLogger(const std::shared_ptr<Logger>& logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.push_back(logger);
    }
    
    void removeLogger(const std::shared_ptr<Logger>& logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find(loggers_.begin(), loggers_.end(), logger);
        if (it != loggers_.end()) {
            loggers_.erase(it);
        }
    }
    
    void clearLoggers() {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.clear();
    }
    
    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->log(level, message);
        }
    }
    
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& metadata) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->logWithMetadata(level, message, metadata);
        }
    }
    
    void setGlobalLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            logger->setLevel(level);
        }
    }
    
    std::vector<std::string> getLoggerNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        for (const auto& logger : loggers_) {
            names.push_back(logger->getName());
        }
        return names;
    }
    
    void setLogLevelByName(const std::string& loggerName, LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& logger : loggers_) {
            if (logger->getName() == loggerName) {
                logger->setLevel(level);
                break;
            }
        }
    }
    
private:
    LoggerManager() = default;
    ~LoggerManager() = default;
    
    std::vector<std::shared_ptr<Logger>> loggers_;
    mutable std::mutex mutex_;
    
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;
};

// Convenience functions
inline void NLM_LOG_TRACE(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Trace, message);
}

inline void NLM_LOG_DEBUG(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Debug, message);
}

inline void NLM_LOG_INFO(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Info, message);
}

inline void NLM_LOG_WARN(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Warn, message);
}

inline void NLM_LOG_ERROR(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Error, message);
}

inline void NLM_LOG_FATAL(const std::string& message) {
    LoggerManager::getInstance().log(LogLevel::Fatal, message);
}

inline void NLM_LOG(const std::string& message) {
    NLM_LOG_INFO(message);
}

// Structured logging convenience functions
inline void NLM_LOG_TRACE_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Trace, message, metadata);
}

inline void NLM_LOG_DEBUG_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Debug, message, metadata);
}

inline void NLM_LOG_INFO_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Info, message, metadata);
}

inline void NLM_LOG_WARN_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Warn, message, metadata);
}

inline void NLM_LOG_ERROR_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Error, message, metadata);
}

inline void NLM_LOG_FATAL_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    LoggerManager::getInstance().logWithMetadata(LogLevel::Fatal, message, metadata);
}

inline void NLM_LOG_WITH(const std::string& message, const std::map<std::string, std::string>& metadata) {
    NLM_LOG_INFO_WITH(message, metadata);
}

} // namespace nlm
