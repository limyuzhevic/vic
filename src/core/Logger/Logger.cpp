// NLM (熙然) - Neural Learning Machine
// Enhanced Logging System - Fixed implementation
//
// This file implements an improved logging system with:
// - Simplified mutex usage with RAII
// - Multiple logger implementations
// - Enhanced features like log levels, formatting, and filtering
// - Better error handling and thread safety
// - Performance optimizations

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <vector>
#include <map>
#include <thread>

namespace nlm {

// Simple thread-safe ring buffer for logging
class SimpleLogBuffer {
public:
    SimpleLogBuffer(size_t capacity = 10000) : capacity_(capacity) {}
    
    void add(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.size() >= capacity_) {
            buffer_.pop_front();
        }
        buffer_.push_back(entry);
    }
    
    std::vector<LogEntry> getRecent(size_t count) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t start = std::max<size_t>(0, buffer_.size() - count);
        return std::vector<LogEntry>(buffer_.begin() + start, buffer_.end());
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.clear();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }
    
private:
    std::deque<LogEntry> buffer_;
    mutable std::mutex mutex_;
    size_t capacity_;
};

// Enhanced console logger with better formatting and features
class EnhancedConsoleLogger : public ILogger {
public:
    struct Config {
        LogLevel minLevel = LogLevel::Info;
        bool useColors = true;
        bool useTimestamps = true;
        bool useThreadIds = true;
        bool useProcessIds = true;
        std::string timeFormat = "%H:%M:%S";
        std::string colorReset = "\033[0m";
    };
    
    EnhancedConsoleLogger(const Config& config = Config()) : config_(config) {
        initColors();
    }
    
    ~EnhancedConsoleLogger() override {
        if (fileOut_.is_open()) {
            fileOut_.close();
        }
    }
    
    void log(const LogEntry& entry) override {
        if (entry.level < config_.minLevel) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string formatted = formatEntry(entry);
        outputToConsole(formatted);
        outputToFile(formatted);
    }
    
    void setLevel(LogLevel level) override {
        config_.minLevel = level;
    }
    
    LogLevel getLevel() const override {
        return config_.minLevel;
    }
    
    // Console logger specific methods
    void setUseColors(bool useColors) {
        config_.useColors = useColors;
        if (useColors) {
            initColors();
        }
    }
    
    bool getUseColors() const {
        return config_.useColors;
    }
    
    void setUseTimestamps(bool use) {
        config_.useTimestamps = use;
    }
    
    bool getUseTimestamps() const {
        return config_.useTimestamps;
    }
    
    void setOutputFile(const std::string& filepath) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileOut_.is_open()) {
            fileOut_.close();
        }
        fileOut_.open(filepath, std::ios::app);
        if (!fileOut_.is_open()) {
            std::cerr << "Failed to open log file: " << filepath << std::endl;
        }
    }
    
    void closeOutputFile() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileOut_.is_open()) {
            fileOut_.close();
        }
    }
    
private:
    void initColors() {
        if (!config_.useColors) {
            return;
        }
        
        colorMap_[LogLevel::Debug] = "\033[36m";    // Cyan
        colorMap_[LogLevel::Info] = "\033[32m";     // Green
        colorMap_[LogLevel::Warning] = "\033[33m";  // Yellow
        colorMap_[LogLevel::Error] = "\033[31m";    // Red
        colorMap_[LogLevel::Critical] = "\033[35m"; // Magenta
    }
    
    std::string getLevelColor(LogLevel level) const {
        auto it = colorMap_.find(level);
        return (it != colorMap_.end()) ? it->second : "";
    }
    
    std::string getLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string formatEntry(const LogEntry& entry) {
        std::ostringstream oss;
        
        // Add level with color
        if (config_.useColors) {
            oss << getLevelColor(entry.level);
        }
        oss << "[" << getLevelString(entry.level) << "]";
        if (config_.useColors) {
            oss << config_.colorReset;
        }
        oss << " ";
        
        // Add timestamp
        if (config_.useTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            oss << "." << std::setfill('0') << std::setw(3)
                << (entry.timestamp.time_since_epoch().milliseconds() % 1000);
            oss << " ";
        }
        
        // Add thread ID (simplified)
        if (config_.useThreadIds) {
            oss << "[TID:" << std::this_thread::get_id() << "] ";
        }
        
        // Add message
        oss << entry.message;
        
        return oss.str();
    }
    
    void outputToConsole(const std::string& formatted) {
        std::cout << formatted << std::endl;
    }
    
    void outputToFile(const std::string& formatted) {
        if (fileOut_.is_open()) {
            fileOut_ << formatted << std::endl;
            fileOut_.flush();
        }
    }
    
    Config config_;
    mutable std::mutex mutex_;
    std::ofstream fileOut_;
    std::map<LogLevel, std::string> colorMap_;
};

// Enhanced Logger class with buffer and filtering
class EnhancedLogger : public Logger {
public:
    EnhancedLogger() : globalLevel_(LogLevel::Info) {
        buffer_ = std::make_unique<SimpleLogBuffer>(50000);
    }
    
    ~EnhancedLogger() override {
        for (auto& logger : loggers_) {
            if (logger) {
                logger->closeOutputFile();
            }
        }
    }
    
    void addLogger(std::shared_ptr<ILogger> logger) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logger) {
            loggers_.push_back(logger);
            logger->setLevel(globalLevel_);
        }
    }
    
    void clearLoggers() override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& logger : loggers_) {
            if (logger) {
                logger->closeOutputFile();
            }
        }
        loggers_.clear();
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        globalLevel_ = level;
        for (auto& logger : loggers_) {
            if (logger) {
                logger->setLevel(level);
            }
        }
    }
    
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "") override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Apply level filter
        if (level < globalLevel_) {
            return;
        }
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Store in buffer for later retrieval
        buffer_->add(entry);
        
        // Send to all loggers
        for (auto& logger : loggers_) {
            if (logger) {
                logger->log(entry);
            }
        }
    }
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") override {
        log(LogLevel::Debug, message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "") override {
        log(LogLevel::Info, message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "") override {
        log(LogLevel::Warning, message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") override {
        log(LogLevel::Error, message, file, line, function);
    }
    
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "") override {
        log(LogLevel::Critical, message, file, line, function);
    }
    
    bool isEnabled(LogLevel level) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return level >= globalLevel_;
    }
    
    // Global logger instance management
    static Logger& getGlobal() {
        static EnhancedLogger globalLogger;
        return globalLogger;
    }
    
    static void setGlobal(std::shared_ptr<Logger> logger) {
        // This is a simplified implementation
        // In practice, we would need to manage the global logger more carefully
    }
    
    // Buffer management methods
    void clearLogBuffer() {
        if (buffer_) {
            buffer_->clear();
        }
    }
    
    std::vector<LogEntry> getRecentLogs(size_t count) {
        if (buffer_) {
            return buffer_->getRecent(count);
        }
        return std::vector<LogEntry>();
    }
    
private:
    std::vector<std::shared_ptr<ILogger>> loggers_;
    mutable std::mutex mutex_;
    LogLevel globalLevel_;
    std::unique_ptr<SimpleLogBuffer> buffer_;
};

// Stream-style logging helper implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    EnhancedLogger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Convenience macro implementations (using EnhancedLogger)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Factory functions for creating different logger types
inline std::shared_ptr<ILogger> createConsoleLogger(const EnhancedConsoleLogger::Config& config = EnhancedConsoleLogger::Config()) {
    return std::make_shared<EnhancedConsoleLogger>(config);
}

inline std::shared_ptr<ILogger> createFileLogger(const std::string& filepath, const EnhancedConsoleLogger::Config& config = EnhancedConsoleLogger::Config()) {
    auto logger = std::make_shared<EnhancedConsoleLogger>(config);
    logger->setOutputFile(filepath);
    return logger;
}

inline std::shared_ptr<Logger> createEnhancedLogger() {
    return std::make_shared<EnhancedLogger>();
}

// Global instance initialization
namespace {
    std::shared_ptr<Logger> globalLoggerInstance_ = createEnhancedLogger();
}

} // namespace nlm
