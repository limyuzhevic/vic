#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <ctime>

namespace nlm {

struct LogStatistics {
    uint64_t totalLogs = 0;
    uint64_t debugCount = 0;
    uint64_t infoCount = 0;
    uint64_t warningCount = 0;
    uint64_t errorCount = 0;
    uint64_t criticalCount = 0;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    double averageLogRate = 0.0;  // logs per second
    std::string sessionId;
    std::string hostname;
    std::string processId;
};

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Advanced features
    bool enableStatistics = true;
    bool enablePerformanceTracking = true;
    bool enableFiltering = false;
    std::string filterPattern;
    size_t maxLogHistory = 1000;
    std::vector<LogEntry> recentLogs;
    LogStatistics statistics;
    
    // Performance tracking
    std::chrono::system_clock::time_point sessionStart;
    uint64_t consoleOutputCount = 0;
    uint64_t fileOutputCount = 0;
    double lastPerformanceReport = 0.0;
    
    // Enhanced formatting
    bool enableTimestamps = true;
    bool enableThreadInfo = false;
    bool enableSourceLocation = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S.%f";
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize session
    pImpl->sessionStart = std::chrono::system_clock::now();
    pImpl->statistics.firstLog = pImpl->sessionStart;
    pImpl->statistics.lastLog = pImpl->sessionStart;
    
    // Get system information
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    pImpl->statistics.hostname = hostname;
    
    // Generate session ID
    pImpl->statistics.sessionId = "sess_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        pImpl->sessionStart.time_since_epoch()).count());
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->sessionStart = std::chrono::system_clock::now();
    pImpl->statistics.firstLog = pImpl->sessionStart;
    pImpl->statistics.lastLog = pImpl->sessionStart;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply filtering if enabled
    if (pImpl->enableFiltering && !filterMatches(entry.message)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "]";
    
    // Add timestamp if enabled
    if (pImpl->enableTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        struct tm tm_time;
        localtime_r(&time, &tm_time);
        oss << " [" << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "]";
    }
    
    // Add thread ID if enabled
    if (pImpl->enableThreadInfo) {
        oss << " [tid:" << std::this_thread::get_id() << "]";
    }
    
    // Add source location if enabled
    if (pImpl->enableSourceLocation && !entry.file.empty()) {
        oss << " [" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        if (!entry.function.empty()) {
            oss << " " << entry.function << "()]";
        } else {
            oss << "]";
        }
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Update statistics
    pImpl->statistics.totalLogs++;
    switch (entry.level) {
        case LogLevel::Debug: pImpl->statistics.debugCount++; break;
        case LogLevel::Info: pImpl->statistics.infoCount++; break;
        case LogLevel::Warning: pImpl->statistics.warningCount++; break;
        case LogLevel::Error: pImpl->statistics.errorCount++; break;
        case LogLevel::Critical: pImpl->statistics.criticalCount++; break;
    }
    
    pImpl->statistics.lastLog = std::chrono::system_clock::now();
    
    // Store recent log for history
    if (pImpl->recentLogs.size() >= pImpl->maxLogHistory) {
        pImpl->recentLogs.erase(pImpl->recentLogs.begin());
    }
    pImpl->recentLogs.push_back(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    pImpl->statistics.consoleOutputCount++;
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->statistics.fileOutputCount++;
    }
    
    // Periodic performance report
    auto now = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration<double>(now - pImpl->sessionStart).count();
    if (elapsed - pImpl->lastPerformanceReport > 60.0) {  // Every minute
        updateLogRateStatistics();
        pImpl->lastPerformanceReport = elapsed;
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
    
    // Global statistics
    LogStatistics globalStats;
    bool enableGlobalStatistics = true;
    std::chrono::system_clock::time_point sessionStart;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->sessionStart = std::chrono::system_clock::now();
    pImpl->globalStats.firstLog = pImpl->sessionStart;
    pImpl->globalStats.lastLog = pImpl->sessionStart;
    
    // Add default console logger
    auto defaultLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    defaultLogger->setUseColors(true);
    addLogger(defaultLogger);
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
    
    // Update global statistics
    if (pImpl->enableGlobalStatistics) {
        pImpl->globalStats.totalLogs++;
        auto now = std::chrono::system_clock::now();
        pImpl->globalStats.lastLog = now;
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

// Advanced logging methods for enhanced user experience
void Logger::neuralEvent(const std::string& event, const std::string& neuronId, const std::string& details) {
    std::ostringstream oss;
    oss << "[NEURAL] " << event << " (Neuron: " << neuronId << ")";
    if (!details.empty()) {
        oss << " - " << details;
    }
    info(oss.str(), "", 0, "neural_system");
}

void Logger::plasticityUpdate(const std::string& type, float delta, const std::string& synapseId) {
    std::ostringstream oss;
    oss << "[PLASTICITY] " << type << " updated " << synapseId << " by " << std::fixed << std::setprecision(6) << delta;
    debug(oss.str(), "", 0, "plasticity_system");
}

void Logger::performanceReport(const std::string& metric, double value, const std::string& unit) {
    std::ostringstream oss;
    oss << "[PERFORMANCE] " << metric << ": " << std::fixed << std::setprecision(3) << value << " " << unit;
    debug(oss.str(), "", 0, "performance_system");
}

void Logger::experimentalResult(const std::string& experimentName, bool success, const std::string& details) {
    std::ostringstream oss;
    oss << "[EXPERIMENT] " << experimentName << " - " << (success ? "SUCCESS" : "FAILED");
    if (!details.empty()) {
        oss << " (" << details << ")";
    }
    info(oss.str(), "", 0, "experimental_system");
}

// Statistics and analytics methods
void Logger::updateLogRateStatistics() {
    auto now = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration<double>(now - pImpl->globalStats.firstLog).count();
    
    if (elapsed > 0.0) {
        pImpl->globalStats.averageLogRate = pImpl->globalStats.totalLogs / elapsed;
    }
}

LogStatistics Logger::getGlobalStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalStats;
}

std::string Logger::getSessionReport() const {
    std::ostringstream oss;
    oss << "=== NLM Session Report ===\n";
    oss << "Session ID: " << pImpl->globalStats.sessionId << "\n";
    oss << "Hostname: " << pImpl->globalStats.hostname << "\n";
    oss << "Start Time: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(pImpl->globalStats.firstLog)), "%Y-%m-%d %H:%M:%S") << "\n";
    oss << "Total Logs: " << pImpl->globalStats.totalLogs << "\n";
    oss << "Debug: " << pImpl->globalStats.debugCount << "\n";
    oss << "Info: " << pImpl->globalStats.infoCount << "\n";
    oss << "Warning: " << pImpl->globalStats.warningCount << "\n";
    oss << "Error: " << pImpl->globalStats.errorCount << "\n";
    oss << "Critical: " << pImpl->globalStats.criticalCount << "\n";
    oss << "Average Log Rate: " << std::fixed << std::setprecision(2) << pImpl->globalStats.averageLogRate << " logs/second\n";
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Filter implementation
bool ConsoleLogger::filterMatches(const std::string& message) const {
    if (pImpl->filterPattern.empty()) {
        return true;
    }
    return message.find(pImpl->filterPattern) != std::string::npos;
}

// Configuration methods for advanced logging
void ConsoleLogger::configureAdvancedFeatures(bool enableStats, bool enablePerfTracking, bool enableFiltering, size_t maxHistory) {
    pImpl->enableStatistics = enableStats;
    pImpl->enablePerformanceTracking = enablePerfTracking;
    pImpl->enableFiltering = enableFiltering;
    pImpl->maxLogHistory = maxHistory;
}

void ConsoleLogger::setFilterPattern(const std::string& pattern) {
    pImpl->filterPattern = pattern;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void Logger::setAdvancedLoggingFeatures(bool enableStats, bool enableGlobalStats) {
    pImpl->enableGlobalStatistics = enableGlobalStats;
    // Configure all loggers with advanced features
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->configureAdvancedFeatures(enableStats, true, false, 1000);
        }
    }
}

} // namespace nlm