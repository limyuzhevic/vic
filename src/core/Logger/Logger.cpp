#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <queue>
#include <condition_variable>
#include <unordered_map>

namespace nlm {

// Forward declarations for async logging
class AsyncLogger;

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) const = 0;
    virtual std::string getName() const = 0;
};

// Simple text formatter
class TextLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) const override {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        if (!entry.file.empty()) {
            oss << " " << entry.file << ":" << entry.line;
            if (!entry.function.empty()) {
                oss << " (" << entry.function << ")";
            }
        }
        
        oss << " " << entry.message;
        
        return oss.str();
    }
    
    std::string getName() const override { return "TextFormatter"; }
};

// JSON formatter
class JsonLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) const override {
        nlohmann::json jsonData;
        jsonData["level"] = static_cast<int>(entry.level);
        jsonData["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
        jsonData["message"] = entry.message;
        jsonData["file"] = entry.file;
        jsonData["line"] = entry.line;
        jsonData["function"] = entry.function;
        
        return jsonData.dump();
    }
    
    std::string getName() const override { return "JsonFormatter"; }
};

// Colored text formatter
class ColoredLogFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) const override {
        std::ostringstream oss;
        oss << levelToColor(entry.level) << "[" << levelToString(entry.level) << "] " << "\033[0m";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message << "\033[0m";
        
        return oss.str();
    }
    
    std::string getName() const override { return "ColoredFormatter"; }
};

// Log filter interface
class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
    virtual std::string getName() const = 0;
};

// Level-based filter
class LevelFilter : public LogFilter {
    LogLevel minLevel;
public:
    LevelFilter(LogLevel level) : minLevel(level) {}
    bool shouldLog(const LogEntry& entry) const override {
        return entry.level >= minLevel;
    }
    std::string getName() const override { return "LevelFilter"; }
};

// String pattern filter
class PatternFilter : public LogFilter {
    std::string pattern;
    bool caseInsensitive;
public:
    PatternFilter(const std::string& pat, bool insensitive = false) 
        : pattern(pat), caseInsensitive(insensitive) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        std::string message = entry.message;
        if (caseInsensitive) {
            std::transform(message.begin(), message.end(), message.begin(), ::tolower);
            std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
        }
        return message.find(pattern) != std::string::npos;
    }
    
    std::string getName() const override { return "PatternFilter"; }
};

// Async logger with thread pool
class AsyncLogger {
public:
    struct LogTask {
        LogEntry entry;
        std::shared_ptr<LogFormatter> formatter;
        std::vector<std::shared_ptr<LogFilter>> filters;
    };
    
    AsyncLogger(size_t numThreads = std::thread::hardware_concurrency())
        : running(true), threadCount(numThreads) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back(&AsyncLogger::workerThread, this);
        }
    }
    
    ~AsyncLogger() {
        shutdown();
    }
    
    void addFormatter(std::shared_ptr<LogFormatter> formatter) {
        std::lock_guard<std::mutex> lock(queueMutex);
        formatters.push_back(formatter);
    }
    
    void addFilter(std::shared_ptr<LogFilter> filter) {
        std::lock_guard<std::mutex> lock(queueMutex);
        filters.push_back(filter);
    }
    
    void log(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        // Check filters
        bool shouldLog = true;
        for (const auto& filter : filters) {
            if (!filter->shouldLog(entry)) {
                shouldLog = false;
                break;
            }
        }
        
        if (!shouldLog) return;
        
        // Create log task
        LogTask task{entry, nullptr, filters};
        taskQueue.push(task);
        queueCondition.notify_one();
    }
    
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            running = false;
        }
        queueCondition.notify_all();
        
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers.clear();
    }
    
    bool isRunning() const { return running; }
    
private:
    void workerThread() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this]() { return !taskQueue.empty() || !running; });
            
            if (!running) break;
            
            LogTask task = taskQueue.front();
            taskQueue.pop();
            lock.unlock();
            
            // Process task
            for (const auto& formatter : formatters) {
                std::string formatted = formatter->format(task.entry);
                outputFormattedMessage(formatted, task.entry.level);
            }
        }
    }
    
    void outputFormattedMessage(const std::string& message, LogLevel level) {
        // Use the main logger's default console output
        std::cout << levelToColor(level) << message << "\033[0m\n";
    }
    
    std::atomic<bool> running;
    size_t threadCount;
    std::vector<std::thread> workers;
    std::queue<LogTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::shared_ptr<LogFormatter>> formatters;
    std::vector<std::shared_ptr<LogFilter>> filters;
};

// Log rotation handler
class LogRotator {
public:
    struct RotationConfig {
        size_t maxFileSize;
        size_t maxFiles;
        std::string backupExtension;
    };
    
    LogRotator(const RotationConfig& config) : config(config) {}
    
    bool shouldRotate(const std::string& filepath) const {
        std::error_code ec;
        auto fileSize = std::filesystem::file_size(filepath, ec);
        if (ec) return false;
        return fileSize >= config.maxFileSize;
    }
    
    std::string getRotatedFilename(const std::string& filepath, size_t index) const {
        std::string filename = filepath;
        size_t dotPos = filename.find_last_of('.');
        if (dotPos != std::string::npos) {
            return filename.substr(0, dotPos) + "." + config.backupExtension + 
                   (index > 0 ? std::to_string(index) : "") + 
                   filename.substr(dotPos);
        }
        return filename + "." + config.backupExtension + (index > 0 ? std::to_string(index) : "");
    }
    
    void rotate(const std::string& filepath) {
        std::error_code ec;
        if (!std::filesystem::exists(filepath, ec)) return;
        
        // Remove oldest file if too many backups
        for (size_t i = config.maxFiles; i > 0; --i) {
            std::string oldFile = getRotatedFilename(filepath, i - 1);
            if (std::filesystem::exists(oldFile, ec)) {
                std::filesystem::remove(oldFile, ec);
            }
        }
        
        // Rotate current file
        for (size_t i = config.maxFiles; i > 0; --i) {
            std::string src = getRotatedFilename(filepath, i - 1);
            std::string dst = getRotatedFilename(filepath, i);
            if (std::filesystem::exists(src, ec)) {
                std::filesystem::rename(src, dst, ec);
            }
        }
        
        // Move current file to backup
        std::string backupFile = getRotatedFilename(filepath, 0);
        std::filesystem::rename(filepath, backupFile, ec);
    }
    
private:
    RotationConfig config;
};

// Enhanced ConsoleLogger with advanced features
class AdvancedConsoleLogger : public ILogger {
public:
    struct Impl {
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::ofstream fileOut;
        std::mutex mutex;
        
        // Advanced features
        std::shared_ptr<LogFormatter> formatter;
        std::vector<std::shared_ptr<LogFilter>> filters;
        std::shared_ptr<AsyncLogger> asyncLogger;
        std::shared_ptr<LogRotator> rotator;
        LogRotator::RotationConfig rotationConfig;
        
        // Statistics
        std::atomic<size_t> totalLogs{0};
        std::atomic<size_t> filteredLogs{0};
        std::chrono::steady_clock::time_point lastRotation;
    };
    
    AdvancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {
        // Initialize with default formatter and filters
        pImpl->formatter = std::make_shared<TextLogFormatter>();
        pImpl->rotationConfig = {10 * 1024 * 1024, 5, "bak"}; // 10MB max, 5 backups
        pImpl->lastRotation = std::chrono::steady_clock::now();
    }
    
    AdvancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
        pImpl->level = level;
        pImpl->formatter = std::make_shared<TextLogFormatter>();
        pImpl->rotationConfig = {10 * 1024 * 1024, 5, "bak"};
        pImpl->lastRotation = std::chrono::steady_clock::now();
    }
    
    ~AdvancedConsoleLogger() = default;
    
    void log(const LogEntry& entry) override {
        if (entry.level < pImpl->level) {
            ++pImpl->filteredLogs;
            return;
        }
        
        // Check filters
        bool shouldLog = true;
        for (const auto& filter : pImpl->filters) {
            if (!filter->shouldLog(entry)) {
                shouldLog = false;
                break;
            }
        }
        
        if (!shouldLog) {
            ++pImpl->filteredLogs;
            return;
        }
        
        // Check rotation
        checkAndRotateFile(entry);
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::string formatted;
        if (pImpl->asyncLogger && pImpl->asyncLogger->isRunning()) {
            pImpl->asyncLogger->log(entry);
        } else {
            formatted = pImpl->formatter->format(entry);
            
            // Console output
            if (pImpl->useColors) {
                std::cout << levelToColor(entry.level) << formatted << "\033[0m\n";
            } else {
                std::cout << formatted << "\n";
            }
            
            // File output if set
            if (pImpl->fileOut.is_open()) {
                pImpl->fileOut << formatted << "\n";
                pImpl->fileOut.flush();
            }
        }
        
        ++pImpl->totalLogs;
    }
    
    void setLevel(LogLevel level) override {
        pImpl->level = level;
    }
    
    LogLevel getLevel() const override {
        return pImpl->level;
    }
    
    void setUseColors(bool useColors) override {
        pImpl->useColors = useColors;
    }
    
    bool getUseColors() const override {
        return pImpl->useColors;
    }
    
    void setOutputFile(const std::string& filepath) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->fileOut.open(filepath, std::ios::app);
    }
    
    void closeOutputFile() override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
    }
    
    // Advanced configuration methods
    void setFormatter(std::shared_ptr<LogFormatter> formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->formatter = formatter;
    }
    
    void addFilter(std::shared_ptr<LogFilter> filter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->filters.push_back(filter);
    }
    
    void enableAsyncLogging(size_t numThreads = std::thread::hardware_concurrency()) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->asyncLogger = std::make_shared<AsyncLogger>(numThreads);
        pImpl->asyncLogger->addFormatter(pImpl->formatter);
        for (const auto& filter : pImpl->filters) {
            pImpl->asyncLogger->addFilter(filter);
        }
    }
    
    void disableAsyncLogging() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (pImpl->asyncLogger) {
            pImpl->asyncLogger->shutdown();
            pImpl->asyncLogger.reset();
        }
    }
    
    void configureRotation(size_t maxFileSize, size_t maxFiles, const std::string& extension) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->rotationConfig = {maxFileSize, maxFiles, extension};
        pImpl->rotator = std::make_shared<LogRotator>(pImpl->rotationConfig);
    }
    
    void logStatistics() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::cout << "Logger Statistics:" << std::endl;
        std::cout << "  Total logs: " << pImpl->totalLogs << std::endl;
        std::cout << "  Filtered logs: " << pImpl->filteredLogs << std::endl;
        std::cout << "  Active filters: " << pImpl->filters.size() << std::endl;
        std::cout << "  Formatter: " << pImpl->formatter->getName() << std::endl;
        std::cout << "  Async logging: " << (pImpl->asyncLogger ? "enabled" : "disabled") << std::endl;
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    void checkAndRotateFile(const LogEntry& entry) {
        if (!pImpl->rotator || pImpl->fileOut.is_open()) return;
        
        auto now = std::chrono::steady_clock::now();
        auto timeSinceRotation = std::chrono::duration_cast<std::chrono::minutes>(now - pImpl->lastRotation);
        
        if (timeSinceRotation.count() >= 60) { // Rotate hourly
            pImpl->rotator->rotate("nlm.log");
            pImpl->lastRotation = now;
        }
    }
    
private:
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Logger with advanced features
class EnhancedLogger : public Logger {
public:
    struct Impl {
        std::vector<std::shared_ptr<ILogger>> loggers;
        std::mutex mutex;
        std::vector<std::shared_ptr<AdvancedConsoleLogger>> advancedLoggers;
        
        // Log levels for different components
        std::unordered_map<std::string, LogLevel> componentLevels;
    };
    
    EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
        // Initialize with default console logger
        auto defaultLogger = std::make_shared<AdvancedConsoleLogger>(LogLevel::Info);
        pImpl->advancedLoggers.push_back(defaultLogger);
        addLogger(defaultLogger);
    }
    
    ~EnhancedLogger() = default;
    
    void addLogger(std::shared_ptr<ILogger> logger) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        loggers.push_back(logger);
    }
    
    void clearLoggers() override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        loggers.clear();
        pImpl->advancedLoggers.clear();
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : loggers) {
            logger->setLevel(level);
        }
        for (auto& advLogger : pImpl->advancedLoggers) {
            advLogger->setLevel(level);
        }
    }
    
    void log(LogLevel level, const std::string& message,
             const std::string& file, int line, const std::string& function) override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Determine appropriate logger based on component
        std::string component = file.empty() ? "Unknown" : file;
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        // Log to all loggers
        for (auto& logger : loggers) {
            logger->log(entry);
        }
    }
    
    void setComponentLevel(const std::string& component, LogLevel level) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->componentLevels[component] = level;
    }
    
    void enableAdvancedFeatures() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Create advanced logger
        auto advancedLogger = std::make_shared<AdvancedConsoleLogger>(LogLevel::Info);
        
        // Add advanced formatter
        auto coloredFormatter = std::make_shared<ColoredLogFormatter>();
        advancedLogger->setFormatter(coloredFormatter);
        
        // Add pattern filters
        auto debugFilter = std::make_shared<PatternFilter>("DEBUG");
        auto errorFilter = std::make_shared<PatternFilter>("ERROR", true);
        
        advancedLogger->addFilter(debugFilter);
        advancedLogger->addFilter(errorFilter);
        
        // Enable async logging
        advancedLogger->enableAsyncLogging();
        
        // Configure rotation
        advancedLogger->configureRotation(10 * 1024 * 1024, 5, "bak"); // 10MB max, 5 backups
        
        pImpl->advancedLoggers.push_back(advancedLogger);
        addLogger(advancedLogger);
    }
    
    void logStatistics() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (const auto& advLogger : pImpl->advancedLoggers) {
            advLogger->logStatistics();
        }
    }
    
    bool isEnabled(LogLevel level) const override {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : loggers) {
            if (logger->getLevel() <= level) {
                return true;
            }
        }
        return false;
    }
    
    Logger& getGlobal() override {
        static EnhancedLogger globalLogger;
        return globalLogger;
    }
    
    void setGlobal(std::shared_ptr<Logger> logger) override {
        // Enhanced global logger management
        static std::shared_ptr<Logger> globalLogger = logger;
    }
    
private:
    std::unique_ptr<Impl> pImpl;
};

// Global logger instance
std::shared_ptr<Logger> globalLogger = std::make_shared<EnhancedLogger>();

void setGlobal(std::shared_ptr<Logger> logger) {
    globalLogger = logger;
}

Logger& getGlobal() {
    return *globalLogger;
}

} // namespace nlm