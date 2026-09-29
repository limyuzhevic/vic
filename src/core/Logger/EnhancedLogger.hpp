#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <fstream>

namespace nlm {

// Enhanced log levels for better granularity
enum class EnhancedLogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Notice = 3,
    Warning = 4,
    Error = 5,
    Critical = 6,
    Alert = 7,
    Emergency = 8,
    Off = 99
};

// Log category for better organization
enum class LogCategory {
    All,
    Core,
    Brain,
    Agent,
    Memory,
    Prediction,
    Cognition,
    Development,
    Neuromodulation,
    Plasticity,
    Sensory,
    Motor,
    Environment,
    Experiment,
    Performance
};

// Enhanced log entry with more metadata
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    LogCategory category;
    std::string message;
    std::string file;
    int line = 0;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    int threadId = 0;
    std::string threadName;
    
    EnhancedLogEntry(EnhancedLogLevel lvl, LogCategory cat, const std::string& msg)
        : level(lvl), category(cat), message(msg), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Enhanced logger interface
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(EnhancedLogLevel level) = 0;
    virtual EnhancedLogLevel getLevel() const = 0;
    virtual void setCategoryFilter(LogCategory category, bool enabled) = 0;
    virtual bool isCategoryEnabled(LogCategory category) const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    virtual void setMetadata(const std::string& key, const std::string& value) = 0;
    virtual std::string getMetadata(const std::string& key) const = 0;
    virtual void clearMetadata() = 0;
};

// Enhanced console logger with better formatting
class EnhancedConsoleLogger : public IEnhancedLogger {
public:
    EnhancedConsoleLogger();
    EnhancedConsoleLogger(EnhancedLogLevel level);
    ~EnhancedConsoleLogger() override = default;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    void setCategoryFilter(LogCategory category, bool enabled) override;
    bool isCategoryEnabled(LogCategory category) const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    void setMetadata(const std::string& key, const std::string& value) override;
    std::string getMetadata(const std::string& key) const override;
    void clearMetadata() override;
    
    // Additional methods
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setUseTimestamps(bool useTimestamps);
    bool getUseTimestamps() const;
    void setIncludeThreadInfo(bool includeInfo);
    bool getIncludeThreadInfo() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool isFileOutputOpen() const;
    
    // Formatting helpers
    static std::string levelToString(EnhancedLogLevel level);
    static std::string levelToColor(EnhancedLogLevel level);
    static std::string categoryToString(LogCategory category);
    std::string formatEntry(const EnhancedLogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logger manager
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger() = default;
    
    void addLogger(std::shared_ptr<IEnhancedLogger> logger);
    void removeLogger(IEnhancedLogger* logger);
    void clearLoggers();
    
    void setLevel(EnhancedLogLevel level);
    EnhancedLogLevel getLevel() const;
    void setGlobalLevel(EnhancedLogLevel level);
    
    void setCategoryFilter(LogCategory category, bool enabled);
    bool isCategoryEnabled(LogCategory category) const;
    
    void setFormat(const std::string& format);
    std::string getFormat() const;
    
    void log(EnhancedLogLevel level, LogCategory category, const std::string& message,
             const std::string& file = "", int line = 0, const std::string& function = "");
    
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    void clearMetadata();
    
    bool isEnabled(EnhancedLogLevel level) const;
    bool isCategoryEnabledForLevel(EnhancedLogLevel level, LogCategory category) const;
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // LogStream for easy logging
    class LogStream {
    public:
        LogStream(EnhancedLogLevel level, LogCategory category,
                  const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            stream_ << value;
            return *this;
        }
        
        LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            stream_ << manip;
            return *this;
        }
        
    private:
        EnhancedLogLevel level_;
        LogCategory category_;
        std::string file_;
        int line_;
        std::string function_;
        std::ostringstream stream_;
    };
    
    LogStream trace(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream debug(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream info(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream notice(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream warning(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream error(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream critical(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream alert(const std::string& file = "", int line = 0, const std::string& function = "");
    LogStream emergency(const std::string& file = "", int line = 0, const std::string& function = "");
    
    static EnhancedLogger& getGlobal();
    
    // Global logger management
    void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logging macros for easy use
#define NLM_TRACE_MSG(msg) nlm::EnhancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_TRACE_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Trace, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_DEBUG_MSG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_DEBUG_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Debug, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_INFO_MSG(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_INFO_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Info, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_NOTICE_MSG(msg) nlm::EnhancedLogger::getGlobal().notice(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_NOTICE_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Notice, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_WARNING_MSG(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_WARNING_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Warning, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_ERROR_MSG(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_ERROR_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Error, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_CRITICAL_MSG(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_CRITICAL_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Critical, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_ALERT_MSG(msg) nlm::EnhancedLogger::getGlobal().alert(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_ALERT_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Alert, cat, msg, __FILE__, __LINE__, __FUNCTION__)

#define NLM_EMERGENCY_MSG(msg) nlm::EnhancedLogger::getGlobal().emergency(msg, __FILE__, __LINE__, __FUNCTION__)
#define NLM_EMERGENCY_MSG_CAT(cat, msg) nlm::EnhancedLogger::getGlobal().log(nlm::EnhancedLogLevel::Emergency, cat, msg, __FILE__, __LINE__, __FUNCTION__)

// Stream logging macros
#define NLM_TRACE nlm::EnhancedLogger::getGlobal().trace(__FILE__, __LINE__, __FUNCTION__)
#define NLM_DEBUG nlm::EnhancedLogger::getGlobal().debug(__FILE__, __LINE__, __FUNCTION__)
#define NLM_INFO nlm::EnhancedLogger::getGlobal().info(__FILE__, __LINE__, __FUNCTION__)
#define NLM_NOTICE nlm::EnhancedLogger::getGlobal().notice(__FILE__, __LINE__, __FUNCTION__)
#define NLM_WARNING nlm::EnhancedLogger::getGlobal().warning(__FILE__, __LINE__, __FUNCTION__)
#define NLM_ERROR nlm::EnhancedLogger::getGlobal().error(__FILE__, __LINE__, __FUNCTION__)
#define NLM_CRITICAL nlm::EnhancedLogger::getGlobal().critical(__FILE__, __LINE__, __FUNCTION__)
#define NLM_ALERT nlm::EnhancedLogger::getGlobal().alert(__FILE__, __LINE__, __FUNCTION__)
#define NLM_EMERGENCY nlm::EnhancedLogger::getGlobal().emergency(__FILE__, __LINE__, __FUNCTION__)

// Helper function to create log entries with metadata
inline EnhancedLogEntry createLogEntry(EnhancedLogLevel level, LogCategory category,
                                       const std::string& message, const std::string& file = "",
                                       int line = 0, const std::string& function = "") {
    EnhancedLogEntry entry(level, category, message);
    entry.file = file;
    entry.line = line;
    entry.function = function;
    return entry;
}

} // namespace nlm
