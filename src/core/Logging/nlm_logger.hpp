// Enhanced Logging System with Severity Levels and Context

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>

#include "nlm_error.hpp"

namespace nlm {

// Enhanced Log Entry Structure
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    ErrorSeverity severity;
    std::string category;
    std::string component;
    std::string message;
    std::string threadId;
    int lineNumber;
    std::string fileName;
    std::vector<std::string> context;
    std::map<std::string, std::string> metadata;
    std::string sessionId;
    
    LogEntry(const std::string& msg, ErrorSeverity sev = ErrorSeverity::INFO,
             const std::string& cat = "General", const std::string& comp = "",
             int line = 0, const std::string& file = "")
        : timestamp(std::chrono::system_clock::now()), severity(sev), 
          category(cat), component(comp), message(msg), 
          lineNumber(line), fileName(file) {
        threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
};

// Log Filter
class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
};

// Log Level Filter
class LevelFilter : public LogFilter {
    ErrorSeverity minLevel;
    
public:
    LevelFilter(ErrorSeverity level) : minLevel(level) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return entry.severity >= minLevel;
    }
};

// Component Filter
class ComponentFilter : public LogFilter {
    std::set<std::string> allowedComponents;
    
public:
    ComponentFilter(const std::set<std::string>& components) 
        : allowedComponents(components) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return allowedComponents.empty() || 
               allowedComponents.find(entry.component) != allowedComponents.end();
    }
};

// Category Filter
class CategoryFilter : public LogFilter {
    std::set<std::string> allowedCategories;
    
public:
    CategoryFilter(const std::set<std::string>& categories) 
        : allowedCategories(categories) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return allowedCategories.empty() || 
               allowedCategories.find(entry.category) != allowedCategories.end();
    }
};

// Log Destination Interface
class LogDestination {
public:
    virtual ~LogDestination() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual bool isEnabled() const = 0;
};

// Console Destination
class ConsoleDestination : public LogDestination {
    ErrorSeverity minLevel;
    bool colorOutput;
    bool useTimestamps;
    std::string timestampFormat;
    
    std::mutex outputMutex;
    
public:
    ConsoleDestination(ErrorSeverity level = ErrorSeverity::DEBUG, 
                      bool colors = true, bool timestamps = true,
                      const std::string& format = "%Y-%m-%d %H:%M:%S")
        : minLevel(level), colorOutput(colors), useTimestamps(timestamps),
          timestampFormat(format) {}
    
    void log(const LogEntry& entry) override;
    void flush() override { std::cout.flush(); }
    bool isEnabled() const override { return true; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string getColoredOutput(const std::string& message, ErrorSeverity severity) const;
};

// File Destination
class FileDestination : public LogDestination {
    std::string filepath;
    std::ofstream fileStream;
    bool append;
    size_t maxFileSize;
    int maxBackupFiles;
    std::mutex fileMutex;
    
public:
    FileDestination(const std::string& path, bool appendMode = true,
                   size_t maxSize = 10 * 1024 * 1024, // 10MB
                   int maxBackups = 5)
        : filepath(path), append(appendMode), maxFileSize(maxSize), 
          maxBackupFiles(maxBackups) {}
    
    bool initialize();
    void log(const LogEntry& entry) override;
    void flush() override;
    bool isEnabled() const override { return fileStream.is_open(); }
    
private:
    void rotateLogFile();
    void cleanupOldBackups();
};

// Rolling File Destination
class RollingFileDestination : public LogDestination {
    std::string basePath;
    std::string filenamePattern;
    size_t maxFileSize;
    int maxFiles;
    bool useTimestamps;
    
    std::mutex fileMutex;
    
public:
    RollingFileDestination(const std::string& path, const std::string& pattern = "log_%Y%m%d_%H%M%S.txt",
                          size_t maxSize = 5 * 1024 * 1024, int maxFiles = 10,
                          bool timestamps = true)
        : basePath(path), filenamePattern(pattern), maxFileSize(maxSize), 
          maxFiles(maxFiles), useTimestamps(timestamps) {}
    
    bool initialize();
    void log(const LogEntry& entry) override;
    void flush() override;
    bool isEnabled() const override { return !currentFileStream_.is_open(); }
    
private:
    std::ofstream currentFileStream_;
    std::string getCurrentLogFile() const;
    std::string generateFilename(const std::chrono::system_clock::time_point& timestamp) const;
};

// Enhanced Logger Class
class Logger {
private:
    std::vector<std::unique_ptr<LogDestination>> destinations;
    std::vector<std::unique_ptr<LogFilter>> filters;
    std::vector<LogEntry> logBuffer;
    std::mutex loggerMutex;
    size_t maxBufferSize;
    bool autoFlush;
    bool useTimestamps;
    std::string sessionId;
    ErrorSeverity currentLevel;
    std::map<std::string, std::string> globalMetadata;
    
    // Singleton pattern
    static std::unique_ptr<Logger> instance;
    static std::mutex instanceMutex;
    
    Logger(); // Private constructor
    
public:
    ~Logger();
    
    // Singleton access
    static Logger& getInstance();
    static void initialize(const std::string& configFile = "");
    static void shutdown();
    
    // Core logging functions
    void log(const std::string& message, ErrorSeverity severity = ErrorSeverity::INFO,
             const std::string& category = "General", const std::string& component = "",
             int lineNumber = 0, const std::string& fileName = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& category = "Debug",
               const std::string& component = "", int line = 0, const std::string& file = "");
    
    void info(const std::string& message, const std::string& category = "Info",
              const std::string& component = "", int line = 0, const std::string& file = "");
    
    void warning(const std::string& message, const std::string& category = "Warning",
                 const std::string& component = "", int line = 0, const std::string& file = "");
    
    void error(const std::string& message, const std::string& category = "Error",
               const std::string& component = "", int line = 0, const std::string& file = "");
    
    void critical(const std::string& message, const std::string& category = "Critical",
                  const std::string& component = "", int line = 0, const std::string& file = "");
    
    // Enhanced logging with context
    void logWithContext(const std::string& message, ErrorSeverity severity,
                       const std::vector<std::string>& context,
                       const std::string& category = "General",
                       const std::string& component = "",
                       const std::map<std::string, std::string>& metadata = {});
    
    // Destination management
    void addDestination(std::unique_ptr<LogDestination> destination);
    void removeDestination(LogDestination* destination);
    void clearDestinations();
    
    // Filter management
    void addFilter(std::unique_ptr<LogFilter> filter);
    void removeFilter(LogFilter* filter);
    void clearFilters();
    
    // Configuration
    void setLogLevel(ErrorSeverity level);
    void setGlobalMetadata(const std::string& key, const std::string& value);
    void setMaxBufferSize(size_t size) { maxBufferSize = size; }
    void enableAutoFlush(bool enable) { autoFlush = enable; }
    
    // Buffer management
    void flushBuffer();
    void enableBuffering(bool enable) { autoFlush = !enable; }
    
    // Log retrieval
    std::vector<LogEntry> getRecentLogs(size_t count = 100) const;
    std::vector<LogEntry> getLogsBySeverity(ErrorSeverity severity, size_t count = 100) const;
    std::vector<LogEntry> getLogsByCategory(const std::string& category, size_t count = 100) const;
    std::vector<LogEntry> getLogsByComponent(const std::string& component, size_t count = 100) const;
    
    // Search and filter
    std::vector<LogEntry> searchLogs(const std::string& pattern, ErrorSeverity severity = ErrorSeverity::DEBUG) const;
    std::vector<LogEntry> searchLogsByTime(const std::chrono::system_clock::time_point& start,
                                           const std::chrono::system_clock::time_point& end) const;
    
    // Export
    bool exportLogs(const std::string& filepath, const std::vector<LogEntry>& entries = {}) const;
    bool exportLogsJSON(const std::string& filepath, const std::vector<LogEntry>& entries = {}) const;
    bool exportLogsCSV(const std::string& filepath, const std::vector<LogEntry>& entries = {}) const;
    
    // Statistics
    struct LogStatistics {
        size_t totalLogs;
        std::map<ErrorSeverity, size_t> logsBySeverity;
        std::map<std::string, size_t> logsByCategory;
        std::map<std::string, size_t> logsByComponent;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    LogStatistics getStatistics() const;
    
    // Session management
    const std::string& getSessionId() const { return sessionId; }
    
    // Configuration from file
    bool loadConfiguration(const std::string& configFile);
    bool saveConfiguration(const std::string& configFile) const;
    
    // Diagnostic
    std::string getLoggerStatus() const;
    bool isHealthy() const;
    
    // Convenience macros for backward compatibility
    #define NLM_LOG_ENTRY(msg, sev, cat, comp) logger.log(msg, sev, cat, comp, __LINE__, __FILE__)
};

// Logging convenience functions
inline void logMessage(const std::string& message, ErrorSeverity severity = ErrorSeverity::INFO,
                      const std::string& category = "General", const std::string& component = "",
                      const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, severity, context, category, component);
}

inline void logDebug(const std::string& message, const std::string& category = "Debug",
                    const std::string& component = "", const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, ErrorSeverity::DEBUG, context, category, component);
}

inline void logInfo(const std::string& message, const std::string& category = "Info",
                   const std::string& component = "", const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, ErrorSeverity::INFO, context, category, component);
}

inline void logWarning(const std::string& message, const std::string& category = "Warning",
                      const std::string& component = "", const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, ErrorSeverity::WARNING, context, category, component);
}

inline void logError(const std::string& message, const std::string& category = "Error",
                    const std::string& component = "", const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, ErrorSeverity::ERROR, context, category, component);
}

inline void logCritical(const std::string& message, const std::string& category = "Critical",
                       const std::string& component = "", const std::vector<std::string>& context = {}) {
    Logger::getInstance().logWithContext(message, ErrorSeverity::CRITICAL, context, category, component);
}

// Legacy logging macros (for backward compatibility)
#define NLM_LOG_INFO(msg) nlm::logInfo(msg)
#define NLM_LOG_WARNING(msg) nlm::logWarning(msg)
#define NLM_LOG_ERROR(msg) nlm::logError(msg)
#define NLM_LOG_DEBUG(msg) nlm::logDebug(msg)
#define NLM_LOG_CRITICAL(msg) nlm::logCritical(msg)