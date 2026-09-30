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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Performance timing
    struct TimingData {
        std::string operation;
        std::chrono::duration<double> duration;
        LogLevel level;
    };
    std::vector<TimingData> timingData;
    
    // Statistics
    struct Statistics {
        size_t logCount = 0;
        size_t errorCount = 0;
        size_t warningCount = 0;
        size_t debugCount = 0;
    } stats;
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
    
    // Update statistics
    pImpl->stats.logCount++;
    switch (level) {
        case LogLevel::Error:
        case LogLevel::Critical:
            pImpl->stats.errorCount++;
            break;
        case LogLevel::Warning:
            pImpl->stats.warningCount++;
            break;
        case LogLevel::Debug:
            pImpl->stats.debugCount++;
            break;
    }
    
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

// Helper function for starting performance timers
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& operation, LogLevel level = LogLevel::Info) 
        : operation_(operation), level_(level), start_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_);
        
        // Log performance data
        std::ostringstream oss;
        oss << "PERFORMANCE: " << operation_ << " took " 
            << std::fixed << std::setprecision(6) << duration.count() << "s";
        
        Logger::getGlobal().log(level_, oss.str());
        
        // Store timing data for later analysis
        Logger::getGlobal().addTimingData(operation_, duration, level_);
    }
    
private:
    std::string operation_;
    LogLevel level_;
    std::chrono::high_resolution_clock::time_point start_;
};

// Helper function for adding timing data to logger
void Logger::addTimingData(const std::string& operation, 
                          const std::chrono::duration<double>& duration, 
                          LogLevel level) {
    if (pImpl) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->timingData.push_back({operation, duration, level});
    }
}

// Helper function to get current performance statistics
void Logger::getPerformanceStats(std::ostringstream& oss) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    oss << "=== Performance Statistics ===\n";
    oss << "Total log messages: " << pImpl->stats.logCount << "\n";
    oss << "Error messages: " << pImpl->stats.errorCount << "\n";
    oss << "Warning messages: " << pImpl->stats.warningCount << "\n";
    oss << "Debug messages: " << pImpl->stats.debugCount << "\n";
    
    if (!pImpl->timingData.empty()) {
        oss << "\n=== Timing Data ===\n";
        for (const auto& timing : pImpl->timingData) {
            oss << timing.operation << ": " 
                << std::fixed << std::setprecision(6) << timing.duration.count() << "s "
                << "(" << static_cast<int>(timing.level) << ")\n";
        }
    }
}

// Helper function to dump system state for debugging
void Logger::dumpSystemState(std::ostringstream& oss, const std::string& context) {
    oss << "=== System State Dump ===\n";
    oss << "Context: " << context << "\n";
    oss << "Timestamp: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n";
    oss << "Statistics:\n";
    oss << "  Log messages: " << pImpl->stats.logCount << "\n";
    oss << "  Error messages: " << pImpl->stats.errorCount << "\n";
    oss << "  Warning messages: " << pImpl->stats.warningCount << "\n";
    oss << "  Debug messages: " << pImpl->stats.debugCount << "\n";
}

// Enhanced logging macros with more context
#define NLM_LOG_PERFORMANCE(operation, level) nlm::PerformanceProfiler _perf_ ## __LINE__(operation, level)
#define NLM_LOG_PERFORMANCE_INFO(operation) nlm::PerformanceProfiler _perf_ ## __LINE__(operation, nlm::LogLevel::Info)
#define NLM_LOG_PERFORMANCE_WARNING(operation) nlm::PerformanceProfiler _perf_ ## __LINE__(operation, nlm::LogLevel::Warning)
#define NLM_LOG_PERFORMANCE_ERROR(operation) nlm::PerformanceProfiler _perf_ ## __LINE__(operation, nlm::LogLevel::Error)
