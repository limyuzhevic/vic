#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <filesystem>
#include <iomanip>
#include <map>

namespace nlm {

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Enhanced Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string category;  // For categorization (e.g., "memory", "cognition", "agent")
    
    LogEntry()
        : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "")
        : level(lvl), message(msg), file(file), line(line), function(function), 
          category(category), timestamp(std::chrono::system_clock::now()) {
        threadId = getCurrentThreadId();
    }
    
    // Generate thread ID
    static std::string getCurrentThreadId() {
        std::ostringstream oss;
        oss << std::hex << std::this_thread::get_id();
        return oss.str();
    }
    
    // Format entry as string with customizable formatting
    std::string format(bool includeTimestamps = true, bool includeLevel = true,
                      bool includeFileLocation = true, bool includeThreadId = true,
                      bool includeFunctionName = false,
                      bool includeCategory = false) const {
        std::ostringstream oss;
        
        if (includeTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(timestamp);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            oss << "." << std::setfill('0') << std::setw(3) 
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                    timestamp.time_since_epoch()) % 1000;
            oss << " ";
        }
        
        if (includeLevel) {
            oss << "[" << levelToString(level) << "] ";
        }
        
        if (includeCategory && !category.empty()) {
            oss << "[" << category << "] ";
        }
        
        if (includeThreadId) {
            oss << "[" << threadId << "] ";
        }
        
        if (includeFileLocation && !file.empty()) {
            oss << file;
            if (line > 0) oss << ":" << line;
            oss << " ";
        }
        
        if (includeFunctionName && !function.empty()) {
            oss << function << "() ";
        }
        
        oss << message;
        
        return oss.str();
    }
    
    // Static helper function for log level to string conversion
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
    virtual void flush() = 0;
    virtual std::string getName() const = 0;
    virtual void setName(const std::string& name) = 0;
    virtual std::string getCategory() const = 0;
    virtual void setCategory(const std::string& category) = 0;
};

// Console logger implementation with enhanced file logging
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    void flush() override;
    std::string getName() const override;
    void setName(const std::string& name) override;
    std::string getCategory() const override;
    void setCategory(const std::string& category) override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Enhanced file logging with rotation support
    void setOutputFile(const std::string& filepath);
    void setOutputFile(const std::string& filepath, bool append);
    void setOutputFile(const std::string& filepath, uint64_t maxFileSizeMB = 10,
                      std::chrono::hours maxFileAge = std::chrono::hours(24),
                      size_t backupCount = 5);
    void closeOutputFile();
    bool isOpen() const;
    std::string getCurrentFilePath() const;
    uint64_t getCurrentFileSize() const;
    
    // File logging configuration
    void setFlushOnLog(bool flush);
    void setFlushInterval(std::chrono::milliseconds interval);
    
    // File format configuration
    void setIncludeTimestamps(bool include);
    void setIncludeLevel(bool include);
    void setIncludeFileLocation(bool include);
    void setIncludeThreadId(bool include);
    void setIncludeFunctionName(bool include);
    void setCategory(const std::string& category);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string getCurrentTimeString() const;
    bool shouldRotateFile() const;
    void rotateFile();
    void cleanupOldFiles();
};

// Main Logger class with enhanced configuration support
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove specific logger by name
    bool removeLogger(const std::string& name);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set level for specific logger by name
    bool setLoggerLevel(const std::string& name, LogLevel level);
    
    // Global logging functions with category support
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "");
    
    // Convenience methods with automatic file/line/function
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "");
    
    // Stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // String stream methods
        LogStream& operator<<(const char* str) {
            oss_ << str;
            return *this;
        }
        
        LogStream& operator<<(const std::string& str) {
            oss_ << str;
            return *this;
        }
        
        // Category for stream logging
        LogStream& category(const std::string& cat) {
            category_ = cat;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::string category_;
    };
    
    // Get log entry count
    size_t getTotalLogCount() const;
    
    // Get logger names
    std::vector<std::string> getLoggerNames() const;
    
    // Get logger stats
    struct LoggerStats {
        size_t totalLogs;
        size_t logsByLevel[5];
        double memoryUsageMB;
        uint64_t totalBytesWritten;
        std::chrono::steady_clock::time_point lastLogTime;
        std::string currentFilePath;
        uint64_t currentFileSize;
    };
    
    LoggerStats getLoggerStats(const std::string& name) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configure from file
    bool loadConfiguration(const std::string& filepath);
    
    // Export configuration
    std::string exportLoggerConfiguration() const;
    
    // Performance metrics
    double getAverageLogTimePerEntry() const;
    size_t getLogsPerSecond() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for common use cases
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream logging macro with category
#define NLM_LOG_STREAM(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)

// Category-specific convenience macros
#define NLM_LOG_MEMORY(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "memory")
#define NLM_LOG_COGNITION(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "cognition")
#define NLM_LOG_AGENT(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "agent")
#define NLM_LOG_DEVELOPMENT(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "development")
#define NLM_LOG_PREDICTION(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "prediction")
#define NLM_LOG_NEUROMODULATION(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "neuromodulation")

// Configuration constants
namespace LogConfig {
    const std::string DEFAULT_LOG_FILE = "nlm.log";
    const uint64_t DEFAULT_MAX_FILE_SIZE_MB = 10;
    const std::chrono::hours DEFAULT_MAX_FILE_AGE = std::chrono::hours(24);
    const size_t DEFAULT_BACKUP_COUNT = 5;
    const bool DEFAULT_FLUSH_ON_LOG = true;
    const std::chrono::milliseconds DEFAULT_FLUSH_INTERVAL = std::chrono::milliseconds(100);
    const bool DEFAULT_INCLUDE_TIMESTAMPS = true;
    const bool DEFAULT_INCLUDE_LEVEL = true;
    const bool DEFAULT_INCLUDE_FILE_LOCATION = true;
    const bool DEFAULT_INCLUDE_THREAD_ID = true;
    const bool DEFAULT_INCLUDE_FUNCTION_NAME = false;
    const bool DEFAULT_USE_COLORS = true;
    const std::string DEFAULT_LOG_CATEGORY = "general";
}

} // namespace nlm
