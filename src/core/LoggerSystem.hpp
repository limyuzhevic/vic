#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>
#include <map>
#include <chrono>
#include <mutex>
#include <iomanip>

namespace nlm {

// Forward declarations
class ILogger;

// Enhanced log severity levels with numeric values for filtering
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Notice = 3,
    Warning = 4,
    Error = 5,
    Critical = 6,
    Fatal = 7
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::string threadId;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    bool formatted;
    
    LogEntry()
        : level(LogLevel::Info), message(), file(), line(0), function(), 
          threadId(), timestamp(), metadata(), formatted(false) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(function),
          threadId(), timestamp(std::chrono::system_clock::now()), 
          metadata(), formatted(false) {
        // Get thread ID for enhanced logging
        std::stringstream ss;
        ss << std::this_thread::get_id();
        threadId = ss.str();
    }
};

// Logger formatter interface
class ILoggerFormatter {
public:
    virtual ~ILoggerFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

// Default text formatter
class TextFormatter : public ILoggerFormatter {
public:
    enum class Style {
        Plain,
        Colored,
        JSON,
        XML,
        CSV
    };
    
    TextFormatter(Style style = Style::Colored, bool useColors = true);
    std::string format(const LogEntry& entry) override;
    
    void setStyle(Style style);
    Style getStyle() const { return style_; }
    void setUseColors(bool useColors);
    bool getUseColors() const { return useColors_; }
    
private:
    Style style_;
    bool useColors_;
    
    std::string formatPlain(const LogEntry& entry) const;
    std::string formatColored(const LogEntry& entry) const;
    std::string formatJSON(const LogEntry& entry) const;
    std::string formatXML(const LogEntry& entry) const;
    std::string formatCSV(const LogEntry& entry) const;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Global logger system with singleton pattern
class LoggerSystem {
public:
    LoggerSystem();
    ~LoggerSystem();
    
    // Disable copying
    LoggerSystem(const LoggerSystem&) = delete;
    LoggerSystem& operator=(const LoggerSystem&) = delete;
    
    // Get global logger system instance
    static LoggerSystem& getInstance();
    
    // Configuration
    void setGlobalLevel(LogLevel level);
    LogLevel getGlobalLevel() const;
    
    // Add log formatters
    void addFormatter(std::shared_ptr<ILoggerFormatter> formatter);
    void removeAllFormatters();
    
    // Add logger sinks
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeAllLoggers();
    
    // Core logging interface
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", 
                int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "");
    void fatal(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Log with metadata
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& metadata,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Performance timing
    void logPerformance(const std::string& operation, std::chrono::milliseconds duration,
                        const std::map<std::string, std::string>& context = {});
    
    // Get current log statistics
    struct LogStatistics {
        size_t totalEntries;
        size_t entriesByLevel[8];  // One per LogLevel
        std::chrono::system_clock::time_point oldestEntry;
        std::chrono::system_clock::time_point newestEntry;
        size_t memoryUsage;
    };
    
    LogStatistics getStatistics() const;
    
    // Clear statistics
    void resetStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Get the default logger instance
    std::shared_ptr<ILogger> getDefaultLogger();
};

} // namespace nlm