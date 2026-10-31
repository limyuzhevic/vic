#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <thread>

namespace nlm {

// Helper functions
namespace {
    std::string getCurrentThreadName() {
        // In a real implementation, this would get the actual thread name
        // For now, return a generic identifier
        return "main";
    }
    
    std::string getThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::string sanitizeLogMessage(const std::string& message) {
        // Remove control characters and limit length
        std::string result;
        result.reserve(message.size());
        for (char c : message) {
            if (c >= 32 && c != 127 || c == '\n' || c == '\r' || c == '\t') {
                result += c;
            }
        }
        if (result.size() > 1024) {
            result.resize(1024);
        }
        return result;
    }
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::string colorScheme = "default";
    bool includeTimestamp = true;
    bool includeThreadInfo = true;
    bool includeLocationInfo = true;
    
    struct LogFile {
        std::ofstream file;
        size_t maxSize = 10 * 1024 * 1024; // 10MB default
        int maxFiles = 5;
        std::string basePath;
        size_t currentSize = 0;
        
        void write(const std::string& message) {
            file << message << std::endl;
            currentSize += message.size() + 1; // +1 for newline
            
            // Check if file needs rotation
            if (currentSize >= maxSize) {
                rotate();
            }
        }
        
        void rotate() {
            file.close();
            
            // Close current file
            if (file.is_open()) {
                file.close();
            }
            
            // Remove oldest file
            for (int i = maxFiles - 1; i >= 0; --i) {
                std::string oldPath = basePath + "." + (i > 0 ? std::to_string(i - 1) : "old");
                std::string newPath = basePath + "." + std::to_string(i);
                
                if (std::filesystem::exists(oldPath)) {
                    if (i == maxFiles - 1) {
                        std::filesystem::remove(oldPath);
                    } else {
                        std::filesystem::rename(oldPath, newPath);
                    }
                }
            }
            
            // Reopen file
            file.open(basePath, std::ios::app);
            currentSize = 0;
            
            // Read existing file to get current size
            if (file.is_open()) {
                file.seekg(0, std::ios::end);
                currentSize = file.tellg();
            }
        }
    };
    
    LogFile fileOutput;
    std::mutex mutex;
    
    // Performance monitoring
    struct PerfStats {
        double totalTime = 0.0;
        uint64_t callCount = 0;
        std::chrono::system_clock::time_point lastUpdate = std::chrono::system_clock::now();
    };
    
    PerfStats perfStats;
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
    
    std::string formattedMessage = formatMessage(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level, pImpl->colorScheme) << formattedMessage << "\033[0m" << std::endl;
    } else {
        std::cout << formattedMessage << std::endl;
    }
    
    // File output if set
    if (pImpl->fileOutput.file.is_open()) {
        pImpl->fileOutput.write(formattedMessage);
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

void ConsoleLogger::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

void ConsoleLogger::setOutputFile(const std::string& filepath, size_t maxSize, int maxFiles) {
    pImpl->fileOutput.file.open(filepath, std::ios::app);
    pImpl->fileOutput.basePath = filepath;
    pImpl->fileOutput.maxSize = maxSize;
    pImpl->fileOutput.maxFiles = maxFiles;
    
    if (pImpl->fileOutput.file.is_open()) {
        pImpl->fileOutput.file.seekg(0, std::ios::end);
        pImpl->fileOutput.currentSize = pImpl->fileOutput.file.tellg();
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOutput.file.is_open()) {
        pImpl->fileOutput.file.close();
    }
}

void ConsoleLogger::rotateLogFile() {
    pImpl->fileOutput.rotate();
}

void ConsoleLogger::setIncludeTimestamp(bool include) {
    pImpl->includeTimestamp = include;
}

void ConsoleLogger::setIncludeThreadInfo(bool include) {
    pImpl->includeThreadInfo = include;
}

void ConsoleLogger::setIncludeLocationInfo(bool include) {
    pImpl->includeLocationInfo = include;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Fatal: return "FATAL";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Silent: return "SILENT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level, const std::string& scheme) const {
    if (!useColors) return "";
    
    if (scheme == "monokai") {
        switch (level) {
            case LogLevel::Fatal: return "\033[1;91m"; // Bright red
            case LogLevel::Error: return "\033[91m";   // Red
            case LogLevel::Warning: return "\033[93m"; // Yellow
            case LogLevel::Notice: return "\033[94m";  // Blue
            case LogLevel::Info: return "\033[92m";    // Green
            case LogLevel::Debug: return "\033[90m";   // Gray
            case LogLevel::Trace: return "\033[35m";   // Magenta
            default: return "\033[0m";
        }
    } else if (scheme == "dark") {
        switch (level) {
            case LogLevel::Fatal: return "\033[1;41;97m"; // White on red background
            case LogLevel::Error: return "\033[31m";      // Red
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Notice: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Debug: return "\033[90m";     // Bright black
            case LogLevel::Trace: return "\033[35m";     // Magenta
            default: return "\033[0m";
        }
    } else { // default
        switch (level) {
            case LogLevel::Fatal: return "\033[1;31m"; // Bold red
            case LogLevel::Error: return "\033[31m";    // Red
            case LogLevel::Warning: return "\033[33m"; // Yellow
            case LogLevel::Notice: return "\033[34m";  // Blue
            case LogLevel::Info: return "\033[32m";    // Green
            case LogLevel::Debug: return "\033[36m";   // Cyan
            case LogLevel::Trace: return "\033[35m";   // Magenta
            default: return "\033[0m";
        }
    }
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Add log level
    oss << "[" << levelToString(entry.level) << "]";
    
    // Add timestamp
    if (pImpl->includeTimestamp) {
        oss << " " << formatTimestamp(entry.timestamp);
    }
    
    // Add thread info
    if (pImpl->includeThreadInfo) {
        oss << " " << entry.threadId;
        if (!entry.threadName.empty()) {
            oss << "(" << entry.threadName << ")";
        }
    }
    
    // Add category
    if (!entry.category.empty()) {
        oss << " [" << entry.category << "]";
    }
    
    // Add message
    oss << " " << sanitizeLogMessage(entry.message);
    
    // Add location info
    if (pImpl->includeLocationInfo && !entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ":" << entry.function << ")";
    }
    
    return oss.str();
}

struct FileLogger::Impl {
    std::string filepath;
    size_t maxSize;
    int maxFiles;
    std::ofstream file;
    size_t currentSize = 0;
    std::mutex mutex;
    
    void write(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        
        file << message << std::endl;
        currentSize += message.size() + 1;
        
        if (currentSize >= maxSize) {
            rotate();
        }
    }
    
    void rotate() {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (!file.is_open()) return;
        
        file.close();
        
        // Remove oldest file
        for (int i = maxFiles - 1; i >= 0; --i) {
            std::string oldPath = filepath + "." + (i > 0 ? std::to_string(i - 1) : "old");
            std::string newPath = filepath + "." + std::to_string(i);
            
            if (std::filesystem::exists(oldPath)) {
                if (i == maxFiles - 1) {
                    std::filesystem::remove(oldPath);
                } else {
                    std::filesystem::rename(oldPath, newPath);
                }
            }
        }
        
        // Reopen file
        file.open(filepath, std::ios::app);
        currentSize = 0;
        
        if (file.is_open()) {
            file.seekg(0, std::ios::end);
            currentSize = file.tellg();
        }
    }
};

FileLogger::FileLogger(const std::string& filepath, size_t maxSize, int maxFiles)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->filepath = filepath;
    pImpl->maxSize = maxSize;
    pImpl->maxFiles = maxFiles;
    
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());
    
    pImpl->file.open(filepath, std::ios::app);
    if (pImpl->file.is_open()) {
        pImpl->file.seekg(0, std::ios::end);
        pImpl->currentSize = pImpl->file.tellg();
    }
}

FileLogger::~FileLogger() {
    flush();
    if (pImpl->file.is_open()) {
        pImpl->file.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << std::setw(8) << std::left << levelToString(entry.level) << "] "
        << formatTimestamp(entry.timestamp) << " "
        << entry.message << std::endl;
    
    pImpl->file << oss.str();
    pImpl->currentSize += oss.str().size();
    
    if (pImpl->currentSize >= maxSize) {
        pImpl->rotate();
    }
}

void FileLogger::setLevel(LogLevel level) {
    // FileLogger ignores level setting - file logs everything
}

LogLevel FileLogger::getLevel() const {
    return LogLevel::Info; // Default level
}

void FileLogger::flush() {
    if (pImpl->file.is_open()) {
        pImpl->file.flush();
    }
}

// Main Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Statistics
    struct Stats {
        uint64_t totalLogs = 0;
        std::unordered_map<LogLevel, uint64_t> logsByLevel;
        std::unordered_map<std::string, uint64_t> logsByCategory;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
        double totalLogTime = 0.0;
        uint64_t logTimeCount = 0;
    };
    
    Stats stats;
    bool perfEnabled = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find(pImpl->loggers.begin(), pImpl->loggers.end(), logger);
    if (it != pImpl->loggers.end()) {
        pImpl->loggers.erase(it);
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

LogLevel Logger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->loggers.empty()) {
        return LogLevel::Info;
    }
    
    // Return minimum level among all loggers
    LogLevel minLevel = pImpl->loggers[0]->getLevel();
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() < minLevel) {
            minLevel = logger->getLevel();
        }
    }
    return minLevel;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function,
                 const std::string& category,
                 const std::string& module) {
    if (!isEnabled(level)) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = getThreadId();
    entry.threadName = getCurrentThreadName();
    entry.category = category;
    entry.module = module;
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->stats.totalLogs++;
        pImpl->stats.logsByLevel[level]++;
        if (!category.empty()) {
            pImpl->stats.logsByCategory[category]++;
        }
        
        if (pImpl->stats.firstLog.time_since_epoch().count() == 0) {
            pImpl->stats.firstLog = entry.timestamp;
        }
        pImpl->stats.lastLog = entry.timestamp;
        
        if (pImpl->perfEnabled) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            pImpl->stats.totalLogTime += duration;
            pImpl->stats.logTimeCount++;
        }
    }
    
    // Log to all loggers
    std::vector<std::shared_ptr<ILogger>> loggersToNotify;
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        loggersToNotify = pImpl->loggers;
    }
    
    for (auto& logger : loggersToNotify) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category,
                   const std::string& module) {
    log(LogLevel::Debug, message, file, line, function, category, module);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category,
                  const std::string& module) {
    log(LogLevel::Info, message, file, line, function, category, module);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category,
                     const std::string& module) {
    log(LogLevel::Warning, message, file, line, function, category, module);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category,
                   const std::string& module) {
    log(LogLevel::Error, message, file, line, function, category, module);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& category,
                      const std::string& module) {
    log(LogLevel::Fatal, message, file, line, function, category, module);
}

void Logger::logWithContext(LogLevel level, const std::string& message,
                            const std::unordered_map<std::string, std::string>& context,
                            const std::string& file, int line, const std::string& function) {
    // Enhanced message with context
    std::ostringstream oss;
    oss << message;
    for (const auto& [key, value] : context) {
        oss << " [" << key << "=" << value << "]";
    }
    
    log(level, oss.str(), file, line, function);
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
    if (logger) {
        clearLoggers();
        addLogger(logger);
    }
}

std::vector<std::shared_ptr<ILogger>> Logger::getLoggers() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loggers;
}

Logger::LogStats Logger::getStats() const {
    LogStats stats;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    stats.totalLogs = pImpl->stats.totalLogs;
    stats.logsByLevel = pImpl->stats.logsByLevel;
    stats.logsByCategory = pImpl->stats.logsByCategory;
    stats.firstLog = pImpl->stats.firstLog;
    stats.lastLog = pImpl->stats.lastLog;
    
    return stats;
}

void Logger::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->perfEnabled = enable;
}

double Logger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->stats.logTimeCount == 0) {
        return 0.0;
    }
    return pImpl->stats.totalLogTime / pImpl->stats.logTimeCount;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line,
                     const std::string& function,
                     const std::string& category,
                     const std::string& module)
    : level_(level), file_(file), line_(line), function_(function),
      category_(category), module_(module) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_, module_);
}

LogStream& LogStream::withContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

LogCategory parseLogCategory(const std::string& categoryStr) {
    static const std::unordered_map<std::string, LogCategory> categoryMap = {
        {"general", LogCategory::GENERAL},
        {"plasticity", LogCategory::PLASTICITY},
        {"development", LogCategory::DEVELOPMENT},
        {"neuromodulation", LogCategory::NEUROMODULATION},
        {"memory", LogCategory::MEMORY},
        {"cognition", LogCategory::COGNITION},
        {"sensory", LogCategory::SENSORY},
        {"motor", LogCategory::MOTOR},
        {"environment", LogCategory::ENVIRONMENT},
        {"experiment", LogCategory::EXPERIMENT},
        {"visualization", LogCategory::VISUALIZATION},
        {"config", LogCategory::CONFIG},
        {"performance", LogCategory::PERFORMANCE}
    };
    
    auto it = categoryMap.find(categoryStr);
    if (it != categoryMap.end()) {
        return it->second;
    }
    
    return LogCategory::GENERAL;
}

} // namespace nlm