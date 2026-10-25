#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <thread>
#include <sstream>
#include <iomanip>

namespace nlm {

// Enhanced Console logger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogFormat format = LogFormat::Enhanced;
    LogFilter filter;
    LogRotationConfig rotationConfig;
    LogStatistics stats;
    bool asyncMode = false;
    size_t queueSize = 10000;
    size_t workerThreads = 2;
    std::queue<LogEntry> messageQueue;
    std::vector<std::thread> workerThreadsList;
    std::condition_variable cv;
    std::atomic<bool> stopWorkers{false};
    std::chrono::steady_clock::time_point lastRotation;
    std::string currentLogFile;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->stats.firstMessageTime = std::chrono::steady_clock::now();
    pImpl->lastRotation = std::chrono::steady_clock::now();
    
    // Initialize worker threads for async logging
    if (pImpl->asyncMode) {
        for (size_t i = 0; i < pImpl->workerThreads; ++i) {
            pImpl->workerThreadsList.emplace_back(&ConsoleLogger::processAsyncQueue, this);
        }
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->stats.firstMessageTime = std::chrono::steady_clock::now();
    pImpl->lastRotation = std::chrono::steady_clock::now();
}

ConsoleLogger::~ConsoleLogger() {
    stopWorkers = true;
    cv.notify_all();
    
    for (auto& thread : pImpl->workerThreadsList) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Update statistics
    pImpl->stats.totalMessages++;
    switch (entry.level) {
        case LogLevel::Debug: pImpl->stats.debugMessages++; break;
        case LogLevel::Info: pImpl->stats.infoMessages++; break;
        case LogLevel::Warning: pImpl->stats.warningMessages++; break;
        case LogLevel::Error: pImpl->stats.errorMessages++; break;
        case LogLevel::Critical: pImpl->stats.criticalMessages++; break;
        case LogLevel::Trace: pImpl->stats.traceMessages++; break;
        case LogLevel::Perf: pImpl->stats.perfMessages++; break;
        case LogLevel::Memory: pImpl->stats.memoryMessages++; break;
        default: break;
    }
    
    pImpl->stats.lastMessageTime = std::chrono::steady_clock::now();
    
    // Check if entry matches filter
    if (!matchesFilter(entry)) {
        return;
    }
    
    // Log entry
    if (pImpl->asyncMode) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->messageQueue.push(entry);
        pImpl->cv.notify_one();
    } else {
        processLogEntry(entry);
    }
    
    // Check rotation
    if (pImpl->rotationConfig.enabled) {
        auto now = std::chrono::steady_clock::now();
        if (now - pImpl->lastRotation > pImpl->rotationConfig.rotationInterval) {
            rotateLogFile();
        }
    }
}

void ConsoleLogger::processLogEntry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string formattedMessage;
    switch (pImpl->format) {
        case LogFormat::Plain:
            formattedMessage = formatPlainEntry(entry);
            break;
        case LogFormat::JSON:
            formattedMessage = formatJSONEntry(entry);
            break;
        case LogFormat::XML:
            formattedMessage = formatXMLEntry(entry);
            break;
        case LogFormat::CSV:
            formattedMessage = formatCSVEntry(entry);
            break;
        case LogFormat::Enhanced:
        default:
            formattedMessage = formatEnhancedEntry(entry);
            break;
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
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

void ConsoleLogger::setFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filter = filter;
}

LogFilter ConsoleLogger::getFilter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->filter;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentLogFile = filepath;
}

void ConsoleLogger::setRotationConfig(const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationConfig = config;
}

LogStatistics ConsoleLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ConsoleLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = LogStatistics();
    pImpl->stats.firstMessageTime = std::chrono::steady_clock::now();
}

bool ConsoleLogger::setAsyncMode(bool enabled, size_t queueSize, size_t workerThreads) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (enabled == pImpl->asyncMode) {
        return true; // No change needed
    }
    
    pImpl->asyncMode = enabled;
    pImpl->queueSize = queueSize;
    pImpl->workerThreads = workerThreads;
    
    // Clear existing queue
    while (!pImpl->messageQueue.empty()) {
        pImpl->messageQueue.pop();
    }
    
    // Restart workers
    stopWorkers = true;
    cv.notify_all();
    
    for (auto& thread : pImpl->workerThreadsList) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    pImpl->workerThreadsList.clear();
    
    if (enabled) {
        stopWorkers = false;
        for (size_t i = 0; i < workerThreads; ++i) {
            pImpl->workerThreadsList.emplace_back(&ConsoleLogger::processAsyncQueue, this);
        }
    }
    
    return true;
}

bool ConsoleLogger::isAsyncMode() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->asyncMode;
}

void ConsoleLogger::logStructured(LogLevel level, const std::string& message,
                                 const std::unordered_map<std::string, std::string>& keyValues,
                                 const std::string& file, int line, const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.keyValues = keyValues;
    entry.category = "structured";
    
    log(entry);
}

void ConsoleLogger::logPerformance(LogLevel level, const std::string& message,
                                   const std::chrono::high_resolution_clock::duration& duration,
                                   const std::string& file, int line, const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.duration = duration;
    entry.hasDuration = true;
    entry.category = "performance";
    
    log(entry);
}

void ConsoleLogger::logMemory(LogLevel level, const std::string& message,
                             size_t memoryUsage, const std::string& file, int line,
                             const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadMemoryUsage = memoryUsage;
    entry.category = "memory";
    
    log(entry);
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::processAsyncQueue() {
    while (!stopWorkers) {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        cv.wait_for(lock, std::chrono::milliseconds(10), [this]() {
            return !pImpl->messageQueue.empty() || stopWorkers;
        });
        
        if (!pImpl->messageQueue.empty()) {
            LogEntry entry = pImpl->messageQueue.front();
            pImpl->messageQueue.pop();
            lock.unlock();
            
            processLogEntry(entry);
            
            lock.lock();
        }
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->rotationConfig.enabled || pImpl->currentLogFile.empty()) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    if (now - pImpl->lastRotation < pImpl->rotationConfig.rotationInterval) {
        return;
    }
    
    // Check file size
    if (std::filesystem::exists(pImpl->currentLogFile)) {
        auto fileSize = std::filesystem::file_size(pImpl->currentLogFile);
        if (fileSize >= pImpl->rotationConfig.maxFileSize) {
            // Rotate files
            std::string rotatedFile = pImpl->currentLogFile + pImpl->rotationConfig.backupSuffix;
            std::filesystem::rename(pImpl->currentLogFile, rotatedFile);
            
            pImpl->fileOut.close();
            pImpl->fileOut.open(pImpl->currentLogFile, std::ios::trunc);
            
            pImpl->lastRotation = now;
        }
    }
}

std::string ConsoleLogger::formatPlainEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " " << entry.message;
    
    return oss.str();
}

std::string ConsoleLogger::formatJSONEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    oss << "  \"level\": \"" << levelToString(entry.level) << "\",\n";
    oss << "  \"message\": \"" << escapeJSON(entry.message) << "\",\n";
    oss << "  \"file\": \"" << entry.file << "\",\n";
    oss << "  \"line\": " << entry.line << ",\n";
    oss << "  \"function\": \"" << entry.function << "\",\n";
    
    if (!entry.category.empty()) {
        oss << "  \"category\": \"" << entry.category << "\",\n";
    }
    
    if (entry.hasDuration) {
        oss << "  \"duration_ms\": " << std::chrono::duration_cast<std::chrono::milliseconds>(entry.duration).count() << ",\n";
    }
    
    oss << "}";
    return oss.str();
}

std::string ConsoleLogger::formatXMLEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "<logEntry>\n";
    oss << "  <timestamp>" << getCurrentTimestamp() << "</timestamp>\n";
    oss << "  <level>" << levelToString(entry.level) << "</level>\n";
    oss << "  <message>" << escapeXML(entry.message) << "</message>\n";
    oss << "  <file>" << entry.file << "</file>\n";
    oss << "  <line>" << entry.line << "</line>\n";
    oss << "  <function>" << entry.function << "</function>\n";
    
    if (!entry.category.empty()) {
        oss << "  <category>" << entry.category << "</category>\n";
    }
    
    oss << "</logEntry>\n";
    return oss.str();
}

std::string ConsoleLogger::formatEnhancedEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    if (!entry.category.empty()) {
        oss << " [" << entry.category << "]";
    }
    
    oss << " " << entry.message;
    
    if (entry.hasDuration) {
        oss << " (" << std::chrono::duration_cast<std::chrono::microseconds>(entry.duration).count()
            << " μs)";
    }
    
    if (!entry.keyValues.empty()) {
        oss << " | {";
        bool first = true;
        for (const auto& [key, value] : entry.keyValues) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

std::string ConsoleLogger::formatCSVEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << getCurrentTimestamp() << ","
        << levelToString(entry.level) << ","
        << escapeCSV(entry.message) << ","
        << escapeCSV(entry.file) << ","
        << entry.line << ","
        << escapeCSV(entry.function) << ","
        << escapeCSV(entry.category);
    
    return oss.str();
}

std::string ConsoleLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool ConsoleLogger::matchesFilter(const LogEntry& entry) const {
    // Level filter
    if (entry.level < pImpl->filter.minLevel || entry.level > pImpl->filter.maxLevel) {
        return false;
    }
    
    // File pattern filter
    if (!pImpl->filter.filePattern.empty()) {
        std::regex pattern(pImpl->filter.filePattern);
        if (!std::regex_search(entry.file, pattern)) {
            return false;
        }
    }
    
    // Function pattern filter
    if (!pImpl->filter.functionPattern.empty()) {
        std::regex pattern(pImpl->filter.functionPattern);
        if (!std::regex_search(entry.function, pattern)) {
            return false;
        }
    }
    
    // Category filter
    if (!pImpl->filter.category.empty() && entry.category != pImpl->filter.category) {
        return false;
    }
    
    // Tag filters
    if (!pImpl->filter.includeTags.empty()) {
        bool hasIncludeTag = false;
        for (const auto& tag : pImpl->filter.includeTags) {
            if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                hasIncludeTag = true;
                break;
            }
        }
        if (!hasIncludeTag) {
            return false;
        }
    }
    
    for (const auto& tag : pImpl->filter.excludeTags) {
        if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
            return false;
        }
    }
    
    // Time range filter
    auto entryTime = entry.timestamp;
    if (entryTime < pImpl->filter.startTime || entryTime > pImpl->filter.endTime) {
        return false;
    }
    
    return true;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Perf: return "PERF";
        case LogLevel::Memory: return "MEM";
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
        case LogLevel::Trace: return "\033[90m";    // Bright Black
        case LogLevel::Perf: return "\033[34m";      // Blue
        case LogLevel::Memory: return "\033[32m";    // Green
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::escapeJSON(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < ' ' || c > '~') {
                    std::stringstream ss;
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    escaped += ss.str();
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
}

std::string ConsoleLogger::escapeXML(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '&': escaped += "&amp;"; break;
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&apos;"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string ConsoleLogger::escapeCSV(const std::string& str) const {
    std::string escaped = str;
    // Replace quotes with double quotes and wrap whole string in quotes
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.insert(pos, "\"");
        pos += 2;
    }
    escaped = "\"" + escaped + "\"";
    return escaped;
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogLevel globalLevel = LogLevel::Info;
    LogFormat globalFormat = LogFormat::Enhanced;
    LogFilter globalFilter;
    LogStatistics globalStats;
    std::unordered_map<std::string, std::string> errorContext;
    std::mutex errorContextMutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->globalStats.firstMessageTime = std::chrono::steady_clock::now();
    // Add default console logger
    addLogger(std::make_shared<ConsoleLogger>(pImpl->globalLevel));
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    logger->setLevel(pImpl->globalLevel);
    logger->setFormat(pImpl->globalFormat);
    logger->setFilter(pImpl->globalFilter);
}

void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove(pImpl->loggers.begin(), pImpl->loggers.end(), logger),
        pImpl->loggers.end()
    );
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if level is enabled globally
    if (level < pImpl->globalLevel) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    
    // Add thread and process info
    entry.threadId = getThreadName();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update global statistics
    pImpl->globalStats.totalMessages++;
    switch (level) {
        case LogLevel::Debug: pImpl->globalStats.debugMessages++; break;
        case LogLevel::Info: pImpl->globalStats.infoMessages++; break;
        case LogLevel::Warning: pImpl->globalStats.warningMessages++; break;
        case LogLevel::Error: pImpl->globalStats.errorMessages++; break;
        case LogLevel::Critical: pImpl->globalStats.criticalMessages++; break;
        case LogLevel::Trace: pImpl->globalStats.traceMessages++; break;
        case LogLevel::Perf: pImpl->globalStats.perfMessages++; break;
        case LogLevel::Memory: pImpl->globalStats.memoryMessages++; break;
        default: break;
    }
}

void Logger::logStructured(LogLevel level, const std::string& message,
                          const std::unordered_map<std::string, std::string>& keyValues,
                          const std::string& file, int line, const std::string& function,
                          const std::string& category) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.keyValues = keyValues;
    entry.category = category;
    
    log(level, message, file, line, function, category);
}

void Logger::logPerformance(const std::string& message,
                           const std::chrono::high_resolution_clock::duration& duration,
                           const std::string& file, int line, const std::string& function,
                           const std::string& category) {
    LogEntry entry;
    entry.level = LogLevel::Perf;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.duration = duration;
    entry.hasDuration = true;
    entry.category = category;
    
    log(LogLevel::Perf, message, file, line, function, category);
}

void Logger::logMemory(const std::string& message,
                      size_t memoryUsage,
                      const std::string& file, int line, const std::string& function,
                      const std::string& category) {
    LogEntry entry;
    entry.level = LogLevel::Memory;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadMemoryUsage = memoryUsage;
    entry.category = category;
    
    log(LogLevel::Memory, message, file, line, function, category);
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                 const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

Logger::Timer::Timer(const std::string& operation, LogLevel level,
                   const std::string& category)
    : operation_(operation), level_(level), category_(category) {
    start_ = std::chrono::high_resolution_clock::now();
    Logger::getGlobal().logPerformance("Starting timer: " + operation, 
                                      std::chrono::high_resolution_clock::duration(0), 
                                      "", 0, "", category);
}

Logger::Timer::~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start_;
    Logger::getGlobal().logPerformance("Completed timer: " + operation_ + " took " +
                                      std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) +
                                      " μs", duration, "", 0, "", category_);
}

std::chrono::high_resolution_clock::duration Logger::Timer::elapsed() const {
    auto end = std::chrono::high_resolution_clock::now();
    return end - start_;
}

Logger::MemoryTracker::MemoryTracker(const std::string& operation, LogLevel level,
                                   const std::string& category)
    : operation_(operation), level_(level), category_(category) {
    // In a real implementation, you would query actual memory usage
    // For now, we'll use a placeholder
    currentMemoryUsage_ = 1024 * 1024; // 1MB placeholder
}

Logger::MemoryTracker::~MemoryTracker() {
    Logger::getGlobal().logMemory("Memory change: " + operation_ + " -> " +
                                 std::to_string(currentMemoryUsage_) + " bytes", 
                                 currentMemoryUsage_, "", 0, "", category_);
}

void Logger::setGlobalLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setGlobalFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFormat = format;
    for (auto& logger : pImpl->loggers) {
        logger->setFormat(format);
    }
}

void Logger::setGlobalFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
    for (auto& logger : pImpl->loggers) {
        logger->setFilter(filter);
    }
}

LogStatistics Logger::getGlobalStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalStats;
}

void Logger::resetGlobalStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalStats = LogStatistics();
    pImpl->globalStats.firstMessageTime = std::chrono::steady_clock::now();
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::setErrorContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->errorContextMutex);
    pImpl->errorContext[key] = value;
}

std::string Logger::getErrorContext(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->errorContextMutex);
    auto it = pImpl->errorContext.find(key);
    return it != pImpl->errorContext.end() ? it->second : "";
}

void Logger::clearErrorContext() {
    std::lock_guard<std::mutex> lock(pImpl->errorContextMutex);
    pImpl->errorContext.clear();
}

void Logger::setThreadName(const std::string& name) {
    // Thread-local storage for thread names
    static thread_local std::string threadName = name;
}

std::string Logger::getThreadName() {
    static thread_local std::string threadName = "unknown";
    return threadName;
}

} // namespace nlm
