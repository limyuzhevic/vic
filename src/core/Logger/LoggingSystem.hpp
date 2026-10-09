#ifndef NLM_LOGGING_SYSTEM_H
#define NLM_LOGGING_SYSTEM_H

#include "NLMConstants.hpp"
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace nlm {

class LogManagerError : public std::runtime_error {
public:
    explicit LogManagerError(const std::string& message) 
        : std::runtime_error("LogManager: " + message) {}
};

class LogManager {
public:
    enum class Level {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Critical = 5
    };
    
    struct LogEntry {
        Level level;
        std::string message;
        std::string source;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
    };
    
    class Logger {
    public:
        Logger(const std::string& name = "NLM", Level level = Level::Info);
        ~Logger() = default;
        
        void trace(const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warn(const std::string& message);
        void error(const std::string& message);
        void critical(const std::string& message);
        
        template<typename... Args>
        void log(Level level, const std::string& format, const Args&... args) {
            std::string message = fmt::format(format, args...);
            logInternal(level, message);
        }
        
        void setLevel(Level level);
        Level getLevel() const { return level_; }
        std::string getName() const { return name_; }
        
        // Log formatting
        std::string formatMessage(const std::string& message, Level level) const;
        std::string getTimestamp() const;
        
        // File logging
        void enableFileLogging(const std::string& filename, bool enable = true);
        void setLogFile(const std::string& filename);
        std::string getLogFile() const { return logFilename_; }
        
        // Log history
        void enableLogHistory(size_t maxEntries = 1000);
        std::vector<LogEntry> getLogHistory() const { return logHistory_; }
        void clearLogHistory() { logHistory_.clear(); }
        
        // Pattern matching for log filtering
        void addLogPattern(const std::string& pattern);
        bool matchesPattern(const std::string& message) const;
        
        // Log rotation
        void setMaxFileSize(size_t maxSize);
        void setMaxLogFiles(size_t maxFiles);
        
        // Flush operations
        void flush();
        
        // Log level checking
        bool isLevelEnabled(Level level) const;
        
        // Singleton pattern
        static std::shared_ptr<Logger> getInstance(const std::string& name = "NLM");
        static void initializeDefaultLogger();
        static void shutdown();
        
    protected:
        void logInternal(Level level, const std::string& message);
        
        std::string name_;
        Level level_;
        std::shared_ptr<spdlog::logger> spdLogger_;
        std::string logFilename_;
        size_t maxFileSize_;
        size_t maxLogFiles_;
        size_t maxHistoryEntries_;
        std::vector<std::string> logPatterns_;
        mutable std::mutex mutex_;
        std::vector<LogEntry> logHistory_;
    };
    
    class MultiLogger {
    public:
        MultiLogger(const std::string& name = "NLM");
        ~MultiLogger() = default;
        
        void addLogger(std::shared_ptr<Logger> logger);
        void removeLogger(const std::string& name);
        std::shared_ptr<Logger> getLogger(const std::string& name) const;
        
        void trace(const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warn(const std::string& message);
        void error(const std::string& message);
        void critical(const std::string& message);
        
        template<typename... Args>
        void log(Level level, const std::string& format, const Args&... args) {
            for (const auto& logger : loggers_) {
                logger->log(level, format, args...);
            }
        }
        
        void setGlobalLevel(Level level);
        Level getGlobalLevel() const { return globalLevel_; }
        
        // Enable/disable log synchronization
        void enableSynchronousLogging(bool enable = true);
        
        // Log statistics
        size_t getTotalLogCount() const;
        std::unordered_map<std::string, size_t> getLogCountsByLevel() const;
        std::unordered_map<std::string, size_t> getLogCountsBySource() const;
        
    protected:
        std::string name_;
        Level globalLevel_;
        bool synchronousLogging_;
        mutable std::mutex mutex_;
        std::vector<std::shared_ptr<Logger>> loggers_;
    };
    
    // Global logger management
    static std::shared_ptr<MultiLogger> getGlobalLogger();
    static void setGlobalLogLevel(Level level);
    static Level getGlobalLogLevel();
    static void enableFileLogging(const std::string& filename, bool enable = true);
    static void enableConsoleLogging(bool enable = true);
    static bool isConsoleLoggingEnabled();
    static bool isFileLoggingEnabled();
    
    // Log message macros for convenience
    #define NLM_LOG_TRACE(message) nlm::LogManager::getGlobalLogger()->trace(message)
    #define NLM_LOG_DEBUG(message) nlm::LogManager::getGlobalLogger()->debug(message)
    #define NLM_LOG_INFO(message) nlm::LogManager::getGlobalLogger()->info(message)
    #define NLM_LOG_WARN(message) nlm::LogManager::getGlobalLogger()->warn(message)
    #define NLM_LOG_ERROR(message) nlm::LogManager::getGlobalLogger()->error(message)
    #define NLM_LOG_CRITICAL(message) nlm::LogManager::getGlobalLogger()->critical(message)
    
    #define NLM_LOG_TRACE_F(format, ...) nlm::LogManager::getGlobalLogger()->trace(fmt::format(format, ##__VA_ARGS__))
    #define NLM_LOG_DEBUG_F(format, ...) nlm::LogManager::getGlobalLogger()->debug(fmt::format(format, ##__VA_ARGS__))
    #define NLM_LOG_INFO_F(format, ...) nlm::LogManager::getGlobalLogger()->info(fmt::format(format, ##__VA_ARGS__))
    #define NLM_LOG_WARN_F(format, ...) nlm::LogManager::getGlobalLogger()->warn(fmt::format(format, ##__VA_ARGS__))
    #define NLM_LOG_ERROR_F(format, ...) nlm::LogManager::getGlobalLogger()->error(fmt::format(format, ##__VA_ARGS__))
    #define NLM_LOG_CRITICAL_F(format, ...) nlm::LogManager::getGlobalLogger()->critical(fmt::format(format, ##__VA_ARGS__))
    
    #define NLM_LOG(level, message) \
        do { \
            if (nlm::LogManager::getGlobalLogger()) { \
                switch (level) { \
                    case LogManager::Level::Trace: nlm::LogManager::getGlobalLogger()->trace(message); break; \
                    case LogManager::Level::Debug: nlm::LogManager::getGlobalLogger()->debug(message); break; \
                    case LogManager::Level::Info: nlm::LogManager::getGlobalLogger()->info(message); break; \
                    case LogManager::Level::Warn: nlm::LogManager::getGlobalLogger()->warn(message); break; \
                    case LogManager::Level::Error: nlm::LogManager::getGlobalLogger()->error(message); break; \
                    case LogManager::Level::Critical: nlm::LogManager::getGlobalLogger()->critical(message); break; \
                } \
            } \
        } while(0)
    
    #define NLM_LOG_F(level, format, ...) \
        do { \
            if (nlm::LogManager::getGlobalLogger()) { \
                switch (level) { \
                    case LogManager::Level::Trace: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Trace, format, ##__VA_ARGS__); break; \
                    case LogManager::Level::Debug: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Debug, format, ##__VA_ARGS__); break; \
                    case LogManager::Level::Info: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Info, format, ##__VA_ARGS__); break; \
                    case LogManager::Level::Warn: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Warn, format, ##__VA_ARGS__); break; \
                    case LogManager::Level::Error: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Error, format, ##__VA_ARGS__); break; \
                    case LogManager::Level::Critical: nlm::LogManager::getGlobalLogger()->log(nlm::LogManager::Level::Critical, format, ##__VA_ARGS__); break; \
                } \
            } \
        } while(0)
    
    // Error handling utilities
    class ErrorHandler {
    public:
        enum class ErrorAction {
            Abort,
            Continue,
            LogAndContinue,
            LogAndAbort,
            GracefulShutdown
        };
        
        ErrorHandler();
        ~ErrorHandler() = default;
        
        void handleError(const std::string& message, const std::string& source = "", ErrorAction action = ErrorAction::LogAndContinue);
        void setDefaultErrorAction(ErrorAction action) { defaultAction_ = action; }
        ErrorAction getDefaultErrorAction() const { return defaultAction_; }
        
        // Error reporting
        void reportError(const std::string& message, const std::string& source = "");
        void reportWarning(const std::string& message, const std::string& source = "");
        
        // Error context
        void addContext(const std::string& key, const std::string& value);
        std::string getErrorContext() const;
        
        // Error statistics
        size_t getErrorCount(ErrorAction action = ErrorAction::Continue) const;
        size_t getWarningCount() const;
        
        // Error recovery
        bool recoverFromError();
        void resetErrorCount();
        
    protected:
        ErrorAction defaultAction_;
        size_t errorCount_;
        size_t warningCount_;
        std::unordered_map<std::string, std::string> errorContext_;
        mutable std::mutex mutex_;
    };
    
    // Global error handler
    static ErrorHandler& getErrorHandler();
    static void initializeErrorHandler(ErrorHandler::ErrorAction defaultAction = ErrorHandler::ErrorAction::LogAndContinue);
    
    // Performance logging
    class PerformanceLogger {
    public:
        PerformanceLogger(const std::string& name, bool enable = true);
        ~PerformanceLogger();
        
        void startTimer();
        void stopTimer();
        double getLastDuration() const { return lastDuration_; }
        double getAverageDuration() const { return averageDuration_; }
        
        void logPerformance(const std::string& operation, size_t iterations = 1);
        
        // Memory tracking
        void trackMemoryUsage();
        double getLastMemoryUsage() const { return lastMemoryUsage_; }
        double getPeakMemoryUsage() const { return peakMemoryUsage_; }
        
        // Performance statistics
        size_t getCallCount() const { return callCount_; }
        double getTotalTime() const { return totalTime_; }
        
        // Performance thresholding
        void setWarningThreshold(double threshold);
        void setErrorThreshold(double threshold);
        
        // Performance alerting
        bool checkPerformanceThreshold(double currentTime);
        
    protected:
        std::string name_;
        bool enabled_;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
        double lastDuration_;
        double averageDuration_;
        double totalTime_;
        size_t callCount_;
        double lastMemoryUsage_;
        double peakMemoryUsage_;
        double warningThreshold_;
        double errorThreshold_;
        mutable std::mutex mutex_;
    };
    
    // Global performance logger
    static PerformanceLogger& getPerformanceLogger(const std::string& name = "Default");
    
    // Logging utilities
    class LogUtils {
    public:
        static std::string sanitizeLogMessage(const std::string& message);
        static std::string formatLogMessage(const std::string& message, Level level);
        static bool isLogPatternMatch(const std::string& message, const std::vector<std::string>& patterns);
        static std::string getLogLevelName(Level level);
        static std::string getLogLevelColor(Level level);
        
        // Log file utilities
        static std::string getLogFilename(const std::string& baseName = "nlm");
        static std::string getTimestampedLogFilename(const std::string& baseName = "nlm");
        static void rotateLogFiles(const std::string& directory, size_t maxFiles = 5);
        
        // Log analysis
        static std::unordered_map<std::string, size_t> analyzeLogFrequency(const std::vector<LogEntry>& entries);
        static double calculateLogVolume(const std::vector<LogEntry>& entries, const std::string& timeRange = "hour");
        
        // Log compression
        static void compressLogFile(const std::string& filename);
        static void compressLogDirectory(const std::string& directory, const std::string& compressionAlgorithm = "gzip");
    };
    
protected:
    static std::shared_ptr<MultiLogger> globalLogger_;
    static ErrorHandler globalErrorHandler_;
    static std::unordered_map<std::string, PerformanceLogger> performanceLoggers_;
    static std::mutex globalMutex_;
};

} // namespace nlm

#endif // NLM_LOGGING_SYSTEM_H
