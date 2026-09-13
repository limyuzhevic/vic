#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/Constants.hpp"

namespace nlm {

/**
 * Advanced logging system with multiple handlers and filtering
 */
class AdvancedLogger {
public:
    // Log levels with numeric values for filtering
    enum class Level {
        Off = -1,
        Fatal = 0,
        Error = 1,
        Warning = 2,
        Info = 3,
        Debug = 4,
        Trace = 5,
        All = 10
    };

    // Log entry with structured data
    struct LogEntry {
        Level level;
        std::chrono::system_clock::time_point timestamp;
        std::string source;
        std::string threadId;
        std::string message;
        std::map<std::string, std::string> fields;
        std::map<std::string, std::string> metadata;
        std::vector<std::string> tags;
        
        std::string getFormattedTimestamp() const;
        std::string getLevelString() const;
        std::string getFormattedMessage() const;
    };

    // Handler for log entries
    using LogHandler = std::function<bool(const LogEntry&)>

    AdvancedLogger(const std::string& name = "NLM", Level level = Level::Info);
    ~AdvancedLogger();

    // Configure logging
    void configure(Level level, const std::string& format = "%timestamp %level %source: %message");
    
    // Add a handler
    void addHandler(LogHandler handler, Level level = Level::All);
    
    // Remove a handler
    void removeHandler(LogHandler handler);
    
    // Set minimum log level
    void setLevel(Level level);
    
    // Get current log level
    Level getLevel() const { return minLevel; }
    
    // Log methods with multiple overloads
    void log(Level level, const std::string& message);
    void log(Level level, const std::string& message, const std::map<std::string, std::string>& fields);
    
    // Convenience methods
    void trace(const std::string& message) { if (Level::Trace >= minLevel) log(Level::Trace, message); }
    void debug(const std::string& message) { if (Level::Debug >= minLevel) log(Level::Debug, message); }
    void info(const std::string& message) { if (Level::Info >= minLevel) log(Level::Info, message); }
    void warning(const std::string& message) { if (Level::Warning >= minLevel) log(Level::Warning, message); }
    void error(const std::string& message) { if (Level::Error >= minLevel) log(Level::Error, message); }
    void fatal(const std::string& message) { if (Level::Fatal >= minLevel) log(Level::Fatal, message); }
    
    // Log with formatted message
    template<typename... Args>
    void trace(const std::string& format, Args&&... args) {
        if (Level::Trace >= minLevel) {
            log(Level::Trace, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        if (Level::Debug >= minLevel) {
            log(Level::Debug, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        if (Level::Info >= minLevel) {
            log(Level::Info, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void warning(const std::string& format, Args&&... args) {
        if (Level::Warning >= minLevel) {
            log(Level::Warning, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        if (Level::Error >= minLevel) {
            log(Level::Error, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void fatal(const std::string& format, Args&&... args) {
        if (Level::Fatal >= minLevel) {
            log(Level::Fatal, formatMessage(format, std::forward<Args>(args)...));
        }
    }
    
    // Log with fields
    void trace(const std::string& message, const std::map<std::string, std::string>& fields) {
        if (Level::Trace >= minLevel) {
            log(Level::Trace, message, fields);
        }
    }
    
    // Add tag to current log
    static void addTag(const std::string& tag);
    
    // Get tag
    static std::vector<std::string> getTags();
    
    // Clear tags
    static void clearTags();
    
    // Get logger name
    const std::string& getName() const { return name; }
    
    // Flush all handlers
    void flush();
    
    // Get statistics
    struct Stats {
        uint64_t totalLogs = 0;
        uint64_t logsByLevel[6] = {0}; // Level enum indices
        double averageLogSize = 0.0;
    };
    
    Stats getStats() const { return stats; }

private:
    std::string name;
    Level minLevel;
    std::string format;
    std::vector<std::pair<LogHandler, Level>> handlers;
    mutable std::mutex mutex;
    Stats stats;
    std::map<std::string, std::string> currentFields;
    std::vector<std::string> tags;
    
    // Log entry
    void logInternal(Level level, const std::string& message, const std::map<std::string, std::string>& fields = {});
    
    // Format message with arguments
    static std::string formatMessage(const std::string& format, std::vector<std::string> args);
    
    // Get thread ID as string
    static std::string getThreadId();
    
    // Update statistics
    void updateStats(Level level, size_t messageSize);
};

// Global logger instance
extern std::shared_ptr<AdvancedLogger> globalLogger;

// Convenience logging macros
#define NLM_TRACE(...) do { if (nlm::globalLogger) nlm::globalLogger->trace(__VA_ARGS__); } while(0)
#define NLM_DEBUG(...) do { if (nlm::globalLogger) nlm::globalLogger->debug(__VA_ARGS__); } while(0)
#define NLM_INFO(...) do { if (nlm::globalLogger) nlm::globalLogger->info(__VA_ARGS__); } while(0)
#define NLM_WARNING(...) do { if (nlm::globalLogger) nlm::globalLogger->warning(__VA_ARGS__); } while(0)
#define NLM_ERROR(...) do { if (nlm::globalLogger) nlm::globalLogger->error(__VA_ARGS__); } while(0)
#define NLM_FATAL(...) do { if (nlm::globalLogger) nlm::globalLogger->fatal(__VA_ARGS__); } while(0)

} // namespace nlm
