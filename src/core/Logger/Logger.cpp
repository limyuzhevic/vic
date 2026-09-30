#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <thread>
#include <map>
#include <set>

namespace nlm {

// Global performance tracking
struct PerformanceTracker {
    std::string operationName;
    std::chrono::steady_clock::time_point startTime;
    std::unordered_map<std::string, std::string> context;
    bool active;
};

std::map<std::string, PerformanceTracker> globalPerformanceTrackers;
std::mutex performanceMutex;

// Global error context
struct GlobalContext {
    std::unordered_map<std::string, std::string> contextMap;
    std::mutex contextMutex;
};

GlobalContext globalContext;

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::function<bool(const LogEntry&)> filter;
    std::set<LogLevel> enabledLevels;
    std::unordered_map<std::string, std::string> formatStrings;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->enabledLevels = {LogLevel::Debug, LogLevel::Info, LogLevel::Warning, LogLevel::Error, LogLevel::Critical};
    pImpl->formatStrings = {
        {"debug", "[{time}] {level} {message}"},
        {"info", "[{time}] {level} {message}"},
        {"warning", "[{time}] {level} {message}"},
        {"error", "[{time}] {level} {message}"},
        {"critical", "[{time}] {level} {message}"}
    };
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->enabledLevels = {LogLevel::Debug, LogLevel::Info, LogLevel::Warning, LogLevel::Error, LogLevel::Critical};
    pImpl->formatStrings = {
        {"debug", "[{time}] {level} {message}"},
        {"info", "[{time}] {level} {message}"},
        {"warning", "[{time}] {level} {message}"},
        {"error", "[{time}] {level} {message}"},
        {"critical", "[{time}] {level} {message}"}
    };
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (pImpl->filter && !pImpl->filter(entry)) {
        return;
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (pImpl->enabledLevels.find(entry.level) == pImpl->enabledLevels.end()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    logFormatted(entry);
}

void ConsoleLogger::logError(const NLMError& error) {
    if (error.code == ErrorCode::SUCCESS) {
        return;
    }
    
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = ErrorUtils::errorCodeToString(error) + ": " + error.message;
    entry.file = error.file;
    entry.line = error.line;
    entry.function = error.function;
    entry.timestamp = error.timestamp;
    entry.context = error.context;
    
    if (!entry.context.empty()) {
        std::ostringstream contextOss;
        contextOss << " [Context:";
        for (const auto& pair : entry.context) {
            contextOss << " " << pair.first << "=" << pair.second;
        }
        contextOss << "]";
        entry.message += contextOss.str();
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    logFormatted(entry);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFilter(const std::function<bool(const LogEntry&)>& filter) {
    pImpl->filter = filter;
}

bool ConsoleLogger::isEnabled(LogLevel level) const {
    return pImpl->enabledLevels.find(level) != pImpl->enabledLevels.end();
}

// Variable argument logging for formatted messages
void ConsoleLogger::logDebug(const std::string& format, ...) {
    if (LogLevel::Debug < pImpl->level) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format message with variable arguments
    va_list args;
    va_start(args, format);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Debug) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    // Format message with variable arguments
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    oss << buffer;
    
    va_end(args);
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Debug) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logInfo(const std::string& format, ...) {
    if (LogLevel::Info < pImpl->level) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    va_list args;
    va_start(args, format);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Info) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    oss << buffer;
    
    va_end(args);
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Info) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logWarning(const std::string& format, ...) {
    if (LogLevel::Warning < pImpl->level) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    va_list args;
    va_start(args, format);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Warning) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    oss << buffer;
    
    va_end(args);
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Warning) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logError(const std::string& format, ...) {
    if (LogLevel::Error < pImpl->level) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    va_list args;
    va_start(args, format);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Error) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    oss << buffer;
    
    va_end(args);
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Error) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logCritical(const std::string& format, ...) {
    if (LogLevel::Critical < pImpl->level) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    va_list args;
    va_start(args, format);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Critical) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    oss << buffer;
    
    va_end(args);
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Critical) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logPerformance(const std::string& operation, double durationMs,
                                   const std::unordered_map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(LogLevel::Info) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " PERFORMANCE ";
    oss << "Operation: " << operation << " ";
    oss << "Duration: " << std::fixed << std::setprecision(2) << durationMs << "ms";
    
    if (!context.empty()) {
        oss << " Context:";
        for (const auto& pair : context) {
            oss << " " << pair.first << "=" << pair.second;
        }
    }
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(LogLevel::Info) << "\033[1m" << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::logFormatted(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " ";
    
    // Add thread ID if available
    if (!entry.threadId.empty() && entry.threadId != "main") {
        oss << "[" << entry.threadId << "] ";
    }
    
    // Add operation ID if available
    if (!entry.operationId.empty()) {
        oss << "[" << entry.operationId << "] ";
    }
    
    // Add message
    oss << entry.message;
    
    // Add context if available
    if (!entry.context.empty()) {
        oss << " [Context:";
        for (const auto& pair : entry.context) {
            oss << " " << pair.first << "=" << pair.second;
        }
        oss << "]";
    }
    
    // Add duration if available
    if (entry.durationMs > 0) {
        oss << " [Duration:" << std::fixed << std::setprecision(3) << entry.durationMs << "ms]";
    }
    
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
        default: return "";
    }
}

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time;
    localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    return entry.message;
}

// PerformanceTracer implementation
struct PerformanceTracer::Impl {
    std::string operationName;
    std::chrono::steady_clock::time_point startTime;
    std::unordered_map<std::string, std::string> context;
    bool active;
};

PerformanceTracer::PerformanceTracer(const std::string& operationName,
                                     const std::unordered_map<std::string, std::string>& context)
    : pImpl(std::make_unique<Impl>()), operationName_(operationName), context_(context), active(true) {
    pImpl->operationName = operationName;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->context = context;
    pImpl->active = true;
}

PerformanceTracer::~PerformanceTracer() {
    if (pImpl->active) {
        double durationMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - pImpl->startTime).count();
        logPerformance(pImpl->operationName, durationMs, pImpl->context);
    }
}

void PerformanceTracer::setContext(const std::string& key, const std::string& value) {
    pImpl->context[key] = value;
}

double PerformanceTracer::getDurationMs() const {
    if (!pImpl->active) return 0.0;
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - pImpl->startTime).count();
}

void PerformanceTracer::startTrace(const std::string& operationName,
                                   const std::unordered_map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(performanceMutex);
    
    std::string traceId = operationName + "_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    
    PerformanceTracker tracker;
    tracker.operationName = operationName;
    tracker.startTime = std::chrono::steady_clock::now();
    tracker.context = context;
    tracker.active = true;
    
    globalPerformanceTrackers[traceId] = tracker;
}

void PerformanceTracer::endTrace(const std::string& operationName, double durationMs) {
    std::lock_guard<std::mutex> lock(performanceMutex);
    
    // Find the most recent trace for this operation
    for (auto it = globalPerformanceTrackers.rbegin(); it != globalPerformanceTrackers.rend(); ++it) {
        if (it->second.operationName == operationName) {
            it->second.active = false;
            logPerformance(operationName, durationMs, it->second.context);
            break;
        }
    }
}

void PerformanceTracer::logPerformance(const std::string& operationName,
                                       double durationMs,
                                       const std::unordered_map<std::string, std::string>& context) {
    Logger::getGlobal().logPerformance(operationName, durationMs, context);
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::unordered_map<std::string, std::string> globalContext;
    std::string currentOperationId;
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
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.context = pImpl->globalContext;
    entry.operationId = pImpl->currentOperationId;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logError(ErrorCode code, const std::string& message,
                      const std::string& file, int line, const std::string& function,
                      const std::string& operationId) {
    NLMError error;
    error.code = code;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    error.context = pImpl->globalContext;
    error.operationId = operationId.empty() ? pImpl->currentOperationId : operationId;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->logError(error);
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

void Logger::logPerformance(const std::string& operation, double durationMs,
                           const std::unordered_map<std::string, std::string>& context) {
    // Log performance through the default logger
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Performance: " + operation + " took " + 
                   std::to_string(durationMs) + "ms";
    entry.context = context;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled(level)) {
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

PerformanceTracer Logger::trace(const std::string& operationName,
                                const std::unordered_map<std::string, std::string>& context) {
    return PerformanceTracer(operationName, context);
}

void Logger::reportError(ErrorCode code, const std::string& message,
                         const std::string& file, int line, const std::string& function,
                         const std::shared_ptr<NLMError>& cause,
                         const std::string& operationId) {
    NLMError error;
    error.code = code;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    error.context = pImpl->globalContext;
    error.operationId = operationId.empty() ? pImpl->currentOperationId : operationId;
    error.cause = cause;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->logError(error);
    }
}

void Logger::setContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext[key] = value;
}

std::string Logger::getContext(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->globalContext.find(key);
    return (it != pImpl->globalContext.end()) ? it->second : "";
}

void Logger::clearContext() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext.clear();
}

// ErrorUtils implementation
namespace ErrorUtils {
    std::string errorCodeToString(ErrorCode code) {
        switch (code) {
            case ErrorCode::SUCCESS: return "SUCCESS";
            case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            case ErrorCode::CONFIGURATION_ERROR: return "CONFIGURATION_ERROR";
            case ErrorCode::BRAIN_OPERATION_ERROR: return "BRAIN_OPERATION_ERROR";
            case ErrorCode::MEMORY_ERROR: return "MEMORY_ERROR";
            case ErrorCode::PLASTICITY_ERROR: return "PLASTICITY_ERROR";
            case ErrorCode::NEUROMODULATION_ERROR: return "NEUROMODULATION_ERROR";
            case ErrorCode::MEMORY_SYSTEM_ERROR: return "MEMORY_SYSTEM_ERROR";
            case ErrorCode::PREDICTION_ERROR: return "PREDICTION_ERROR";
            case ErrorCode::DEVELOPMENT_ERROR: return "DEVELOPMENT_ERROR";
            case ErrorCode::SIMULATION_ERROR: return "SIMULATION_ERROR";
            case ErrorCode::FILE_IO_ERROR: return "FILE_IO_ERROR";
            case ErrorCode::NETWORK_ERROR: return "NETWORK_ERROR";
            case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
            default: return "UNKNOWN_ERROR";
        }
    }

    std::string errorCodeToString(const NLMError& error) {
        return errorCodeToString(error.code);
    }

    std::string formatError(const NLMError& error, bool includeStackTrace) {
        std::ostringstream oss;
        oss << "Error: " << errorCodeToString(error.code) << ": " << error.message;
        oss << " at " << error.file << ":" << error.line << " in " << error.function;
        
        if (!error.context.empty()) {
            oss << "\nContext:";
            for (const auto& pair : error.context) {
                oss << " " << pair.first << "=" << pair.second;
            }
        }
        
        if (error.cause) {
            oss << "\nCaused by: " << formatError(*error.cause, includeStackTrace);
        }
        
        if (includeStackTrace && !error.stackTrace.empty()) {
            oss << "\nStack trace:";
            for (const auto& frame : error.stackTrace) {
                oss << "\n  " << frame;
            }
        }
        
        return oss.str();
    }

    bool isRecoverable(ErrorCode code) {
        switch (code) {
            case ErrorCode::CONFIGURATION_ERROR:
            case ErrorCode::BRAIN_OPERATION_ERROR:
            case ErrorCode::MEMORY_ERROR:
            case ErrorCode::PLASTICITY_ERROR:
            case ErrorCode::NEUROMODULATION_ERROR:
                return true;
            default:
                return false;
        }
    }

    std::string getErrorSuggestion(ErrorCode code) {
        switch (code) {
            case ErrorCode::CONFIGURATION_ERROR:
                return "Check configuration file and parameters.";
            case ErrorCode::BRAIN_OPERATION_ERROR:
                return "Ensure brain is properly initialized and try resetting.";
            case ErrorCode::MEMORY_ERROR:
                return "Check memory capacity and free up resources.";
            case ErrorCode::PLASTICITY_ERROR:
                return "Check plasticity parameters and neuron connectivity.";
            case ErrorCode::NEUROMODULATION_ERROR:
                return "Check neuromodulator configuration and scaling factors.";
            case ErrorCode::MEMORY_SYSTEM_ERROR:
                return "Verify memory system configuration and data integrity.";
            case ErrorCode::PREDICTION_ERROR:
                return "Review prediction system parameters and training data.";
            case ErrorCode::DEVELOPMENT_ERROR:
                return "Check developmental stage parameters and progression rates.";
            case ErrorCode::SIMULATION_ERROR:
                return "Verify simulation time steps and world configuration.";
            case ErrorCode::FILE_IO_ERROR:
                return "Check file permissions and disk space.";
            case ErrorCode::VALIDATION_ERROR:
                return "Validate input parameters and system state.";
            default:
                return "Please check system logs and try again.";
        }
    }
}

} // namespace nlm