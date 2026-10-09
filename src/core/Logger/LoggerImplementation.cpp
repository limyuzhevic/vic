#include "Logger.hpp"
#include "EnterpriseLogger.hpp"
#include "core/ErrorHandling/NLMExceptions.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

// Global instances for enterprise components
static std::unique_ptr<Enterprise::CorrelationManager> g_correlationManager;
static std::unique_ptr<MemoryTracker> g_memoryTracker;
static std::unique_ptr<ErrorReporter> g_errorReporter;
static bool g_enterpriseMode = false;

// Global Logger instance management
static std::shared_ptr<Logger> g_globalLogger;
static std::mutex g_loggerMutex;

// Implementation details
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool enterpriseMode = false;
    std::shared_ptr<Enterprise::CorrelationManager> correlationManager;
    std::unique_ptr<MemoryTracker> memoryTracker;
    std::shared_ptr<ErrorReporter> errorReporter;
    std::string currentThreadId;
    std::string currentProcessId;
    
    Impl() {
        // Initialize thread and process info
        std::ostringstream threadIdOss;
        threadIdOss << std::this_thread::get_id();
        currentThreadId = threadIdOss.str();
        
        currentProcessId = std::to_string(getpid());
        
        // Initialize enterprise components
        correlationManager = std::make_shared<Enterprise::CorrelationManager>();
        memoryTracker = std::make_unique<MemoryTracker>();
        errorReporter = std::make_shared<ErrorReporter>();
    }
};

// Initialize global enterprise components
void initializeEnterpriseSystem() {
    if (!g_correlationManager) {
        g_correlationManager = std::make_unique<Enterprise::CorrelationManager>();
    }
    if (!g_memoryTracker) {
        g_memoryTracker = std::make_unique<MemoryTracker>();
    }
    if (!g_errorReporter) {
        g_errorReporter = std::make_unique<ErrorReporter>();
    }
    g_enterpriseMode = true;
}

Enterprise::CorrelationManager& getCorrelationManager() {
    if (!g_correlationManager) {
        initializeEnterpriseSystem();
    }
    return *g_correlationManager;
}

MemoryTracker& getMemoryTracker() {
    if (!g_memoryTracker) {
        initializeEnterpriseSystem();
    }
    return *g_memoryTracker;
}

ErrorReporter& getErrorReporter() {
    if (!g_errorReporter) {
        initializeEnterpriseSystem();
    }
    return *g_errorReporter;
}

bool isEnterpriseMode() {
    return g_enterpriseMode;
}

// Error reporter implementation
void ErrorReporter::report(const NLMException& exception, bool autoRecover) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ErrorReport report;
    report.id = generateReportId();
    report.message = exception.what();
    report.category = exception.getCategory();
    report.severity = exception.getSeverity();
    report.file = exception.getFile();
    report.line = exception.getLine();
    report.function = exception.getFunction();
    report.timestamp = exception.getTimestamp();
    report.stackTrace = extractStackTrace(exception);
    report.recovered = false;
    report.recoveryAction = "none";
    
    // Store error
    errorHistory_.push_back(report);
    
    // Trim history if too large
    if (errorHistory_.size() > maxHistorySize_) {
        errorHistory_.erase(errorHistory_.begin(), errorHistory_.begin() + 50);
    }
    
    // Try automatic recovery if enabled
    if (autoRecover && exception.canRecover()) {
        report.recoveryAction = "auto_recovered";
        report.recovered = true;
        // In real implementation, would perform recovery
    }
    
    // Notify callbacks
    for (const auto& callback : callbacks_) {
        callback(report);
    }
    
    // Update statistics
    updateErrorStats(report);
}

void ErrorReporter::report(const std::string& message, ExceptionCategory category, 
                          ExceptionSeverity severity) {
    NLMException exception(message, category, severity);
    report(exception);
}

// Global error handling functions
void reportError(const NLMException& exception, bool autoRecover) {
    getErrorReporter().report(exception, autoRecover);
}

void reportError(const std::string& message, ExceptionCategory category, 
                 ExceptionSeverity severity) {
    getErrorReporter().report(message, category, severity);
}

// Console logger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool useEmoji = false;
    std::unordered_map<LogLevel, std::string> colors;
    std::unordered_map<LogLevel, std::string> emojis;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    int maxFiles = 5;
    bool logToFile = false;
    std::string filepath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    initializeColors();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    initializeColors();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::initializeColors() {
    pImpl->colors[LogLevel::Trace] = "\033[90m";     // Gray
    pImpl->colors[LogLevel::Debug] = "\033[36m";     // Cyan
    pImpl->colors[LogLevel::Info] = "\033[32m";      // Green
    pImpl->colors[LogLevel::Warning] = "\033[33m";   // Yellow
    pImpl->colors[LogLevel::Error] = "\033[31m";     // Red
    pImpl->colors[LogLevel::Critical] = "\033[35m"; // Magenta
    pImpl->colors[LogLevel::Alert] = "\033[91m";     // Bright Red
    pImpl->colors[LogLevel::Emergency] = "\033[95m"; // Bright Magenta
    
    pImpl->emojis[LogLevel::Trace] = "🔍 ";
    pImpl->emojis[LogLevel::Debug] = "🐛 ";
    pImpl->emojis[LogLevel::Info] = "ℹ️ ";
    pImpl->emojis[LogLevel::Warning] = "⚠️ ";
    pImpl->emojis[LogLevel::Error] = "❌ ";
    pImpl->emojis[LogLevel::Critical] = "💥 ";
    pImpl->emojis[LogLevel::Alert] = "🚨 ";
    pImpl->emojis[LogLevel::Emergency] = "🔥 ";
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if file rotation is needed
    if (pImpl->logToFile && pImpl->fileOut.is_open()) {
        if (pImpl->fileOut.tellp() >= static_cast<std::streamoff>(pImpl->maxFileSize)) {
            // Rotate file
            closeOutputFile();
            rotateFile();
        }
    }
    
    std::ostringstream oss;
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Level with color and emoji
    if (pImpl->useColors) {
        oss << " " << pImpl->colors[entry.level];
        if (pImpl->useEmoji) {
            oss << pImpl->emojis[entry.level];
        }
        oss << levelToString(entry.level) << " ";
        oss << "\033[0m"; // Reset
    } else {
        oss << " " << levelToString(entry.level) << " ";
    }
    
    // Thread ID
    oss << "[" << entry.threadId << "] ";
    
    // Module
    if (!entry.module.empty()) {
        oss << "[" << entry.module << "] ";
    }
    
    // Message
    oss << entry.message;
    
    // Performance metrics
    if (entry.executionTime.count() > 0) {
        oss << " [t: " << entry.executionTime.count() << "μs]";
    }
    if (entry.memoryUsage > 0) {
        oss << " [mem: " << entry.memoryUsage << "B]";
    }
    
    // Fields
    for (const auto& field : entry.fields) {
        oss << " " << field.first << "=" << field.second;
    }
    
    // File and line
    if (!entry.file.empty() && entry.line > 0) {
        size_t pos = entry.file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? entry.file.substr(pos + 1) : entry.file;
        oss << " (" << filename << ":" << entry.line << ")";
    }
    
    std::string output = oss.str();
    
    // Console output
    std::cout << output << std::endl;
    
    // File output if set
    if (pImpl->logToFile && pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << std::endl;
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    closeOutputFile();
    
    pImpl->filepath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->logToFile = pImpl->fileOut.is_open();
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->logToFile = false;
    }
}

void ConsoleLogger::rotateFile() {
    if (pImpl->filepath.empty()) return;
    
    // Close current file
    closeOutputFile();
    
    // Rotate existing files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string oldName = pImpl->filepath;
        std::string newName = pImpl->filepath;
        
        if (i < pImpl->maxFiles - 1) {
            oldName = pImpl->filepath + "." + std::to_string(i);
            newName = pImpl->filepath + "." + std::to_string(i + 1);
        }
        
        if (std::filesystem::exists(oldName)) {
            std::filesystem::rename(oldName, newName);
        }
    }
    
    // Reopen for new log
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
    pImpl->logToFile = pImpl->fileOut.is_open();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    auto it = pImpl->colors.find(level);
    if (it != pImpl->colors.end()) {
        return it->second;
    }
    return "\033[0m";
}

// Logger implementation
Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    consoleLogger->setUseEmoji(true);
    addLogger(consoleLogger);
}

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
                 const std::string& file, int line,
                 const std::string& function) {
    // Create log entry with enhanced metadata
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = pImpl->currentThreadId;
    entry.processId = pImpl->currentProcessId;
    entry.module = "";  // Will be set by caller if needed
    entry.correlationId = pImpl->correlationManager ? 
                         pImpl->correlationManager->getCurrentCorrelationId() : 0;
    entry.executionTime = std::chrono::microseconds(0);
    entry.memoryUsage = 0;
    
    // Log to all handlers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file,
                  int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file,
                     int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file,
                      int line, const std::string& function) {
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
    std::lock_guard<std::mutex> lock(g_loggerMutex);
    if (!g_globalLogger) {
        g_globalLogger = std::make_shared<Logger>();
    }
    return *g_globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(g_loggerMutex);
    g_globalLogger = logger;
}

// Enterprise logging methods
void Logger::setEnterpriseMode(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enterpriseMode = enabled;
    
    if (enabled) {
        initializeEnterpriseSystem();
        pImpl->correlationManager = g_correlationManager;
        pImpl->memoryTracker = g_memoryTracker;
        pImpl->errorReporter = g_errorReporter;
    }
}

bool Logger::isEnterpriseMode() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enterpriseMode;
}

Enterprise::PerformanceLogger Logger::startPerformanceLog(const std::string& module) {
    return Enterprise::PerformanceLogger(module, 
        pImpl->correlationManager ? pImpl->correlationManager->getCurrentCorrelationId() : 0);
}

void Logger::logMemoryStats(const std::string& context) {
    if (pImpl->memoryTracker) {
        auto stats = pImpl->memoryTracker->getStats();
        std::ostringstream oss;
        oss << context << " Memory: " << stats.currentAllocated << "B (peak: " 
            << stats.peakAllocated << "B, allocs: " << stats.allocationCount << ")";
        
        LogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = oss.str();
        entry.module = "MemoryTracker";
        entry.correlationId = getCorrelationManager().getCurrentCorrelationId();
        
        log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }
}

bool Logger::performHealthCheck() {
    bool healthy = true;
    std::string healthMessage = "Health Check: OK";
    
    if (pImpl->errorReporter) {
        auto stats = pImpl->errorReporter->getSystemStats();
        if (stats.unrecoveredErrors > 100) {
            healthMessage += ", HIGH_ERROR_RATE: " + std::to_string(stats.unrecoveredErrors) + " errors";
            healthy = false;
        }
    }
    
    if (pImpl->memoryTracker) {
        auto memStats = pImpl->memoryTracker->getStats();
        if (memStats.memoryFragmentation > 0.5) {
            healthMessage += ", HIGH_FRAGMENTATION: " + 
                           std::to_string(memStats.memoryFragmentation * 100) + "%";
            healthy = false;
        }
    }
    
    LogEntry entry;
    entry.level = healthy ? LogLevel::Info : LogLevel::Warning;
    entry.message = healthMessage;
    entry.module = "HealthMonitor";
    entry.correlationId = getCorrelationManager().getCurrentCorrelationId();
    
    log(entry.level, entry.message, entry.file, entry.line, entry.function);
    
    return healthy;
}

void Logger::startCorrelation(const std::string& context) {
    if (pImpl->correlationManager) {
        pImpl->correlationManager->startCorrelation(context);
    }
}

void Logger::endCorrelation() {
    if (pImpl->correlationManager) {
        pImpl->correlationManager->endCorrelation();
    }
}

uint64_t Logger::getCurrentCorrelationId() const {
    return pImpl->correlationManager ? 
        pImpl->correlationManager->getCurrentCorrelationId() : 0;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    // Get global logger and log the stream content
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// PerformanceLogger implementation
Enterprise::PerformanceLogger::PerformanceLogger(const std::string& module, uint64_t correlationId)
    : module_(module), correlationId_(correlationId) {
    startTime_ = std::chrono::high_resolution_clock::now();
    activeTimers_.push_back(startTime_);
}

Enterprise::PerformanceLogger::~PerformanceLogger() {
    if (!activeTimers_.empty()) {
        activeTimers_.pop_back();
    }
    logPerformance();
}

void Enterprise::PerformanceLogger::startTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    TimerInfo timer;
    timer.operation = operation;
    timer.startTime = std::chrono::high_resolution_clock::now();
    timer.correlationId = correlationId_;
    timer.module = module_;
    activeTimers_.push_back(timer.startTime);
}

void Enterprise::PerformanceLogger::endTiming(const std::string& operation, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (activeTimers_.size() < 2) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto startTime = std::prev(activeTimers_.end())->startTime;
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::ostringstream oss;
    oss << "Performance: " << operation << " took " << duration.count() << " μs";
    
    LogEntry entry;
    entry.level = level;
    entry.message = oss.str();
    entry.module = module_;
    entry.correlationId = correlationId_;
    entry.executionTime = duration;
    
    // Log to logger if available
    if (logger_) {
        logger_->log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }
    
    activeTimers_.pop_back();
}

void Enterprise::PerformanceLogger::logPerformance() {
    if (activeTimers_.size() > 1) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = activeTimers_.front().startTime;
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        std::ostringstream oss;
        oss << "Operation completed in " << duration.count() << " μs";
        
        LogEntry entry;
        entry.level = LogLevel::Debug;
        entry.message = oss.str();
        entry.module = module_;
        entry.correlationId = correlationId_;
        entry.executionTime = duration;
        
        if (logger_) {
            logger_->log(entry.level, entry.message, entry.file, entry.line, entry.function);
        }
    }
}

// MemoryTracker implementation
void MemoryTracker::recordAllocation(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalAllocated += size;
    stats_.currentAllocated += size;
    stats_.peakAllocated = std::max(stats_.peakAllocated, stats_.currentAllocated);
    stats_.allocationCount++;
}

void MemoryTracker::recordDeallocation(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (size <= stats_.currentAllocated) {
        stats_.currentAllocated -= size;
    }
    stats_.deallocationCount++;
}

void MemoryTracker::recordGC(std::chrono::system_clock::time_point time, size_t collected, size_t generations) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.lastGC = time;
    stats_.gcCollectionCount += collected;
    
    if (stats_.totalAllocated > 0) {
        stats_.memoryFragmentation = 
            1.0 - (static_cast<double>(stats_.currentAllocated) / stats_.totalAllocated);
    }
}

std::string MemoryTracker::formatStats() const {
    std::ostringstream oss;
    oss << "Memory: " << stats_.currentAllocated << "B (peak: " << stats_.peakAllocated 
        << "B, allocs: " << stats_.allocationCount << ")";
    return oss.str();
}

// CorrelationManager implementation
void Enterprise::CorrelationManager::startCorrelation(const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    CorrelationContext ctx;
    ctx.id = generateCorrelationId();
    ctx.context = context;
    ctx.startTime = std::chrono::system_clock::now();
    
    correlationContexts_.push_back(ctx);
    currentCorrelationId_ = ctx.id;
}

void Enterprise::CorrelationManager::endCorrelation() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!correlationContexts_.empty()) {
        correlationContexts_.pop_back();
        if (!correlationContexts_.empty()) {
            currentCorrelationId_ = correlationContexts_.back().id;
        } else {
            currentCorrelationId_ = generateCorrelationId();
        }
    }
}

uint64_t Enterprise::CorrelationManager::generateCorrelationId() {
    static std::atomic<uint64_t> idCounter{0};
    return ++idCounter;
}

// Global instances initialization
void initializeEnterpriseSystem() {
    if (!g_correlationManager) {
        g_correlationManager = std::make_unique<Enterprise::CorrelationManager>();
    }
    if (!g_memoryTracker) {
        g_memoryTracker = std::make_unique<MemoryTracker>();
    }
    if (!g_errorReporter) {
        g_errorReporter = std::make_unique<ErrorReporter>();
    }
    g_enterpriseMode = true;
}

Enterprise::CorrelationManager& getCorrelationManager() {
    if (!g_correlationManager) {
        initializeEnterpriseSystem();
    }
    return *g_correlationManager;
}

MemoryTracker& getMemoryTracker() {
    if (!g_memoryTracker) {
        initializeEnterpriseSystem();
    }
    return *g_memoryTracker;
}

ErrorReporter& getErrorReporter() {
    if (!g_errorReporter) {
        initializeEnterpriseSystem();
    }
    return *g_errorReporter;
}

bool isEnterpriseMode() {
    return g_enterpriseMode;
}

} // namespace nlm

