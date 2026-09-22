// Logger.hpp improvements for thread safety
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>

namespace nlm {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying, enable moving
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    
    void log(LogLevel level, const std::string& message,
            const std::string& file = "", int line = 0, const std::string& function = "");
    
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    bool isEnabled(LogLevel level) const;
    
    // Statistics and monitoring
    size_t getLoggerCount() const;
    size_t getTotalLogCount() const;
    std::string getStatistics() const;
    
    // Performance optimization
    void setLogLevelCache(bool enable);
    bool isLogLevelCacheEnabled() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Log stream for convenience
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            oss_ << manip;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::string file_;
        int line_;
        std::string function_;
        std::ostringstream oss_;
    };
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global logger instance
inline Logger& logger() {
    return Logger::getGlobal();
}

// Convenience macros for logging with file and line info
#define NLM_LOG_DEBUG(message) logger().debug(message, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(message) logger().info(message, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(message) logger().warning(message, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(message) logger().error(message, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(message) logger().critical(message, __FILE__, __LINE__, __func__)

// Convenience log stream macros
#define NLM_LOG_STREAM(level) logger().LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
