#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <regex>
#include <cmath>
#include <ctime>

namespace nlm {

// Global statistics
static LogStatistics globalStatistics;
static std::mutex globalMutex;
static bool researchModeEnabled = false;

// File rotation support
struct FileRotation {
    std::ofstream file;
    std::string filepath;
    size_t maxSize;
    size_t currentSize;
    size_t fileNumber;
    std::chrono::system_clock::time_point lastWrite;
    
    FileRotation(const std::string& path, size_t maxSize) 
        : filepath(path), maxSize(maxSize), currentSize(0), fileNumber(1), lastWrite(std::chrono::system_clock::now()) {
        rotate();
    }
    
    void rotate() {
        if (file.is_open()) {
            file.close();
        }
        
        std::string rotatedPath = filepath;
        if (fileNumber > 1) {
            size_t dotPos = filepath.find_last_of('.');
            if (dotPos != std::string::npos) {
                rotatedPath = filepath.substr(0, dotPos) + "_" + 
                             std::to_string(fileNumber) + filepath.substr(dotPos);
            } else {
                rotatedPath = filepath + "_" + std::to_string(fileNumber);
            }
        }
        
        file.open(rotatedPath, std::ios::app);
        currentSize = file.tellp();
        fileNumber++;
    }
    
    bool write(const std::string& data) {
        if (!file.is_open()) {
            return false;
        }
        
        currentSize += data.size();
        file << data;
        file.flush();
        lastWrite = std::chrono::system_clock::now();
        
        // Check if rotation is needed
        if (currentSize >= maxSize) {
            rotate();
        }
        
        return true;
    }
    
    void close() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    bool isOpen() const {
        return file.is_open();
    }
};

// ConsoleLogger implementation with enhanced features
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::mutex mutex;
    
    // File rotation
    std::unique_ptr<FileRotation> fileRotation;
    bool enableRotation = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    
    // Filtering
    std::vector<std::function<bool(const LogEntry&)>> filters;
    std::string sourceFilterPattern;
    std::string moduleFilter;
    
    // Statistics
    LogStatistics statistics;
    
    // Structured logging
    bool enableStructuredLogging = false;
    
    // Performance tracking
    bool enablePerformanceTracking = false;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    std::lock_guard<std::mutex> lock(globalMutex);
    globalStatistics.startTime = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    std::lock_guard<std::mutex> lock(globalMutex);
    globalStatistics.startTime = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileRotation) {
        pImpl->fileRotation->close();
    }
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Apply filters
    for (const auto& filter : pImpl->filters) {
        if (!filter(entry)) {
            return false;
        }
    }
    
    // Source pattern filtering
    if (!pImpl->sourceFilterPattern.empty()) {
        std::regex pattern(pImpl->sourceFilterPattern, std::regex::icase);
        if (!std::regex_search(entry.file, pattern) && 
            !std::regex_search(entry.function, pattern)) {
            return false;
        }
    }
    
    // Module filtering
    if (!pImpl->moduleFilter.empty() && entry.sourceModule != pImpl->moduleFilter) {
        return false;
    }
    
    return true;
}

std::string ConsoleLogger::formatStructured(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\",\n";
    oss << "  \"level\": \"" << levelToString(entry.level) << "\",\n";
    oss << "  \"message\": \"" << entry.message << "\",\n";
    oss << "  \"file\": \"" << entry.file << "\",\n";
    oss << "  \"line\": " << entry.line << ",\n";
    oss << "  \"function\": \"" << entry.function << "\",\n";
    oss << "  \"source_module\": \"" << entry.sourceModule << "\",\n";
    
    if (!entry.experimentId.empty()) {
        oss << "  \"experiment_id\": \"" << entry.experimentId << "\",\n";
    }
    
    if (!entry.fields.empty()) {
        oss << "  \"fields\": {\n";
        bool first = true;
        for (const auto& pair : entry.fields) {
            if (!first) oss << ",\n";
            oss << "    \"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        oss << "\n  },\n";
    }
    
    if (!entry.numericFields.empty()) {
        oss << "  \"numeric_fields\": {\n";
        bool first = true;
        for (const auto& pair : entry.numericFields) {
            if (!first) oss << ",\n";
            oss << "    \"" << pair.first << "\": " << pair.second";
            first = false;
        }
        oss << "\n  },\n";
    }
    
    if (!entry.boolFields.empty()) {
        oss << "  \"bool_fields\": {\n";
        bool first = true;
        for (const auto& pair : entry.boolFields) {
            if (!first) oss << ",\n";
            oss << "    \"" << pair.first << "\": " << (pair.second ? "true" : "false")";
            first = false;
        }
        oss << "\n  },\n";
    }
    
    if (entry.executionTime > 0.0) {
        oss << "  \"execution_time_ms\": " << entry.executionTime << ",\n";
    }
    
    if (entry.memoryUsage > 0) {
        oss << "  \"memory_usage_bytes\": " << entry.memoryUsage << ",\n";
    }
    
    oss << "}\n";
    return oss.str();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update statistics
    pImpl->statistics.totalLogs++;
    switch (entry.level) {
        case LogLevel::Trace: pImpl->statistics.debugLogs++; break;
        case LogLevel::Debug: pImpl->statistics.debugLogs++; break;
        case LogLevel::Info: pImpl->statistics.infoLogs++; break;
        case LogLevel::Warning: pImpl->statistics.warningLogs++; break;
        case LogLevel::Error: pImpl->statistics.errorLogs++; break;
        case LogLevel::Critical: pImpl->statistics.criticalLogs++; break;
        case LogLevel::Fatal: pImpl->statistics.criticalLogs++; break;
    }
    
    // Update global statistics
    {
        std::lock_guard<std::mutex> globalLock(globalMutex);
        globalStatistics.totalLogs++;
        switch (entry.level) {
            case LogLevel::Trace: globalStatistics.debugLogs++; break;
            case LogLevel::Debug: globalStatistics.debugLogs++; break;
            case LogLevel::Info: globalStatistics.infoLogs++; break;
            case LogLevel::Warning: globalStatistics.warningLogs++; break;
            case LogLevel::Error: globalStatistics.errorLogs++; break;
            case LogLevel::Critical: globalStatistics.criticalLogs++; break;
            case LogLevel::Fatal: globalStatistics.criticalLogs++; break;
        }
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    // Add source module if present
    if (!entry.sourceModule.empty()) {
        oss << " [Module:" << entry.sourceModule << "]";
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (pImpl->fileRotation && pImpl->fileRotation->isOpen()) {
        std::string logOutput = output;
        if (pImpl->enableStructuredLogging) {
            logOutput = formatStructured(entry);
        }
        pImpl->fileRotation->write(logOutput + "\n");
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (filepath.empty()) {
        if (pImpl->fileRotation) {
            pImpl->fileRotation->close();
            pImpl->fileRotation.reset();
        }
        return;
    }
    
    if (!pImpl->enableRotation) {
        pImpl->fileRotation = std::make_unique<FileRotation>(filepath, pImpl->maxFileSize);
    } else {
        pImpl->fileRotation = std::make_unique<FileRotation>(filepath, pImpl->maxFileSize);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileRotation) {
        pImpl->fileRotation->close();
    }
}

void ConsoleLogger::setFileRotation(size_t maxFileSize, size_t maxFiles) {
    pImpl->enableRotation = true;
    pImpl->maxFileSize = maxFileSize;
    if (pImpl->fileRotation) {
        pImpl->fileRotation->maxSize = maxFileSize;
    }
}

void ConsoleLogger::enableFileRotation(bool enable) {
    pImpl->enableRotation = enable;
}

void ConsoleLogger::addFilter(const std::function<bool(const LogEntry&)>& filter) {
    pImpl->filters.push_back(filter);
}

void ConsoleLogger::clearFilters() {
    pImpl->filters.clear();
}

LogStatistics ConsoleLogger::getStatistics() const {
    return pImpl->statistics;
}

void ConsoleLogger::resetStatistics() {
    pImpl->statistics.reset();
}

void ConsoleLogger::setSourceFilter(const std::string& pattern) {
    pImpl->sourceFilterPattern = pattern;
}

void ConsoleLogger::setModuleFilter(const std::string& module) {
    pImpl->moduleFilter = module;
}

void ConsoleLogger::logStructured(LogLevel level, const std::string& message,
                                  const std::unordered_map<std::string, std::string>& fields) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.fields = fields;
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void ConsoleLogger::logWithMetrics(LogLevel level, const std::string& message,
                                   double executionTime, size_t memoryUsage) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.executionTime = executionTime;
    entry.memoryUsage = memoryUsage;
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[37m";    // White
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Fatal: return "\033[41m";     // Red background
        default: return "\033[0m";
    }
}

// Global Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<Logger> globalLogger;
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
                const std::string& file, int line, const std::string& function,
                const std::string& sourceModule) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.sourceModule = sourceModule;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Trace, message, file, line, function, sourceModule);
}

void Logger::debug(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Debug, message, file, line, function, sourceModule);
}

void Logger::info(const std::string& message, const std::string& file, int line, 
                  const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Info, message, file, line, function, sourceModule);
}

void Logger::warning(const std::string& message, const std::string& file, int line, 
                     const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Warning, message, file, line, function, sourceModule);
}

void Logger::error(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Error, message, file, line, function, sourceModule);
}

void Logger::critical(const std::string& message, const std::string& file, int line, 
                      const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Critical, message, file, line, function, sourceModule);
}

void Logger::fatal(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& sourceModule) {
    log(LogLevel::Fatal, message, file, line, function, sourceModule);
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
    std::lock_guard<std::mutex> lock(globalMutex);
    // Store reference to global logger
}

LogStatistics Logger::getGlobalStatistics() {
    std::lock_guard<std::mutex> lock(globalMutex);
    return globalStatistics;
}

void Logger::resetGlobalStatistics() {
    std::lock_guard<std::mutex> lock(globalMutex);
    globalStatistics.reset();
}

double Logger::measureExecutionTime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void Logger::logStructured(LogLevel level, const std::string& message,
                          const std::unordered_map<std::string, std::string>& fields,
                          const std::string& sourceModule) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.fields = fields;
    entry.sourceModule = sourceModule;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->logStructured(level, message, fields);
    }
}

void Logger::enableResearchMode(bool enable) {
    researchModeEnabled = enable;
}

bool Logger::isResearchModeEnabled() {
    return researchModeEnabled;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, 
                     const std::string& function, const std::string& sourceModule)
    : level_(level), file_(file), line_(line), function_(function), sourceModule_(sourceModule) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

LogStream::~LogStream() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
    
    std::string message = oss_.str();
    
    // Extract structured fields if any
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, double> numericFields;
    std::unordered_map<std::string, bool> boolFields;
    
    // Parse structured fields (simple key=value parsing)
    size_t pos = 0;
    while (pos < message.size()) {
        size_t equalPos = message.find('=', pos);
        if (equalPos == std::string::npos) break;
        
        std::string key = message.substr(pos, equalPos - pos);
        size_t valueStart = equalPos + 1;
        size_t valueEnd = message.find(' ', valueStart);
        if (valueEnd == std::string::npos) valueEnd = message.size();
        
        std::string value = message.substr(valueStart, valueEnd - valueStart);
        
        // Try to convert to numeric
        try {
            size_t numEnd;
            double numVal = std::stod(value, &numEnd);
            if (numEnd == value.size()) {
                numericFields[key] = numVal;
            } else {
                fields[key] = value;
            }
        } catch (...) {
            fields[key] = value;
        }
        
        pos = valueEnd + 1;
        while (pos < message.size() && std::isspace(message[pos])) {
            pos++;
        }
    }
    
    // Log with metrics
    Logger::getGlobal().logWithMetrics(level_, message, duration);
}

LogStream& LogStream::field(const std::string& key, const std::string& value) {
    return *this;
}

LogStream& LogStream::field(const std::string& key, double value) {
    return *this;
}

LogStream& LogStream::field(const std::string& key, int value) {
    return *this;
}

LogStream& LogStream::field(const std::string& key, bool value) {
    return *this;
}

} // namespace nlm
