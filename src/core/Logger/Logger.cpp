#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <thread>
#include <unordered_map>
#include <filesystem>

namespace nlm {

// Helper functions
namespace {
    LogLevel stringToLevel(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "debug") return LogLevel::Debug;
        if (lower == "info") return LogLevel::Info;
        if (lower == "warn" || lower == "warning") return LogLevel::Warning;
        if (lower == "error") return LogLevel::Error;
        if (lower == "crit" || lower == "critical") return LogLevel::Critical;
        return LogLevel::Info; // Default
    }
    
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool coloredOutput = true;
    bool performanceTimestamps = false;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeThreadId = false;
    bool includeProcessId = false;
    std::ofstream fileOut;
    std::mutex mutex;
    LogFilter filter;
    std::string format = "[{level}] {timestamp} {threadId}{processId}{message}";
    std::function<void(const std::string&)> errorHandler;
    std::chrono::system_clock::time_point lastLogTime;
    std::string lastLevel;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastLogTime = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->lastLogTime = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check level filter
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply log filter
    if (!pImpl->filter.matches(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Process performance timing
    if (pImpl->performanceTimestamps) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - pImpl->lastLogTime);
        if (duration.count() > 1000) { // Log performance every 1ms
            // Log performance entry
            LogEntry perfEntry;
            perfEntry.level = LogLevel::Debug;
            perfEntry.message = "PERFORMANCE: " + std::to_string(duration.count()) + " us since last log";
            perfEntry.brainId = entry.brainId;
            perfEntry.timestamp = now;
            outputToConsole(formatEntry(perfEntry));
            pImpl->lastLogTime = now;
        }
    }
    
    std::string formattedMessage = formatEntry(entry);
    outputToConsole(formattedMessage);
    
    // Output to file if set
    if (pImpl->fileOut.is_open()) {
        outputToFile(formattedMessage);
    }
    
    // Handle error if needed
    if (pImpl->errorHandler && entry.level >= LogLevel::Error) {
        pImpl->errorHandler(formattedMessage);
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

void ConsoleLogger::setColoredOutput(bool colored) {
    pImpl->coloredOutput = colored;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        if (pImpl->errorHandler) {
            pImpl->errorHandler("Failed to open log file: " + filepath);
        }
    }
}

void ConsoleLogger::appendOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isOutputFileOpen() const {
    return pImpl->fileOut.is_open();
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void ConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

void ConsoleLogger::setIncludeProcessId(bool include) {
    pImpl->includeProcessId = include;
}

void ConsoleLogger::enablePerformanceTimestamps(bool enable) {
    pImpl->performanceTimestamps = enable;
}

bool ConsoleLogger::isPerformanceTimestampsEnabled() const {
    return pImpl->performanceTimestamps;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    pImpl->filter = filter;
}

LogFilter ConsoleLogger::getFilter() const {
    return pImpl->filter;
}

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setErrorHandler(std::function<void(const std::string&)> handler) {
    pImpl->errorHandler = handler;
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Start with level
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp if enabled
    if (!pImpl->timestampFormat.empty()) {
        oss << formatTimestamp(entry.timestamp, pImpl->timestampFormat) << " ";
    }
    
    // Add thread ID if enabled
    if (pImpl->includeThreadId) {
        oss << "[T" << entry.threadId << "] ";
    }
    
    // Add process ID if enabled
    if (pImpl->includeProcessId) {
        oss << "[P" << getpid() << "] ";
    }
    
    // Add message
    oss << entry.message;
    
    return oss.str();
}

void ConsoleLogger::outputToConsole(const std::string& message) {
    if (pImpl->coloredOutput && pImpl->useColors) {
        // Try to determine log level from message for coloring
        LogLevel level = LogLevel::Info;
        if (message.find("[DEBUG]") != std::string::npos) level = LogLevel::Debug;
        else if (message.find("[INFO]") != std::string::npos) level = LogLevel::Info;
        else if (message.find("[WARN]") != std::string::npos) level = LogLevel::Warning;
        else if (message.find("[ERROR]") != std::string::npos) level = LogLevel::Error;
        else if (message.find("[CRIT]") != std::string::npos) level = LogLevel::Critical;
        
        std::cout << levelToColor(level) << message << "\033[0m\n";
    } else {
        std::cout << message << "\n";
    }
}

void ConsoleLogger::outputToFile(const std::string& message) {
    pImpl->fileOut << message << "\n";
    pImpl->fileOut.flush();
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::string filepath;
    size_t rotationSize = 10 * 1024 * 1024; // 10MB
    size_t rotationCount = 5;
    bool compressionEnabled = false;
    std::chrono::milliseconds flushInterval = std::chrono::milliseconds(1000);
    std::chrono::system_clock::time_point lastFlush;
    std::mutex mutex;
    LogFilter filter;
    std::string format = "[{level}] {timestamp} {message}\n";
    std::ofstream currentFile;
    std::string currentFilename;
    size_t currentFileSize = 0;
};

FileLogger::FileLogger(const std::string& filepath) {
    pImpl = std::make_unique<Impl>();
    pImpl->filepath = filepath;
    pImpl->lastFlush = std::chrono::system_clock::now();
    rotateNow();
}

FileLogger::~FileLogger() {
    if (pImpl->currentFile.is_open()) {
        pImpl->currentFile.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!pImpl->filter.matches(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if rotation is needed
    if (pImpl->currentFileSize >= pImpl->rotationSize) {
        rotateNow();
    }
    
    // Format and write message
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    if (!pImpl->format.empty()) {
        // Apply format (simplified)
        oss << entry.message << "\n";
    } else {
        oss << entry.message << "\n";
    }
    
    std::string message = oss.str();
    pImpl->currentFile << message;
    pImpl->currentFileSize += message.size();
    pImpl->currentFile.flush();
    
    // Check if flush interval exceeded
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastFlush).count() >= 
        pImpl->flushInterval.count()) {
        pImpl->currentFile.flush();
        pImpl->lastFlush = now;
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setRotationSize(size_t maxBytes) {
    pImpl->rotationSize = maxBytes;
}

void FileLogger::setRotationCount(size_t maxFiles) {
    pImpl->rotationCount = maxFiles;
}

void FileLogger::enableCompression(bool enable) {
    pImpl->compressionEnabled = enable;
}

void FileLogger::setFlushInterval(std::chrono::milliseconds interval) {
    pImpl->flushInterval = interval;
}

void FileLogger::setFilter(const LogFilter& filter) {
    pImpl->filter = filter;
}

LogFilter FileLogger::getFilter() const {
    return pImpl->filter;
}

void FileLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string FileLogger::getFormat() const {
    return pImpl->format;
}

void FileLogger::rotateNow() {
    if (pImpl->currentFile.is_open()) {
        pImpl->currentFile.close();
    }
    
    // Generate filename with rotation index
    std::ostringstream oss;
    oss << pImpl->filepath;
    
    // For now, just append a number for rotation
    static int rotationIndex = 0;
    if (rotationIndex > 0) {
        oss << "." << rotationIndex;
    }
    
    pImpl->currentFilename = oss.str();
    pImpl->currentFile.open(pImpl->currentFilename, std::ios::app);
    pImpl->currentFileSize = pImpl->currentFile.is_open() ? 
        std::filesystem::file_size(pImpl->currentFilename) : 0;
    
    if (!pImpl->currentFile.is_open() && pImpl->currentFileSize == 0) {
        // Create new file if it doesn't exist
        pImpl->currentFile.open(pImpl->currentFilename, std::ios::out);
        pImpl->currentFileSize = 0;
    }
    
    rotationIndex++;
    if (rotationIndex > pImpl->rotationCount) {
        rotationIndex = 1;
    }
}

std::vector<std::string> FileLogger::getLogFiles() const {
    std::vector<std::string> files;
    // In a real implementation, this would scan the directory for log files
    return files;
}

void FileLogger::clearCurrentFile() {
    if (pImpl->currentFile.is_open()) {
        pImpl->currentFile.close();
    }
    std::filesystem::remove(pImpl->currentFilename);
    pImpl->currentFileSize = 0;
    pImpl->currentFile.open(pImpl->currentFilename, std::ios::out);
}

// PerformanceLogger implementation
struct PerformanceLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeTimers;
    std::vector<std::pair<std::string, std::chrono::microseconds>> completedTimings;
    std::chrono::microseconds slowOperationThreshold = std::chrono::microseconds(1000000); // 1 second
    bool thresholdLogging = true;
    LogFilter filter;
    std::string format = "[PERF] {operation}: {duration} us";
    std::mutex mutex;
};

PerformanceLogger::PerformanceLogger() {
    pImpl = std::make_unique<Impl>();
}

PerformanceLogger::~PerformanceLogger() = default;

void PerformanceLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!pImpl->filter.matches(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if this is a timing completion entry
    if (entry.structuredData.find("operation") != entry.structuredData.end() && 
        entry.structuredData.find("duration") != entry.structuredData.end()) {
        
        std::string operation = entry.structuredData.at("operation");
        std::chrono::microseconds duration(std::stoll(entry.structuredData.at("duration")));
        
        pImpl->completedTimings.push_back({operation, duration});
        
        // Check threshold
        if (pImpl->thresholdLogging && duration > pImpl->slowOperationThreshold) {
            std::cout << "[SLOW] " << operation << ": " << duration.count() << " us (threshold exceeded)" << std::endl;
        }
        
        // Log the timing
        std::string message = formatPerformanceEntry(operation, duration);
        std::cout << message << std::endl;
    }
}

void PerformanceLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel PerformanceLogger::getLevel() const {
    return pImpl->level;
}

void PerformanceLogger::startTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->activeTimers[operation] = std::chrono::system_clock::now();
}

void PerformanceLogger::endTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(operation);
    if (it != pImpl->activeTimers.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - it->second);
        pImpl->completedTimings.push_back({operation, duration});
        pImpl->activeTimers.erase(it);
        
        // Log if it exceeded threshold
        if (pImpl->thresholdLogging && duration > pImpl->slowOperationThreshold) {
            std::cout << "[SLOW] " << operation << ": " << duration.count() << " us (threshold exceeded)" << std::endl;
        }
    }
}

void PerformanceLogger::logTiming(const std::string& operation, std::chrono::microseconds duration) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->completedTimings.push_back({operation, duration});
    
    if (pImpl->thresholdLogging && duration > pImpl->slowOperationThreshold) {
        std::cout << "[SLOW] " << operation << ": " << duration.count() << " us (threshold exceeded)" << std::endl;
    }
}

void PerformanceLogger::setSlowOperationThreshold(std::chrono::microseconds threshold) {
    pImpl->slowOperationThreshold = threshold;
}

void PerformanceLogger::enableThresholdLogging(bool enable) {
    pImpl->thresholdLogging = enable;
}

void PerformanceLogger::setFilter(const LogFilter& filter) {
    pImpl->filter = filter;
}

LogFilter PerformanceLogger::getFilter() const {
    return pImpl->filter;
}

void PerformanceLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string PerformanceLogger::getFormat() const {
    return pImpl->format;
}

std::string PerformanceLogger::formatPerformanceEntry(const std::string& operation, std::chrono::microseconds duration) const {
    std::ostringstream oss;
    oss << "[PERF] " << operation << ": " << duration.count() << " us";
    return oss.str();
}

std::string PerformanceLogger::generateProfileReport() const {
    std::ostringstream oss;
    oss << "Performance Profile Report\n";
    oss << "==========================\n\n";
    
    if (pImpl->completedTimings.empty()) {
        oss << "No performance data available.\n";
        return oss.str();
    }
    
    oss << "Operations completed: " << pImpl->completedTimings.size() << "\n";
    
    // Sort by duration (slowest first)
    auto sortedTimings = pImpl->completedTimings;
    std::sort(sortedTimings.begin(), sortedTimings.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    oss << "\nTop 10 Slowest Operations:\n";
    for (size_t i = 0; i < std::min(size_t(10), sortedTimings.size()); ++i) {
        const auto& timing = sortedTimings[i];
        oss << std::setw(3) << (i + 1) << ". " << std::setw(20) << timing.first 
            << ": " << std::setw(8) << timing.second.count() << " us\n";
    }
    
    // Calculate statistics
    auto totalDuration = std::accumulate(sortedTimings.begin(), sortedTimings.end(),
                                         std::chrono::microseconds(0),
                                         [](const auto& acc, const auto& timing) {
                                             return acc + timing.second;
                                         });
    
    auto avgDuration = totalDuration.count() / sortedTimings.size();
    oss << "\nStatistics:\n";
    oss << "  Total time: " << totalDuration.count() << " us\n";
    oss << "  Average time: " << avgDuration << " us\n";
    oss << "  Slowest: " << sortedTimings.front().second.count() << " us (" << sortedTimings.front().first << ")\n";
    
    return oss.str();
}

void PerformanceLogger::resetProfile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->completedTimings.clear();
    pImpl->activeTimers.clear();
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::string currentBrainId;
    std::string currentRegionId;
    std::string currentNeuronId;
    std::string currentSynapseId;
    bool performanceMonitoring = false;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> performanceTimers;
    std::unordered_map<std::string, std::chrono::microseconds> performanceResults;
    Logger::LogStatistics statistics;
    std::function<void(const std::string&)> errorHandler;
    LogFilter globalFilter;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add default console logger
    addLogger(std::make_shared<ConsoleLogger>());
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
            [&logger](const std::shared_ptr<ILogger>& l) { return l == logger; }),
        pImpl->loggers.end()
    );
}

void Logger::replaceLogger(std::shared_ptr<ILogger> oldLogger, std::shared_ptr<ILogger> newLogger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (size_t i = 0; i < pImpl->loggers.size(); ++i) {
        if (pImpl->loggers[i] == oldLogger) {
            pImpl->loggers[i] = newLogger;
            break;
        }
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
    return pImpl->globalFilter;
}

void Logger::setBrainContext(const std::string& brainId, const std::string& regionId, const std::string& neuronId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentBrainId = brainId;
    pImpl->currentRegionId = regionId;
    pImpl->currentNeuronId = neuronId;
}

void Logger::clearBrainContext() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentBrainId.clear();
    pImpl->currentRegionId.clear();
    pImpl->currentNeuronId.clear();
    pImpl->currentSynapseId.clear();
}

void Logger::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMonitoring = enable;
}

bool Logger::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoring;
}

void Logger::log(LogLevel level, const std::string& message, 
                 const std::string& file, int line, 
                 const std::string& function, const std::string& tag) {
    // Create log entry
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.brainId = pImpl->currentBrainId;
    entry.regionId = pImpl->currentRegionId;
    entry.neuronId = pImpl->currentNeuronId;
    entry.synapseId = pImpl->currentSynapseId;
    
    // Add tag if provided
    if (!tag.empty()) {
        entry.structuredData["tag"] = tag;
    }
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->statistics.totalLogs++;
        pImpl->statistics.logsByLevel[level]++;
        if (!tag.empty()) {
            pImpl->statistics.logsByTag[tag]++;
        }
    }
    
    // Log to all loggers
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& tag) {
    log(LogLevel::Debug, message, file, line, function, tag);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& tag) {
    log(LogLevel::Info, message, file, line, function, tag);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& tag) {
    log(LogLevel::Warning, message, file, line, function, tag);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& tag) {
    log(LogLevel::Error, message, file, line, function, tag);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& tag) {
    log(LogLevel::Critical, message, file, line, function, tag);
}

void Logger::logStructured(LogLevel level, const std::unordered_map<std::string, std::string>& data, const std::string& file, int line, const std::string& function) {
    std::string message;
    for (const auto& pair : data) {
        if (!message.empty()) message += " ";
        message += pair.first + "=" + pair.second;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.structuredData = data;
    entry.brainId = pImpl->currentBrainId;
    entry.regionId = pImpl->currentRegionId;
    entry.neuronId = pImpl->currentNeuronId;
    entry.synapseId = pImpl->currentSynapseId;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::startPerformanceTimer(const std::string& operation) {
    if (pImpl->performanceMonitoring) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->performanceTimers[operation] = std::chrono::system_clock::now();
    }
}

void Logger::endPerformanceTimer(const std::string& operation) {
    if (pImpl->performanceMonitoring) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        auto it = pImpl->performanceTimers.find(operation);
        if (it != pImpl->performanceTimers.end()) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - it->second);
            pImpl->performanceResults[operation] = duration;
            
            // Update statistics
            pImpl->statistics.totalProcessingTime += duration;
            if (duration > pImpl->statistics.slowestLog) {
                pImpl->statistics.slowestLog = duration;
                pImpl->statistics.slowestOperation = operation;
            }
            
            // Log performance
            LogEntry entry;
            entry.level = LogLevel::Debug;
            entry.message = "PERFORMANCE: " + operation + ": " + std::to_string(duration.count()) + " us";
            entry.timestamp = std::chrono::system_clock::now();
            entry.structuredData["operation"] = operation;
            entry.structuredData["duration"] = std::to_string(duration.count());
            entry.brainId = pImpl->currentBrainId;
            
            for (auto& logger : pImpl->loggers) {
                logger->log(entry);
            }
            
            pImpl->performanceTimers.erase(it);
        }
    }
}

std::chrono::microseconds Logger::getElapsedTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->performanceResults.find(operation);
    if (it != pImpl->performanceResults.end()) {
        return it->second;
    }
    return std::chrono::microseconds(0);
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

Logger::LogStatistics Logger::getStatistics() const {
    return pImpl->statistics;
}

std::string Logger::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Log Report ===\n";
    oss << "Generated at: " << formatTimestamp(std::chrono::system_clock::now()) << "\n";
    oss << "Total log entries: " << pImpl->statistics.totalLogs << "\n";
    oss << "\nLog breakdown by level:\n";
    
    for (const auto& pair : pImpl->statistics.logsByLevel) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nLog breakdown by tag:\n";
    for (const auto& pair : pImpl->statistics.logsByTag) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nPerformance statistics:\n";
    oss << "  Total processing time: " << pImpl->statistics.totalProcessingTime.count() << " us\n";
    oss << "  Average processing time: " << (pImpl->statistics.totalLogs > 0 ? 
        pImpl->statistics.totalProcessingTime.count() / pImpl->statistics.totalLogs : 0) << " us\n";
    oss << "  Slowest operation: " << pImpl->statistics.slowestOperation << " (" << 
        pImpl->statistics.slowestLog.count() << " us)\n";
    
    return oss.str();
}

void Logger::exportLogs(const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << generateReport();
        file.close();
    }
}

void Logger::setErrorHandler(std::function<void(const std::string&)> handler) {
    pImpl->errorHandler = handler;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& tag)
    : level_(level), file_(file), line_(line), function_(function), tag_(tag) {
    startTime_ = std::chrono::system_clock::now();
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, tag_);
    
    // Add performance timing if available
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    if (duration.count() > 100) { // Log if longer than 100us
        Logger::getGlobal().logTiming("LogStream operation", duration);
    }
}

// Helper functions
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp, const std::string& format) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    localtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, format.c_str());
    return oss.str();
}

std::string sanitizeLogMessage(const std::string& message) {
    std::string result = message;
    // Remove potentially problematic characters
    result.erase(std::remove_if(result.begin(), result.end(),
        [](char c) { return c == '\r' || c == '\n'; }), result.end());
    return result;
}

// Explicit instantiations for template functions
namespace {
    template void ConsoleLogger::setFilter(const LogFilter& filter);
    template LogFilter ConsoleLogger::getFilter() const;
    
    template void FileLogger::setFilter(const LogFilter& filter);
    template LogFilter FileLogger::getFilter() const;
    
    template void PerformanceLogger::setFilter(const LogFilter& filter);
    template LogFilter PerformanceLogger::getFilter() const;
}

} // namespace nlm