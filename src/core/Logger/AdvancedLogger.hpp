#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <functional>
#include <unordered_map>

namespace nlm {

// Log levels
enum class LogLevel {
    Off = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// Log message structure
struct LogMessage {
    std::string timestamp;
    LogLevel level;
    std::string category;
    std::string message;
    std::string source;
    int line;
    std::unordered_map<std::string, std::string> metadata;
    
    LogMessage(LogLevel lvl, const std::string& cat, const std::string& msg, 
               const std::string& src = "", int lineNum = 0)
        : level(lvl), category(cat), message(msg), source(src), line(lineNum) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        timestamp = ss.str();
    }
};

// Log handler interface
class LogHandler {
public:
    virtual ~LogHandler() = default;
    virtual void handle(const LogMessage& msg) = 0;
};

// Console log handler
class ConsoleLogHandler : public LogHandler {
public:
    void handle(const LogMessage& msg) override {
        std::string levelStr;
        switch (msg.level) {
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Trace: levelStr = "TRACE"; break;
            default: levelStr = "UNKNOWN"; break;
        }
        
        std::cout << "[" << msg.timestamp << "] " << levelStr << " " << msg.category;
        if (!msg.source.empty()) {
            std::cout << " (" << msg.source << ":" << msg.line << ")";
        }
        std::cout << " - " << msg.message << std::endl;
        
        // Output metadata if any
        if (!msg.metadata.empty()) {
            std::cout << "  Metadata:";
            for (const auto& pair : msg.metadata) {
                std::cout << " " << pair.first << "=" << pair.second;
            }
            std::cout << std::endl;
        }
    }
};

// File log handler
class FileLogHandler : public LogHandler {
public:
    FileLogHandler(const std::string& filename, bool append = true) {
        std::ofstream file(filename, append ? std::ios::app : std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open log file: " + filename);
        }
        file.close();
        filename_ = filename;
    }
    
    void handle(const LogMessage& msg) override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        std::ofstream file(filename_, std::ios::app);
        if (!file.is_open()) {
            return; // Silently fail if file cannot be opened
        }
        
        std::string levelStr;
        switch (msg.level) {
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Trace: levelStr = "TRACE"; break;
            default: levelStr = "UNKNOWN"; break;
        }
        
        file << "[" << msg.timestamp << "] " << levelStr << " " << msg.category;
        if (!msg.source.empty()) {
            file << " (" << msg.source << ":" << msg.line << ")";
        }
        file << " - " << msg.message << "\n";
        
        // Output metadata if any
        if (!msg.metadata.empty()) {
            file << "  Metadata:";
            for (const auto& pair : msg.metadata) {
                file << " " << pair.first << "=" << pair.second;
            }
            file << "\n";
        }
        
        file.flush();
    }
    
private:
    std::string filename_;
    std::mutex fileMutex_;
};

// Log buffer for batch processing
class LogBuffer {
public:
    LogBuffer(size_t maxSize = 1000, double flushInterval = 1.0) 
        : maxSize_(maxSize), flushInterval_(flushInterval), 
          lastFlush_(0.0), messageCount_(0) {
        startTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    void addMessage(const LogMessage& msg) {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        buffer_.push_back(msg);
        messageCount_++;
        
        double now = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (messageCount_ >= maxSize_ || (now - lastFlush_) >= flushInterval_) {
            flush();
        }
    }
    
    void flush() {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        if (buffer_.empty()) return;
        
        // Forward buffered messages to handlers
        for (const auto& msg : buffer_) {
            for (const auto& handler : handlers_) {
                handler->handle(msg);
            }
        }
        
        buffer_.clear();
        messageCount_ = 0;
        lastFlush_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    void addHandler(std::shared_ptr<LogHandler> handler) {
        std::lock_guard<std::mutex> lock(handlersMutex_);
        handlers_.push_back(handler);
    }
    
    void removeHandler(LogHandler* handler) {
        std::lock_guard<std::mutex> lock(handlersMutex_);
        handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(),
                                      [handler](const std::shared_ptr<LogHandler>& h) {
                                          return h.get() == handler;
                                      }), handlers_.end());
    }
    
    size_t getMessageCount() const {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        return messageCount_;
    }
    
    void setLevelFilter(LogLevel level) {
        std::lock_guard<std::mutex> lock(filterMutex_);
        minLevel_ = level;
    }
    
    LogLevel getLevelFilter() const {
        std::lock_guard<std::mutex> lock(filterMutex_);
        return minLevel_;
    }
    
private:
    std::vector<LogMessage> buffer_;
    std::vector<std::shared_ptr<LogHandler>> handlers_;
    
    mutable std::mutex bufferMutex_;
    mutable std::mutex handlersMutex_;
    mutable std::mutex filterMutex_;
    
    size_t maxSize_;
    double flushInterval_;
    double lastFlush_;
    size_t messageCount_;
    double startTime_;
    
    LogLevel minLevel_ = LogLevel::Off;
};

// Global logger
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void initialize() {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (!initialized_) {
            logBuffer_ = std::make_shared<LogBuffer>();
            initialized_ = true;
            
            // Add default console handler
            logBuffer_->addHandler(std::make_shared<ConsoleLogHandler>());
            
            // Set default level
            setLevel(LogLevel::Info);
            
            logBuffer_->addHandler(std::make_shared<ConsoleLogHandler>());
            log(LogLevel::Info, "Core", "Logger initialized");
        }
    }
    
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(logMutex_);
        logBuffer_->setLevelFilter(level);
    }
    
    LogLevel getLevel() const {
        std::lock_guard<std::mutex> lock(logMutex_);
        return logBuffer_->getLevelFilter();
    }
    
    void addHandler(std::shared_ptr<LogHandler> handler) {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (initialized_) {
            logBuffer_->addHandler(handler);
        }
    }
    
    void removeHandler(LogHandler* handler) {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (initialized_) {
            logBuffer_->removeHandler(handler);
        }
    }
    
    template<typename... Args>
    void log(LogLevel level, const std::string& category, const std::string& format, Args... args) {
        if (!initialized_) initialize();
        
        std::lock_guard<std::mutex> lock(logMutex_);
        
        // Check if level is enabled
        if (level < logBuffer_->getLevelFilter()) {
            return;
        }
        
        // Format message
        std::string message = formatMessage(format, args...);
        
        // Add stack trace (simplified)
        std::string source = "";
        int line = 0;
        
        // Create log message
        LogMessage logMsg(level, category, message, source, line);
        
        // Add to buffer
        logBuffer_->addMessage(logMsg);
    }
    
    void flush() {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (initialized_) {
            logBuffer_->flush();
        }
    }
    
    // Convenience methods
    void error(const std::string& category, const std::string& message) {
        log(LogLevel::Error, category, message);
    }
    
    void warning(const std::string& category, const std::string& message) {
        log(LogLevel::Warning, category, message);
    }
    
    void info(const std::string& category, const std::string& message) {
        log(LogLevel::Info, category, message);
    }
    
    void debug(const std::string& category, const std::string& message) {
        log(LogLevel::Debug, category, message);
    }
    
    void trace(const std::string& category, const std::string& message) {
        log(LogLevel::Trace, category, message);
    }
    
    // Performance monitoring
    void logPerformance(const std::string& operation, double duration, const std::unordered_map<std::string, std::string>& metadata = {}) {
        std::ostringstream stream;
        stream << operation << " took " << duration << "ms";
        
        LogMessage msg(LogLevel::Debug, "Performance", stream.str(), "");
        for (const auto& pair : metadata) {
            msg.metadata[pair.first] = pair.second;
        }
        
        std::lock_guard<std::mutex> lock(logMutex_);
        logBuffer_->addMessage(msg);
    }
    
    // Memory monitoring
    void logMemoryUsage(const std::string& category, size_t memoryBytes) {
        std::ostringstream stream;
        stream << "Memory usage: " << memoryBytes / 1024 << " KB";
        
        LogMessage msg(LogLevel::Debug, category, stream.str(), "");
        msg.metadata["memory_bytes"] = std::to_string(memoryBytes);
        
        std::lock_guard<std::mutex> lock(logMutex_);
        logBuffer_->addMessage(msg);
    }
    
    // State management
    void shutdown() {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (initialized_) {
            logBuffer_->flush();
            logBuffer_.reset();
            initialized_ = false;
        }
    }
    
    // Log rotation
    void setMaxFileSize(size_t maxSize) {
        // This would require access to file handlers
        // For now, just store the setting
        maxFileSize_ = maxSize;
    }
    
    size_t getMaxFileSize() const { return maxFileSize_; }
    
private:
    Logger() : initialized_(false), maxFileSize_(100 * 1024 * 1024) {} // 100 MB default
    
    std::string formatMessage(const std::string& format) {
        return format;
    }
    
    template<typename T, typename... Args>
    std::string formatMessage(const std::string& format, T first, Args... args) {
        size_t pos = format.find("{}");
        if (pos == std::string::npos) {
            return format;
        }
        
        std::string result = format.substr(0, pos);
        result += std::to_string(first);
        result += format.substr(pos + 2);
        
        return formatMessage(result, args...);
    }
    
    std::shared_ptr<LogBuffer> logBuffer_;
    mutable std::mutex logMutex_;
    bool initialized_;
    size_t maxFileSize_;
};

// Global logger instance
Logger& getLogger() {
    return Logger::getInstance();
}

// Convenience macros
#define NLM_LOG_ERROR(category, message) nlm::getLogger().error(category, message)
#define NLM_LOG_WARNING(category, message) nlm::getLogger().warning(category, message)
#define NLM_LOG_INFO(category, message) nlm::getLogger().info(category, message)
#define NLM_LOG_DEBUG(category, message) nlm::getLogger().debug(category, message)
#define NLM_LOG_TRACE(category, message) nlm::getLogger().trace(category, message)

// Performance monitoring macros
#define NLM_PERF_LOG(operation, duration) nlm::getLogger().logPerformance(operation, duration)
#define NLM_MEM_LOG(category, bytes) nlm::getLogger().logMemoryUsage(category, bytes)

// Initialize global logger on first use
class LoggerInitializer {
public:
    LoggerInitializer() {
        nlm::Logger::getInstance().initialize();
    }
};

// Auto-initialize logger
static LoggerInitializer loggerInitializer;

} // namespace nlm