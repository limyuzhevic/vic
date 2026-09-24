#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <regex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced formatting options
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeFileInfo = true;
    bool includeFunctionInfo = true;
    bool includeCategoryInfo = true;
    std::unordered_map<std::string, LogLevel> categoryThresholds;
    
    // File management
    std::string filepath;
    bool append = true;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t currentFileSize = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check if level is enabled
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check category thresholds
    if (!pImpl->categoryThresholds.empty() && pImpl->includeCategoryInfo) {
        auto it = pImpl->categoryThresholds.find(entry.category);
        if (it != pImpl->categoryThresholds.end() && entry.level < it->second) {
            return;
        }
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    oss << formatTimestamp(entry.timestamp);
    
    if (pImpl->includeCategoryInfo && !entry.category.empty()) {
        oss << " [" << entry.category << "]";
    }
    
    if (pImpl->includeFileInfo && !entry.file.empty()) {
        // Extract filename from path
        size_t pos = entry.file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? entry.file.substr(pos + 1) : entry.file;
        oss << " " << filename << ":" << entry.line;
    }
    
    if (pImpl->includeFunctionInfo && !entry.function.empty()) {
        oss << " " << entry.function;
    }
    
    oss << " " << entry.message;
    
    // Add context information
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
    
    if (entry.neuronId.has_value()) {
        oss << " [Neuron:" << entry.neuronId.value() << "]";
    }
    
    if (entry.synapseId.has_value()) {
        oss << " [Synapse:" << entry.synapseId.value() << "]";
    }
    
    if (entry.regionId.has_value()) {
        oss << " [Region:" << entry.regionId.value() << "]";
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
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Check file size and rotate if needed
        if (pImpl->currentFileSize >= pImpl->maxFileSize) {
            rotate();
        }
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

void ConsoleLogger::setOutputFile(const std::string& filepath, bool append) {
    pImpl->filepath = filepath;
    pImpl->append = append;
    pImpl->fileOut.open(filepath, append ? std::ios::app : std::ios::trunc);
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void ConsoleLogger::setIncludeFileInfo(bool include) {
    pImpl->includeFileInfo = include;
}

void ConsoleLogger::setIncludeFunctionInfo(bool include) {
    pImpl->includeFunctionInfo = include;
}

void ConsoleLogger::setIncludeCategoryInfo(bool include) {
    pImpl->includeCategoryInfo = include;
}

void ConsoleLogger::setLevelThreshold(const std::string& category, LogLevel level) {
    pImpl->categoryThresholds[category] = level;
}

LogLevel ConsoleLogger::getLevelThreshold(const std::string& category) const {
    auto it = pImpl->categoryThresholds.find(category);
    return (it != pImpl->categoryThresholds.end()) ? it->second : pImpl->level;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[36m";   // Cyan
        case LogLevel::Info: return "\033[32m";     // Green
        case LogLevel::Notice: return "\033[37m";  // White
        case LogLevel::Warning: return "\033[33m";  // Yellow
        case LogLevel::Error: return "\033[31m";    // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[95m";   // Bright Magenta
        case LogLevel::Emergency: return "\033[41m"; // Red background
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_buf;
    localtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    if (pImpl->timestampFormat == "%Y-%m-%d %H:%M:%S") {
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    } else if (pImpl->timestampFormat == "%H:%M:%S") {
        oss << std::put_time(&tm_buf, "%H:%M:%S");
    } else if (pImpl->timestampFormat == "%Y/%m/%d") {
        oss << std::put_time(&tm_buf, "%Y/%m/%d");
    } else {
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    }
    return oss.str();
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    
    std::string filepath;
    size_t maxFileSize = 50 * 1024 * 1024; // 50MB
    size_t currentFileSize = 0;
    size_t maxFileCount = 5;
    bool compression = false;
    bool append = true;
    
    std::vector<std::string> fileHistory;
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << formatTimestamp(entry.timestamp);
    
    if (!entry.category.empty()) {
        oss << " [" << entry.category << "]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1;
    
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFilepath(const std::string& filepath) {
    pImpl->filepath = filepath;
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = 0;
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFileCount(size_t maxCount) {
    pImpl->maxFileCount = maxCount;
}

void FileLogger::setCompression(bool enable) {
    pImpl->compression = enable;
}

void FileLogger::rotate() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Close current file and create new one
    std::time_t now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf);
    
    std::ostringstream oss;
    oss << pImpl->filepath << "." << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    
    if (pImpl->compression) {
        oss << ".gz";
    }
    
    std::string newFilepath = oss.str();
    pImpl->fileOut.open(newFilepath, std::ios::trunc);
    pImpl->currentFileSize = 0;
    
    // Update file history
    pImpl->fileHistory.push_back(newFilepath);
    if (pImpl->fileHistory.size() > pImpl->maxFileCount) {
        // Remove oldest file
        std::remove(pImpl->fileHistory[0].c_str());
        pImpl->fileHistory.erase(pImpl->fileHistory.begin());
    }
}

bool FileLogger::isFileOpen() const {
    return pImpl->fileOut.is_open();
}

std::string FileLogger::getCurrentFilepath() const {
    return pImpl->filepath;
}

// AsyncLogger implementation
struct AsyncLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    std::condition_variable cv;
    bool running = false;
    bool stopped = false;
    size_t bufferSize = 8192;
    std::vector<LogEntry> buffer;
    std::thread workerThread;
    std::ofstream fileOut;
    std::string filepath;
    bool append = true;
    std::mutex flushMutex;
    std::condition_variable flushCv;
    bool needsFlush = false;
};

AsyncLogger::AsyncLogger() : pImpl(std::make_unique<Impl>()) {}

AsyncLogger::AsyncLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

AsyncLogger::~AsyncLogger() {
    stop();
    if (pImpl->workerThread.joinable()) {
        pImpl->workerThread.join();
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->buffer.push_back(entry);
    
    if (pImpl->buffer.size() >= pImpl->bufferSize) {
        pImpl->cv.notify_one();
    }
}

void AsyncLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void AsyncLogger::setBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->bufferSize = size;
}

void AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->flushMutex);
    pImpl->needsFlush = true;
    pImpl->flushCv.notify_one();
}

void AsyncLogger::waitForFlush() {
    std::unique_lock<std::mutex> lock(pImpl->flushMutex);
    pImpl->flushCv.wait(lock, [this] { return !pImpl->needsFlush; });
}

void AsyncLogger::start() {
    if (pImpl->running) return;
    
    pImpl->running = true;
    pImpl->stopped = false;
    
    pImpl->workerThread = std::thread([this]() {
        while (true) {
            std::unique_lock<std::mutex> lock(pImpl->mutex);
            
            pImpl->cv.wait(lock, [this]() {
                return !pImpl->buffer.empty() || pImpl->stopped;
            });
            
            if (pImpl->stopped && pImpl->buffer.empty()) {
                break;
            }
            
            // Process buffer
            while (!pImpl->buffer.empty()) {
                LogEntry entry = pImpl->buffer.front();
                pImpl->buffer.erase(pImpl->buffer.begin());
                
                // Log the entry
                logInternal(entry);
                
                // Check if we should flush
                {
                    std::lock_guard<std::mutex> flushLock(pImpl->flushMutex);
                    if (pImpl->needsFlush) {
                        pImpl->needsFlush = false;
                        pImpl->flushCv.notify_one();
                    }
                }
            }
        }
    });
}

void AsyncLogger::stop() {
    if (!pImpl->running) return;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->stopped = true;
    }
    
    pImpl->cv.notify_one();
    
    if (pImpl->workerThread.joinable()) {
        pImpl->workerThread.join();
    }
    
    pImpl->running = false;
}

void AsyncLogger::logInternal(const LogEntry& entry) {
    // Simple implementation - could be enhanced with proper formatting
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] " << formatTimestamp(entry.timestamp) << " " << entry.message;
    
    std::cout << oss.str() << std::endl;
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Statistics
    size_t totalLogs = 0;
    std::unordered_map<LogLevel, size_t> logCounts;
    
    // Performance timing
    std::unordered_map<std::string, std::pair<std::chrono::microseconds, size_t>> performanceMetrics;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

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
    
    pImpl->totalLogs++;
    pImpl->logCounts[level]++;
}

void Logger::logWithContext(LogLevel level, const std::string& message,
                           const std::unordered_map<std::string, std::string>& context,
                           const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = context;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->totalLogs++;
    pImpl->logCounts[level]++;
}

void Logger::debugWithContext(const std::string& message,
                             const std::unordered_map<std::string, std::string>& context,
                             const std::string& file, int line, const std::string& function) {
    logWithContext(LogLevel::Debug, message, context, file, line, function);
}

void Logger::infoWithContext(const std::string& message,
                            const std::unordered_map<std::string, std::string>& context,
                            const std::string& file, int line, const std::string& function) {
    logWithContext(LogLevel::Info, message, context, file, line, function);
}

void Logger::warningWithContext(const std::string& message,
                               const std::unordered_map<std::string, std::string>& context,
                               const std::string& file, int line, const std::string& function) {
    logWithContext(LogLevel::Warning, message, context, file, line, function);
}

void Logger::errorWithContext(const std::string& message,
                             const std::unordered_map<std::string, std::string>& context,
                             const std::string& file, int line, const std::string& function) {
    logWithContext(LogLevel::Error, message, context, file, line, function);
}

void Logger::timeOperation(LogLevel level, const std::string& operation,
                          std::function<void()> func,
                          const std::string& category) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::ostringstream oss;
    oss << "Operation \"" << operation << "\" completed in " << duration.count() << " microseconds";
    
    logWithContext(level, oss.str(), {{ "operation", operation, "duration_us", std::to_string(duration.count()) }}, "", 0, "");
    
    // Update performance metrics
    auto& metrics = pImpl->performanceMetrics[operation];
    metrics.first += duration;
    metrics.second++;
}

void Logger::logPerformance(const std::string& operation,
                           const std::chrono::microseconds& duration,
                           size_t memoryUsage,
                           const std::unordered_map<std::string, std::string>& context) {
    std::ostringstream oss;
    oss << "Performance: \"" << operation << "\" - " << duration.count() << " us, " << memoryUsage << " bytes";
    
    logWithContext(LogLevel::Performance, oss.str(), context);
}

void Logger::logError(const std::string& errorCode, const std::string& errorMessage,
                     bool isRecoverable, const std::string& recoveryAction,
                     const std::string& file, int line, const std::string& function) {
    std::ostringstream oss;
    oss << "Error " << errorCode << ": " << errorMessage;
    if (!recoveryAction.empty()) {
        oss << " (Recovery: " << recoveryAction << ")";
    }
    
    logWithContext(LogLevel::Error, oss.str(), {{ "error_code", errorCode, "recoverable", isRecoverable ? "true" : "false" }}, file, line, function);
}

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

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

size_t Logger::getLogCount(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->logCounts.find(level);
    return (it != pImpl->logCounts.end()) ? it->second : 0;
}

std::string Logger::getLogStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Log Statistics ===\n";
    oss << "Total logs: " << pImpl->totalLogs << "\n";
    oss << "Log counts by level:\n";
    
    for (const auto& pair : pImpl->logCounts) {
        oss << "  " << levelToString(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nPerformance metrics:\n";
    for (const auto& pair : pImpl->performanceMetrics) {
        auto& metrics = pair.second;
        double avgDuration = (double)metrics.first.count() / metrics.second;
        oss << "  " << pair.first << ": " << metrics.second << " calls, avg duration: " << avgDuration << " us\n";
    }
    
    return oss.str();
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

LogStream& LogStream::addContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

LogStream& LogStream::addContext(const std::string& key, int value) {
    context_[key] = std::to_string(value);
    return *this;
}

LogStream& LogStream::addContext(const std::string& key, float value) {
    context_[key] = std::to_string(value);
    return *this;
}

LogStream& LogStream::withNeuron(int neuronId) {
    neuronId_ = neuronId;
    return *this;
}

LogStream& LogStream::withSynapse(int synapseId) {
    synapseId_ = synapseId;
    return *this;
}

LogStream& LogStream::withRegion(int regionId) {
    regionId_ = regionId;
    return *this;
}

LogStream& LogStream::withCategory(const std::string& category) {
    category_ = category;
    return *this;
}

LogStream& LogStream::withSubcategory(const std::string& subcategory) {
    subcategory_ = subcategory;
    return *this;
}

// LogError implementation
LogError::LogError(const std::string& errorCode, const std::string& errorMessage,
                 bool isRecoverable, const std::string& recoveryAction)
    : errorCode_(errorCode), errorMessage_(errorMessage),
      isRecoverable_(isRecoverable), recoveryAction_(recoveryAction),
      file_(""), line_(0), function_("") {}

void LogError::log() {
    Logger::getGlobal().logError(errorCode_, errorMessage_, isRecoverable_, recoveryAction_, file_, line_, function_);
}

void LogError::logWithRecovery() {
    std::ostringstream oss;
    oss << errorMessage_;
    if (!recoveryAction_.empty()) {
        oss << " (Attempting recovery: " << recoveryAction_ << ")";
    }
    
    Logger::getGlobal().logWithContext(LogLevel::Warning, oss.str(), 
                                      {{ "error_code", errorCode_, "recoverable", isRecoverable_ ? "true" : "false" }}, file_, line_, function_);
}

// Helper function
const char* GetLogCategoryString(LogCategory category) {
    switch (category) {
        case LogCategory::System: return "System";
        case LogCategory::Memory: return "Memory";
        case LogCategory::Performance: return "Performance";
        case LogCategory::Network: return "Network";
        case LogCategory::Neuron: return "Neuron";
        case LogCategory::Synapse: return "Synapse";
        case LogCategory::Plasticity: return "Plasticity";
        case LogCategory::Development: return "Development";
        case LogCategory::Learning: return "Learning";
        case LogCategory::Sensory: return "Sensory";
        case LogCategory::Motor: return "Motor";
        case LogCategory::Environment: return "Environment";
        case LogCategory::World: return "World";
        case LogCategory::Visualization: return "Visualization";
        case LogCategory::Configuration: return "Configuration";
        case LogCategory::FileIO: return "FileIO";
        case LogCategory::NetworkIO: return "NetworkIO";
        case LogCategory::Error: return "Error";
        case LogCategory::Warning: return "Warning";
        case LogCategory::Debug: return "Debug";
        case LogCategory::Custom: return "Custom";
        default: return "Unknown";
    }
}

} // namespace nlm
