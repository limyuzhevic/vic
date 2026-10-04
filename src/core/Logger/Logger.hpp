#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    bool isPerformanceLog;
    std::string category;
    
    // Performance measurement
    std::chrono::duration<double> duration;
    std::string operation;
    
    // Memory tracking
    size_t memoryAllocated;
    size_t memoryDeallocated;
    
    // Network tracking
    std::string remoteAddress;
    uint16_t remotePort;
    size_t bytesTransferred;
    
    LogEntry() 
        : level(LogLevel::Info), line(0), threadId(0), 
          isPerformanceLog(false), memoryAllocated(0), memoryDeallocated(0),
          bytesTransferred(0), remotePort(0) {}
};

// Enhanced logger interface with debugging capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Debugging capabilities
    virtual void setDebugMode(bool enabled) = 0;
    virtual bool isDebugMode() const = 0;
    virtual void setMaxLogFileSize(size_t maxSize) = 0;
    virtual size_t getMaxLogFileSize() const = 0;
    virtual void setLogRotationCount(int count) = 0;
    virtual int getLogRotationCount() const = 0;
    
    // Performance logging
    virtual void logPerformance(const std::string& operation, 
                               std::chrono::duration<double> duration) = 0;
    
    // Memory tracking
    virtual void logMemoryAllocation(size_t bytes, const std::string& context = "") = 0;
    virtual void logMemoryDeallocation(size_t bytes, const std::string& context = "") = 0;
    
    // Network tracking
    virtual void logNetworkActivity(const std::string& remoteAddress, uint16_t remotePort,
                                   size_t bytesTransferred, const std::string& protocol = "") = 0;
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
    
    // Debugging capabilities
    void setDebugMode(bool enabled) override;
    bool isDebugMode() const override;
    void setMaxLogFileSize(size_t maxSize) override;
    size_t getMaxLogFileSize() const override;
    void setLogRotationCount(int count) override;
    int getLogRotationCount() const override;
    
    // Performance logging
    void logPerformance(const std::string& operation,
                       std::chrono::duration<double> duration) override;
    
    // Memory tracking
    void logMemoryAllocation(size_t bytes, const std::string& context = "") override;
    void logMemoryDeallocation(size_t bytes, const std::string& context = "") override;
    
    // Network tracking
    void logNetworkActivity(const std::string& remoteAddress, uint16_t remotePort,
                           size_t bytesTransferred, const std::string& protocol = "") override;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string getCurrentTimestamp() const;
    std::string formatDuration(std::chrono::duration<double> duration) const;
    std::string truncateMessage(const std::string& message) const;
};

// Enhanced Main Logger class
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
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", bool isPerformanceLog = false,
             const std::string& category = "", const std::string& operation = "");
    
    // Convenience methods with additional parameters
    void logPerformance(const std::string& operation,
                       std::chrono::duration<double> duration,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    void logMemoryAllocation(size_t bytes, const std::string& context = "",
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    
    void logMemoryDeallocation(size_t bytes, const std::string& context = "",
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
    
    void logNetworkActivity(const std::string& remoteAddress, uint16_t remotePort,
                           size_t bytesTransferred, const std::string& protocol = "",
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    
    // Convenience methods
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
    
    // Debugging utilities
    void enableDebugMode(bool enabled = true);
    bool isDebugMode() const;
    void setMaxLogFileSize(size_t maxSize);
    size_t getMaxLogFileSize() const;
    void setLogRotationCount(int count);
    int getLogRotationCount() const;
    
    // Log analysis and statistics
    struct LogStatistics {
        size_t totalLogs;
        size_t debugLogs;
        size_t infoLogs;
        size_t warningLogs;
        size_t errorLogs;
        size_t criticalLogs;
        size_t performanceLogs;
        size_t memoryLogs;
        size_t networkLogs;
        double averageLogSize;
        std::string mostCommonCategory;
    };
    
    LogStatistics getStatistics() const;
    
    // Log filtering and querying
    std::vector<LogEntry> queryLogs(LogLevel minLevel = LogLevel::Debug,
                                  const std::string& category = "",
                                  const std::string& operation = "",
                                  const std::chrono::system_clock::time_point& startTime = 
                                      std::chrono::system_clock::time_point::min(),
                                  const std::chrono::system_clock::time_point& endTime =
                                      std::chrono::system_clock::time_point::max()) const;
    
    // Log export and import
    std::string exportLogsAsJSON(const std::vector<LogEntry>& entries) const;
    bool importLogsFromJSON(const std::string& json, std::vector<LogEntry>& entries) const;
    
    // Performance analysis
    struct PerformanceStats {
        std::string operation;
        double totalDuration;
        size_t callCount;
        double averageDuration;
        double minDuration;
        double maxDuration;
        std::vector<double> durationSamples;
    };
    
    std::vector<PerformanceStats> getPerformanceStatistics() const;
    
    // Memory usage tracking
    struct MemoryStats {
        size_t peakMemoryUsage;
        size_t totalAllocated;
        size_t totalDeallocated;
        std::vector<std::pair<size_t, std::string>> allocationHistory;
    };
    
    MemoryStats getMemoryStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
             bool isPerformanceLog = false, const std::string& category = "",
             const std::string& operation = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance logging stream
    LogStream& duration(std::chrono::duration<double> dur) {
        performanceDuration_ = dur;
        return *this;
    }
    
    // Memory logging stream
    LogStream& memory(size_t bytes, bool isAllocation = true) {
        memoryBytes_ = bytes;
        isMemoryAllocation_ = isAllocation;
        return *this;
    }
    
    // Network logging stream
    LogStream& network(const std::string& remoteAddr, uint16_t remotePort,
                     size_t bytesTransferred, const std::string& protocol = "") {
        remoteAddress_ = remoteAddr;
        remotePort_ = remotePort;
        bytesTransferred_ = bytesTransferred;
        networkProtocol_ = protocol;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    bool isPerformanceLog_;
    std::string category_;
    std::string operation_;
    std::chrono::duration<double> performanceDuration_;
    size_t memoryBytes_;
    bool isMemoryAllocation_;
    std::string remoteAddress_;
    uint16_t remotePort_;
    size_t bytesTransferred_;
    std::string networkProtocol_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_PERFORMANCE(operation) nlm::LogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__).duration(std::chrono::steady_clock::now())
#define NLM_LOG_MEMORY(bytes, context) nlm::LogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__).memory(bytes, true)
#define NLM_LOG_NETWORK(remoteAddr, remotePort, bytes, protocol) nlm::LogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__).network(remoteAddr, remotePort, bytes, protocol)

// Log utility functions
namespace LogUtils {
    std::string levelToString(LogLevel level);
    LogLevel stringToLevel(const std::string& str);
    std::string truncate(const std::string& str, size_t maxLength);
    std::string formatDuration(std::chrono::duration<double> duration);
    std::string getCurrentTimestamp();
    std::string getThreadId();
    std::string getStackTrace();
    
    // Log file management
    bool isLogFileRotationNeeded(const std::string& filepath, size_t maxSize);
    void rotateLogFiles(const std::string& directory, int keepCount);
    
    // Log compression
    bool compressLogFile(const std::string& filepath);
    bool isLogFileCompressed(const std::string& filepath);
    
    // Log analysis
    struct LogPattern {
        std::string regex;
        std::function<bool(const LogEntry&)> matcher;
        std::string description;
    };
    
    std::vector<LogEntry> filterLogs(const std::vector<LogEntry>& logs,
                                   const std::vector<LogPattern>& patterns);
}

} // namespace nlm