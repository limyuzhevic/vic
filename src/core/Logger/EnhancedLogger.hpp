// Enhanced Logger System with Better Features and Reduced Duplication
// Provides advanced logging capabilities with reduced code duplication
// Includes structured logging, filtering, routing, and enhanced formatting

#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <regex>

namespace nlm {

// Enhanced LogEntry with additional metadata
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    
    EnhancedLogEntry() : level(LogLevel::Info), line(0), threadId("") {
        timestamp = std::chrono::system_clock::now();
    }
    
    EnhancedLogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "",
                    int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function), threadId("") {
        timestamp = std::chrono::system_clock::now();
        updateThreadId();
    }
    
private:
    void updateThreadId() {
        threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
};

// Log formatter for structured output
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const EnhancedLogEntry& entry) const = 0;
};

// Plain text formatter
class PlainTextFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream ss;
        
        // Level prefix
        ss << "[" << levelToString(entry.level) << "] ";
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) 
           << (std::chrono::duration_cast<std::chrono::milliseconds>(
                   entry.timestamp.time_since_epoch()).count() % 1000);
        ss << " " << entry.message;
        
        // Add file/line info if available
        if (!entry.file.empty() && entry.line > 0) {
            ss << " (" << entry.file << ":" << entry.line << ")";
        }
        
        // Add function name if available
        if (!entry.function.empty()) {
            ss << " in " << entry.function;
        }
        
        // Add thread ID
        if (!entry.threadId.empty()) {
            ss << " [thread:" << entry.threadId << "]";
        }
        
        // Add metadata if any
        if (!entry.metadata.empty()) {
            ss << " {";
            bool first = true;
            for (const auto& meta : entry.metadata) {
                if (!first) ss << ", ";
                ss << meta.first << "=" << meta.second;
                first = false;
            }
            ss << "}";
        }
        
        return ss.str();
    }
};

// JSON formatter
class JSONFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream ss;
        ss << "{\"timestamp\": \"" << getTimestampString(entry.timestamp) << "\", ";
        ss << "\"level\": \"" << levelToString(entry.level) << "\", ";
        ss << "\"message\": \"" << escapeString(entry.message) << "\", ";
        ss << "\"thread_id\": \"" << entry.threadId << "\", ";
        
        if (!entry.file.empty() && entry.line > 0) {
            ss << "\"location\": \"" << entry.file << ":" << entry.line << "\", ";
        }
        if (!entry.function.empty()) {
            ss << "\"function\": \"" << escapeString(entry.function) << "\", ";
        }
        
        if (!entry.metadata.empty()) {
            ss << "\"metadata\": {";
            bool first = true;
            for (const auto& meta : entry.metadata) {
                if (!first) ss << ", ";
                ss << "\"" << escapeString(meta.first) << "\": \"" << escapeString(meta.second) << "\"";
                first = false;
            }
            ss << "}, ";
        }
        
        ss << "}\";
        return ss.str();
    }
    
private:
    std::string escapeString(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        result += "\\u" + std::hex + std::setw(4) + std::setfill('0') 
                                 + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
            }
        }
        return result;
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%3f");
        return ss.str();
    }
};

// Colored text formatter
class ColoredFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream ss;
        
        // Colored level prefix
        ss << levelToColor(entry.level) << "[" << levelToString(entry.level) << "]" 
           << "\033[0m ";
        
        // Normal timestamp and message
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << " " << entry.message;
        
        // Add file/line info if available
        if (!entry.file.empty() && entry.line > 0) {
            ss << " (" << entry.file << ":" << entry.line << ")";
        }
        
        return ss.str();
    }
    
private:
    std::string levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
};

// Log destination interface for different output methods
class LogDestination {
public:
    virtual ~LogDestination() = default;
    virtual void write(const std::string& formattedMessage) = 0;
    virtual void setLevel(LogLevel level) {}
    virtual LogLevel getLevel() const { return LogLevel::Info; }
};

// Console log destination
class ConsoleDestination : public LogDestination {
public:
    ConsoleDestination() = default;
    ConsoleDestination(LogLevel level) : level_(level), useColors_(true) {}
    
    void write(const std::string& formattedMessage) override {
        std::cout << formattedMessage << std::endl;
    }
    
    void setLevel(LogLevel level) override {
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    void setUseColors(bool useColors) {
        useColors_ = useColors;
    }
    
    bool getUseColors() const {
        return useColors_;
    }
    
private:
    LogLevel level_ = LogLevel::Info;
    bool useColors_ = true;
    std::mutex mutex_;
};

// File log destination
class FileDestination : public LogDestination {
public:
    FileDestination() = default;
    FileDestination(LogLevel level, const std::string& filepath) 
        : level_(level), filepath_(filepath) {
        openFile();
    }
    
    ~FileDestination() {
        closeFile();
    }
    
    void write(const std::string& formattedMessage) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileOut_.is_open()) {
            fileOut_ << formattedMessage << std::endl;
            fileOut_.flush();
        }
    }
    
    void setLevel(LogLevel level) override {
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    void setOutputFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        closeFile();
        filepath_ = filepath;
        openFile();
    }
    
    void closeOutputFile() {
        std::lock_guard<std::mutex> lock(mutex_);
        closeFile();
    }
    
    bool isOpen() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return fileOut_.is_open();
    }
    
private:
    void openFile() {
        fileOut_.open(filepath_, std::ios::app);
    }
    
    void closeFile() {
        if (fileOut_.is_open()) {
            fileOut_.close();
        }
    }
    
    LogLevel level_ = LogLevel::Info;
    std::string filepath_;
    std::ofstream fileOut_;
    std::mutex mutex_;
};

// Rotating file destination
class RotatingFileDestination : public FileDestination {
public:
    RotatingFileDestination(LogLevel level, const std::string& filepath,
                           size_t maxFileSize = 10 * 1024 * 1024, // 10MB
                           int maxFiles = 5)
        : FileDestination(level, filepath), maxFileSize_(maxFileSize), maxFiles_(maxFiles) {}
    
    void write(const std::string& formattedMessage) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if file needs rotation
        if (fileOut_.is_open() && getFileSize() >= maxFileSize_) {
            rotateFiles();
        }
        
        FileDestination::write(formattedMessage);
    }
    
private:
    size_t getFileSize() const {
        if (!fileOut_.is_open()) return 0;
        auto currentPos = fileOut_.tellp();
        fileOut_.seekp(0, std::ios::end);
        auto endPos = fileOut_.tellp();
        fileOut_.seekp(currentPos);
        return static_cast<size_t>(endPos - currentPos);
    }
    
    void rotateFiles() {
        closeOutputFile();
        
        // Close existing files
        for (int i = maxFiles_ - 1; i >= 0; --i) {
            std::string oldFile = getRotatedFilename(i);
            std::string newFile = getRotatedFilename(i + 1);
            
            if (std::filesystem::exists(oldFile)) {
                if (i == maxFiles_ - 1) {
                    // Delete oldest file
                    std::filesystem::remove(oldFile);
                } else {
                    // Rename to next index
                    std::filesystem::rename(oldFile, newFile);
                }
            }
        }
        
        // Reopen main file
        openFile();
    }
    
    std::string getRotatedFilename(int index) const {
        if (index == 0) {
            return filepath_;
        }
        
        size_t dotPos = filepath_.find_last_of('.');
        if (dotPos == std::string::npos) {
            return filepath_ + "." + std::to_string(index);
        }
        
        return filepath_.substr(0, dotPos) + "." + std::to_string(index) +
               filepath_.substr(dotPos);
    }
    
    size_t maxFileSize_;
    int maxFiles_;
};

// Log filter for filtering log messages
class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool shouldLog(const EnhancedLogEntry& entry) const = 0;
};

// Level-based filter
class LevelFilter : public LogFilter {
public:
    LevelFilter(LogLevel minLevel) : minLevel_(minLevel) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        return entry.level >= minLevel_;
    }
    
private:
    LogLevel minLevel_;
};

// Pattern filter
class PatternFilter : public LogFilter {
public:
    PatternFilter(const std::regex& pattern, bool match = true)
        : pattern_(pattern), match_(match) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        bool matches = std::regex_search(entry.message, pattern_);
        return match_ ? matches : !matches;
    }
    
private:
    std::regex pattern_;
    bool match_;
};

// Keyword filter
class KeywordFilter : public LogFilter {
public:
    KeywordFilter(const std::vector<std::string>& keywords, bool requireAll = false)
        : keywords_(keywords), requireAll_(requireAll) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        std::vector<std::string> found;
        for (const auto& keyword : keywords_) {
            if (entry.message.find(keyword) != std::string::npos) {
                found.push_back(keyword);
            }
        }
        
        if (requireAll_) {
            return found.size() == keywords_.size();
        } else {
            return !found.empty();
        }
    }
    
private:
    std::vector<std::string> keywords_;
    bool requireAll_;
};

// Logger class with enhanced features
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying, enable moving
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    EnhancedLogger(EnhancedLogger&&) noexcept;
    EnhancedLogger& operator=(EnhancedLogger&&) noexcept;
    
    // Log management
    void addDestination(std::shared_ptr<LogDestination> destination);
    void removeDestination(const LogDestination* destination);
    void clearDestinations();
    
    // Filter management
    void addFilter(std::shared_ptr<LogFilter> filter);
    void removeFilter(const LogFilter* filter);
    void clearFilters();
    
    // Formatter management
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    void addFormatter(std::shared_ptr<LogFormatter> formatter);
    void removeFormatter(const LogFormatter* formatter);
    void clearFormatters();
    
    // Log level management
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    
    // Log methods
    void log(LogLevel level, const std::string& message, const std::string& file = "",
            int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "",
             int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "",
                int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");
    
    // Structured logging
    template<typename T>
    void logStructured(LogLevel level, const std::string& message,
                      const std::map<std::string, T>& metadata,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Convenience methods with metadata
    void logWithContext(LogLevel level, const std::string& message,
                       const std::string& operation,
                       const std::string& userId = "",
                       const std::string& requestId = "",
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Configuration
    void setConsoleColors(bool enabled);
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Statistics
    struct LoggerStats {
        size_t totalLogs;
        size_t logsByLevel[5]; // Debug, Info, Warning, Error, Critical
        size_t destinations;
        size_t filters;
        size_t formatters;
        double memoryUsageKB;
    };
    
    LoggerStats getStats() const;
    
    // Configuration for advanced features
    void enableStructuredLogging(bool enable = true);
    void enablePerformanceMetrics(bool enable = true);
    void enableThreadInfo(bool enable = true);
    void enableFileLogging(bool enable = true, const std::string& filepath = "");
    void enableConsoleLogging(bool enable = true);
    
    // Performance monitoring
    struct PerformanceMetrics {
        double totalTimeMs;
        size_t logCount;\n        double avgTimePerLogUs;
        size_t memoryPeakKB;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Log levels with more granular control
    enum class EnhancedLogLevel {
        TRACE = -1,
        DEBUG = 0,
        INFO = 1,
        NOTICE = 2,
        WARNING = 3,
        ERROR = 4,
        CRITICAL = 5,
        ALERT = 6,
        EMERGENCY = 7,
        CUSTOM = 100
    };
    
    // Convert between old and new log levels
    static LogLevel convertToLegacyLevel(EnhancedLogLevel level);
    static EnhancedLogLevel convertToEnhancedLevel(LogLevel level);
    
    // Custom log level configuration
    void addCustomLevel(EnhancedLogLevel level, const std::string& name,
                       const std::string& color = "");
    
    // Log level names
    std::string getLevelName(EnhancedLogLevel level) const;
    EnhancedLogLevel getLevelByName(const std::string& name) const;
    
    // Get all level names
    std::vector<std::string> getLevelNames() const;
    
    // Log level hierarchy
    static bool isLevelMoreSevere(LogLevel a, LogLevel b);
    static bool isLevelMoreSevere(EnhancedLogLevel a, EnhancedLogLevel b);
    
    // Log level comparison
    static int compareLevels(LogLevel a, LogLevel b);
    static int compareLevels(EnhancedLogLevel a, EnhancedLogLevel b);
    
    // Log level utilities
    static LogLevel getMinLevel(LogLevel a, LogLevel b);
    static LogLevel getMaxLevel(LogLevel a, LogLevel b);
    static bool isSameLevel(LogLevel a, LogLevel b);
    
    // Log level conversion utilities
    static EnhancedLogLevel fromInt(int value);
    static LogLevel toInt(EnhancedLogLevel level);
    static std::string toString(EnhancedLogLevel level);
    static EnhancedLogLevel fromString(const std::string& str);
    
    // Log level comparison operators
    static bool operator<(LogLevel a, LogLevel b);
    static bool operator<=(LogLevel a, LogLevel b);
    static bool operator>(LogLevel a, LogLevel b);
    static bool operator>=(LogLevel a, LogLevel b);
    static bool operator==(LogLevel a, LogLevel b);
    static bool operator!=(LogLevel a, LogLevel b);
    
    // Enhanced log level operators
    static bool operator<(EnhancedLogLevel a, EnhancedLogLevel b);
    static bool operator<=(EnhancedLogLevel a, EnhancedLogLevel b);
    static bool operator>(EnhancedLogLevel a, EnhancedLogLevel b);
    static bool operator>=(EnhancedLogLevel a, EnhancedLogLevel b);
    static bool operator==(EnhancedLogLevel a, EnhancedLogLevel b);
    static bool operator!=(EnhancedLogLevel a, EnhancedLogLevel b);
    
    // Log level arithmetic
    static LogLevel operator+(LogLevel a, int offset);
    static LogLevel operator-(LogLevel a, int offset);
    static EnhancedLogLevel operator+(EnhancedLogLevel a, int offset);
    static EnhancedLogLevel operator-(EnhancedLogLevel a, int offset);
    
    // Log level difference
    static int getLevelDifference(LogLevel a, LogLevel b);
    static int getLevelDifference(EnhancedLogLevel a, EnhancedLogLevel b);
    
    // Log level range check
    static bool isLevelInRange(LogLevel level, LogLevel min, LogLevel max);
    static bool isLevelInRange(EnhancedLogLevel level, EnhancedLogLevel min, EnhancedLogLevel max);
    
    // Log level validation
    static bool isValidLevel(LogLevel level);
    static bool isValidLevel(EnhancedLogLevel level);
    
    // Log level description
    static std::string getLevelDescription(LogLevel level);
    static std::string getLevelDescription(EnhancedLogLevel level);
    
    // Log level color
    static std::string getLevelColor(LogLevel level);
    static std::string getLevelColor(EnhancedLogLevel level);
    
    // Log level priority
    static int getLevelPriority(LogLevel level);
    static int getLevelPriority(EnhancedLogLevel level);
    
    // Log level comparison
    static bool isLevelMoreImportant(LogLevel a, LogLevel b);
    static bool isLevelMoreImportant(EnhancedLogLevel a, EnhancedLogLevel b);
    
    // Log level utilities
    static LogLevel getNextLevel(LogLevel level);
    static LogLevel getPreviousLevel(LogLevel level);
    static EnhancedLogLevel getNextEnhancedLevel(EnhancedLogLevel level);
    static EnhancedLogLevel getPreviousEnhancedLevel(EnhancedLogLevel level);
    
    // Log level validation
    static bool isLogLevelValid(LogLevel level);
    static bool isEnhancedLogLevelValid(EnhancedLogLevel level);
    
    // Log level conversion
    static LogLevel toLegacyLevel(EnhancedLogLevel level);
    static EnhancedLogLevel toEnhancedLevel(LogLevel level);
    
    // Log level comparison operators (legacy)
    static bool operator<(LogLevel a, LogLevel b) {
        return static_cast<int>(a) < static_cast<int>(b);
    }
    
    static bool operator<=(LogLevel a, LogLevel b) {
        return static_cast<int>(a) <= static_cast<int>(b);
    }
    
    static bool operator>(LogLevel a, LogLevel b) {
        return static_cast<int>(a) > static_cast<int>(b);
    }
    
    static bool operator>=(LogLevel a, LogLevel b) {
        return static_cast<int>(a) >= static_cast<int>(b);
    }
    
    static bool operator==(LogLevel a, LogLevel b) {
        return static_cast<int>(a) == static_cast<int>(b);
    }
    
    static bool operator!=(LogLevel a, LogLevel b) {
        return static_cast<int>(a) != static_cast<int>(b);
    }
    
    // Enhanced log level comparison operators
    static bool operator<(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) < static_cast<int>(b);
    }
    
    static bool operator<=(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) <= static_cast<int>(b);
    }
    
    static bool operator>(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) > static_cast<int>(b);
    }
    
    static bool operator>=(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) >= static_cast<int>(b);
    }
    
    static bool operator==(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) == static_cast<int>(b);
    }
    
    static bool operator!=(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) != static_cast<int>(b);
    }
    
    // Log level arithmetic operators
    static LogLevel operator+(LogLevel a, int offset) {
        int result = static_cast<int>(a) + offset;
        if (result < 0) return LogLevel::Debug;
        if (result > 4) return LogLevel::Critical;
        return static_cast<LogLevel>(result);
    }
    
    static LogLevel operator-(LogLevel a, int offset) {
        int result = static_cast<int>(a) - offset;
        if (result < 0) return LogLevel::Debug;
        if (result > 4) return LogLevel::Critical;
        return static_cast<LogLevel>(result);
    }
    
    static EnhancedLogLevel operator+(EnhancedLogLevel a, int offset) {
        int result = static_cast<int>(a) + offset;
        if (result < -1) return EnhancedLogLevel::TRACE;
        if (result > 7) return EnhancedLogLevel::EMERGENCY;
        return static_cast<EnhancedLogLevel>(result);
    }
    
    static EnhancedLogLevel operator-(EnhancedLogLevel a, int offset) {
        int result = static_cast<int>(a) - offset;
        if (result < -1) return EnhancedLogLevel::TRACE;
        if (result > 7) return EnhancedLogLevel::EMERGENCY;
        return static_cast<EnhancedLogLevel>(result);
    }
    
    // Log level difference
    static int getLevelDifference(LogLevel a, LogLevel b) {
        return static_cast<int>(a) - static_cast<int>(b);
    }
    
    static int getLevelDifference(EnhancedLogLevel a, EnhancedLogLevel b) {
        return static_cast<int>(a) - static_cast<int>(b);
    }
    
    // Log level range check
    static bool isLevelInRange(LogLevel level, LogLevel min, LogLevel max) {
        return level >= min && level <= max;
    }
    
    static bool isLevelInRange(EnhancedLogLevel level, EnhancedLogLevel min, EnhancedLogLevel max) {
        return level >= min && level <= max;
    }
    
    // Log level validation
    static bool isValidLevel(LogLevel level) {
        return static_cast<int>(level) >= 0 && static_cast<int>(level) <= 4;
    }
    
    static bool isValidLevel(EnhancedLogLevel level) {
        return static_cast<int>(level) >= -1 && static_cast<int>(level) <= 7;
    }
    
    // Log level conversion
    static LogLevel toLegacyLevel(EnhancedLogLevel level) {
        if (level <= EnhancedLogLevel::DEBUG) return LogLevel::Debug;
        if (level <= EnhancedLogLevel::INFO) return LogLevel::Info;
        if (level <= EnhancedLogLevel::WARNING) return LogLevel::Warning;
        if (level <= EnhancedLogLevel::ERROR) return LogLevel::Error;
        return LogLevel::Critical;
    }
    
    static EnhancedLogLevel toEnhancedLevel(LogLevel level) {
        return static_cast<EnhancedLogLevel>(static_cast<int>(level));
    }
    
    // Log level utilities
    static LogLevel getNextLevel(LogLevel level) {
        int current = static_cast<int>(level);
        if (current < 4) return static_cast<LogLevel>(current + 1);
        return level;
    }
    
    static LogLevel getPreviousLevel(LogLevel level) {
        int current = static_cast<int>(level);
        if (current > 0) return static_cast<LogLevel>(current - 1);
        return level;
    }
    
    static EnhancedLogLevel getNextEnhancedLevel(EnhancedLogLevel level) {
        int current = static_cast<int>(level);
        if (current < 7) return static_cast<EnhancedLogLevel>(current + 1);
        return level;
    }
    
    static EnhancedLogLevel getPreviousEnhancedLevel(EnhancedLogLevel level) {
        int current = static_cast<int>(level);
        if (current > -1) return static_cast<EnhancedLogLevel>(current - 1);
        return level;
    }
    
    // Global logger instances
    static EnhancedLogger& getGlobalLogger() {
        static EnhancedLogger logger;
        return logger;
    }
    
    static std::shared_ptr<EnhancedLogger> getGlobalLoggerPtr() {
        static std::shared_ptr<EnhancedLogger> logger = std::make_shared<EnhancedLogger>();
        return logger;
    }
    
    static void setGlobalLogger(std::shared_ptr<EnhancedLogger> logger) {
        globalLogger_ = logger;
    }
    
    // Convenience functions
    static void logMessage(LogLevel level, const std::string& message) {
        getGlobalLogger().log(level, message);
    }
    
    static void logDebug(const std::string& message) {
        getGlobalLogger().debug(message);
    }
    
    static void logInfo(const std::string& message) {
        getGlobalLogger().info(message);
    }
    
    static void logWarning(const std::string& message) {
        getGlobalLogger().warning(message);
    }
    
    static void logError(const std::string& message) {
        getGlobalLogger().error(message);
    }
    
    static void logCritical(const std::string& message) {
        getGlobalLogger().critical(message);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static std::shared_ptr<EnhancedLogger> globalLogger_;
    
    // Helper functions
    std::string levelToString(LogLevel level) const;
    std::string levelToString(EnhancedLogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string levelToColor(EnhancedLogLevel level) const;
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) const;
    bool shouldLog(const EnhancedLogEntry& entry) const;
    std::string formatMessage(const EnhancedLogEntry& entry) const;
    void updateStats(const EnhancedLogEntry& entry);
    void updatePerformanceMetrics(const EnhancedLogEntry& entry);
    size_t getMemoryUsage() const;
    
    // Configuration
    bool structuredLoggingEnabled_;
    bool performanceMetricsEnabled_;
    bool threadInfoEnabled_;
    bool fileLoggingEnabled_;
    bool consoleLoggingEnabled_;
    
    // Custom levels
    std::map<EnhancedLogLevel, std::string> customLevelNames_;
    std::map<EnhancedLogLevel, std::string> customLevelColors_;
};

// Global enhanced logger instance
inline EnhancedLogger globalEnhancedLogger;

// Legacy Logger compatibility layer
class LegacyLogger {
public:
    static LegacyLogger& getGlobal() {
        static LegacyLogger logger;
        return logger;
    }
    
    static void setGlobal(std::shared_ptr<LegacyLogger> logger) {
        globalLogger_ = logger;
    }
    
    void log(LogLevel level, const std::string& message, const std::string& file = "",
             int line = 0, const std::string& function = "") {
        // Convert to enhanced logger format and log
        EnhancedLogger::getGlobalLogger().log(level, message, file, line, function);
    }
    
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") {
        EnhancedLogger::getGlobalLogger().debug(message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "") {
        EnhancedLogger::getGlobalLogger().info(message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "") {
        EnhancedLogger::getGlobalLogger().warning(message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") {
        EnhancedLogger::getGlobalLogger().error(message, file, line, function);
    }
    
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "") {
        EnhancedLogger::getGlobalLogger().critical(message, file, line, function);
    }
    
    // Convenience macros
    #define NLM_LOG_DEBUG(msg) EnhancedLogger::logDebug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) EnhancedLogger::logInfo(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) EnhancedLogger::logWarning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) EnhancedLogger::logError(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) EnhancedLogger::logCritical(msg, __FILE__, __LINE__, __func__)
    
    // Stream-style logging
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
            : level_(level), file_(file), line_(line), function_(function) {}
        
        ~LogStream() {
            if (!oss_.str().empty()) {
                EnhancedLogger::getGlobalLogger().log(level_, oss_.str(), file_, line_, function_);
            }
        }
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };
    
    #define NLM_LOG_STREAM(level) EnhancedLogger::LogStream(level, __FILE__, __LINE__, __func__)
    
private:
    static std::shared_ptr<LegacyLogger> globalLogger_;
};

// Helper functions for enhanced logging
namespace nlm {
    inline EnhancedLogger& getGlobalLogger() {
        return EnhancedLogger::getGlobalLogger();
    }
    
    inline std::shared_ptr<EnhancedLogger> getGlobalLoggerPtr() {
        return EnhancedLogger::getGlobalLoggerPtr();
    }
    
    inline void setGlobalLogger(std::shared_ptr<EnhancedLogger> logger) {
        EnhancedLogger::setGlobalLogger(logger);
    }
    
    // Convenience functions for legacy compatibility
    inline void logMessage(LogLevel level, const std::string& message) {
        EnhancedLogger::logMessage(level, message);
    }
    
    inline void logDebug(const std::string& message) {
        EnhancedLogger::logDebug(message);
    }
    
    inline void logInfo(const std::string& message) {
        EnhancedLogger::logInfo(message);
    }
    
    inline void logWarning(const std::string& message) {
        EnhancedLogger::logWarning(message);
    }
    
    inline void logError(const std::string& message) {
        EnhancedLogger::logError(message);
    }
    
    inline void logCritical(const std::string& message) {
        EnhancedLogger::logCritical(message);
    }
    
    // Macro for convenience
    #define NLM_LOG_DEBUG(msg) EnhancedLogger::logDebug(msg)
    #define NLM_LOG_INFO(msg) EnhancedLogger::logInfo(msg)
    #define NLM_LOG_WARNING(msg) EnhancedLogger::logWarning(msg)
    #define NLM_LOG_ERROR(msg) EnhancedLogger::logError(msg)
    #define NLM_LOG_CRITICAL(msg) EnhancedLogger::logCritical(msg)
    
    // Enhanced logging macros
    #define NLM_LOG_DEBUG_EX(msg, file, line, func) EnhancedLogger::logDebug(msg, file, line, func)
    #define NLM_LOG_INFO_EX(msg, file, line, func) EnhancedLogger::logInfo(msg, file, line, func)
    #define NLM_LOG_WARNING_EX(msg, file, line, func) EnhancedLogger::logWarning(msg, file, line, func)
    #define NLM_LOG_ERROR_EX(msg, file, line, func) EnhancedLogger::logError(msg, file, line, func)
    #define NLM_LOG_CRITICAL_EX(msg, file, line, func) EnhancedLogger::logCritical(msg, file, line, func)
} // namespace nlm

// Implementation details
namespace nlm {
    // EnhancedLogger implementation
    EnhancedLogger::EnhancedLogger()
        : structuredLoggingEnabled_(false), performanceMetricsEnabled_(false),
          threadInfoEnabled_(false), fileLoggingEnabled_(false), consoleLoggingEnabled_(true) {
        pImpl = std::make_unique<Impl>();
    }
    
    EnhancedLogger::~EnhancedLogger() = default;
    
    EnhancedLogger::EnhancedLogger(EnhancedLogger&&) noexcept = default;
    EnhancedLogger& EnhancedLogger::operator=(EnhancedLogger&&) noexcept = default;
    
    void EnhancedLogger::addDestination(std::shared_ptr<LogDestination> destination) {
        if (destination) {
            pImpl->destinations.push_back(destination);
        }
    }
    
    void EnhancedLogger::removeDestination(const LogDestination* destination) {
        auto it = std::find_if(pImpl->destinations.begin(), pImpl->destinations.end(),
                              [destination](const std::shared_ptr<LogDestination>& d) {
                                  return d.get() == destination;
                              });
        if (it != pImpl->destinations.end()) {
            pImpl->destinations.erase(it);
        }
    }
    
    void EnhancedLogger::clearDestinations() {
        pImpl->destinations.clear();
    }
    
    void EnhancedLogger::addFilter(std::shared_ptr<LogFilter> filter) {
        if (filter) {
            pImpl->filters.push_back(filter);
        }
    }
    
    void EnhancedLogger::removeFilter(const LogFilter* filter) {
        auto it = std::find_if(pImpl->filters.begin(), pImpl->filters.end(),
                              [filter](const std::shared_ptr<LogFilter>& f) {
                                  return f.get() == filter;
                              });
        if (it != pImpl->filters.end()) {
            pImpl->filters.erase(it);
        }
    }
    
    void EnhancedLogger::clearFilters() {
        pImpl->filters.clear();
    }
    
    void EnhancedLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
        pImpl->formatters.clear();
        if (formatter) {
            pImpl->formatters.push_back(formatter);
        }
    }
    
    void EnhancedLogger::addFormatter(std::shared_ptr<LogFormatter> formatter) {
        if (formatter) {
            pImpl->formatters.push_back(formatter);
        }
    }
    
    void EnhancedLogger::removeFormatter(const LogFormatter* formatter) {
        auto it = std::find_if(pImpl->formatters.begin(), pImpl->formatters.end(),
                              [formatter](const std::shared_ptr<LogFormatter>& f) {
                                  return f.get() == formatter;
                              });
        if (it != pImpl->formatters.end()) {
            pImpl->formatters.erase(it);
        }
    }
    
    void EnhancedLogger::clearFormatters() {
        pImpl->formatters.clear();
    }
    
    void EnhancedLogger::setLevel(LogLevel level) {
        for (const auto& destination : pImpl->destinations) {
            destination->setLevel(level);
        }
    }
    
    LogLevel EnhancedLogger::getLevel() const {
        if (!pImpl->destinations.empty()) {
            return pImpl->destinations[0]->getLevel();
        }
        return LogLevel::Info;
    }
    
    void EnhancedLogger::log(LogLevel level, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function) {
        if (!isEnabled(level)) {
            return;
        }
        
        EnhancedLogEntry entry(level, message, file, line, function);
        
        if (shouldLog(entry)) {
            std::string formattedMessage = formatMessage(entry);
            
            for (const auto& destination : pImpl->destinations) {
                if (destination->getLevel() <= level) {
                    destination->write(formattedMessage);
                }
            }
            
            updateStats(entry);
            if (performanceMetricsEnabled_) {
                updatePerformanceMetrics(entry);
            }
        }
    }
    
    // Additional implementations would continue...
    
    // Helper function implementations would continue...
    
} // namespace nlm
