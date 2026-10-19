#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <filesystem>
#include "Config/Config.hpp"

namespace nlm {

// Enhanced log severity levels with more granular control
enum class LogLevel {
    Silent = -1,        // No logging at all
    Emergency = 0,       // Critical system-wide issues
    Alert,              // Action must be taken immediately
    Critical,
    Error,
    Warning,
    Notice,             // Normal operational messages
    Info,
    Debug,              // Detailed debugging information
    Trace,              // Very detailed, fine-grained information
    All = 100           // All log levels (used for filtering)
};

// Custom exception hierarchy for Logger errors
class LoggerException : public std::runtime_error {
public:
    explicit LoggerException(const std::string& message)
        : std::runtime_error("Logger error: " + message) {}
};

class FileOperationError : public LoggerException {
public:
    explicit FileOperationError(const std::string& message)
        : LoggerException("File operation error: " + message) {}
};

class ThreadSafetyError : public LoggerException {
public:
    explicit ThreadSafetyError(const std::string& message)
        : LoggerException("Thread safety error: " + message) {}
};

class ConfigurationError : public LoggerException {
public:
    explicit ConfigurationError(const std::string& message)
        : LoggerException("Configuration error: " + message) {}
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::string threadId;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    LogEntry()
        : level(LogLevel::Info), line(0), threadId("") {
        timestamp = std::chrono::system_clock::now();
    }
};

    // Enhanced logger configuration with granular control
    // Provides flexible output formatting, multi-level severity, and file management
    // Supports both synchronous and asynchronous logging modes
    // Includes color-coded terminal output and structured metadata
    struct LoggerConfig {
        LogLevel level = LogLevel::Info;               // Minimum log level to record
        std::string outputFile;                       // File path for log output
        bool useColors = true;                        // Enable color coding in console
        bool enableConsole = true;                    // Output to console
        bool enableFile = false;                      // Output to file
        size_t maxFileSize = 10 * 1024 * 1024;       // Rotate at 10MB
        size_t maxFiles = 5;                          // Keep 5 rotated files
        std::string colorTheme = "default";          // Color scheme for output
        bool asyncLogging = false;                   // Queue entries for background processing
        size_t bufferSize = 4096;                     // Size of internal log buffer
        
        LoggerConfig() = default;
        
        bool operator==(const LoggerConfig& other) const {
            return level == other.level &&
                   outputFile == other.outputFile &&
                   useColors == other.useColors &&
                   enableConsole == other.enableConsole &&
                   enableFile == other.enableFile &&
                   maxFileSize == other.maxFileSize &&
                   maxFiles == other.maxFiles &&
                   colorTheme == other.colorTheme &&
                   asyncLogging == other.asyncLogging &&
                   bufferSize == other.bufferSize;
        }
    };

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void configure(const LoggerConfig& config) = 0;
    virtual LoggerConfig getConfig() const = 0;
    virtual void flush() = 0;
};

// Enhanced console logger implementation with RAII and error handling
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
        configure(defaultConfig);
    }
    
    explicit ConsoleLogger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
        configure(config);
    }
    
    ~ConsoleLogger() override {
        try {
            closeOutputFile();
        } catch (const std::exception& e) {
            std::fprintf(stderr, "Logger destructor error: %s\n", e.what());
        }
    }
    
    void log(const LogEntry& entry) override {
        if (!pImpl->enabled || entry.level < pImpl->level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        try {
            std::string formattedMessage = formatMessage(entry);
            
            if (pImpl->enableConsole) {
                if (pImpl->useColors && !formattedMessage.empty()) {
                    std::cout << formattedMessage;
                } else if (!formattedMessage.empty()) {
                    std::cout << formattedMessage;
                }
            }
            
            if (pImpl->enableFile && pImpl->fileOut.is_open() && !formattedMessage.empty()) {
                pImpl->fileOut << formattedMessage;
                if (pImpl->fileOut.bad()) {
                    throw FileOperationError("Failed to write to log file: " + pImpl->outputFile);
                }
                pImpl->fileOut.flush();
            }
            
        } catch (const std::exception& e) {
            try {
                std::cerr << "[LOGGER ERROR] Failed to log message: " << e.what() << std::endl;
            } catch (...) {
                std::fprintf(stderr, "[LOGGER ERROR] Critical logging failure\n");
            }
        }
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->level = level;
    }
    
    LogLevel getLevel() const override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->level;
    }
    
    void configure(const LoggerConfig& config) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        bool needsReopen = false;
        
        if (config != pImpl->config) {
            pImpl->config = config;
            needsReopen = true;
        }
        
        if (config.useColors != pImpl->useColors) {
            pImpl->useColors = config.useColors;
        }
        
        if (needsReopen || config.outputFile != pImpl->outputFile) {
            closeOutputFile();
            if (config.enableFile && !config.outputFile.empty()) {
                setOutputFile(config.outputFile);
            }
        }
        
        pImpl->enabled = (config.level != LogLevel::Silent);
    }
    
    LoggerConfig getConfig() const override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->config;
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->enableFile && pImpl->fileOut.is_open()) {
            pImpl->fileOut.flush();
        }
    }
    
    void setUseColors(bool useColors) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->useColors = useColors;
    }
    
    bool getUseColors() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->useColors;
    }
    
    void setOutputFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        closeOutputFile();
        
        if (filepath.empty()) {
            return;
        }
        
        std::error_code ec;
        uintmax_t fileSize = 0;
        if (std::filesystem::exists(filepath, ec)) {
            fileSize = std::filesystem::file_size(filepath, ec);
        }
        
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            throw FileOperationError("Cannot open log file: " + filepath);
        }
        
        if (fileSize >= pImpl->config.maxFileSize) {
            std::string rotatedPath = filepath + ".1";
            std::rename(filepath.c_str(), rotatedPath.c_str());
        }
        
        pImpl->outputFile = filepath;
        pImpl->enableFile = true;
    }
    
    void closeOutputFile() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->fileOut.is_open()) {
            try {
                pImpl->fileOut.close();
            } catch (const std::exception& e) {
                std::fprintf(stderr, "Warning: Failed to close log file %s: %s\n", 
                           pImpl->outputFile.c_str(), e.what());
            }
        }
        
        pImpl->fileOut.clear();
        pImpl->enableFile = false;
    }
    
    std::string formatMessage(const LogEntry& entry) const {
        std::ostringstream oss;
        
        if (pImpl->useColors && entry.level != LogLevel::Silent) {
            oss << levelToColor(entry.level);
        }
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
        
        oss << "[" << getCurrentThreadId() << "] ";
        
        oss << "[" << levelToString(entry.level) << "] ";
        
        if (!entry.file.empty()) {
            oss << "(" << entry.file;
            if (entry.line > 0) {
                oss << ":" << entry.line;
            }
            if (!entry.function.empty()) {
                oss << " " << entry.function << ")";
            } else {
                oss << ")";
            }
            oss << " ";
        }
        
        oss << entry.message;
        
        if (!entry.metadata.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& [key, value] : entry.metadata) {
                if (!first) oss << ", ";
                oss << key << "=" << value;
                first = false;
            }
            oss << "}";
        }
        
        if (pImpl->useColors && entry.level != LogLevel::Silent) {
            oss << "\033[0m";
        }
        
        oss << "\n";
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) const {
        static const std::unordered_map<LogLevel, std::string> levelStrings = {
            {LogLevel::Silent, "SILENT"},
            {LogLevel::Emergency, "EMERGENCY"},
            {LogLevel::Alert, "ALERT"},
            {LogLevel::Critical, "CRITICAL"},
            {LogLevel::Error, "ERROR"},
            {LogLevel::Warning, "WARN"},
            {LogLevel::Notice, "NOTICE"},
            {LogLevel::Info, "INFO"},
            {LogLevel::Debug, "DEBUG"},
            {LogLevel::Trace, "TRACE"},
            {LogLevel::All, "ALL"}
        };
        
        auto it = levelStrings.find(level);
        return it != levelStrings.end() ? it->second : "UNKNOWN";
    }
    
    std::string levelToColor(LogLevel level) const {
        if (!pImpl->useColors) {
            return "";
        }
        
        static const std::unordered_map<LogLevel, std::string> levelColors = {
            {LogLevel::Emergency, "\033[95m"},
            {LogLevel::Alert, "\033[91m"},
            {LogLevel::Critical, "\033[91m"},
            {LogLevel::Error, "\033[31m"},
            {LogLevel::Warning, "\033[93m"},
            {LogLevel::Notice, "\033[94m"},
            {LogLevel::Info, "\033[92m"},
            {LogLevel::Debug, "\033[36m"},
            {LogLevel::Trace, "\033[90m"},
        };
        
        auto it = levelColors.find(level);
        return it != levelColors.end() ? it->second : "\033[0m";
    }
    
    static std::string getCurrentThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
private:
    struct Impl {
        LogLevel level = LogLevel::Info;
        LoggerConfig config;
        std::ofstream fileOut;
        std::mutex mutex;
        std::string outputFile;
        bool useColors = true;
        bool enableFile = false;
        bool enabled = true;
    };
    
    static const LoggerConfig defaultConfig;
    std::unique_ptr<Impl> pImpl;
};

const LoggerConfig ConsoleLogger::defaultConfig = {
    .level = LogLevel::Info,
    .outputFile = "",
    .useColors = true,
    .enableConsole = true,
    .enableFile = false,
    .maxFileSize = 10 * 1024 * 1024,
    .maxFiles = 5,
    .colorTheme = "default",
    .asyncLogging = false,
    .bufferSize = 4096
};

class Logger {
public:
    Logger() : pImpl(std::make_unique<Impl>()) {
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        addLogger(consoleLogger);
    }
    
    ~Logger() = default;
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    Logger(Logger&&) noexcept = default;
    Logger& operator=(Logger&&) noexcept = default;
    
    void addLogger(std::shared_ptr<ILogger> logger) {
        if (!logger) {
            throw LoggerException("Cannot add null logger");
        }
        
        std::lock_guard<std::shared_mutex> lock(pImpl->configMutex);
        pImpl->loggers.push_back(logger);
    }
    
    void clearLoggers() {
        std::lock_guard<std::shared_mutex> lock(pImpl->configMutex);
        pImpl->loggers.clear();
    }
    
    void setLevel(LogLevel level) {
        std::lock_guard<std::shared_mutex> lock(pImpl->configMutex);
        pImpl->config.level = level;
        
        for (auto& logger : pImpl->loggers) {
            logger->setLevel(level);
        }
    }
    
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::unordered_map<std::string, std::string>& metadata = {}) {
        if (!isLevelEnabled(level)) {
            return;
        }
        
        std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.threadId = ConsoleLogger::getCurrentThreadId();
        entry.metadata = metadata;
        
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                std::fprintf(stderr, "Logger error: %s\n", e.what());
            }
        }
    }
    
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Debug, message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Info, message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Warning, message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Error, message, file, line, function);
    }
    
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Critical, message, file, line, function);
    }
    
    bool isEnabled(LogLevel level) const {
        return isLevelEnabled(level);
    }
    
    static Logger& getGlobal() {
        static Logger globalLogger;
        return globalLogger;
    }
    
    static void setGlobal(std::shared_ptr<Logger> logger) {
    }
    
    void configureFromConfig(const Config& config);
    
    static LogLevel getLogLevelFromString(const std::string& levelStr);
    
    std::string getLogLevelString(LogLevel level) const;
    
    bool isLevelEnabled(LogLevel level) const;
    
    void flushAll() {
        std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
        for (auto& logger : pImpl->loggers) {
            try {
                logger->flush();
            } catch (const std::exception& e) {
                std::fprintf(stderr, "Logger flush error: %s\n", e.what());
            }
        }
    }
    
    LoggerConfig getConfig() const {
        std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
        return pImpl->config;
    }
    
private:
    struct Impl {
        std::vector<std::shared_ptr<ILogger>> loggers;
        std::shared_mutex configMutex;
        LoggerConfig config;
    };
    
    std::unique_ptr<Impl> pImpl;
};

class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              std::unordered_map<std::string, std::string> metadata = {})
        : level_(level), file_(file), line_(line), function_(function), metadata_(metadata) {}
    
    ~LogStream() {
        try {
            Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, metadata_);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "LogStream error: %s\n", e.what());
        }
    }
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& addMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
};

#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
