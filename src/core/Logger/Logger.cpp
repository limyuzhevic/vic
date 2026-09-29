#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>

namespace nlm {

// Implementation for error handling integration
namespace {
    
    // Global error handler function
    std::function<void(const NLMError&)> globalErrorHandler;
    
    // Error reporting enabled flag
    bool errorReportingEnabled = true;
    
    // Thread-local error suppression
    thread_local bool suppressCurrentErrors = false;
}

// Enhanced ConsoleLogger implementation with error handling support
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool errorReportingEnabled = true;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool errorReportingEnabled = true;
    std::function<void(const NLMError&)> globalErrorHandler;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// Error handling implementation
void Logger::logException(const NLMError& error) {
    if (!pImpl->errorReportingEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogLevel level = errorCategoryToLogLevel(error.getCategory());
    
    std::ostringstream oss;
    oss << "=== " << error.getErrorType() << " ===\n";
    oss << "Category: " << error.getCategoryString() << "\n";
    oss << "Message: " << error.what() << "\n";
    
    LogEntry entry;
    entry.level = level;
    entry.message = oss.str();
    entry.file = error.getFilePath();
    entry.line = 0;
    entry.function = "NLMException";
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logError(const NLMError& error) {
    logException(error);
}

void Logger::logErrorWithContext(const NLMError& error, const std::string& additionalContext) {
    std::ostringstream oss;
    oss << "=== " << error.getErrorType() << " ===\n";
    oss << "Additional Context: " << additionalContext << "\n";
    oss << "Message: " << error.what() << "\n";
    oss << "============================\n";
    
    LogLevel level = errorCategoryToLogLevel(error.getCategory());
    log(level, oss.str(), error.getFilePath(), 0, "");
}

LogLevel Logger::errorCategoryToLogLevel(ErrorCategory category) const {
    switch (category) {
        case ErrorCategory::System:
        case ErrorCategory::Memory:
            return LogLevel::Critical;
        case ErrorCategory::Configuration:
        case ErrorCategory::Validation:
        case ErrorCategory::File:
        case ErrorCategory::Network:
        case ErrorCategory::Initialization:
        case ErrorCategory::Simulation:
            return LogLevel::Error;
        case ErrorCategory::Plasticity:
        case ErrorCategory::Python:
            return LogLevel::Warning;
        default:
            return LogLevel::Error;
    }
}

void Logger::setErrorReportingEnabled(bool enabled) {
    pImpl->errorReportingEnabled = enabled;
}

bool Logger::isErrorReportingEnabled() const {
    return pImpl->errorReportingEnabled;
}

void Logger::setGlobalErrorHandler(std::function<void(const NLMError&)> handler) {
    globalErrorHandler = handler;
}

void Logger::handleError(const NLMError& error) {
    if (!errorReportingEnabled) return;
    
    if (globalErrorHandler) {
        globalErrorHandler(error);
    }
    
    logError(error);
    
    // Add to error chain for debugging
    getGlobalErrorChain().addError(std::make_shared<NLMError>(error));
}

// Global error reporting configuration
namespace {
    bool globalErrorReportingEnabled = true;
}

void setGlobalErrorReporting(bool enabled) {
    globalErrorReportingEnabled = enabled;
}

bool isGlobalErrorReportingEnabled() {
    return globalErrorReportingEnabled;
}

// Global error handler
void setGlobalErrorHandler(std::function<void(const NLMError&)> handler) {
    globalErrorHandler = handler;
}

void handleError(const NLMError& error) {
    if (!globalErrorReportingEnabled) return;
    
    if (globalErrorHandler) {
        globalErrorHandler(error);
    }
    
    Logger::getGlobal().logError(error);
}

// Macro definitions for easy error handling
#define NLM_THROW_ERROR(type, message, ...) throw type(message, ##__VA_ARGS__)

#define NLM_ERROR_REPORT(logger, error) \
    if (&(error) != nullptr) { \
        logger->error(error.what()); \
    }

#define NLM_HANDLE_ERROR(type, message, func) \
    try { return func(); } catch (const type& e) { \
        Logger::getGlobal().logError(e); \
        throw; \
    }

// Error recovery utilities
namespace ErrorRecovery {
    
    template<typename Func>
    bool handleWithRecovery(Func func, const std::string& errorContext) {
        try {
            func();
            return true;
        } catch (const NLMError& e) {
            Logger::getGlobal().logErrorWithContext(e, "Context: " + errorContext);
        } catch (const std::exception& e) {
            SystemError systemError(std::string("Unexpected exception: ") + e.what());
            Logger::getGlobal().logError(systemError);
        } catch (...) {
            SystemError systemError("Unknown error occurred");
            Logger::getGlobal().logError(systemError);
        }
        return false;
    }
    
    template<typename Func>
    bool handleWithRetry(Func func, int maxRetries, int retryDelayMs) {
        int attempts = 0;
        while (attempts < maxRetries) {
            try {
                func();
                return true;
            } catch (const NLMError& e) {
                Logger::getGlobal().logErrorWithContext(e, 
                    "Attempt " + std::to_string(++attempts) + "/" + std::to_string(maxRetries));
                if (attempts >= maxRetries) throw;
            }
        }
        return false;
    }
}

// Error chain and suppression utilities
namespace {
    ErrorChain globalErrorChain;
    ErrorSuppression globalErrorSuppression;
}

ErrorChain& getGlobalErrorChain() {
    return globalErrorChain;
}

ErrorSuppression& getGlobalErrorSuppression() {
    return globalErrorSuppression;
}

void logErrorWithContext(const NLMError& error, const Logger& logger, const std::string& additionalContext) {
    std::ostringstream oss;
    oss << "=== " << error.getErrorType() << " ===\n";
    oss << "Category: " << error.getCategoryString() << "\n";
    oss << "Message: " << error.what() << "\n";
    
    if (!additionalContext.empty()) {
        oss << "Additional Context: " << additionalContext << "\n";
    }
    
    oss << "============================\n";
    
    LogLevel level = logger.errorCategoryToLogLevel(error.getCategory());
    logger.log(level, oss.str(), error.getFilePath(), 0, "");
}

} // namespace nlm

// Error handling system

namespace nlm {

// Error categories for user-friendly handling
enum class ErrorCategory {
    System = 0,
    Configuration,
    Initialization,
    Memory,
    Validation,
    File,
    Plasticity,
    Network,
    Simulation,
    Python,
    Count
};

const char* errorCategoryToString(ErrorCategory category) {
    static const char* categories[] = {
        "System", "Configuration", "Initialization", "Memory",
        "Validation", "File", "Plasticity", "Network", "Simulation",
        "Python", "Unknown"
    };
    return categories[static_cast<int>(category)];
}

// Base error class
class NLMError : public std::exception {
protected:
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    ErrorCategory category_;
    mutable std::string whatStr_;
    std::map<std::string, std::string> context_;
    
public:
    NLMError(ErrorCategory category,
             const std::string& message,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "")
        : message_(message), file_(file), line_(line), function_(function),
          category_(category), whatStr_("") {
        formatMessage();
    }
    
    virtual ~NLMError() noexcept {}
    
    const char* what() const noexcept override {
        if (whatStr_.empty()) {
            formatWhat();
        }
        return whatStr_.c_str();
    }
    
    const std::string& getMessage() const { return message_; }
    ErrorCategory getCategory() const { return category_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }
    
    const std::string& getContext(const std::string& key) const {
        static const std::string empty = "";
        auto it = context_.find(key);
        return (it != context_.end()) ? it->second : empty;
    }
    
    bool hasContext(const std::string& key) const {
        return context_.find(key) != context_.end();
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "[" << errorCategoryToString(category_) << "] ";
        ss << "File: " << file_ << ":" << line_ << " ";
        if (!function_.empty()) {
            ss << "Function: " << function_ << " ";
        }
        ss << "Message: " << message_;
        if (!context_.empty()) {
            ss << " Context:";
            for (const auto& pair : context_) {
                ss << " " << pair.first << "=" << pair.second;
            }
        }
        return ss.str();
    }
    
protected:
    virtual void formatMessage() {}
    virtual void formatWhat() const {
        whatStr_ = toString();
    }
};

// Specific error types
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message,
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "",
                      const std::string& key = "",
                      const std::string& value = "",
                      const std::string& expected = "")
        : NLMError(ErrorCategory::Configuration, message, file, line, function) {
        if (!key.empty()) addContext("key", key);
        if (!value.empty()) addContext("value", value);
        if (!expected.empty()) addContext("expected", expected);
    }
};

class InitializationError : public NLMError {
public:
    InitializationError(const std::string& message,
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "",
                       const std::string& component = "")
        : NLMError(ErrorCategory::Initialization, message, file, line, function) {
        if (!component.empty()) addContext("component", component);
    }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "",
               size_t bytesRequested = 0)
        : NLMError(ErrorCategory::Memory, message, file, line, function) {
        if (bytesRequested > 0) {
            addContext("bytes", std::to_string(bytesRequested));
        }
    }
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "",
                   const std::string& parameter = "",
                   const std::string& value = "",
                   const std::string& constraint = "")
        : NLMError(ErrorCategory::Validation, message, file, line, function) {
        if (!parameter.empty()) addContext("parameter", parameter);
        if (!value.empty()) addContext("value", value);
        if (!constraint.empty()) addContext("constraint", constraint);
    }
};

class FileError : public NLMError {
public:
    FileError(const std::string& message,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "",
             const std::string& path = "",
             const std::string& operation = "")
        : NLMError(ErrorCategory::File, message, file, line, function) {
        if (!path.empty()) addContext("path", path);
        if (!operation.empty()) addContext("operation", operation);
    }
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message,
                  const std::string& file = "",
                  int line = 0,
                  const std::string& function = "",
                  const std::string& neuronType = "",
                  const std::string& synapseType = "",
                  double weightChange = 0.0)
        : NLMError(ErrorCategory::Plasticity, message, file, line, function) {
        if (!neuronType.empty()) addContext("neuronType", neuronType);
        if (!synapseType.empty()) addContext("synapseType", synapseType);
        if (weightChange != 0.0) {
            addContext("weightChange", std::to_string(weightChange));
        }
    }
};

class NetworkError : public NLMError {
public:
    NetworkError(const std::string& message,
                const std::string& file = "",
                int line = 0,
                const std::string& function = "",
                const std::string& source = "",
                const std::string& target = "",
                int connectionId = -1)
        : NLMError(ErrorCategory::Network, message, file, line, function) {
        if (!source.empty()) addContext("source", source);
        if (!target.empty()) addContext("target", target);
        if (connectionId >= 0) {
            addContext("connectionId", std::to_string(connectionId));
        }
    }
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "",
                   int step = -1,
                   const std::string& region = "")
        : NLMError(ErrorCategory::Simulation, message, file, line, function) {
        if (step >= 0) addContext("step", std::to_string(step));
        if (!region.empty()) addContext("region", region);
    }
};

// Error handling macros and functions
#define NLM_THROW(type, message, ...) \
    throw type(message, __FILE__, __LINE__, __func__); \

#define NLM_THROW_WITH_CONTEXT(type, message, file, line, function, ...) \
    type error((message), (file), (line), (function), ##__VA_ARGS__); \
    logger->error(error.toString(), error.getFile(), error.getLine(), error.getFunction()); \
    throw error; \

#define NLM_THROW_AND_LOG(type, message, ...) \
    { \
        type error((message), __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        logger->error(error.toString(), error.getFile(), error.getLine(), error.getFunction()); \
        throw error; \
    }

#define NLM_ERROR_REPORT(type, message, file, line, function) \
    { \
        type error((message), (file), (line), (function)); \
        logger->error(error.toString(), error.getFile(), error.getLine(), error.getFunction()); \
    }

#define NLM_HANDLE_ERROR(type, message, func) \
    try { \
        return func(); \
    } catch (const type& e) { \
        logger->error(e.toString(), e.getFile(), e.getLine(), e.getFunction()); \
        throw; \
    }

// Helper function to get logger
nlm::Logger* getLogger() {
    return &nlm::Logger::getGlobal();
}

} // namespace nlm
