#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogFilter filter;
    LogStatistics stats;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> performanceTimers;
    std::unordered_map<std::string, std::pair<double, bool>> performanceResults;
    std::chrono::system_clock::time_point sessionStart;
    
    Impl() {
        sessionStart = std::chrono::system_clock::now();
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update statistics
    ++pImpl->stats.totalLogs;
    ++pImpl->stats.logsByLevel[static_cast<int>(entry.level)];
    
    // Category tracking
    bool categoryFound = false;
    for (int i = 0; i < 18; ++i) {
        if (static_cast<LogCategory>(i) == LogCategory::All || 
            entry.category == categoryToString(static_cast<LogCategory>(i))) {
            ++pImpl->stats.logsByCategory[i];
            categoryFound = true;
            break;
        }
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add category
    oss << "[" << categoryToString(LogCategory::None) << "] ";
    
    // Add timestamp
    oss << "[" << entry.getFormattedTimestamp() << "] ";
    
    // Add thread ID
    if (!entry.threadId.empty()) {
        oss << "[" << entry.threadId << "] ";
    }
    
    oss << entry.message;
    
    // Add metadata if present
    if (!entry.metadata.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& meta : entry.metadata) {
            if (!first) oss << ", ";
            oss << meta.first << "=" << meta.second;
            first = false;
        }
        oss << "}";
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
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double logTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Update average log time
    pImpl->stats.averageLogTime = ((pImpl->stats.averageLogTime * (pImpl->stats.totalLogs - 1)) + logTime) / pImpl->stats.totalLogs;
    
    // Update memory usage (approximate)
    pImpl->stats.memoryUsagePeak = std::max(pImpl->stats.memoryUsagePeak, entry.memoryUsage);
    
    // Update first/last log times
    if (pImpl->stats.firstLog.time_since_epoch().count() == 0) {
        pImpl->stats.firstLog = entry.timestamp;
    }
    pImpl->stats.lastLog = entry.timestamp;
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filter = filter;
}

LogFilter ConsoleLogger::getFilter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->filter;
}

std::string ConsoleLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats.getSummary();
}

void ConsoleLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = LogStatistics();
    pImpl->stats.firstLog = std::chrono::system_clock::now();
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isOutputFileOpen() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOut.is_open();
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

std::string ConsoleLogger::categoryToString(LogCategory category) const {
    switch (category) {
        case LogCategory::None: return "NONE";
        case LogCategory::All: return "ALL";
        case LogCategory::System: return "SYSTEM";
        case LogCategory::Brain: return "BRAIN";
        case LogCategory::Sensory: return "SENSOR";
        case LogCategory::Motor: return "MOTOR";
        case LogCategory::Plasticity: return "PLASTICITY";
        case LogCategory::Memory: return "MEMORY";
        case LogCategory::Cognition: return "COGNITION";
        case LogCategory::Development: return "DEVELOPMENT";
        case LogCategory::Simulation: return "SIMULATION";
        case LogCategory::Agent: return "AGENT";
        case LogCategory::Experiment: return "EXPERIMENT";
        case LogCategory::Performance: return "PERF";
        case LogCategory::Configuration: return "CONFIG";
        case LogCategory::Network: return "NETWORK";
        case LogCategory::Thread: return "THREAD";
        case LogCategory::File: return "FILE";
        case LogCategory::Custom: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    if (entry.level < pImpl->level) return false;
    
    return pImpl->filter.matches(entry);
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogFilter globalFilter;
    LogStatistics stats;
    std::chrono::system_clock::time_point sessionStart;
    
    Impl() {
        sessionStart = std::chrono::system_clock::now();
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (logger) {
        pImpl->loggers.push_back(logger);
    }
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

void Logger::log(LogLevel level, const std::string& category, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& threadId) {
    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = threadId.empty() ? LogDiagnostics::getThreadId() : threadId;
    
    logInternal(entry);
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    log(level, "None", message, file, line, function);
}

void Logger::debug(const std::string& category, const std::string& message,
                   const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, category, message, file, line, function);
}

void Logger::info(const std::string& category, const std::string& message,
                  const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, category, message, file, line, function);
}

void Logger::warning(const std::string& category, const std::string& message,
                     const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, category, message, file, line, function);
}

void Logger::error(const std::string& category, const std::string& message,
                   const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, category, message, file, line, function);
}

void Logger::critical(const std::string& category, const std::string& message,
                      const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, category, message, file, line, function);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    debug("None", message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    info("None", message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    warning("None", message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    error("None", message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    critical("None", message, file, line, function);
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

static std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    // Add console logger by default
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    ptr->addLogger(consoleLogger);
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

std::string Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::string stats = "Global Logger Statistics:\n";
    for (auto& logger : pImpl->loggers) {
        stats += "  " + logger->getStatistics() + "\n";
    }
    return stats;
}

void Logger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->resetStatistics();
    }
}

void Logger::setFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
    for (auto& logger : pImpl->loggers) {
        logger->setFilter(filter);
    }
}

LogFilter Logger::getFilter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalFilter;
}

void Logger::startPerformanceMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.firstLog = std::chrono::system_clock::now();
    pImpl->performanceTimers[operation] = std::chrono::high_resolution_clock::now();
}

void Logger::endPerformanceMeasurement(const std::string& operation, bool success) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->performanceTimers.find(operation);
    if (it != pImpl->performanceTimers.end()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - it->second).count();
        pImpl->performanceResults[operation] = std::make_pair(duration, success);
        pImpl->performanceTimers.erase(it);
    }
}

std::string Logger::getPerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::stringstream ss;
    ss << "Performance Report:\n";
    ss << "  Session duration: " 
       << std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::system_clock::now() - pImpl->sessionStart).count()
       << "ms\n";
    
    if (!pImpl->performanceResults.empty()) {
        ss << "  Operation timings:\n";
        for (const auto& result : pImpl->performanceResults) {
            ss << "    " << result.first << ": " << result.second.first << "ms"
               << " (" << (result.second.second ? "SUCCESS" : "FAILED") << ")\n";
        }
    }
    
    return ss.str();
}

void Logger::logInternal(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& category, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    entry_.level = level;
    entry_.category = category;
    entry_.timestamp = std::chrono::system_clock::now();
    entry_.threadId = LogDiagnostics::getThreadId();
}

LogStream::~LogStream() {
    Logger::getGlobal().log(entry_.level, entry_.category, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::metadata(const std::string& key, const std::string& value) {
    entry_.metadata[key] = value;
    return *this;
}

// Diagnostic utilities
namespace LogDiagnostics {
    std::string getSystemInfo() {
        std::stringstream ss;
        ss << "System Info:\n";
        
        // Get hostname
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        ss << "  Hostname: " << hostname << "\n";
        
        // Get user
        char username[256];
        if (getlogin_r(username, sizeof(username)) == 0) {
            ss << "  User: " << username << "\n";
        }
        
        // Get OS info
        std::ifstream osRelease("/etc/os-release");
        if (osRelease.is_open()) {
            std::string line;
            while (std::getline(osRelease, line)) {
                if (line.find("PRETTY_NAME=") == 0) {
                    ss << "  OS: " << line.substr(13) << "\n";
                    break;
                }
            }
        }
        
        // Get CPU info
        std::ifstream cpuInfo("/proc/cpuinfo");
        if (cpuInfo.is_open()) {
            std::string line;
            while (std::getline(cpuInfo, line)) {
                if (line.find("model name") == 0) {
                    ss << "  CPU: " << line.substr(line.find(':') + 2) << "\n";
                    break;
                }
            }
        }
        
        // Get memory info
        std::ifstream memInfo("/proc/meminfo");
        if (memInfo.is_open()) {
            std::string line;
            while (std::getline(memInfo, line)) {
                if (line.find("MemTotal=") == 0) {
                    long memTotal = 0;
                    sscanf(line.c_str(), "MemTotal: %ld kB", &memTotal);
                    ss << "  Memory: " << (memTotal / 1024) << " MB\n";
                    break;
                }
            }
        }
        
        return ss.str();
    }
    
    size_t getMemoryUsage() {
        std::ifstream status("/proc/self/status");
        if (!status.is_open()) return 0;
        
        std::string line;
        while (std::getline(status, line)) {
            if (line.find("VmRSS:") == 0) {
                long memKB = 0;
                sscanf(line.c_str(), "VmRSS: %ld kB", &memKB);
                return static_cast<size_t>(memKB);
            }
        }
        return 0;
    }
    
    std::string getThreadId() {
        std::stringstream ss;
        pid_t tid = getpid();
        ss << tid;
        return ss.str();
    }
    
    std::chrono::system_clock::time_point getStartTime() {
        static auto startTime = std::chrono::system_clock::now();
        return startTime;
    }
    
    std::string formatLogEntry(const LogEntry& entry) {
        std::stringstream ss;
        ss << "[" << (entry.level == LogLevel::Debug ? "DEBUG" :
                     entry.level == LogLevel::Info ? "INFO" :
                     entry.level == LogLevel::Warning ? "WARN" :
                     entry.level == LogLevel::Error ? "ERROR" :
                     entry.level == LogLevel::Critical ? "CRIT" : "UNK") << "] ";
        
        if (!entry.category.empty() && entry.category != "None") {
            ss << "[" << entry.category << "] ";
        }
        
        ss << entry.getFormattedTimestamp();
        
        if (!entry.threadId.empty()) {
            ss << " [" << entry.threadId << "]";
        }
        
        ss << " " << entry.message;
        
        return ss.str();
    }
    
    std::string createPerformanceReport(const std::string& loggerName) {
        return "Performance report for logger: " + loggerName + "\n";
    }
}

} // namespace nlm
