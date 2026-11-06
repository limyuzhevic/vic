#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <optional>
#include <queue>
#include <thread>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log categories for organization
enum class LogCategory {
    None = 0,
    All = 1,
    System = 2,
    Brain = 3,
    Sensory = 4,
    Motor = 5,
    Plasticity = 6,
    Memory = 7,
    Cognition = 8,
    Development = 9,
    Simulation = 10,
    Agent = 11,
    Experiment = 12,
    Performance = 13,
    Configuration = 14,
    Network = 15,
    Thread = 16,
    File = 17,
    Custom = 255
};

// Enhanced log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
    size_t memoryUsage;
    double executionTime;
    
    LogEntry()
        : level(LogLevel::Info), category("None"), line(0), memoryUsage(0), executionTime(0.0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg)
        : level(lvl), category("None"), message(msg), line(0), 
          memoryUsage(0), executionTime(0.0) {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F");
        return ss.str();
    }
};

// Log filter for selective logging
struct LogFilter {
    LogLevel minLevel = LogLevel::Debug;
    std::vector<LogCategory> enabledCategories;
    std::unordered_map<std::string, bool> enabledModules;
    std::vector<std::string> excludedMessages;
    bool exactMatch = false;
    
    bool matches(const LogEntry& entry) const {
        if (entry.level < minLevel) return false;
        
        if (!enabledCategories.empty() && enabledCategories[0] != LogCategory::All) {
            auto it = std::find(enabledCategories.begin(), enabledCategories.end(), 
                               static_cast<LogCategory>(static_cast<int>(entry.category)));
            if (it == enabledCategories.end()) return false;
        }
        
        for (const auto& excluded : excludedMessages) {
            if (exactMatch) {
                if (entry.message == excluded) return false;
            } else {
                if (entry.message.find(excluded) != std::string::npos) return false;
            }
        }
        
        return true;
    }
};

// Log statistics for performance monitoring
struct LogStatistics {
    size_t totalLogs;
    size_t logsByLevel[5];  // Debug, Info, Warning, Error, Critical
    size_t logsByCategory[18];  // All categories
    double averageLogTime;
    size_t memoryUsagePeak;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    
    LogStatistics() : totalLogs(0), averageLogTime(0.0), memoryUsagePeak(0) {
        std::fill(std::begin(logsByLevel), std::end(logsByLevel), 0);
        std::fill(std::begin(logsByCategory), std::end(logsByCategory), 0);
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Log Statistics:\n";
        ss << "  Total logs: " << totalLogs << "\n";
        ss << "  By level: DEBUG=" << logsByLevel[0] 
           << " INFO=" << logsByLevel[1]
           << " WARN=" << logsByLevel[2]
           << " ERROR=" << logsByLevel[3]
           << " CRIT=" << logsByLevel[4] << "\n";
        ss << "  By category: ";
        bool first = true;
        for (int i = 0; i < 18; ++i) {
            if (logsByCategory[i] > 0) {
                if (!first) ss << ", ";
                ss << static_cast<LogCategory>(i) << "=" << logsByCategory[i];
                first = false;
            }
        }
        ss << "\n";
        ss << "  Avg log time: " << averageLogTime << "ms\n";
        ss << "  Memory peak: " << memoryUsagePeak << " KB\n";
        if (firstLog.time_since_epoch().count() > 0) {
            ss << "  Duration: " 
               << std::chrono::duration_cast<std::chrono::milliseconds>(lastLog - firstLog).count()
               << "ms\n";
        }
        return ss.str();
    }
};

// Enhanced logger interface with diagnostics
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Filter management
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    
    // Diagnostic capabilities
    virtual std::string getStatistics() const = 0;
    virtual void resetStatistics() = 0;
    
    // Configuration
    virtual void setUseColors(bool useColors) = 0;
    virtual bool getUseColors() const = 0;
    
    // File output
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    virtual bool isOutputFileOpen() const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Filter management
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    
    // Diagnostics
    std::string getStatistics() const override;
    void resetStatistics() override;
    
    // Configuration
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    // File output
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    bool isOutputFileOpen() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string categoryToString(LogCategory category) const;
    bool shouldLog(const LogEntry& entry) const;
};

// Main enhanced Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Enhanced logging methods with category and metadata
    void log(LogLevel level, const std::string& category, const std::string& message,
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& threadId = "");
    
    // Convenience methods
    void debug(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& category, const std::string& message,
              const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& category, const std::string& message,
                 const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& category, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Legacy methods for backward compatibility
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get statistics
    std::string getStatistics() const;
    void resetStatistics();
    
    // Log filtering
    void setFilter(const LogFilter& filter);
    LogFilter getFilter() const;
    
    // Performance monitoring
    void startPerformanceMeasurement(const std::string& operation);
    void endPerformanceMeasurement(const std::string& operation, bool success = true);
    std::string getPerformanceReport() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper method
    void logInternal(const LogEntry& entry);
};

// Enhanced macros with category support
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Category-based macros
#define NLM_LOG_DEBUG_CAT(category, msg) nlm::Logger::getGlobal().debug(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_CAT(category, msg) nlm::Logger::getGlobal().info(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_CAT(category, msg) nlm::Logger::getGlobal().warning(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_CAT(category, msg) nlm::Logger::getGlobal().error(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_CAT(category, msg) nlm::Logger::getGlobal().critical(category, msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with enhanced features
class LogStream {
public:
    LogStream(LogLevel level, const std::string& category, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Metadata support
    LogStream& metadata(const std::string& key, const std::string& value);
    LogStream& timestamp() {
        entry_.timestamp = std::chrono::system_clock::now();
        return *this;
    }
    
    LogStream& category(const std::string& cat) {
        entry_.category = cat;
        return *this;
    }

private:
    LogEntry entry_;
    std::ostringstream oss_;
};

// Enhanced macros for stream logging
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Diagnostic utilities
namespace LogDiagnostics {
    // Get system information
    std::string getSystemInfo();
    
    // Get memory usage
    size_t getMemoryUsage();
    
    // Get thread ID
    std::string getThreadId();
    
    // Get application start time
    std::chrono::system_clock::time_point getStartTime();
    
    // Format log entry with rich metadata
    std::string formatLogEntry(const LogEntry& entry);
    
    // Create performance report
    std::string createPerformanceReport(const std::string& loggerName);
}

} // namespace nlm
