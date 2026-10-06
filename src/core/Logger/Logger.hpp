// Comprehensive Logging and Error Handling System
// Phase 6: Advanced Logging and Safety Improvements

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <exception>

namespace nlm {

// Log levels with severity hierarchy
enum class LogLevel {
    Trace,      // Detailed debug information
    Debug,      // General debugging information
    Info,       // General information messages
    Warning,    // Warning conditions that don't prevent operation
    Error,      // Error conditions that prevent operation
    Critical,   // Critical errors that may cause failure
    Off         // No logging
};

// Log category for organizing log messages
enum class LogCategory {
    Core,           // Core system components
    Brain,          // Brain simulation
    Memory,         // Memory systems
    Plasticity,     // Plasticity rules
    Neuromodulation, // Neuromodulation systems
    Prediction,     // Prediction system
    Cognition,      // Cognitive mechanisms
    Sensory,        // Sensory processing
    Motor,          // Motor system
    Environment,    // Environment interaction
    Performance,    // Performance metrics
    Configuration,  // Configuration management
    Plugin,         // Plugin system
    Network,        // Network operations
    Development,    // Development processes
    Experiment      // Experiment framework
};

// Enhanced log entry with comprehensive metadata
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    LogCategory category;
    std::string message;
    std::string sourceFile;
    std::string sourceFunction;
    int sourceLine;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    
    LogEntry() : level(LogLevel::Info), category(LogCategory::Core), 
                 sourceFile(""), sourceFunction(""), sourceLine(0) {}
    
    LogEntry(LogLevel lvl, LogCategory cat, const std::string& msg,
             const std::string& file = "", const std::string& function = "", 
             int line = 0)
        : level(lvl), category(cat), message(msg), sourceFile(file), 
          sourceFunction(function), sourceLine(line) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Thread-safe log sink interface
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual bool supportsAsync() const { return false; }
};

// Console log sink with formatting
class ConsoleLogSink : public LogSink {
private:
    LogLevel minLevel;
    bool useColors;
    std::mutex mutex;
    
    std::string getLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Off: return "OFF";
            default: return "UNKNOWN";
        }
    }
    
    std::string getCategoryString(LogCategory category) const {
        switch (category) {
            case LogCategory::Core: return "CORE";
            case LogCategory::Brain: return "BRAIN";
            case LogCategory::Memory: return "MEMORY";
            case LogCategory::Plasticity: return "PLASTICITY";
            case LogCategory::Neuromodulation: return "NEUROMOD";
            case LogCategory::Prediction: return "PREDICT";
            case LogCategory::Cognition: return "COGNITION";
            case LogCategory::Sensory: return "SENSORY";
            case LogCategory::Motor: return "MOTOR";
            case LogCategory::Environment: return "ENV";
            case LogCategory::Performance: return "PERF";
            case LogCategory::Configuration: return "CONFIG";
            case LogCategory::Plugin: return "PLUGIN";
            case LogCategory::Network: return "NETWORK";
            case LogCategory::Development: return "DEV";
            case LogCategory::Experiment: return "EXPER";
            default: return "UNKNOWN";
        }
    }
    
    std::string getColorCode(LogLevel level) const {
        if (!useColors) return "";
        
        switch (level) {
            case LogLevel::Trace: return "\033[90m"; // Gray
            case LogLevel::Debug: return "\033[36m"; // Cyan
            case LogLevel::Info: return "\033[32m"; // Green
            case LogLevel::Warning: return "\033[33m"; // Yellow
            case LogLevel::Error: return "\033[31m"; // Red
            case LogLevel::Critical: return "\033[41m\033[30m"; // Red background
            default: return "";
        }
    }
    
    std::string getResetColor() const {
        return useColors ? "\033[0m" : "";
    }
    
public:
    explicit ConsoleLogSink(LogLevel minLevel = LogLevel::Info, 
                           bool useColors = true)
        : minLevel(minLevel), useColors(useColors) {}
    
    void write(const LogEntry& entry) override {
        if (entry.level < minLevel) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto timePoint = entry.timestamp;
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timePoint.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << "] [" << getLevelString(entry.level) << "]"
            << " [" << getCategoryString(entry.category) << "]"
            << " " << entry.message;
        
        if (!entry.sourceFile.empty()) {
            oss << " (" << entry.sourceFile;
            if (!entry.sourceFunction.empty()) {
                oss << "::" << entry.sourceFunction;
            }
            oss << ":" << entry.sourceLine << ")";
        }
        
        std::cout << getColorCode(entry.level) << oss.str() << getResetColor() << std::endl;
        
        // Output metadata if present
        for (const auto& [key, value] : entry.metadata) {
            std::cout << "    Metadata: " << key << " = " << value << std::endl;
        }
    }
    
    void flush() override {
        std::cout.flush();
    }
};

// File log sink with rotation
class FileLogSink : public LogSink {
private:
    std::string filepath;
    std::ofstream file;
    std::mutex mutex;
    size_t maxFileSize;
    size_t currentSize;
    int fileNumber;
    bool useRotation;
    
    void rotateFile() {
        if (!useRotation) return;
        
        file.close();
        
        // Close current file
        while (std::filesystem::exists(filepath + "." + std::to_string(fileNumber))) {
            fileNumber++;
        }
        
        // Rename current file
        std::filesystem::rename(filepath, filepath + "." + std::to_string(fileNumber));
        
        // Create new file
        file.open(filepath, std::ios::app);
        currentSize = 0;
    }
    
public:
    FileLogSink(const std::string& filepath, 
               LogLevel minLevel = LogLevel::Info,
               size_t maxFileSize = 10 * 1024 * 1024, // 10MB
               bool useRotation = true)
        : filepath(filepath), maxFileSize(maxFileSize), currentSize(0), 
          fileNumber(0), useRotation(useRotation) {
        
        file.open(filepath, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filepath);
        }
    }
    
    ~FileLogSink() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void write(const LogEntry& entry) override {
        if (entry.level < minLevel) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check file rotation
        if (useRotation && currentSize >= maxFileSize) {
            rotateFile();
        }
        
        // Write entry to file
        std::ostringstream oss;
        
        auto timePoint = entry.timestamp;
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timePoint.time_since_epoch()) % 1000;
        
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << "] [" << static_cast<int>(entry.level) << "]"
            << " [" << static_cast<int>(entry.category) << "]"
            << " " << entry.message << std::endl;
        
        file << oss.str();
        currentSize += oss.str().size();
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(mutex);
        file.flush();
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual void handleError(const std::string& message, 
                           const std::string& source = "",
                           int line = 0) = 0;
    virtual bool shouldTerminateOnError() const { return true; }
};

// Exception-based error handler
class ExceptionErrorHandler : public ErrorHandler {
public:
    void handleError(const std::string& message, 
                   const std::string& source = "",
                   int line = 0) override {
        std::string fullMessage = "ERROR";
        if (!source.empty()) {
            fullMessage += " in " + source;
        }
        if (line > 0) {
            fullMessage += " at line " + std::to_string(line);
        }
        fullMessage += ": " + message;
        
        throw std::runtime_error(fullMessage);
    }
};

// Log manager with thread-safe operations
class LogManager {
private:
    std::vector<std::shared_ptr<LogSink>> sinks;
    std::vector<std::shared_ptr<ErrorHandler>> errorHandlers;
    LogLevel globalMinLevel;
    LogCategory globalMinCategory;
    bool asyncEnabled;
    std::mutex mutex;
    
    // Performance optimization: Pre-allocated log entry buffer
    std::vector<LogEntry> logBuffer;
    size_t bufferSize;
    
public:
    LogManager(size_t bufferSize = 1000) 
        : globalMinLevel(LogLevel::Info), 
          globalMinCategory(LogCategory::Core),
          asyncEnabled(false), bufferSize(bufferSize) {
        logBuffer.reserve(bufferSize);
    }
    
    ~LogManager() {
        flushAll();
    }
    
    // Log entry with comprehensive metadata
    void log(LogLevel level, LogCategory category, const std::string& message,
             const std::string& sourceFile = "",
             const std::string& sourceFunction = "",
             int sourceLine = 0) {
        
        // Check if this log entry should be processed
        if (level < globalMinLevel || category < globalMinCategory) {
            return;
        }
        
        LogEntry entry(level, category, message, sourceFile, sourceFunction, sourceLine);
        
        // Add to buffer for batch processing
        std::lock_guard<std::mutex> lock(mutex);
        
        if (asyncEnabled && logBuffer.size() < bufferSize) {
            logBuffer.push_back(std::move(entry));
            // Process in background thread (simplified)
            processBufferedLogs();
        } else {
            writeToSinks(entry);
        }
    }
    
    // Error handling
    void error(const std::string& message, const std::string& source = "",
              int line = 0) {
        for (const auto& handler : errorHandlers) {
            handler->handleError(message, source, line);
        }
        
        // Also log as error
        log(LogLevel::Error, LogCategory::Core, message, source, "", line);
    }
    
    // Warning handling
    void warning(const std::string& message, const std::string& source = "",
                 int line = 0) {
        log(LogLevel::Warning, LogCategory::Core, message, source, "", line);
    }
    
    // Info handling
    void info(const std::string& message, const std::string& source = "",
              int line = 0) {
        log(LogLevel::Info, LogCategory::Core, message, source, "", line);
    }
    
    // Debug handling
    void debug(const std::string& message, const std::string& source = "",
               int line = 0) {
        log(LogLevel::Debug, LogCategory::Core, message, source, "", line);
    }
    
    // Add sinks
    void addSink(std::shared_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(mutex);
        sinks.push_back(std::move(sink));
    }
    
    void addConsoleSink(LogLevel level = LogLevel::Info, bool useColors = true) {
        addSink(std::make_shared<ConsoleLogSink>(level, useColors));
    }
    
    void addFileSink(const std::string& filepath, 
                    LogLevel level = LogLevel::Info,
                    size_t maxSize = 10 * 1024 * 1024,
                    bool rotation = true) {
        addSink(std::make_shared<FileLogSink>(filepath, level, maxSize, rotation));
    }
    
    void addErrorHandler(std::shared_ptr<ErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        errorHandlers.push_back(std::move(handler));
    }
    
    void addExceptionErrorHandler() {
        addErrorHandler(std::make_shared<ExceptionErrorHandler>());
    }
    
    // Configuration
    void setLevel(LogLevel level) { globalMinLevel = level; }
    void setCategory(LogCategory category) { globalMinCategory = category; }
    void enableAsync(bool enabled) { asyncEnabled = enabled; }
    
    // Utility methods
    LogLevel getLevel() const { return globalMinLevel; }
    LogCategory getCategory() const { return globalMinCategory; }
    
    size_t getSinkCount() const { return sinks.size(); }
    size_t getErrorHandlerCount() const { return errorHandlers.size(); }
    
    void flushAll() {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& sink : sinks) {
            sink->flush();
        }
    }
    
private:
    void processBufferedLogs() {
        // Process buffered logs (simplified for this implementation)
        // In a real implementation, this would be done in a separate thread
        for (const auto& entry : logBuffer) {
            writeToSinks(entry);
        }
        logBuffer.clear();
    }
    
    void writeToSinks(const LogEntry& entry) {
        for (const auto& sink : sinks) {
            sink->write(entry);
        }
    }
};

// Global log manager instance
class GlobalLogger {
private:
    static std::unique_ptr<LogManager> instance;
    static std::mutex initMutex;
    
public:
    static LogManager& getInstance() {
        std::lock_guard<std::mutex> lock(initMutex);
        if (!instance) {
            instance = std::make_unique<LogManager>();
            // Add default console sink
            instance->addConsoleSink();
        }
        return *instance;
    }
    
    static void initialize(const std::string& name = "NLM") {
        std::lock_guard<std::mutex> lock(initMutex);
        instance = std::make_unique<LogManager>();
        instance->addConsoleSink();
    }
    
    static void shutdown() {
        std::lock_guard<std::mutex> lock(initMutex);
        instance.reset();
    }
    
    static LogManager& logger() {
        return getInstance();
    }
};

std::unique_ptr<LogManager> GlobalLogger::instance = nullptr;
std::mutex GlobalLogger::initMutex;

// Convenience macros for logging
#define NLM_LOG_TRACE(message) nlm::GlobalLogger::logger().debug(message, __FILE__, __LINE__)
#define NLM_LOG_DEBUG(message) nlm::GlobalLogger::logger().debug(message, __FILE__, __LINE__)
#define NLM_LOG_INFO(message) nlm::GlobalLogger::logger().info(message, __FILE__, __LINE__)
#define NLM_LOG_WARNING(message) nlm::GlobalLogger::logger().warning(message, __FILE__, __LINE__)
#define NLM_LOG_ERROR(message) nlm::GlobalLogger::logger().error(message, __FILE__, __LINE__)

// Enhanced error handling with stack trace
class EnhancedErrorHandler : public ErrorHandler {
private:
    std::vector<std::shared_ptr<ErrorHandler>> handlers;
    bool captureStackTrace;
    
public:
    EnhancedErrorHandler(bool captureStackTrace = true) 
        : captureStackTrace(captureStackTrace) {}
    
    void addHandler(std::shared_ptr<ErrorHandler> handler) {
        handlers.push_back(std::move(handler));
    }
    
    void handleError(const std::string& message, 
                   const std::string& source = "",
                   int line = 0) override {
        std::string enhancedMessage = message;
        
        if (captureStackTrace) {
            enhancedMessage += "\nStack trace:\n";
            enhancedMessage += getStackTrace();
        }
        
        if (!source.empty()) {
            enhancedMessage += "\nSource: " + source;
        }
        
        if (line > 0) {
            enhancedMessage += "\nLine: " + std::to_string(line);
        }
        
        // Handle through all registered handlers
        for (const auto& handler : handlers) {
            handler->handleError(enhancedMessage, "", 0);
        }
    }
    
    bool shouldTerminateOnError() const override {
        return true; // Default to termination on error
    }
    
private:
    std::string getStackTrace() {
        std::ostringstream oss;
        // Simplified stack trace collection
        oss << "  Stack trace not implemented (consider using backtrace library)";
        return oss.str();
    }
};

// Smart pointer types for automatic resource management
using LogManagerPtr = std::shared_ptr<LogManager>;
using ErrorHandlerPtr = std::shared_ptr<ErrorHandler>;
using LogSinkPtr = std::shared_ptr<LogSink>;

} // namespace nlm

// Initialize global logger
nlm::GlobalLogger::getInstance();

// Enhanced logging functions for the NLM codebase
#define NLM_LOG_TRACE_CATEGORY(message, category) \
    nlm::GlobalLogger::logger().log(nlm::LogLevel::Trace, category, message, __FILE__, __funcName__, __LINE__)

#define NLM_LOG_DEBUG_CATEGORY(message, category) \
    nlm::GlobalLogger::logger().log(nlm::LogLevel::Debug, category, message, __FILE__, __funcName__, __LINE__)

#define NLM_LOG_INFO_CATEGORY(message, category) \
    nlm::GlobalLogger::logger().log(nlm::LogLevel::Info, category, message, __FILE__, __funcName__, __LINE__)

#define NLM_LOG_WARNING_CATEGORY(message, category) \
    nlm::GlobalLogger::logger().log(nlm::LogLevel::Warning, category, message, __FILE__, __funcName__, __LINE__)

#define NLM_LOG_ERROR_CATEGORY(message, category) \
    nlm::GlobalLogger::logger().log(nlm::LogLevel::Error, category, message, __FILE__, __funcName__, __LINE__)

// Type safety helper macros
#define NLM_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            nlm::GlobalLogger::logger().error(message, __FILE__, __LINE__); \
            throw std::runtime_error(message); \
        } \
    } while(0)

#define NLM_THROW_IF(condition, exception, message) \
    do { \
        if (condition) { \
            nlm::GlobalLogger::logger().error(message, __FILE__, __LINE__); \
            throw exception(message); \
        } \
    } while(0)

// Memory safety helper macros
#define NLM_SAFE_DELETE(ptr) \
    do { \
        if (ptr) { \
            delete ptr; \
            ptr = nullptr; \
        } \
    } while(0)

#define NLM_SAFE_DELETE_ARRAY(ptr) \
    do { \
        if (ptr) { \
            delete[] ptr; \
            ptr = nullptr; \
        } \
    } while(0)

#define NLM_SAFE_RELEASE(ptr) \
    do { \
        if (ptr) { \
            ptr->release(); \
            ptr = nullptr; \
        } \
    } while(0)