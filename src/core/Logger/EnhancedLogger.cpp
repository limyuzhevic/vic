#include "EnhancedLogger.hpp"
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <cstdint>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <map>
#include <numeric>
#include <iomanip>

namespace nlm {

// EnhancedConsoleLogger implementation
struct EnhancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Correlation ID management
    uint64_t currentCorrelationId = 0;
    uint64_t threadLocalCorrelationId = 0;
    
    // Performance metrics
    std::map<std::string, std::chrono::system_clock::time_point> activeTimers;
    std::map<std::string, std::vector<double>> performanceMetrics;
    
    // Log file rotation
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxFiles = 5;
    std::string baseFilename = "nlm.log";
    size_t currentFileSize = 0;
    
    // Memory tracking
    size_t peakMemoryUsage = 0;
    std::chrono::system_clock::time_point lastMemoryCheck;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastMemoryCheck = std::chrono::system_clock::now();
}

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->lastMemoryCheck = std::chrono::system_clock::now();
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() = default;

void EnhancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry enhancedEntry;
    enhancedEntry.level = entry.level;
    enhancedEntry.message = entry.message;
    enhancedEntry.file = entry.file;
    enhancedEntry.line = entry.line;
    enhancedEntry.function = entry.function;
    enhancedEntry.timestamp = entry.timestamp;
    enhancedEntry.correlationId = pImpl->currentCorrelationId;
    enhancedEntry.threadId = std::to_string(getCurrentThreadId());
    
    // Output to console
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add thread ID
    oss << " [" << enhancedEntry.threadId << "]";
    
    // Add correlation ID
    if (enhancedEntry.correlationId != 0) {
        oss << " [corr:" << enhancedEntry.correlationId << "]";
    }
    
    oss << " " << entry.message;
    
    // Add file location for debugging
    if (!entry.file.empty() && entry.line > 0) {
        oss << " [" << entry.file << ":" << entry.line << "]";
    }
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Check for file rotation
        if (pImpl->currentFileSize >= pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        pImpl->fileOut.flush();
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->baseFilename = filepath;
    pImpl->currentFileSize = pImpl->fileOut.tellp();
}

void EnhancedConsoleLogger::setMaxFileSize(size_t maxSizeBytes) {
    pImpl->maxFileSize = maxSizeBytes;
}

void EnhancedConsoleLogger::setMaxFiles(int maxFiles) {
    pImpl->maxFiles = maxFiles;
}

void EnhancedConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Rotate existing files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string from = pImpl->baseFilename;
        std::string to;
        
        if (i == pImpl->maxFiles - 1) {
            from += ".1";
        } else if (i > 0) {
            from = pImpl->baseFilename + "." + std::to_string(i);
            to = pImpl->baseFilename + "." + std::to_string(i + 1);
        }
        
        if (std::filesystem::exists(from)) {
            if (!to.empty()) {
                std::filesystem::rename(from, to);
            } else {
                std::filesystem::remove(from);
            }
        }
    }
    
    // Open new log file
    pImpl->fileOut.open(pImpl->baseFilename, std::ios::app);
    pImpl->currentFileSize = 0;
}

void EnhancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

uint64_t EnhancedConsoleLogger::generateCorrelationId() {
    static std::atomic<uint64_t> counter(1);
    return counter.fetch_add(1);
}

void EnhancedConsoleLogger::setCorrelationId(uint64_t id) {
    pImpl->currentCorrelationId = id;
}

uint64_t EnhancedConsoleLogger::getCurrentCorrelationId() const {
    return pImpl->currentCorrelationId;
}

void EnhancedConsoleLogger::logStructured(LogLevel level, const std::string& message,
                                         const std::vector<std::string>& context) {
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.correlationId = pImpl->currentCorrelationId;
    entry.threadId = std::to_string(getCurrentThreadId());
    entry.context = context;
    
    log(entry);
}

void EnhancedConsoleLogger::startPerformanceTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->activeTimers[operation] = std::chrono::system_clock::now();
}

void EnhancedConsoleLogger::stopPerformanceTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(operation);
    if (it != pImpl->activeTimers.end()) {
        auto duration = std::chrono::duration<double, std::milli>(
            std::chrono::system_clock::now() - it->second).count();
        
        pImpl->performanceMetrics[operation].push_back(duration);
        
        // Log performance if level allows
        if (getLevel() <= LogLevel::Info) {
            std::cout << "[PERF] " << operation << " took " 
                     << duration << " ms\n";
        }
        
        pImpl->activeTimers.erase(it);
    }
}

void EnhancedConsoleLogger::logPerformanceMetric(const std::string& name, double value,
                                                 const std::string& unit) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMetrics[name].push_back(value);
    
    // Log performance metric
    std::cout << "[METRIC] " << name << ": " << value << " " << unit << "\n";
}

std::string EnhancedConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string EnhancedConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string EnhancedConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time;
    gmtime_r(&time, &tm_time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string EnhancedConsoleLogger::formatStructured(const EnhancedLogEntry& entry) const {
    std::ostringstream oss;
    oss << formatTimestamp(entry.timestamp) << " "
        << levelToString(entry.level) << " "
        << entry.message;
    
    if (!entry.threadId.empty()) {
        oss << " thread:" << entry.threadId;
    }
    
    if (entry.correlationId != 0) {
        oss << " correlation:" << entry.correlationId;
    }
    
    for (const auto& ctx : entry.context) {
        oss << " " << ctx;
    }
    
    return oss.str();
}

uint64_t EnhancedConsoleLogger::getCurrentThreadId() const {
    return static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

// PerformanceMetrics implementation
struct PerformanceMetrics::Impl {
    std::map<std::string, std::vector<std::pair<double, std::string>>> metrics;
    std::mutex mutex;
    size_t totalMemoryUsage = 0;
    double totalCpuUsage = 0.0;
};

PerformanceMetrics::PerformanceMetrics() : pImpl(std::make_unique<Impl>()) {}
PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::recordMetric(const std::string& name, double value,
                                     const std::string& unit,
                                     const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics[category].emplace_back(value, unit);
}

void PerformanceMetrics::recordMemoryUsage(size_t memoryBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalMemoryUsage = memoryBytes;
    if (memoryBytes > pImpl->totalMemoryUsage) {
        pImpl->totalMemoryUsage = memoryBytes;
    }
}

void PerformanceMetrics::recordCpuUsage(double cpuPercent) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalCpuUsage = cpuPercent;
}

std::vector<std::pair<std::string, double>> PerformanceMetrics::getMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::pair<std::string, double>> result;
    
    for (const auto& category : pImpl->metrics) {
        for (const auto& metric : category.second) {
            result.emplace_back(category.first + "." + metric.first, metric.first);
        }
    }
    
    return result;
}

void PerformanceMetrics::clearMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.clear();
    pImpl->totalMemoryUsage = 0;
    pImpl->totalCpuUsage = 0.0;
}

PerformanceMetrics& PerformanceMetrics::getGlobalMetrics() {
    static PerformanceMetrics metrics;
    return metrics;
}

// EnhancedLogger implementation
EnhancedLogger::EnhancedLogger() : Logger() {}
EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::startOperation(const std::string& name) {
    // Find console logger and start timer
    for (const auto& logger : getGlobal().pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger)) {
            consoleLogger->startPerformanceTimer(name);
            break;
        }
    }
}

void EnhancedLogger::endOperation(const std::string& name) {
    for (const auto& logger : getGlobal().pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger)) {
            consoleLogger->stopPerformanceTimer(name);
            break;
        }
    }
}

void EnhancedLogger::logPerformance(const std::string& name, double duration,
                                   const std::string& unit) {
    getMetrics().recordMetric(name, duration, unit, "performance");
    
    for (const auto& logger : getGlobal().pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger)) {
            consoleLogger->logPerformanceMetric(name, duration, unit);
            break;
        }
    }
}

void EnhancedLogger::logStructured(LogLevel level, const std::string& message,
                                  const std::vector<std::string>& context) {
    for (const auto& logger : getGlobal().pImpl->loggers) {
        if (auto enhancedLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger)) {
            enhancedLogger->logStructured(level, message, context);
            break;
        }
    }
}

void EnhancedLogger::trackError(const std::string& errorCode, const std::string& message,
                               const std::string& context) {
    ErrorHandler::logError(ErrorCategory::System, ErrorSeverity::Error, message + " [" + errorCode + "]", 
                         "", 0, "", errorCode);
}

PerformanceMetrics& EnhancedLogger::getMetrics() {
    return PerformanceMetrics::getGlobalMetrics();
}

// NLMError implementation
NLMError::NLMError(ErrorCategory category, ErrorSeverity severity,
                  const std::string& message, const std::string& file,
                  int line, const std::string& function,
                  const std::string& errorCode, uint64_t correlationId)
    : category_(category), severity_(severity), message_(message),
      file_(file), line_(line), function_(function),
      errorCode_(errorCode), correlationId_(correlationId),
      timestamp_(std::chrono::system_clock::now()) {
}

void NLMError::log() const {
    std::string formattedMessage = getDescription();
    
    // Convert severity to LogLevel
    LogLevel logLevel;
    switch (severity_) {
        case ErrorSeverity::Debug: logLevel = LogLevel::Debug; break;
        case ErrorSeverity::Info: logLevel = LogLevel::Info; break;
        case ErrorSeverity::Warning: logLevel = LogLevel::Warning; break;
        case ErrorSeverity::Error: logLevel = LogLevel::Error; break;
        case ErrorSeverity::Critical: logLevel = LogLevel::Critical; break;
        case ErrorSeverity::Fatal: logLevel = LogLevel::Critical; break;
        default: logLevel = LogLevel::Info;
    }
    
    Logger::getGlobal().log(logLevel, formattedMessage, file_, line_, function_);
}

bool NLMError::isRecoverable() const {
    return severity_ != ErrorSeverity::Fatal;
}

std::string NLMError::getDescription() const {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(category_) << "] "
        << "[" << static_cast<int>(severity_) << "] "
        << message_;
    
    if (!errorCode_.empty()) {
        oss << " (Code: " << errorCode_ << ")";
    }
    
    if (!file_.empty() && line_ > 0) {
        oss << " in " << file_ << ":" << line_;
    }
    
    if (!function_.empty()) {
        oss << " at " << function_;
    }
    
    if (correlationId_ != 0) {
        oss << " (Correlation: " << correlationId_ << ")";
    }
    
    oss << " at " << std::chrono::system_clock::to_time_t(timestamp_);
    
    return oss.str();
}

// Specific error type implementations
void ConfigurationError::log() const {
    Logger::getGlobal().log(LogLevel::Error, getDescription(), 
                          file_, line_, function_);
}

bool ConfigurationError::isRecoverable() const {
    return true; // Most configuration errors are recoverable
}

std::string ConfigurationError::getDescription() const {
    return "Configuration Error: " + message_;
}

void ValidationError::log() const {
    Logger::getGlobal().log(LogLevel::Warning, getDescription(),
                          file_, line_, function_);
}

bool ValidationError::isRecoverable() const {
    return true; // Validation errors are usually recoverable
}

std::string ValidationError::getDescription() const {
    return "Validation Error: " + message_;
}

void MemoryError::log() const {
    Logger::getGlobal().log(LogLevel::Critical, getDescription(),
                          file_, line_, function_);
}

bool MemoryError::isRecoverable() const {
    return severity_ != ErrorSeverity::Fatal; // Most memory errors are recoverable
}

std::string MemoryError::getDescription() const {
    return "Memory Error: " + message_;
}

// ErrorHandler implementation
namespace {
    std::function<void(const std::shared_ptr<NLMError>)> errorReportingCallback;
    uint64_t globalCorrelationId = 0;
    std::mutex errorHandlerMutex;
}

void ErrorHandler::logError(const std::shared_ptr<NLMError> error) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    
    if (error) {
        error->log();
        
        if (errorReportingCallback) {
            errorReportingCallback(error);
        }
    }
}

void ErrorHandler::logError(ErrorCategory category, ErrorSeverity severity,
                          const std::string& message,
                          const std::string& file, int line,
                          const std::string& function,
                          const std::string& errorCode) {
    auto error = std::make_shared<NLMError>(category, severity, message, file, line, function, errorCode, getGlobalCorrelationId());
    logError(error);
}

bool ErrorHandler::isRecoverable(const std::shared_ptr<NLMError> error) {
    return error && error->isRecoverable();
}

std::string ErrorHandler::getFormattedError(const std::shared_ptr<NLMError> error) {
    return error ? error->getDescription() : "Unknown error";
}

void ErrorHandler::setErrorReportingCallback(std::function<void(const std::shared_ptr<NLMError>)> callback) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    errorReportingCallback = callback;
}

uint64_t ErrorHandler::generateCorrelationId() {
    static std::atomic<uint64_t> counter(1);
    return counter.fetch_add(1);
}

void ErrorHandler::setGlobalCorrelationId(uint64_t id) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    globalCorrelationId = id;
}

uint64_t ErrorHandler::getGlobalCorrelationId() {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    return globalCorrelationId;
}

} // namespace nlm
