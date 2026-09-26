// Unified Logging System for NLM
// Comprehensive logging system with structured logging, multiple output options, and filtering

#ifndef NLM_LOGGING_HPP
#define NLM_LOGGING_HPP

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <queue>
#include <thread>
#include <atomic>
#include "Errors.hpp"

// Log Levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

// Log Categories
enum class LogCategory {
    PERFORMANCE,
    VALIDATION,
    MEMORY,
    NEURAL,
    WORLD,
    PYTHON
};

// Log Output Destinations
enum class LogOutput {
    CONSOLE,
    FILE,
    SYSLOG
};

// Log Entry Structure for structured logging
struct LogEntry {
    LogLevel level;
    LogCategory category;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    SourceLocation location;
    std::unordered_map<std::string, std::string> context;
    
    LogEntry(LogLevel lvl, LogCategory cat, const std::string& msg, 
             const SourceLocation& loc = SourceLocation())
        : level(lvl), category(cat), timestamp(std::chrono::system_clock::now()),
          message(msg), location(loc) {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getFormattedMessage() const {
        std::stringstream ss;
        ss << "[" << getLevelString(level) << "] ";
        ss << "[" << getCategoryString(category) << "] ";
        ss << "[" << getFormattedTimestamp() << "] ";
        ss << "At " << location.file << ":" << location.line << " ";
        if (!location.function.empty()) {
            ss << "in " << location.function << "; ";
        }
        ss << message;
        
        for (const auto& pair : context) {
            ss << " " << pair.first << "=" << pair.second;
        }
        return ss.str();
    }
    
    static std::string getLevelString(LogLevel level) {
        static const std::map<LogLevel, std::string> levelStrings = {
            {LogLevel::DEBUG, "DEBUG"},
            {LogLevel::INFO, "INFO"},
            {LogLevel::WARNING, "WARNING"},
            {LogLevel::ERROR, "ERROR"},
            {LogLevel::CRITICAL, "CRITICAL"}
        };
        auto it = levelStrings.find(level);
        return it != levelStrings.end() ? it->second : "UNKNOWN";
    }
    
    static std::string getCategoryString(LogCategory category) {
        static const std::map<LogCategory, std::string> categoryStrings = {
            {LogCategory::PERFORMANCE, "PERFORMANCE"},
            {LogCategory::VALIDATION, "VALIDATION"},
            {LogCategory::MEMORY, "MEMORY"},
            {LogCategory::NEURAL, "NEURAL"},
            {LogCategory::WORLD, "WORLD"},
            {LogCategory::PYTHON, "PYTHON"}
        };
        auto it = categoryStrings.find(category);
        return it != categoryStrings.end() ? it->second : "UNKNOWN";
    }
};

// Logging configuration
struct LogConfig {
    std::map<LogCategory, LogLevel> categoryLevels;
    std::vector<LogOutput> outputs;
    std::string logFile;
    size_t maxFileSize;
    int maxFiles;
    bool consoleEnabled;
    bool syslogEnabled;
    std::string syslogIdentifier;
    bool structuredOutput;
    bool includeStackTrace;
    
    LogConfig() : maxFileSize(10 * 1024 * 1024), maxFiles(5), // 10MB, 5 files
                  consoleEnabled(true), syslogEnabled(false),
                  structuredOutput(true), includeStackTrace(false) {
        // Set default log levels for each category
        categoryLevels[LogCategory::PERFORMANCE] = LogLevel::INFO;
        categoryLevels[LogCategory::VALIDATION] = LogLevel::WARNING;
        categoryLevels[LogCategory::MEMORY] = LogLevel::WARNING;
        categoryLevels[LogCategory::NEURAL] = LogLevel::INFO;
        categoryLevels[LogCategory::WORLD] = LogLevel::INFO;
        categoryLevels[LogCategory::PYTHON] = LogLevel::ERROR;
    }
};

// Log Message Handler Interface
class LogHandler {
public:
    virtual ~LogHandler() {}
    virtual void handleLogEntry(const LogEntry& entry) = 0;
    virtual bool shouldLog(LogLevel level, LogCategory category) const = 0;
};

// Console Log Handler
class ConsoleLogHandler : public LogHandler {
private:
    std::mutex mutex;
    LogLevel minLevel;
    
public:
    ConsoleLogHandler(LogLevel minLevel = LogLevel::DEBUG) : minLevel(minLevel) {}
    
    void handleLogEntry(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::string formatted = entry.getFormattedMessage();
        
        switch (entry.level) {
            case LogLevel::DEBUG:
                std::cout << "[DEBUG] " << formatted << std::endl;
                break;
            case LogLevel::INFO:
                std::cout << "[INFO] " << formatted << std::endl;
                break;
            case LogLevel::WARNING:
                std::cout << "[WARNING] " << formatted << std::endl;
                break;
            case LogLevel::ERROR:
                std::cout << "[ERROR] " << formatted << std::endl;
                break;
            case LogLevel::CRITICAL:
                std::cout << "[CRITICAL] " << formatted << std::endl;
                break;
        }
    }
    
    bool shouldLog(LogLevel level, LogCategory category) const override {
        return level >= minLevel;
    }
};

// File Log Handler
class FileLogHandler : public LogHandler {
private:
    std::mutex mutex;
    std::ofstream logFile;
    std::string filename;
    LogLevel minLevel;
    size_t maxSize;
    int fileCount;
    std::atomic<size_t> currentSize;
    
public:
    FileLogHandler(const std::string& filename, LogLevel minLevel = LogLevel::DEBUG,
                   size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5)
        : filename(filename), minLevel(minLevel), maxSize(maxSize), maxFiles(maxFiles),
          currentSize(0), fileCount(0) {
        rotateLogFile();
    }
    
    ~FileLogHandler() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void rotateLogFile() {
        if (logFile.is_open()) {
            logFile.close();
        }
        
        // Generate filename with rotation number
        std::string rotatedFilename = filename;
        if (fileCount > 0) {
            size_t dotPos = filename.find_last_of('.');
            if (dotPos != std::string::npos) {
                rotatedFilename = filename.substr(0, dotPos) + "_" + 
                                 std::to_string(fileCount) + filename.substr(dotPos);
            }
        }
        
        logFile.open(rotatedFilename, std::ios::app);
        currentSize = logFile.tellp();
        
        if (!logFile.is_open()) {
            throw FileSystemError("Failed to open log file: " + filename);
        }
    }
    
    void handleLogEntry(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check file size and rotate if necessary
        if (currentSize >= maxSize) {
            fileCount++;
            rotateLogFile();
        }
        
        if (logFile.is_open()) {
            logFile << entry.getFormattedMessage() << std::endl;
            currentSize = logFile.tellp();
        }
    }
    
    bool shouldLog(LogLevel level, LogCategory category) const override {
        return level >= minLevel;
    }
};

// Structured Logger Class
class Logger {
private:
    static Logger* instance;
    LogConfig config;
    std::vector<LogHandler*> handlers;
    std::mutex mutex;
    bool initialized;
    
    Logger() : initialized(false) {}
    
public:
    static Logger& getInstance() {
        if (instance == nullptr) {
            instance = new Logger();
        }
        return *instance;
    }
    
    static void initialize(const LogConfig& cfg = LogConfig()) {
        getInstance().initializeInternal(cfg);
    }
    
    void initializeInternal(const LogConfig& cfg) {
        std::lock_guard<std::mutex> lock(mutex);
        config = cfg;
        
        // Add console handler if enabled
        if (config.consoleEnabled) {
            handlers.push_back(new ConsoleLogHandler(LogLevel::DEBUG));
        }
        
        // Add file handler if enabled
        if (!config.logFile.empty()) {
            try {
                handlers.push_back(new FileLogHandler(config.logFile, LogLevel::DEBUG, 
                                                    config.maxFileSize, config.maxFiles));
            } catch (const FileSystemError& e) {
                // If file logging fails, continue without it
                // Could fall back to console only
            }
        }
        
        initialized = true;
    }
    
    static void shutdown() {
        getInstance().shutdownInternal();
    }
    
    void shutdownInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        for (LogHandler* handler : handlers) {
            delete handler;
        }
        handlers.clear();
        initialized = false;
    }
    
    // Core logging method
    template<typename... Args>
    void log(LogLevel level, LogCategory category, const std::string& format, Args&&... args) {
        if (!initialized) {
            return; // Silently fail if not initialized
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check if this category/level should be logged
        auto it = config.categoryLevels.find(category);
        if (it != config.categoryLevels.end() && level < it->second) {
            return;
        }
        
        // Format message with arguments
        std::stringstream ss;
        formatMessage(ss, format, std::forward<Args>(args)...);
        
        // Create log entry
        LogEntry entry(level, category, ss.str());
        
        // Send to all handlers that should log this entry
        for (LogHandler* handler : handlers) {
            if (handler->shouldLog(level, category)) {
                handler->handleLogEntry(entry);
            }
        }
    }
    
    // Helper to format message with arguments
    template<typename... Args>
    void formatMessage(std::stringstream& ss, const std::string& format, Args&&... args) {
        size_t argIndex = 0;
        for (size_t i = 0; i < format.length(); ++i) {
            if (i + 1 < format.length() && format[i] == '{' && format[i + 1] == '}') {
                // Simple placeholder replacement: {0}, {1}, etc.
                ss << std::get<argsIndex>(std::forward_as_tuple>(args...));
                argIndex++;
            } else {
                ss << format[i];
            }
        }
    }
    
    // Convenience logging methods
    template<typename... Args>
    void debug(LogCategory category, const std::string& format, Args&&... args) {
        log(LogLevel::DEBUG, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(LogCategory category, const std::string& format, Args&&... args) {
        log(LogLevel::INFO, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(LogCategory category, const std::string& format, Args&&... args) {
        log(LogLevel::WARNING, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(LogCategory category, const std::string& format, Args&&... args) {
        log(LogLevel::ERROR, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void critical(LogCategory category, const std::string& format, Args&&... args) {
        log(LogLevel::CRITICAL, category, format, std::forward<Args>(args)...);
    }
    
    // Error-specific logging methods with automatic error context
    template<typename... Args>
    void logError(const NLMException& error) {
        log(error.getSeverity(), error.getCategory(), error.what());
    }
    
    // Configuration methods
    static void setCategoryLevel(LogCategory category, LogLevel level) {
        getInstance().setCategoryLevelInternal(category, level);
    }
    
    void setCategoryLevelInternal(LogCategory category, LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex);
        config.categoryLevels[category] = level;
    }
    
    static LogLevel getCategoryLevel(LogCategory category) {
        return getInstance().getCategoryLevelInternal(category);
    }
    
    LogLevel getCategoryLevelInternal(LogCategory category) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = config.categoryLevels.find(category);
        return it != config.categoryLevels.end() ? it->second : LogLevel::ERROR;
    }
    
    // Utility macros for common logging patterns
    #define NLM_LOG_DEBUG(category, format, ...) \
        Logger::getInstance().debug(category, format, ##__VA_ARGS__)
    
    #define NLM_LOG_INFO(category, format, ...) \
        Logger::getInstance().info(category, format, ##__VA_ARGS__)
    
    #define NLM_LOG_WARNING(category, format, ...) \
        Logger::getInstance().warning(category, format, ##__VA_ARGS__)
    
    #define NLM_LOG_ERROR(category, format, ...) \
        Logger::getInstance().error(category, format, ##__VA_ARGS__)
    
    #define NLM_LOG_CRITICAL(category, format, ...) \
        Logger::getInstance().critical(category, format, ##__VA_ARGS__)
    
    #define NLM_LOG_ERROR_THROW(error) \
        do { Logger::getInstance().logError(error); throw error; } while(0)
    
    #define NLM_LOG_ERROR_RETURN(error, value) \
        do { Logger::getInstance().logError(error); return value; } while(0)
    
    #define NLM_LOG_ERROR_BREAK \
        do { Logger::getInstance().error(LogCategory::CORE, "Error occurred, breaking loop"); break; } while(0)
    
    #define NLM_LOG_ERROR_CONTINUE \
        do { Logger::getInstance().error(LogCategory::CORE, "Error occurred, continuing"); continue; } while(0)
};

#endif // NLM_LOGGING_HPP