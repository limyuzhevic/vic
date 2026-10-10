// NLM Advanced Features - Logging System
// Comprehensive logging framework with multiple destinations, rotation, and formatting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <functional>
#include <deque>

namespace nlm {
namespace advanced {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Output format
enum class LogFormat {
    Text,
    JSON,
    CSV,
    XML,
    YAML
};

// Output destination
enum class LogDestination {
    Console,
    File,
    Network,
    Database
};

// Log entry structure
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string source;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::map<std::string, std::string> metadata;
    uint64_t threadId;
    
    LogEntry() : level(LogLevel::Info), line(0), threadId(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Log statistics
struct LogStatistics {
    uint64_t totalLogs;
    uint64_t logsByLevel[5]; // Index by LogLevel
    uint64_t logsBySource[100]; // Index by source name hash
    std::chrono::system_clock::time_point firstLogTime;
    std::chrono::system_clock::time_point lastLogTime;
    double logRatePerMinute;
    std::string mostFrequentMessage;
    uint64_t mostFrequentMessageCount;
    std::deque<std::string> recentMessages;
    
    LogStatistics() : totalLogs(0), logRatePerMinute(0.0) {
        for (int i = 0; i < 5; ++i) logsByLevel[i] = 0;
        for (int i = 0; i < 100; ++i) logsBySource[i] = 0;
    }
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

// Text log formatter
class TextLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::stringstream ss;
        
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        ss << " [" << levelToString(entry.level) << "]";
        
        if (!entry.source.empty()) {
            ss << " [" << entry.source << "]";
        }
        
        ss << " " << entry.message;
        
        if (!entry.file.empty() && entry.line > 0) {
            ss << " (" << entry.file << ":" << entry.line << ")";
        }
        
        if (!entry.function.empty()) {
            ss << " in " << entry.function;
        }
        
        // Add metadata if present
        if (!entry.metadata.empty()) {
            ss << " {";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
            ss << "}";
        }
        
        return ss.str();
    }
    
private:
    std::string levelToString(LogLevel level) {
        static const std::map<LogLevel, std::string> levelMap = {
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Info, "INFO"},
            {LogLevel::Warning, "WARNING"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Critical, "CRITICAL"}
        };
        
        auto it = levelMap.find(level);
        return (it != levelMap.end()) ? it->second : "UNKNOWN";
    }
};

// JSON log formatter
class JsonLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::stringstream ss;
        
        ss << "{";
        ss << "\"timestamp\": \"" << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S") << "Z\",";
        ss << "\"level\": \"" << levelToString(entry.level) << "\",";
        ss << "\"source\": \"" << entry.source << "\",";
        ss << "\"message\": \"" << escapeString(entry.message) << "\",";
        
        if (!entry.file.empty() && entry.line > 0) {
            ss << "\"location\": \"" << entry.file << ":" << entry.line << "\",";
        }
        
        if (!entry.function.empty()) {
            ss << "\"function\": \"" << entry.function << "\",";
        }
        
        if (!entry.metadata.empty()) {
            ss << "\"metadata\": {";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) ss << ", ";
                ss << "\"" << pair.first << "\": \"" << escapeString(pair.second) << "\"";
                first = false;
            }
            ss << "}";
        }
        
        ss << "}";
        return ss.str();
    }
    
private:
    std::string levelToString(LogLevel level) {
        static const std::map<LogLevel, std::string> levelMap = {
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Info, "INFO"},
            {LogLevel::Warning, "WARNING"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Critical, "CRITICAL"}
        };
        
        auto it = levelMap.find(level);
        return (it != levelMap.end()) ? it->second : "UNKNOWN";
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

// Log destination handler interface
class LogDestinationHandler {
public:
    virtual ~LogDestinationHandler() = default;
    virtual bool write(const LogEntry& entry, const std::string& formatted) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
};

// Console log destination
class ConsoleLogDestination : public LogDestinationHandler {
public:
    bool write(const LogEntry& entry, const std::string& formatted) override {
        std::cout << formatted << std::endl;
        return true;
    }
    
    void flush() override {
        std::cout.flush();
    }
    
    void close() override {
        // Nothing to close for console
    }
};

// File log destination with rotation
class FileLogDestination : public LogDestinationHandler {
public:
    FileLogDestination(const std::string& filepath, size_t maxSize, int maxFiles) 
        : filepath_(filepath), maxSize_(maxSize), maxFiles_(maxFiles) {
        rotation_ = 0;
        currentFile_.open(filepath_, std::ios::app | std::ios::binary);
        if (currentFile_.is_open()) {
            currentFile_.seekg(0, std::ios::end);
            currentSize_ = currentFile_.tellg();
        }
    }
    
    ~FileLogDestination() {
        close();
    }
    
    bool write(const LogEntry& entry, const std::string& formatted) override {
        if (!currentFile_.is_open()) {
            return false;
        }
        
        if (currentSize_ + formatted.size() + 1 > maxSize_) {
            rotateFiles();
        }
        
        currentFile_ << formatted << std::endl;
        if (!currentFile_.good()) {
            return false;
        }
        
        currentSize_ += formatted.size() + 1;
        return true;
    }
    
    void flush() override {
        if (currentFile_.is_open()) {
            currentFile_.flush();
        }
    }
    
    void close() override {
        if (currentFile_.is_open()) {
            currentFile_.close();
        }
    }
    
private:
    void rotateFiles() {
        if (currentFile_.is_open()) {
            currentFile_.close();
        }
        
        // Close current file and rotate if needed
        std::string currentPath = filepath_ + "." + std::to_string(rotation_);
        if (std::filesystem::exists(currentPath)) {
            // Remove oldest file if we have too many
            if (rotation_ >= static_cast<size_t>(maxFiles_) - 1) {
                std::filesystem::remove(currentPath);
                for (int i = maxFiles_ - 2; i >= 0; --i) {
                    std::string src = filepath_ + "." + std::to_string(i);
                    std::string dst = filepath_ + "." + std::to_string(i + 1);
                    if (std::filesystem::exists(src)) {
                        std::filesystem::rename(src, dst);
                    }
                }
                rotation_ = maxFiles_ - 2;
            } else {
                rotation_++;
            }
        }
        
        // Reopen with new name
        currentPath = filepath_ + "." + std::to_string(rotation_);
        currentFile_.open(currentPath, std::ios::trunc | std::ios::binary);
        if (currentFile_.is_open()) {
            currentSize_ = 0;
        }
    }
    
    std::string filepath_;
    size_t maxSize_;
    int maxFiles_;
    int rotation_;
    std::fstream currentFile_;
    size_t currentSize_;
};

// Logging system class
class LoggingSystem {
public:
    LoggingSystem();
    ~LoggingSystem();
    
    // Configure logging
    void configure(LogLevel level = LogLevel::Info, 
                   LogFormat format = LogFormat::Text,
                   const std::vector<LogDestination>& destinations = {LogDestination::Console});
    
    // Add destination
    void addDestination(LogDestination destination, const std::string& param = "");
    void addDestination(std::shared_ptr<LogDestinationHandler> handler);
    
    // Log methods
    void log(LogLevel level, const std::string& source, const std::string& message,
             const std::string& file = "", int line = 0, const std::string& function = "");
    void log(LogLevel level, const std::string& source, const std::string& message,
             const std::map<std::string, std::string>& metadata);
    
    // Convenience methods
    void debug(const std::string& source, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& source, const std::string& message,
              const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& source, const std::string& message,
                 const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& source, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& source, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Statistics
    LogStatistics getStatistics() const;
    void resetStatistics();
    
    // Configuration
    void setLevel(LogLevel level);
    void setFormat(LogFormat format);
    void setSourceFilter(const std::string& sourceFilter);
    void setAsyncLogging(bool enabled);
    
    // Log rotation
    void setFileRotation(const std::string& filepath, size_t maxSize, int maxFiles);
    void setMaxFileSize(size_t maxSize);
    void setMaxBackupFiles(int maxFiles);
    
    // Callbacks
    void setLogCallback(const std::function<void(const LogEntry&)>& callback);
    void setStatisticsCallback(const std::function<void(const LogStatistics&)>& callback);
    
    // File logging
    void setLogFile(const std::string& filepath);
    void closeLogFile();
    std::string getLogFilePath() const;
    
    // Formatting
    void setCustomFormatter(std::shared_ptr<LogFormatter> formatter);
    
    // Buffer management
    void setBufferSize(size_t bufferSize);
    void flushBuffers();
    
    // Thread safety
    void lock();
    void unlock();
    bool tryLock();
    
    // Global instance
    static LoggingSystem& getInstance();
    static void resetInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string createLogEntry(const std::string& source, const std::string& message,
                               LogLevel level, const std::string& file = "",
                               int line = 0, const std::string& function = "");
    
    // Global logging functions (for backward compatibility)
    static LoggingSystem& getGlobalLogger() {
        return getInstance();
    }
};

// Convenience macros (to be included in source files)
#define NLM_LOG_DEBUG(msg) LoggingSystem::getInstance().debug("", msg)
#define NLM_LOG_INFO(msg) LoggingSystem::getInstance().info("", msg)
#define NLM_LOG_WARNING(msg) LoggingSystem::getInstance().warning("", msg)
#define NLM_LOG_ERROR(msg) LoggingSystem::getInstance().error("", msg)
#define NLM_LOG_CRITICAL(msg) LoggingSystem::getInstance().critical("", msg)

// Advanced logging macros with source
#define NLM_LOG_DEBUG_SRC(src, msg) LoggingSystem::getInstance().debug(src, msg)
#define NLM_LOG_INFO_SRC(src, msg) LoggingSystem::getInstance().info(src, msg)
#define NLM_LOG_WARNING_SRC(src, msg) LoggingSystem::getInstance().warning(src, msg)
#define NLM_LOG_ERROR_SRC(src, msg) LoggingSystem::getInstance().error(src, msg)
#define NLM_LOG_CRITICAL_SRC(src, msg) LoggingSystem::getInstance().critical(src, msg)

// Stream-style logging
class LogStream {
public:
    LogStream(LogLevel level, const std::string& source, const std::string& file = "",
              int line = 0, const std::string& function = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::string source_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
};

#define NLM_LOG_STREAM(level, source) LogStream(level, source, __FILE__, __LINE__, __func__)

} // namespace advanced
} // namespace nlm

// Include implementation
#include "LoggingSystem.cpp"

// Export key functions for external use
namespace nlm {
    namespace advanced {
        // Global logging functions
        inline void logDebug(const std::string& source, const std::string& message,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "") {
            LoggingSystem::getInstance().debug(source, message, file, line, function);
        }
        
        inline void logInfo(const std::string& source, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "") {
            LoggingSystem::getInstance().info(source, message, file, line, function);
        }
        
        inline void logWarning(const std::string& source, const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
            LoggingSystem::getInstance().warning(source, message, file, line, function);
        }
        
        inline void logError(const std::string& source, const std::string& message,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "") {
            LoggingSystem::getInstance().error(source, message, file, line, function);
        }
        
        inline void logCritical(const std::string& source, const std::string& message,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "") {
            LoggingSystem::getInstance().critical(source, message, file, line, function);
        }
        
        // Convenience macros
        #define NLM_LOG_DEBUG(msg) nlm::advanced::logDebug("", msg)
        #define NLM_LOG_INFO(msg) nlm::advanced::logInfo("", msg)
        #define NLM_LOG_WARNING(msg) nlm::advanced::logWarning("", msg)
        #define NLM_LOG_ERROR(msg) nlm::advanced::logError("", msg)
        #define NLM_LOG_CRITICAL(msg) nlm::advanced::logCritical("", msg)
        
        #define NLM_LOG_DEBUG_SRC(src, msg) nlm::advanced::logDebug(src, msg)
        #define NLM_LOG_INFO_SRC(src, msg) nlm::advanced::logInfo(src, msg)
        #define NLM_LOG_WARNING_SRC(src, msg) nlm::advanced::logWarning(src, msg)
        #define NLM_LOG_ERROR_SRC(src, msg) nlm::advanced::logError(src, msg)
        #define NLM_LOG_CRITICAL_SRC(src, msg) nlm::advanced::logCritical(src, msg)
        
    } // namespace advanced
} // namespace nlm