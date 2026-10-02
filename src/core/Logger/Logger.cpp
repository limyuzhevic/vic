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
    
    // Category-specific log levels
    std::map<LogCategory, LogLevel> categoryLevels;
    
    // Performance timers
    std::map<std::string, std::chrono::steady_clock::time_point> performanceTimers;
    
    // Memory tracking
    size_t currentMemoryUsage;
    size_t peakMemoryUsage;
    size_t availableMemory;
    
    // Error tracking
    size_t errorCount;
    size_t warningCount;
    
    Logger::Impl() : currentMemoryUsage(0), peakMemoryUsage(0), availableMemory(0), 
                     errorCount(0), warningCount(0) {}
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default category levels
    for (int i = 0; i <= static_cast<int>(LogCategory::Configuration); ++i) {
        pImpl->categoryLevels[static_cast<LogCategory>(i)] = LogLevel::Info;
    }
    
    // Initialize memory tracking (placeholder values)
    pImpl->currentMemoryUsage = 0;
    pImpl->peakMemoryUsage = 0;
    pImpl->availableMemory = 0;
    
    // Initialize error tracking
    pImpl->errorCount = 0;
    pImpl->warningCount = 0;
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    
    // Set category levels for the new logger
    for (const auto& [category, level] : pImpl->categoryLevels) {
        logger->setCategoryLevel(category, level);
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

void Logger::setCategoryLevel(LogCategory category, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryLevels[category] = level;
    for (auto& logger : pImpl->loggers) {
        logger->setCategoryLevel(category, level);
    }
}

LogLevel Logger::getCategoryLevel(LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->categoryLevels.find(category);
    return (it != pImpl->categoryLevels.end()) ? it->second : LogLevel::Info;
}

void Logger::log(LogLevel level, LogCategory category, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if this category/level combination is enabled
    if (level < getCategoryLevel(category)) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.duration = std::chrono::microseconds(0); // Default
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.processId = getpid();
    entry.operation = "log";
    entry.simulationTime = 0.0; // Default
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, LogCategory::System, message, file, line, function);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, LogCategory::System, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, LogCategory::System, message, file, line, function);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Notice, LogCategory::System, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, LogCategory::System, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, LogCategory::System, message, file, line, function);
    pImpl->errorCount++;
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, LogCategory::System, message, file, line, function);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Alert, LogCategory::System, message, file, line, function);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Emergency, LogCategory::System, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level, LogCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    LogLevel catLevel = getCategoryLevel(category);
    return level <= catLevel;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
