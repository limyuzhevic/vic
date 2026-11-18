// NLM Enhanced Error Handling and Logging System
//
// Advanced error handling and logging system for NLM neural simulation framework
// Provides comprehensive error recovery, logging, and debugging capabilities.

#ifndef NLM_ERROR_HANDLING_LOGGING_HPP
#define NLM_ERROR_HANDLING_LOGGING_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <functional>
#include <map>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error categories
enum class ErrorCategory {
    Configuration,
    NeuralNetwork,
    Simulation,
    Memory,
    InputOutput,
    Network,
    Plasticity,
    Development,
    Neuromodulation,
    MemorySystem,
    Cognition,
    Sensory,
    Motor,
    Environment,
    Experiment,
    Visualization,
    Unknown
};

// Exception hierarchy
class NLMException : public std::runtime_error {
protected:
    std::string message;
    ErrorSeverity severity;
    ErrorCategory category;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
    std::string suggestion;
    std::unordered_map<std::string, std::string> metadata;
    
public:
    NLMException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::Error,
                 ErrorCategory cat = ErrorCategory::Unknown,
                 const std::string& ctx = "")
        : std::runtime_error(msg), message(msg), severity(sev), category(cat),
          context(ctx) {
        timestamp = std::chrono::system_clock::now();
    }
    
    virtual ~NLMException() noexcept = default;
    
    const std::string& getMessage() const { return message; }
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    const std::string& getContext() const { return context; }
    const std::string& getSuggestion() const { return suggestion; }
    
    void setContext(const std::string& ctx) { context = ctx; }
    void setSuggestion(const std::string& sug) { suggestion = sug; }
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    std::string getMetadata(const std::string& key) const {
        auto it = metadata.find(key);
        return it != metadata.end() ? it->second : "";
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "[" << timestampToString(timestamp) << "] ";
        ss << severityToString(severity) << ": ";
        ss << categoryToString(category) << ": ";
        ss << message;
        if (!context.empty()) {
            ss << " (Context: " << context << ")";
        }
        if (!suggestion.empty()) {
            ss << "\nSuggestion: " << suggestion;
        }
        return ss.str();
    }
    
    static std::string timestampToString(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    static std::string severityToString(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    static std::string categoryToString(ErrorCategory cat) {
        switch (cat) {
            case ErrorCategory::Configuration: return "CONFIGURATION";
            case ErrorCategory::NeuralNetwork: return "NEURAL_NETWORK";
            case ErrorCategory::Simulation: return "SIMULATION";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::InputOutput: return "INPUT_OUTPUT";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::Plasticity: return "PLASTICITY";
            case ErrorCategory::Development: return "DEVELOPMENT";
            case ErrorCategory::Neuromodulation: return "NEUROMODULATION";
            case ErrorCategory::MemorySystem: return "MEMORY_SYSTEM";
            case ErrorCategory::Cognition: return "COGNITION";
            case ErrorCategory::Sensory: return "SENSORY";
            case ErrorCategory::Motor: return "MOTOR";
            case ErrorCategory::Environment: return "ENVIRONMENT";
            case ErrorCategory::Experiment: return "EXPERIMENT";
            case ErrorCategory::Visualization: return "VISUALIZATION";
            default: return "UNKNOWN";
        }
    }
};

// Specialized exceptions for different error types
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& msg, const std::string& ctx = "")
        : NLMException(msg, ErrorSeverity::Error, ErrorCategory::Configuration, ctx) {}
};

class NeuralNetworkException : public NLMException {
public:
    NeuralNetworkException(const std::string& msg, const std::string& ctx = "")
        : NLMException(msg, ErrorSeverity::Error, ErrorCategory::NeuralNetwork, ctx) {}
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& msg, const std::string& ctx = "")
        : NLMException(msg, ErrorSeverity::Error, ErrorCategory::Simulation, ctx) {}
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& msg, const std::string& ctx = "")
        : NLMException(msg, ErrorSeverity::Error, ErrorCategory::Memory, ctx) {}
};

class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& msg, const std::string& ctx = "")
        : NLMException(msg, ErrorSeverity::Error, ErrorCategory::Plasticity, ctx) {}
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    virtual void handleError(const NLMException& error) = 0;
    virtual bool shouldRetry(const NLMException& error) const = 0;
    virtual void logError(const NLMException& error) const = 0;
};

// Default error handler
class DefaultErrorHandler : public ErrorHandler {
private:
    std::string logFile;
    bool writeToConsole;
    bool writeToFile;
    bool includeStackTrace;
    std::unordered_map<ErrorSeverity, std::string> levelColors;
    
public:
    DefaultErrorHandler(const std::string& logFile = "nlm_errors.log",
                       bool console = true, bool file = true,
                       bool stackTrace = false)
        : logFile(logFile), writeToConsole(console), writeToFile(file),
          includeStackTrace(stackTrace) {
        // Initialize colors (ANSI escape codes for terminal)
        levelColors[ErrorSeverity::Debug] = "\033[36m"; // Cyan
        levelColors[ErrorSeverity::Info] = "\033[32m"; // Green
        levelColors[ErrorSeverity::Warning] = "\033[33m"; // Yellow
        levelColors[ErrorSeverity::Error] = "\033[31m"; // Red
        levelColors[ErrorSeverity::Critical] = "\033[35m"; // Magenta
        levelColors[ErrorSeverity::Fatal] = "\033[41m\033[37m"; // White background, red text
    }
    
    void handleError(const NLMException& error) override {
        logError(error);
        
        // Determine if we should retry based on error type
        if (shouldRetry(error)) {
            // Retry logic could be implemented here
            std::cerr << "Error may be transient. Retrying..." << std::endl;
        }
    }
    
    bool shouldRetry(const NLMException& error) const override {
        // Don't retry fatal errors
        if (error.getSeverity() == ErrorSeverity::Fatal) {
            return false;
        }
        
        // Don't retry certain error categories
        switch (error.getCategory()) {
            case ErrorCategory::Configuration:
            case ErrorCategory::Memory:
                return false;
            default:
                return true;
        }
    }
    
    void logError(const NLMException& error) const override {
        std::stringstream ss;
        ss << error.toString() << std::endl;
        
        if (includeStackTrace) {
            // In a real implementation, we would capture stack trace here
            ss << "Stack trace: (not implemented in this simplified version)" << std::endl;
        }
        
        std::string logMessage = ss.str();
        
        if (writeToConsole) {
            std::cerr << levelColors[error.getSeverity()] << logMessage << "\033[0m" << std::endl;
        }
        
        if (writeToFile) {
            std::ofstream logFileStream(logFile, std::ios::app);
            if (logFileStream.is_open()) {
                logFileStream << logMessage << std::endl;
            }
        }
    }
};

// Exception translator
class ExceptionTranslator {
public:
    static NLMException translateException(const std::exception& e) {
        std::string msg = e.what();
        
        // Check for known exception types and translate
        if (msg.find("configuration") != std::string::npos ||
            msg.find("Configuration") != std::string::npos) {
            return ConfigurationException(msg, "Configuration management");
        }
        
        if (msg.find("neural") != std::string::npos ||
            msg.find("Neural") != std:: TAL("network") != std::string::npos) {
            return NeuralNetworkException(msg, "Neural network operations");
        }
        
        if (msg.find("simulation") != std::string::npos ||
            msg.find("Simulation") != std::string::npos) {
            return SimulationException(msg, "Simulation execution");
        }
        
        if (msg.find("memory") != std::string::npos ||
            msg.find("Memory") != std::string::npos) {
            return MemoryException(msg, "Memory allocation");
        }
        
        if (msg.find("plasticity") != std::string::npos ||
            msg.find("Plasticity") != std::string::npos) {
            return PlasticityException(msg, "Plasticity operations");
        }
        
        // Default to generic NLM exception
        return NLMException(msg, ErrorSeverity::Error, ErrorCategory::Unknown);
    }
};

// Error recovery manager
class ErrorRecoveryManager {
private:
    std::unordered_map<std::string, std::function<bool()>> recoveryStrategies;
    std::unordered_map<std::string, int> retryCounts;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> lastErrorTimes;
    
public:
    void registerRecoveryStrategy(const std::string& errorKey,
                                  const std::function<bool()>& strategy) {
        recoveryStrategies[errorKey] = strategy;
    }
    
    bool attemptRecovery(const std::string& errorKey, const NLMException& error) {
        auto it = recoveryStrategies.find(errorKey);
        if (it == recoveryStrategies.end()) {
            return false;
        }
        
        // Check retry count
        int currentCount = retryCounts[errorKey];
        if (currentCount >= 3) { // Maximum 3 retries
            return false;
        }
        
        // Check time since last error (rate limiting)
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastErrorTimes[errorKey]);
        
        if (duration.count() < 1) { // Don't retry more than once per second
            return false;
        }
        
        // Attempt recovery
        try {
            bool success = it->second();
            if (success) {
                retryCounts[errorKey] = 0;
                lastErrorTimes[errorKey] = now;
            } else {
                retryCounts[errorKey] = currentCount + 1;
            }
            return success;
        } catch (...) {
            retryCounts[errorKey] = currentCount + 1;
            return false;
        }
    }
    
    void resetRecovery(const std::string& errorKey) {
        retryCounts[errorKey] = 0;
    }
};

// Error context manager
class ErrorContextManager {
private:
    std::unordered_map<std::string, std::string> contextStack;
    std::vector<NLMException> errorHistory;
    
public:
    void pushContext(const std::string& key, const std::string& value) {
        contextStack[key] = value;
    }
    
    void popContext(const std::string& key) {
        contextStack.erase(key);
    }
    
    std::string getContext(const std::string& key) const {
        auto it = contextStack.find(key);
        return it != contextStack.end() ? it->second : "";
    }
    
    void addError(const NLMException& error) {
        errorHistory.push_back(error);
    }
    
    std::vector<NLMException> getErrorHistory() const {
        return errorHistory;
    }
    
    void clearErrorHistory() {
        errorHistory.clear();
    }
    
    std::string getContextSummary() const {
        std::stringstream ss;
        ss << "Current Context:" << std::endl;
        for (const auto& pair : contextStack) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return ss.str();
    }
    
    std::string getErrorSummary() const {
        std::stringstream ss;
        ss << "Error History (" << errorHistory.size() << " errors):" << std::endl;
        for (const auto& error : errorHistory) {
            ss << "  " << error.getMessage() << std::endl;
        }
        return ss.str();
    }
};

// Global error handling functions
inline std::unique_ptr<ErrorHandler> globalErrorHandler;
inline std::unique_ptr<ErrorRecoveryManager> globalRecoveryManager;
inline std::unique_ptr<ErrorContextManager> globalContextManager;

inline void initializeErrorHandling(const std::string& logFile = "nlm_errors.log",
                                    bool console = true, bool file = true) {
    globalErrorHandler = std::make_unique<DefaultErrorHandler>(logFile, console, file);
    globalRecoveryManager = std::make_unique<ErrorRecoveryManager>();
    globalContextManager = std::make_unique<ErrorContextManager>();
}

inline void handleException(const std::exception& e) {
    if (globalErrorHandler) {
        NLMException translatedError = ExceptionTranslator::translateException(e);
        globalErrorHandler->handleError(translatedError);
        if (globalContextManager) {
            globalContextManager->addError(translatedError);
        }
    }
}

inline void setContext(const std::string& key, const std::string& value) {
    if (globalContextManager) {
        globalContextManager->pushContext(key, value);
    }
}

inline std::string getContext(const std::string& key) {
    if (globalContextManager) {
        return globalContextManager->getContext(key);
    }
    return "";
}

inline std::string getErrorContextSummary() {
    if (globalContextManager) {
        return globalContextManager->getContextSummary();
    }
    return "Error context manager not initialized";
}

inline std::string getErrorSummary() {
    if (globalContextManager) {
        return globalContextManager->getErrorSummary();
    }
    return "Error history not available";
}

// Enhanced logging system
class Logger {
private:
    std::string logFile;
    bool consoleOutput;
    bool fileOutput;
    bool timestampEnabled;
    bool threadIdEnabled;
    bool moduleEnabled;
    std::unordered_map<std::string, std::ofstream> moduleLoggers;
    
public:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string threadId;
        std::string module;
        ErrorSeverity level;
        std::string message;
        std::string context;
    };
    
    Logger(const std::string& logFile = "nlm.log",
           bool console = true, bool file = true,
           bool timestamp = true, bool threadId = true, bool module = true)
        : logFile(logFile), consoleOutput(console), fileOutput(file),
          timestampEnabled(timestamp), threadIdEnabled(threadId), moduleEnabled(module) {}
    
    void log(ErrorSeverity level, const std::string& message,
             const std::string& module = "NLM", const std::string& context = "") {
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = getCurrentThreadId();
        entry.module = module;
        entry.level = level;
        entry.message = message;
        entry.context = context;
        
        writeLogEntry(entry);
    }
    
    void debug(const std::string& message, const std::string& module = "NLM",
               const std::string& context = "") {
        log(ErrorSeverity::Debug, message, module, context);
    }
    
    void info(const std::string& message, const std::string& module = "NLM",
              const std::string& context = "") {
        log(ErrorSeverity::Info, message, module, context);
    }
    
    void warning(const std::string& message, const std::string& module = "NLM",
                 const std::string& context = "") {
        log(ErrorSeverity::Warning, message, module, context);
    }
    
    void error(const std::string& message, const std::string& module = "NLM",
               const std::string& context = "") {
        log(ErrorSeverity::Error, message, module, context);
    }
    
    void critical(const std::string& message, const std::string& module = "NLM",
                  const std::string& context = "") {
        log(ErrorSeverity::Critical, message, module, context);
    }
    
    void fatal(const std::string& message, const std::string& module = "NLM",
               const std::string& context = "") {
        log(ErrorSeverity::Fatal, message, module, context);
    }
    
    void startModule(const std::string& module) {
        if (moduleEnabled) {
            log(ErrorSeverity::Info, "Module started", module, "initialization");
        }
    }
    
    void endModule(const std::string& module) {
        if (moduleEnabled) {
            log(ErrorSeverity::Info, "Module ended", module, "shutdown");
        }
    }
    
    std::vector<LogEntry> getLogEntries(const std::string& module = "",
                                        ErrorSeverity minLevel = ErrorSeverity::Debug) const {
        // This would read from log file and return entries matching criteria
        // For now, return empty vector
        return std::vector<LogEntry>();
    }
    
    void clearLogFile() {
        if (fileOutput) {
            std::ofstream clearFile(logFile, std::ios::trunc);
        }
    }
    
private:
    void writeLogEntry(const LogEntry& entry) {
        std::stringstream ss;
        
        if (timestampEnabled) {
            ss << "[" << timestampToString(entry.timestamp) << "] ";
        }
        
        if (threadIdEnabled) {
            ss << "[" << entry.threadId << "] ";
        }
        
        if (moduleEnabled) {
            ss << "[" << entry.module << "] ";
        }
        
        ss << levelToString(entry.level) << ": ";
        ss << entry.message;
        
        if (!entry.context.empty()) {
            ss << " (Context: " << entry.context << ")";
        }
        
        std::string logMessage = ss.str();
        
        if (consoleOutput) {
            std::cout << logMessage << std::endl;
        }
        
        if (fileOutput) {
            std::ofstream logFileStream(logFile, std::ios::app);
            if (logFileStream.is_open()) {
                logFileStream << logMessage << std::endl;
            }
        }
    }
    
    std::string getCurrentThreadId() const {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string timestampToString(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    static std::string levelToString(ErrorSeverity level) {
        switch (level) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

// Global logger instance
inline std::unique_ptr<Logger> globalLogger;

inline void initializeLogger(const std::string& logFile = "nlm.log",
                             bool console = true, bool file = true,
                             bool timestamp = true, bool threadId = true, bool module = true) {
    globalLogger = std::make_unique<Logger>(logFile, console, file, timestamp, threadId, module);
}

inline void logMessage(ErrorSeverity level, const std::string& message,
                       const std::string& module = "NLM", const std::string& context = "") {
    if (globalLogger) {
        globalLogger->log(level, message, module, context);
    }
}

inline void logDebug(const std::string& message, const std::string& module = "NLM",
                     const std::string& context = "") {
    logMessage(ErrorSeverity::Debug, message, module, context);
}

inline void logInfo(const std::string& message, const std::string& module = "NLM",
                    const std::string& context = "") {
    logMessage(ErrorSeverity::Info, message, module, context);
}

inline void logWarning(const std::string& message, const std::string& module = "NLM",
                       const std::string& context = "") {
    logMessage(ErrorSeverity::Warning, message, module, context);
}

inline void logError(const std::string& message, const std::string& module = "NLM",
                     const std::string& context = "") {
    logMessage(ErrorSeverity::Error, message, module, context);
}

inline void logCritical(const std::string& message, const std::string& module = "NLM",
                        const std::string& context = "") {
    logMessage(ErrorSeverity::Critical, message, module, context);
}

inline void logFatal(const std::string& message, const std::string& module = "NLM",
                     const std::string& context = "") {
    logMessage(ErrorSeverity::Fatal, message, module, context);
}

// Safe execution wrappers
inline bool safeExecute(const std::function<void()>& func,
                       const std::string& operationName = "operation") {
    try {
        func();
        return true;
    } catch (const std::exception& e) {
        handleException(e);
        logError("Operation failed: " + std::string(e.what()), "SAFE_EXEC",
                 "Operation: " + operationName);
        return false;
    }
}

inline bool safeExecute(const std::function<void()>& func,
                       const std::function<bool(const NLMException&)>& errorHandler,
                       const std::string& operationName = "operation") {
    try {
        func();
        return true;
    } catch (const std::exception& e) {
        NLMException translatedError = ExceptionTranslator::translateException(e);
        if (errorHandler(translatedError)) {
            return false;
        }
        handleException(e);
        logError("Operation failed: " + std::string(e.what()), "SAFE_EXEC",
                 "Operation: " + operationName);
        return false;
    }
}

// Configuration with error handling
class SafeConfig {
private:
    std::unordered_map<std::string, std::string> config;
    Logger& logger;
    
public:
    SafeConfig(Logger& log) : logger(log) {}
    
    void set(const std::string& key, const std::string& value,
             const std::string& context = "") {
        try {
            config[key] = value;
            logger.info("Configuration set: " + key + " = " + value, "CONFIG",
                       "Context: " + context);
        } catch (const std::exception& e) {
            logger.error("Failed to set configuration: " + std::string(e.what()),
                        "CONFIG", "Context: " + context);
            throw ConfigurationException("Failed to set configuration: " + std::string(e.what()),
                                        context);
        }
    }
    
    std::string get(const std::string& key, const std::string& defaultValue = "",
                   const std::string& context = "") {
        auto it = config.find(key);
        if (it != config.end()) {
            return it->second;
        }
        
        logger.warning("Configuration key not found: " + key, "CONFIG",
                      "Context: " + context);
        return defaultValue;
    }
    
    bool has(const std::string& key) const {
        return config.find(key) != config.end();
    }
    
    void remove(const std::string& key, const std::string& context = "") {
        auto it = config.find(key);
        if (it != config.end()) {
            config.erase(it);
            logger.info("Configuration removed: " + key, "CONFIG",
                       "Context: " + context);
        } else {
            logger.warning("Attempted to remove non-existent configuration key: " + key,
                          "CONFIG", "Context: " + context);
        }
    }
    
    std::size_t size() const {
        return config.size();
    }
    
    void clear() {
        config.clear();
        logger.info("Configuration cleared", "CONFIG", "Context: clearing all");
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_LOGGING_HPP
