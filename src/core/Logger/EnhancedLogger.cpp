#include "EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <regex>
#include <atomic>
#include <shared_mutex>

namespace nlm {

// AsyncLogger implementation
struct AsyncLogger::Impl {
    std::queue<LogEntry> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::vector<std::thread> workerThreads;
    std::atomic<bool> running;
    size_t maxQueueSize;
    
    LogLevel level;
    std::function<bool(const LogEntry&)> filter;
    std::function<std::string(const LogEntry&)> formatter;
    std::function<void(const LogEntry&)> errorHandler;
    
    std::unordered_map<std::string, bool> componentFilters;
    std::string logFile;
    size_t maxFileSize;
    size_t maxFiles;
    bool rotateDaily;
    std::string format;
    
    std::ofstream fileOut;
    std::mutex fileMutex;
    std::chrono::system_clock::time_point lastRotation;
    
    Impl(size_t maxQueueSize, size_t workerThreads)
        : running(true), maxQueueSize(maxQueueSize) {
        for (size_t i = 0; i < workerThreads; ++i) {
            this->workerThreads.emplace_back(&AsyncLogger::Impl::workerThread, this);
        }
    }
    
    ~Impl() {
        running = false;
        queueCond.notify_all();
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
    
    void workerThread() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCond.wait(lock, [this]() {
                return !messageQueue.empty() || !running;
            });
            
            if (!running && messageQueue.empty()) {
                break;
            }
            
            LogEntry entry = std::move(messageQueue.front());
            messageQueue.pop();
            lock.unlock();
            
            processEntry(entry);
        }
    }
    
    void processEntry(const LogEntry& entry) {
        if (filter && !filter(entry)) {
            return;
        }
        
        if (!componentFilters.empty()) {
            bool componentEnabled = true;
            if (!entry.component.empty()) {
                auto it = componentFilters.find(entry.component);
                if (it != componentFilters.end()) {
                    componentEnabled = it->second;
                }
            }
            if (!componentEnabled) {
                return;
            }
        }
        
        std::string formatted;
        if (formatter) {
            formatted = formatter(entry);
        } else {
            formatted = formatEntry(entry);
        }
        
        // Output to console
        std::cout << formatted;
        
        // Output to file
        if (fileOut.is_open()) {
            std::lock_guard<std::mutex> fileLock(fileMutex);
            fileOut << formatted;
            fileOut.flush();
            
            // Check if rotation is needed
            if (maxFileSize > 0 && fileOut.tellp() > static_cast<std::streamsize>(maxFileSize)) {
                rotateFile();
            }
        }
        
        // Handle errors
        if (errorHandler && entry.level >= LogLevel::Error) {
            errorHandler(entry);
        }
    }
    
    void rotateFile() {
        if (!rotateDaily) {
            // Simple file rotation: add number
            size_t pos = logFile.find_last_of('.');
            std::string base = logFile.substr(0, pos);
            std::string ext = logFile.substr(pos);
            
            std::vector<std::string> files;
            for (size_t i = maxFiles - 1; i > 0; --i) {
                std::string numberedFile = base + "_" + std::to_string(i) + ext;
                if (std::filesystem::exists(numberedFile)) {
                    std::filesystem::rename(numberedFile, base + "_" + std::to_string(i + 1) + ext);
                }
            }
            
            std::filesystem::rename(logFile, base + "_1" + ext);
            fileOut.close();
            fileOut.open(logFile, std::ios::app);
        } else {
            // Daily rotation - for simplicity, just close and reopen
            fileOut.close();
            fileOut.open(logFile, std::ios::app);
        }
    }
    
    std::string formatEntry(const LogEntry& entry) const {
        std::ostringstream oss;
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "." 
            << std::setfill('0') << std::setw(3) 
            << (entry.timestamp.time_since_epoch().count() % 1000) << " ";
        
        // Level
        oss << "[" << levelToString(entry.level) << "]";
        
        // Component
        if (!entry.component.empty()) {
            oss << "[" << entry.component << "]";
        }
        
        // Message
        oss << " " << entry.message;
        
        // File and line
        if (!entry.file.empty()) {
            oss << " [" << entry.file << ":" << entry.line << "]";
        }
        
        // Function
        if (!entry.function.empty()) {
            oss << " in " << entry.function;
        }
        
        // Context
        if (!entry.context.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& pair : entry.context) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "}";
        }
        
        // Duration
        if (entry.durationMs > 0) {
            oss << " [" << entry.durationMs << "ms]";
        }
        
        oss << "\n";
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::NeuralDebug: return "N-DBG";
            case LogLevel::PlasticityDebug: return "P-DBG";
            case LogLevel::MemoryDebug: return "M-DBG";
            case LogLevel::Performance: return "PERF";
            case LogLevel::System: return "SYS";
            case LogLevel::UserAction: return "USER";
            case LogLevel::Validation: return "VAL";
            case LogLevel::Security: return "SEC";
            case LogLevel::Network: return "NET";
            case LogLevel::Config: return "CONF";
            case LogLevel::Experiment: return "EXP";
            default: return "UNKNOWN";
        }
    }
};

AsyncLogger::AsyncLogger(size_t maxQueueSize, size_t workerThreads)
    : pImpl(std::make_unique<Impl>(maxQueueSize, workerThreads)) {}

AsyncLogger::~AsyncLogger() = default;

void AsyncLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    if (pImpl->messageQueue.size() >= pImpl->maxQueueSize) {
        // Queue is full, drop the oldest message
        pImpl->messageQueue.pop();
    }
    pImpl->messageQueue.push(entry);
    pImpl->queueCond.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void AsyncLogger::setFormatter(std::function<std::string(const LogEntry&)> formatter) {
    pImpl->formatter = std::move(formatter);
}

void AsyncLogger::setErrorHandler(std::function<void(const LogEntry&)> errorHandler) {
    pImpl->errorHandler = std::move(errorHandler);
}

void AsyncLogger::flush() {
    // For async logger, we wait for the queue to be processed
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCond.wait(lock, [this]() {
        return pImpl->messageQueue.empty();
    });
}

void AsyncLogger::setComponentFilter(const std::string& component, bool enabled) {
    pImpl->componentFilters[component] = enabled;
}

bool AsyncLogger::isComponentEnabled(const std::string& component) const {
    auto it = pImpl->componentFilters.find(component);
    return it == pImpl->componentFilters.end() || it->second;
}

void AsyncLogger::setLogFile(const std::string& filepath, size_t maxFileSize, 
                             size_t maxFiles, bool rotateDaily) {
    pImpl->logFile = filepath;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
    pImpl->rotateDaily = rotateDaily;
    
    if (fileOut.is_open()) {
        fileOut.close();
    }
    fileOut.open(filepath, std::ios::app);
}

void AsyncLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

// EnhancedConsoleLogger implementation
struct EnhancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    bool structuredLogging = false;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeLocation = true;
    bool includeFunction = true;
    bool includeThreadId = false;
    
    std::function<bool(const LogEntry&)> filter;
    std::function<std::string(const LogEntry&)> formatter;
    std::function<void(const LogEntry&)> errorHandler;
    
    std::unordered_map<std::string, bool> componentFilters;
    
    // Sanitization
    std::vector<std::regex> sanitizePatterns;
    std::vector<std::string> redactFields;
    
    // Timing information
    std::unordered_map<std::string, std::chrono::system_clock::time_point> timingStart;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() = default;

void EnhancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (pImpl->filter && !pImpl->filter(entry)) {
        return;
    }
    
    if (!pImpl->componentFilters.empty()) {
        bool componentEnabled = true;
        if (!entry.component.empty()) {
            auto it = pImpl->componentFilters.find(entry.component);
            if (it != pImpl->componentFilters.end()) {
                componentEnabled = it->second;
            }
        }
        if (!componentEnabled) {
            return;
        }
    }
    
    std::string formatted = formatEntry(entry);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formatted << "\033[0m";
    } else {
        std::cout << formatted;
    }
    
    // File output
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formatted;
        pImpl->fileOut.flush();
    }
    
    if (pImpl->errorHandler && entry.level >= LogLevel::Error) {
        pImpl->errorHandler(entry);
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void EnhancedConsoleLogger::setFormatter(std::function<std::string(const LogEntry&)> formatter) {
    pImpl->formatter = std::move(formatter);
}

void EnhancedConsoleLogger::setErrorHandler(std::function<void(const LogEntry&)> errorHandler) {
    pImpl->errorHandler = std::move(errorHandler);
}

void EnhancedConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

void EnhancedConsoleLogger::setComponentFilter(const std::string& component, bool enabled) {
    pImpl->componentFilters[component] = enabled;
}

bool EnhancedConsoleLogger::isComponentEnabled(const std::string& component) const {
    auto it = pImpl->componentFilters.find(component);
    return it == pImpl->componentFilters.end() || it->second;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
}

void EnhancedConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void EnhancedConsoleLogger::enableStructuredLogging(bool enable) {
    pImpl->structuredLogging = enable;
}

void EnhancedConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void EnhancedConsoleLogger::setIncludeLocation(bool include) {
    pImpl->includeLocation = include;
}

void EnhancedConsoleLogger::setIncludeFunction(bool include) {
    pImpl->includeFunction = include;
}

void EnhancedConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

std::string EnhancedConsoleLogger::formatEntry(const LogEntry& entry) const {
    if (pImpl->formatter) {
        return pImpl->formatter(entry);
    }
    
    std::ostringstream oss;
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << "[" << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str()) << "]";
    
    // Level
    oss << "[" << levelToString(entry.level) << "]";
    
    // Component
    if (!entry.component.empty()) {
        oss << "[" << entry.component << "]";
    }
    
    // Message
    oss << " " << entry.message;
    
    // Location
    if (pImpl->includeLocation && !entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    // Function
    if (pImpl->includeFunction && !entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    // Thread ID
    if (pImpl->includeThreadId) {
        oss << " [thread:" << std::this_thread::get_id() << "]";
    }
    
    // Context
    if (!entry.context.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ", ";
            
            // Check if this field should be redacted
            bool redact = false;
            for (const auto& field : pImpl->redactFields) {
                if (pair.first == field) {
                    redact = true;
                    break;
                }
            }
            
            if (redact) {
                oss << pair.first << "=***";
            } else {
                // Sanitize sensitive information
                std::string value = pair.second;
                for (const auto& pattern : pImpl->sanitizePatterns) {
                    value = std::regex_replace(value, pattern, "***");
                }
                oss << pair.first << "=" << value;
            }
            
            first = false;
        }
        oss << "}";
    }
    
    oss << "\n";
    return oss.str();
}

std::string EnhancedConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::NeuralDebug: return "\033[34m";// Blue
        case LogLevel::PlasticityDebug: return "\033[33m"; // Orange
        case LogLevel::MemoryDebug: return "\033[32m"; // Green
        case LogLevel::Performance: return "\033[90m"; // Bright Black
        case LogLevel::System: return "\033[37m";   // White
        case LogLevel::UserAction: return "\033[35m"; // Magenta
        case LogLevel::Validation: return "\033[33m"; // Yellow
        case LogLevel::Security: return "\033[31m";   // Red
        case LogLevel::Network: return "\033[34m";   // Blue
        case LogLevel::Config: return "\033[36m";   // Cyan
        case LogLevel::Experiment: return "\033[32m"; // Green
        default: return "\033[0m";
    }
}

std::string EnhancedConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str());
    return ss.str();
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::shared_mutex mutex;
    
    // Global context
    std::unordered_map<std::string, std::string> globalContext;
    
    // Component-specific levels
    std::unordered_map<std::string, LogLevel> componentLevels;
    
    // Thread-local context
    thread_local static std::unordered_map<std::string, std::string> threadContext;
    
    // Performance timing
    std::unordered_map<std::string, std::chrono::system_clock::time_point> timingStart;
    
    // Log rotation
    size_t maxFileSize = 10485760; // 10MB
    size_t maxFiles = 5;
    bool rotateDaily = true;
    
    // Sanitization
    std::vector<std::regex> sanitizePatterns;
    std::vector<std::string> redactFields;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    // Set default log level for components
    pImpl->componentLevels["brain"] = LogLevel::Warning;
    pImpl->componentLevels["plasticity"] = LogLevel::Info;
    pImpl->componentLevels["memory"] = LogLevel::Info;
    pImpl->componentLevels["sensory"] = LogLevel::Info;
    pImpl->componentLevels["motor"] = LogLevel::Info;
    pImpl->componentLevels["environment"] = LogLevel::Info;
    pImpl->componentLevels["neuromodulation"] = LogLevel::Info;
    pImpl->componentLevels["development"] = LogLevel::Info;
    pImpl->componentLevels["cognition"] = LogLevel::Info;
    pImpl->componentLevels["performance"] = LogLevel::Performance;
    pImpl->componentLevels["system"] = LogLevel::System;
    pImpl->componentLevels["experiment"] = LogLevel::Experiment;
}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::addLogger(std::shared_ptr<ILogger> logger) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    
    // Set the level and component filters for the new logger
    for (const auto& pair : pImpl->componentLevels) {
        logger->setComponentFilter(pair.first, true);
    }
}

void EnhancedLogger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    auto it = std::find(pImpl->loggers.begin(), pImpl->loggers.end(), logger);
    if (it != pImpl->loggers.end()) {
        pImpl->loggers.erase(it);
    }
}

void EnhancedLogger::clearLoggers() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void EnhancedLogger::setLevel(LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void EnhancedLogger::setComponentLevel(const std::string& component, LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->componentLevels[component] = level;
    
    // Update all loggers
    for (auto& logger : pImpl->loggers) {
        logger->setComponentFilter(component, level != LogLevel::Debug);
    }
}

void EnhancedLogger::setGlobalContext(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->globalContext[key] = value;
}

void EnhancedLogger::setContext(const std::string& key, const std::string& value) {
    setGlobalContext(key, value);
}

std::string EnhancedLogger::getContext(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    auto it = pImpl->globalContext.find(key);
    if (it != pImpl->globalContext.end()) {
        return it->second;
    }
    return "";
}

void EnhancedLogger::log(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& component,
                         const std::unordered_map<std::string, std::string>& context) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Merge context
    entry.context = pImpl->globalContext;
    entry.context.insert(context.begin(), context.end());
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logWithDuration(LogLevel level, const std::string& message,
                                    double durationMs,
                                    const std::string& file, int line,
                                    const std::string& function,
                                    const std::string& component) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.durationMs = durationMs;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logError(const std::string& errorCode, const std::string& errorMessage,
                             const std::string& errorCategory,
                             const std::string& stackTrace,
                             const std::string& component) {
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = errorMessage;
    entry.errorCode = errorCode;
    entry.errorCategory = errorCategory;
    entry.stackTrace = stackTrace;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Debug, message, file, line, function, component);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Info, message, file, line, function, component);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Warning, message, file, line, function, component);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Error, message, file, line, function, component);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Critical, message, file, line, function, component);
}

void EnhancedLogger::neuralDebug(const std::string& message, const std::string& component) {
    log(LogLevel::NeuralDebug, message, "", 0, "", component);
}

void EnhancedLogger::plasticityDebug(const std::string& message, const std::string& component) {
    log(LogLevel::PlasticityDebug, message, "", 0, "", component);
}

void EnhancedLogger::memoryDebug(const std::string& message, const std::string& component) {
    log(LogLevel::MemoryDebug, message, "", 0, "", component);
}

void EnhancedLogger::performance(const std::string& message, double durationMs, const std::string& component) {
    logWithDuration(LogLevel::Performance, message, durationMs, "", 0, "", component);
}

void EnhancedLogger::system(const std::string& message, const std::string& component) {
    log(LogLevel::System, message, "", 0, "", component);
}

void EnhancedLogger::userAction(const std::string& message, const std::string& userId, const std::string& component) {
    LogEntry entry;
    entry.level = LogLevel::UserAction;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (!userId.empty()) {
        entry.context["userId"] = userId;
    }
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::validation(const std::string& message, const std::string& component) {
    log(LogLevel::Validation, message, "", 0, "", component);
}

void EnhancedLogger::security(const std::string& message, const std::string& component) {
    log(LogLevel::Security, message, "", 0, "", component);
}

void EnhancedLogger::config(const std::string& message, const std::string& component) {
    log(LogLevel::Config, message, "", 0, "", component);
}

void EnhancedLogger::experiment(const std::string& message, const std::string& experimentId, const std::string& component) {
    LogEntry entry;
    entry.level = LogLevel::Experiment;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (!experimentId.empty()) {
        entry.context["experimentId"] = experimentId;
    }
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

bool EnhancedLogger::isEnabled(LogLevel level) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

bool EnhancedLogger::isEnabledForComponent(const std::string& component, LogLevel level) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->componentLevels.find(component);
    if (it != pImpl->componentLevels.end()) {
        return it->second <= level;
    }
    return false;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void EnhancedLogger::setThreadLocalContext(const std::string& key, const std::string& value) {
    pImpl->threadContext[key] = value;
}

std::string EnhancedLogger::getThreadLocalContext(const std::string& key) const {
    auto it = pImpl->threadContext.find(key);
    if (it != pImpl->threadContext.end()) {
        return it->second;
    }
    return "";
}

void EnhancedLogger::startTiming(const std::string& operationId) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->timingStart[operationId] = std::chrono::system_clock::now();
}

void EnhancedLogger::endTiming(const std::string& operationId, const std::string& category) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timingStart.find(operationId);
    if (it != pImpl->timingStart.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - it->second).count();
        
        std::string message = "Operation " + operationId + " completed";
        if (!category.empty()) {
            message += " (" + category + ")";
        }
        
        performance(message, static_cast<double>(duration), "performance");
        
        pImpl->timingStart.erase(it);
    }
}

void EnhancedLogger::setLogRotation(size_t maxFileSize, size_t maxFiles, bool rotateDaily) {
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
    pImpl->rotateDaily = rotateDaily;
}

void EnhancedLogger::setSanitizePatterns(const std::vector<std::string>& patterns) {
    for (const auto& pattern : patterns) {
        try {
            pImpl->sanitizePatterns.emplace_back(pattern);
        } catch (const std::regex_error&) {
            // Skip invalid patterns
        }
    }
}

void EnhancedLogger::setRedactFields(const std::vector<std::string>& fields) {
    pImpl->redactFields = fields;
}

// Initialize thread-local static member
thread_local std::unordered_map<std::string, std::string> EnhancedLogger::Impl::threadContext;

} // namespace nlm
