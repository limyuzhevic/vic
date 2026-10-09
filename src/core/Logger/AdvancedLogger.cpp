#include "AdvancedLogger.hpp"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cmath>
#include <zlib.h>
#include <json.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

using json = nlohmann::json;

namespace nlm {

// Helper functions
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

static std::string getThreadName() {
    std::thread::id this_id = std::this_thread::get_id();
    std::hash<std::thread::id> hasher;
    return std::to_string(hasher(this_id));
}

static std::string formatDuration(double seconds) {
    if (seconds < 1.0) {
        return std::to_string(seconds * 1000) + " ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + " s";
    } else {
        return std::to_string(seconds / 60.0) + " min";
    }
}

// ConsoleLoggerSink implementation
struct ConsoleLoggerSink::Impl {
    LogLevel minLevel = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool useThreadInfo = true;
    bool performanceHighlighting = true;
    bool enableCategoryPrefix = true;
    LogFormat logFormat = LogFormat::PlainText;
    bool jsonPrettify = false;
    bool includeProcessingTime = true;
    bool includeMetrics = true;
    std::mutex mutex;
    std::string levelStrings[6] = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
    std::string colorCodes[6] = {"\033[36m", "\033[32m", "\033[32m", "\033[33m", "\033[31m", "\033[35m"};
    std::string resetCode = "\033[0m";
};

ConsoleLoggerSink::ConsoleLoggerSink() : pImpl(std::make_unique<Impl>()) {}

ConsoleLoggerSink::ConsoleLoggerSink(LogLevel minLevel) : pImpl(std::make_unique<Impl>()) {
    pImpl->minLevel = minLevel;
}

ConsoleLoggerSink::~ConsoleLoggerSink() = default;

void ConsoleLoggerSink::write(const EnhancedLogEntry& entry) {
    if (entry.level < pImpl->minLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output;
    
    switch (pImpl->logFormat) {
        case LogFormat::PlainText:
            output = formatEntry(entry);
            break;
        case LogFormat::JSON:
            output = formatJSON(entry);
            break;
        case LogFormat::CSV:
            // Implement CSV format
            output = "CSV_FORMAT_NOT_IMPLEMENTED";
            break;
        case LogFormat::XML:
            // Implement XML format
            output = "XML_FORMAT_NOT_IMPLEMENTED";
            break;
        case LogFormat::HTML:
            // Implement HTML format
            output = "HTML_FORMAT_NOT_IMPLEMENTED";
            break;
    }
    
    std::cout << output;
    if (!output.empty() && output.back() != '\n') {
        std::cout << "\n";
    }
}

void ConsoleLoggerSink::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout.flush();
}

void ConsoleLoggerSink::close() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
}

bool ConsoleLoggerSink::isConnected() const {
    return true;
}

void ConsoleLoggerSink::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minLevel = level;
}

void ConsoleLoggerSink::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

void ConsoleLoggerSink::setUseTimestamps(bool useTimestamps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useTimestamps = useTimestamps;
}

void ConsoleLoggerSink::setTimestampFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timestampFormat = format;
}

void ConsoleLoggerSink::setThreadInfoEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useThreadInfo = enabled;
}

void ConsoleLoggerSink::setPerformanceHighlighting(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceHighlighting = enabled;
}

void ConsoleLoggerSink::setEnableCategoryPrefix(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableCategoryPrefix = enabled;
}

void ConsoleLoggerSink::setLogFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logFormat = format;
}

void ConsoleLoggerSink::setEnableJSONPrettify(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->jsonPrettify = enabled;
}

void ConsoleLoggerSink::setIncludeProcessingTime(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeProcessingTime = include;
}

void ConsoleLoggerSink::setIncludeMetrics(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeMetrics = include;
}

std::string ConsoleLoggerSink::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str());
    return ss.str();
}

std::string ConsoleLoggerSink::formatEntry(const EnhancedLogEntry& entry) const {
    std::stringstream ss;
    
    if (pImpl->useTimestamps) {
        ss << "[" << formatTimestamp(entry.timestamp) << "] ";
    }
    
    if (pImpl->enableCategoryPrefix && entry.category != LogCategory::Custom) {
        std::string categoryStr;
        switch (entry.category) {
            case LogCategory::System: categoryStr = "SYSTEM"; break;
            case LogCategory::Neural: categoryStr = "NEURAL"; break;
            case LogCategory::Simulation: categoryStr = "SIMULATION"; break;
            case LogCategory::Performance: categoryStr = "PERF"; break;
            case LogCategory::Memory: categoryStr = "MEMORY"; break;
            case LogCategory::Configuration: categoryStr = "CONFIG"; break;
            case LogCategory::Validation: categoryStr = "VALID"; break;
            case LogCategory::World: categoryStr = "WORLD"; break;
            case LogCategory::Agent: categoryStr = "AGENT"; break;
            case LogCategory::Development: categoryStr = "DEV"; break;
            default: categoryStr = "CUSTOM"; break;
        }
        ss << "[" << categoryStr << "] ";
    }
    
    // Level prefix
    if (entry.level < pImpl->minLevel) {
        return ""; // Skip this entry
    }
    
    if (pImpl->useColors && pImpl->levelStrings[static_cast<int>(entry.level)] != "UNKNOWN") {
        ss << pImpl->colorCodes[static_cast<int>(entry.level)];
    }
    
    ss << "[" << pImpl->levelStrings[static_cast<int>(entry.level)] << "]";
    
    if (pImpl->useColors && pImpl->levelStrings[static_cast<int>(entry.level)] != "UNKNOWN") {
        ss << pImpl->resetCode << " ";
    } else {
        ss << " ";
    }
    
    // Thread information
    if (pImpl->useThreadInfo) {
        ss << "[T:" << entry.threadId << "]";
    }
    
    // Message
    ss << entry.message;
    
    // Processing time for performance events
    if (pImpl->performanceHighlighting && entry.isPerformanceEvent && pImpl->includeProcessingTime) {
        ss << " [" << formatDuration(entry.processingTime.count()) << "]";
    }
    
    // Metrics
    if (pImpl->includeMetrics && !entry.metrics.empty()) {
        ss << " ";
        bool first = true;
        for (const auto& metric : entry.metrics) {
            if (!first) ss << ", ";
            ss << metric.first << "=" << metric.second;
            first = false;
        }
    }
    
    return ss.str();
}

std::string ConsoleLoggerSink::formatJSON(const EnhancedLogEntry& entry) const {
    json j;
    j["level"] = pImpl->levelStrings[static_cast<int>(entry.level)];
    j["timestamp"] = formatTimestamp(entry.timestamp);
    j["message"] = entry.message;
    j["thread_id"] = entry.threadId;
    j["file"] = entry.file;
    j["line"] = entry.line;
    j["function"] = entry.function;
    j["category"] = static_cast<int>(entry.category);
    j["simulation_step"] = entry.simulationStep;
    j["agent_id"] = entry.agentId;
    j["region_id"] = entry.brainRegionId;
    
    if (pImpl->includeProcessingTime && entry.isPerformanceEvent) {
        j["processing_time_ms"] = entry.processingTime.count() * 1000.0;
    }
    
    if (pImpl->includeMetrics && !entry.metrics.empty()) {
        j["metrics"] = entry.metrics;
    }
    
    if (!entry.metadata.empty()) {
        j["metadata"] = entry.metadata;
    }
    
    if (pImpl->jsonPrettify) {
        return j.dump(2);
    } else {
        return j.dump();
    }
}

// FileLoggerSink implementation
struct FileLoggerSink::Impl {
    std::string basePath;
    std::string filenamePrefix;
    bool rotationEnabled = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t maxBackupFiles = 5;
    std::chrono::system_clock::time_point rotationInterval = std::chrono::system_clock::now() + std::chrono::hours(24);
    bool compressionEnabled = false;
    bool indexingEnabled = false;
    bool asyncWrite = false;
    size_t bufferSize = 64 * 1024; // 64 KB
    std::ofstream currentFile;
    std::string currentFilename;
    std::mutex mutex;
    std::thread asyncWriter;
    std::queue<EnhancedLogEntry> writeQueue;
    std::condition_variable queueCondition;
    bool running = false;
    bool queueFull = false;
    std::atomic<size_t> fileSize{0};
    std::chrono::system_clock::time_point lastRotationCheck;
    std::unordered_map<std::string, std::vector<std::string>> searchIndex;
};

FileLoggerSink::FileLoggerSink(const std::string& basePath, const std::string& filenamePrefix) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->basePath = basePath;
    pImpl->filenamePrefix = filenamePrefix;
    pImpl->lastRotationCheck = std::chrono::system_clock::now();
    
    std::filesystem::create_directories(basePath);
    
    if (pImpl->asyncWrite) {
        pImpl->running = true;
        pImpl->asyncWriter = std::thread(&FileLoggerSink::asyncWriteThread, this);
    }
}

FileLoggerSink::~FileLoggerSink() {
    close();
}

void FileLoggerSink::asyncWriteThread() {
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->writeQueue.empty()) {
            pImpl->queueCondition.wait_for(lock, std::chrono::milliseconds(100), 
                                         [this]() { return !pImpl->running || !pImpl->writeQueue.empty(); });
        }
        
        if (!pImpl->running || pImpl->writeQueue.empty()) {
            continue;
        }
        
        EnhancedLogEntry entry = pImpl->writeQueue.front();
        pImpl->writeQueue.pop();
        
        lock.unlock();
        
        if (shouldRotateFile()) {
            closeCurrentFile();
        }
        
        openCurrentFile();
        
        // Write entry to file (simplified for brevity)
        std::stringstream ss;
        ss << "[" << getCurrentTimestamp() << "] "
           << "[" << static_cast<int>(entry.level) << "] "
           << entry.message << "\n";
        
        pImpl->currentFile << ss.str();
        pImpl->fileSize += ss.str().size();
        
        if (pImpl->indexingEnabled) {
            // Update search index
        }
        
        lock.lock();
    }
}

void FileLoggerSink::write(const EnhancedLogEntry& entry) {
    if (pImpl->asyncWrite) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->writeQueue.size() > pImpl->bufferSize / sizeof(EnhancedLogEntry)) {
            pImpl->queueFull = true;
            return; // Drop entry if queue is full
        }
        
        pImpl->writeQueue.push(entry);
        pImpl->queueCondition.notify_one();
    } else {
        if (shouldRotateFile()) {
            closeCurrentFile();
        }
        
        openCurrentFile();
        
        // Write entry synchronously
        std::stringstream ss;
        ss << "[" << getCurrentTimestamp() << "] "
           << "[" << static_cast<int>(entry.level) << "] "
           << entry.message << "\n";
        
        pImpl->currentFile << ss.str();
        pImpl->fileSize += ss.str().size();
        
        if (pImpl->indexingEnabled) {
            // Update search index
        }
    }
}

void FileLoggerSink::flush() {
    if (pImpl->asyncWrite) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->queueCondition.notify_one();
    } else {
        if (pImpl->currentFile.is_open()) {
            pImpl->currentFile.flush();
        }
    }
}

void FileLoggerSink::close() {
    if (pImpl->asyncWrite) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->running = false;
            pImpl->queueCondition.notify_one();
        }
        
        if (pImpl->asyncWriter.joinable()) {
            pImpl->asyncWriter.join();
        }
    }
    
    closeCurrentFile();
}

bool FileLoggerSink::isConnected() const {
    return pImpl->currentFile.is_open();
}

void FileLoggerSink::setFileRotationEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationEnabled = enabled;
}

void FileLoggerSink::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void FileLoggerSink::setMaxBackupFiles(size_t maxBackups) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxBackupFiles = maxBackups;
}

void FileLoggerSink::setRotationInterval(const std::chrono::system_clock::time_point& interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationInterval = interval;
}

void FileLoggerSink::setCompressionEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->compressionEnabled = enabled;
}

void FileLoggerSink::setIndexingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->indexingEnabled = enabled;
}

void FileLoggerSink::setBufferSize(size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->bufferSize = bufferSize;
}

void FileLoggerSink::setWriteAsync(bool async) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (async != pImpl->asyncWrite) {
        pImpl->asyncWrite = async;
        if (pImpl->asyncWrite) {
            pImpl->running = true;
            pImpl->asyncWriter = std::thread(&FileLoggerSink::asyncWriteThread, this);
        } else {
            pImpl->running = false;
            pImpl->queueCondition.notify_one();
            if (pImpl->asyncWriter.joinable()) {
                pImpl->asyncWriter.join();
            }
        }
    }
}

void FileLoggerSink::openCurrentFile() {
    if (!pImpl->currentFile.is_open()) {
        std::string filename = pImpl->filenamePrefix + "_" 
                             + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())
                             + ".log";
        std::string fullPath = pImpl->basePath + "/" + filename;
        
        pImpl->currentFile.open(fullPath, std::ios::app);
        pImpl->currentFilename = fullPath;
        pImpl->fileSize = 0;
        
        if (pImpl->currentFile.is_open()) {
            // Write header
            pImpl->currentFile << "=== NLM LOG FILE ===\n";
            pImpl->currentFile << "Created: " << getCurrentTimestamp() << "\n";
            pImpl->currentFile << "Base Path: " << pImpl->basePath << "\n";
            pImpl->currentFile << "Format: Plain Text\n\n";
        }
    }
}

void FileLoggerSink::closeCurrentFile() {
    if (pImpl->currentFile.is_open()) {
        pImpl->currentFile.close();
    }
}

bool FileLoggerSink::shouldRotateFile() {
    if (!pImpl->rotationEnabled) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    
    // Check time-based rotation
    if (now > pImpl->rotationInterval) {
        return true;
    }
    
    // Check size-based rotation
    if (pImpl->fileSize >= pImpl->maxFileSize) {
        return true;
    }
    
    // Check periodic rotation
    auto timeSinceLastCheck = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastRotationCheck);
    if (timeSinceLastCheck.count() >= 3600) { // Check every hour
        pImpl->lastRotationCheck = now;
        // Check if we need to rotate based on time
        if (std::filesystem::exists(pImpl->currentFilename)) {
            auto fileTime = std::filesystem::last_write_time(pImpl->currentFilename);
            auto fileAge = std::chrono::system_clock::now() - std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
            if (fileAge > std::chrono::hours(24)) {
                return true;
            }
        }
    }
    
    return false;
}

// RemoteLoggerSink implementation
struct RemoteLoggerSink::Impl {
    std::string endpoint;
    std::string protocol;
    bool reconnectEnabled = true;
    size_t maxReconnectAttempts = 5;
    size_t batchSize = 100;
    double sendInterval = 1.0;
    std::mutex mutex;
    bool connected = false;
    size_t reconnectAttempts = 0;
    std::thread senderThread;
    std::queue<EnhancedLogEntry> logQueue;
    std::condition_variable queueCondition;
    bool running = false;
    std::chrono::system_clock::time_point lastSendTime;
};

RemoteLoggerSink::RemoteLoggerSink(const std::string& endpoint, const std::string& protocol) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->endpoint = endpoint;
    pImpl->protocol = protocol;
    pImpl->running = true;
    pImpl->lastSendTime = std::chrono::system_clock::now();
    
    pImpl->senderThread = std::thread(&RemoteLoggerSink::sendThread, this);
}

RemoteLoggerSink::~RemoteLoggerSink() {
    close();
}

void RemoteLoggerSink::sendThread() {
    while (pImpl->running) {
        {
            std::unique_lock<std::mutex> lock(pImpl->mutex);
            
            // Check if we need to send batched entries
            auto now = std::chrono::system_clock::now();
            auto timeSinceLastSend = std::chrono::duration<double>(now - pImpl->lastSendTime);
            
            if (timeSinceLastSend.count() >= pImpl->sendInterval || !pImpl->logQueue.empty()) {
                // Send batched entries
                std::vector<EnhancedLogEntry> batch;
                batch.reserve(pImpl->batchSize);
                
                while (!pImpl->logQueue.empty() && batch.size() < pImpl->batchSize) {
                    batch.push_back(pImpl->logQueue.front());
                    pImpl->logQueue.pop();
                }
                
                lock.unlock();
                
                // Send batch (simplified for brevity)
                if (!batch.empty()) {
                    sendBatch(batch);
                }
                
                lock.lock();
                pImpl->lastSendTime = now;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void RemoteLoggerSink::sendBatch(const std::vector<EnhancedLogEntry>& batch) {
    // Simplified implementation - would need actual network code
    // For brevity, this is a placeholder
}

void RemoteLoggerSink::write(const EnhancedLogEntry& entry) {
    if (!pImpl->running) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->logQueue.push(entry);
    pImpl->queueCondition.notify_one();
}

void RemoteLoggerSink::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->queueCondition.notify_one();
}

void RemoteLoggerSink::close() {
    if (pImpl->running) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->running = false;
            pImpl->queueCondition.notify_one();
        }
        
        if (pImpl->senderThread.joinable()) {
            pImpl->senderThread.join();
        }
    }
}

bool RemoteLoggerSink::isConnected() const {
    return pImpl->connected;
}

void RemoteLoggerSink::setReconnectEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->reconnectEnabled = enabled;
}

void RemoteLoggerSink::setMaxReconnectAttempts(size_t attempts) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxReconnectAttempts = attempts;
}

void RemoteLoggerSink::setBatchSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->batchSize = size;
}

void RemoteLoggerSink::setSendInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sendInterval = interval;
}

// LogAggregator implementation
struct LogAggregator::Impl {
    AggregationStrategy strategy = AggregationStrategy::None;
    double aggregationWindow = 60.0; // seconds
    size_t maxBatchSize = 1000;
    bool enableMetricsAggregation = false;
    std::mutex mutex;
    std::vector<EnhancedLogEntry> entries;
    std::chrono::system_clock::time_point lastAggregationTime;
    std::function<void(const std::vector<EnhancedLogEntry>&)> aggregationCompleteCallback;
    std::atomic<size_t> totalProcessed{0};
    std::chrono::duration<double> totalProcessingTime{0.0};
};

LogAggregator::LogAggregator() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastAggregationTime = std::chrono::system_clock::now();
}

LogAggregator::~LogAggregator() = default;

void LogAggregator::processEntry(const EnhancedLogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto startTime = std::chrono::system_clock::now();
    
    pImpl->entries.push_back(entry);
    pImpl->totalProcessed++;
    
    // Perform aggregation if needed
    if (pImpl->strategy != AggregationStrategy::None) {
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastAggregation = std::chrono::duration<double>(now - pImpl->lastAggregationTime);
        
        if (timeSinceLastAggregation.count() >= pImpl->aggregationWindow || 
            pImpl->entries.size() >= pImpl->maxBatchSize) {
            
            flushAggregates();
        }
    }
    
    auto endTime = std::chrono::system_clock::now();
    pImpl->totalProcessingTime += (endTime - startTime);
}

void LogAggregator::setAggregationStrategy(AggregationStrategy strategy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->strategy = strategy;
}

void LogAggregator::setAggregationWindow(double windowSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->aggregationWindow = windowSeconds;
}

void LogAggregator::setMaxBatchSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxBatchSize = size;
}

void LogAggregator::setEnableMetricsAggregation(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableMetricsAggregation = enabled;
}

void LogAggregator::flushAggregates() {
    if (pImpl->entries.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto entriesCopy = std::move(pImpl->entries);
    
    // Call callback if set
    if (pImpl->aggregationCompleteCallback) {
        pImpl->aggregationCompleteCallback(entriesCopy);
    }
    
    pImpl->lastAggregationTime = std::chrono::system_clock::now();
}

void LogAggregator::clearAggregates() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::vector<EnhancedLogEntry> LogAggregator::getAggregatedEntries() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries;
}

size_t LogAggregator::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

double LogAggregator::getProcessingRate() const {
    if (pImpl->totalProcessed == 0) {
        return 0.0;
    }
    return pImpl->totalProcessed / pImpl->totalProcessingTime.count();
}

void LogAggregator::setAggregationCompleteCallback(
    std::function<void(const std::vector<EnhancedLogEntry>&)> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->aggregationCompleteCallback = callback;
}

// LogFilter implementation
struct LogFilter::Impl {
    std::mutex mutex;
    std::vector<std::pair<LogLevel, LogLevel>> levelFilters;
    std::vector<LogCategory> categoryFilters;
    std::vector<std::string> keywordFilters;
    std::vector<std::pair<std::string, std::string>> metadataFilters;
    std::vector<std::pair<double, double>> performanceThresholds;
    std::vector<std::pair<std::string, std::pair<double, double>>> metricThresholds;
    std::vector<std::pair<uint64_t, uint64_t>> simulationStepFilters;
    std::vector<uint64_t> agentFilters;
    std::vector<uint64_t> regionFilters;
    std::vector<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> timeFilters;
    bool caseInsensitive = true;
    bool regexMatching = false;
};

LogFilter::LogFilter() : pImpl(std::make_unique<Impl>()) {}

LogFilter::~LogFilter() = default;

void LogFilter::addLevelFilter(LogLevel minLevel, LogLevel maxLevel) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->levelFilters.emplace_back(minLevel, maxLevel);
}

void LogFilter::addCategoryFilter(LogCategory category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilters.push_back(category);
}

void LogFilter::addKeywordFilter(const std::string& keyword) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->keywordFilters.push_back(keyword);
}

void LogFilter::addMetadataFilter(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metadataFilters.emplace_back(key, value);
}

void LogFilter::addPerformanceThreshold(double minProcessingTime, double maxProcessingTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceThresholds.emplace_back(minProcessingTime, maxProcessingTime);
}

void LogFilter::addMetricThreshold(const std::string& metricName, double minValue, double maxValue) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metricThresholds.emplace_back(metricName, std::make_pair(minValue, maxValue));
}

void LogFilter::addSimulationStepFilter(uint64_t minStep, uint64_t maxStep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->simulationStepFilters.emplace_back(minStep, maxStep);
}

void LogFilter::addAgentFilter(uint64_t agentId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->agentFilters.push_back(agentId);
}

void LogFilter::addRegionFilter(uint64_t regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->regionFilters.push_back(regionId);
}

void LogFilter::addTimeFilter(const std::chrono::system_clock::time_point& start, 
                             const std::chrono::system_clock::time_point& end) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timeFilters.emplace_back(start, end);
}

void LogFilter::clearFilters() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->levelFilters.clear();
    pImpl->categoryFilters.clear();
    pImpl->keywordFilters.clear();
    pImpl->metadataFilters.clear();
    pImpl->performanceThresholds.clear();
    pImpl->metricThresholds.clear();
    pImpl->simulationStepFilters.clear();
    pImpl->agentFilters.clear();
    pImpl->regionFilters.clear();
    pImpl->timeFilters.clear();
}

bool LogFilter::shouldLog(const EnhancedLogEntry& entry) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check level filters
    bool levelMatch = true;
    if (!pImpl->levelFilters.empty()) {
        levelMatch = false;
        for (const auto& filter : pImpl->levelFilters) {
            if (entry.level >= filter.first && entry.level <= filter.second) {
                levelMatch = true;
                break;
            }
        }
    }
    
    if (!levelMatch) return false;
    
    // Check category filters
    bool categoryMatch = true;
    if (!pImpl->categoryFilters.empty()) {
        categoryMatch = false;
        for (const auto& filter : pImpl->categoryFilters) {
            if (filter == entry.category) {
                categoryMatch = true;
                break;
            }
        }
    }
    
    if (!categoryMatch) return false;
    
    // Check keyword filters
    if (!pImpl->keywordFilters.empty()) {
        bool keywordMatch = false;
        for (const auto& keyword : pImpl->keywordFilters) {
            std::string searchText = entry.message;
            if (pImpl->caseInsensitive) {
                // Convert both to lowercase for case-insensitive comparison
                // (simplified implementation)
                if (searchText.find(keyword) != std::string::npos) {
                    keywordMatch = true;
                    break;
                }
            } else {
                if (searchText.find(keyword) != std::string::npos) {
                    keywordMatch = true;
                    break;
                }
            }
        }
        if (!keywordMatch) return false;
    }
    
    // Check metadata filters
    for (const auto& filter : pImpl->metadataFilters) {
        auto it = entry.metadata.find(filter.first);
        if (it == entry.metadata.end() || it->second != filter.second) {
            return false;
        }
    }
    
    // Check performance thresholds
    for (const auto& threshold : pImpl->performanceThresholds) {
        if (entry.isPerformanceEvent && 
            (entry.processingTime.count() < threshold.first || entry.processingTime.count() > threshold.second)) {
            return false;
        }
    }
    
    // Check metric thresholds
    for (const auto& threshold : pImpl->metricThresholds) {
        auto it = entry.metrics.find(threshold.first);
        if (it != entry.metrics.end()) {
            if (it->second < threshold.second.first || it->second > threshold.second.second) {
                return false;
            }
        } else {
            return false; // Metric not found
        }
    }
    
    // Check simulation step filters
    bool stepMatch = true;
    if (!pImpl->simulationStepFilters.empty()) {
        stepMatch = false;
        for (const auto& filter : pImpl->simulationStepFilters) {
            if (entry.simulationStep >= filter.first && entry.simulationStep <= filter.second) {
                stepMatch = true;
                break;
            }
        }
    }
    
    if (!stepMatch) return false;
    
    // Check agent filters
    bool agentMatch = true;
    if (!pImpl->agentFilters.empty()) {
        agentMatch = false;
        for (const auto& filter : pImpl->agentFilters) {
            if (filter == entry.agentId) {
                agentMatch = true;
                break;
            }
        }
    }
    
    if (!agentMatch) return false;
    
    // Check region filters
    bool regionMatch = true;
    if (!pImpl->regionFilters.empty()) {
        regionMatch = false;
        for (const auto& filter : pImpl->regionFilters) {
            if (filter == entry.brainRegionId) {
                regionMatch = true;
                break;
            }
        }
    }
    
    if (!regionMatch) return false;
    
    // Check time filters
    for (const auto& filter : pImpl->timeFilters) {
        if (entry.timestamp < filter.first || entry.timestamp > filter.second) {
            return false;
        }
    }
    
    return true;
}

void LogFilter::setCaseInsensitive(bool caseInsensitive) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->caseInsensitive = caseInsensitive;
}

void LogFilter::setRegexMatching(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->regexMatching = enabled;
}

// AdvancedLogger implementation
struct AdvancedLogger::Impl {
    std::mutex mutex;
    std::vector<std::shared_ptr<ILoggerSink>> sinks;
    std::shared_ptr<LogFilter> globalFilter;
    std::shared_ptr<LogAggregator> globalAggregator;
    LogLevel minLevel = LogLevel::Info;
    LogCategory defaultCategory = LogCategory::Custom;
    bool enableThreadId = true;
    bool enableThreadName = true;
    bool enablePerformanceLogging = true;
    bool enableMetadataLogging = true;
    std::atomic<uint64_t> threadCounter{0};
    std::unordered_map<std::thread::id, uint64_t> threadIds;
};

AdvancedLogger::AdvancedLogger() : pImpl(std::make_unique<Impl>()) {}

AdvancedLogger::~AdvancedLogger() = default;

void AdvancedLogger::addSink(std::shared_ptr<ILoggerSink> sink) {
    if (sink) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->sinks.push_back(sink);
    }
}

void AdvancedLogger::removeSink(const ILoggerSink* sink) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.erase(std::remove_if(pImpl->sinks.begin(), pImpl->sinks.end(),
                                    [sink](const std::shared_ptr<ILoggerSink>& s) { return s.get() == sink; }),
                     pImpl->sinks.end());
}

void AdvancedLogger::clearSinks() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.clear();
}

void AdvancedLogger::setGlobalFilter(std::shared_ptr<LogFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
}

void AdvancedLogger::removeGlobalFilter() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter.reset();
}

void AdvancedLogger::setGlobalAggregator(std::shared_ptr<LogAggregator> aggregator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalAggregator = aggregator;
}

void AdvancedLogger::removeGlobalAggregator() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalAggregator.reset();
}

void AdvancedLogger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minLevel = level;
}

void AdvancedLogger::setDefaultCategory(LogCategory category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->defaultCategory = category;
}

void AdvancedLogger::setEnableThreadId(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableThreadId = enabled;
}

void AdvancedLogger::setEnableThreadName(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableThreadName = enabled;
}

void AdvancedLogger::setEnablePerformanceLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enablePerformanceLogging = enabled;
}

void AdvancedLogger::setEnableMetadataLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableMetadataLogging = enabled;
}

void AdvancedLogger::log(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function, LogCategory category,
                         const std::unordered_map<std::string, std::string>& metadata) {
    if (level < pImpl->minLevel) {
        return;
    }
    
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = pImpl->enableThreadId ? getCurrentThreadId() : 0;
    entry.threadName = pImpl->enableThreadName ? getThreadName() : "";
    entry.category = category;
    entry.metadata = metadata;
    
    // Check global filter
    if (pImpl->globalFilter && !pImpl->globalFilter->shouldLog(entry)) {
        return;
    }
    
    // Add to global aggregator if set
    if (pImpl->globalAggregator) {
        pImpl->globalAggregator->processEntry(entry);
    }
    
    // Send to all sinks
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& sink : pImpl->sinks) {
        sink->write(entry);
    }
}

void AdvancedLogger::trace(const std::string& message, const std::string& file, int line,
                          const std::string& function, LogCategory category) {
    log(LogLevel::Trace, message, file, line, function, category);
}

void AdvancedLogger::debug(const std::string& message, const std::string& file, int line,
                          const std::string& function, LogCategory category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void AdvancedLogger::info(const std::string& message, const std::string& file, int line,
                         const std::string& function, LogCategory category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void AdvancedLogger::warning(const std::string& message, const std::string& file, int line,
                            const std::string& function, LogCategory category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void AdvancedLogger::error(const std::string& message, const std::string& file, int line,
                          const std::string& function, LogCategory category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void AdvancedLogger::critical(const std::string& message, const std::string& file, int line,
                              const std::string& function, LogCategory category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

void AdvancedLogger::logPerformance(const std::string& operation,
                                   std::chrono::duration<double> processingTime,
                                   const std::unordered_map<std::string, double>& metrics) {
    if (!pImpl->enablePerformanceLogging) {
        return;
    }
    
    std::string message = "Performance: " + operation + " took " + formatDuration(processingTime.count());
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.isPerformanceEvent = true;
    entry.processingTime = processingTime;
    entry.metrics = metrics;
    entry.category = LogCategory::Performance;
    
    log(entry.level, entry.message, "", 0, "", entry.category, entry.metadata);
}

void AdvancedLogger::logNeuralEvent(const std::string& eventType, uint64_t neuronId,
                                   uint64_t regionId, double value,
                                   const std::unordered_map<std::string, std::string>& metadata) {
    std::string message = "Neural Event: " + eventType + " (Neuron: " + 
                         std::to_string(neuronId) + ", Region: " + 
                         std::to_string(regionId) + ")";
    
    if (value != 0.0) {
        message += " Value: " + std::to_string(value);
    }
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = LogCategory::Neural;
    entry.metadata = metadata;
    entry.agentId = regionId; // Using region ID as agent ID for this example
    
    log(entry.level, entry.message, "", 0, "", entry.category, entry.metadata);
}

void AdvancedLogger::logSimulationStep(uint64_t step, double time,
                                       const std::unordered_map<std::string, double>& metrics) {
    std::string message = "Simulation Step: " + std::to_string(step) + " (Time: " + 
                         std::to_string(time) + ")";
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = LogCategory::Simulation;
    entry.metrics = metrics;
    entry.simulationStep = step;
    
    log(entry.level, entry.message, "", 0, "", entry.category, entry.metadata);
}

void AdvancedLogger::logMemoryEvent(const std::string& eventType, uint64_t bytes,
                                   const std::string& context) {
    std::string message = "Memory: " + eventType + " " + 
                         (bytes >= 1024 * 1024 ? 
                          std::to_string(bytes / (1024 * 1024)) + " MB" :
                          bytes >= 1024 ? std::to_string(bytes / 1024) + " KB" : 
                          std::to_string(bytes) + " bytes");
    
    if (!context.empty()) {
        message += " (Context: " + context + ")";
    }
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = LogCategory::Memory;
    
    log(entry.level, entry.message, "", 0, "", entry.category, entry.metadata);
}

void AdvancedLogger::logHealthCheck(const std::string& checkName, bool passed,
                                   const std::string& details) {
    std::string message = "Health Check: " + checkName + " - " +
                         (passed ? "PASSED" : "FAILED");
    
    if (!details.empty()) {
        message += " (" + details + ")";
    }
    
    LogLevel level = passed ? LogLevel::Info : LogLevel::Warning;
    
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = LogCategory::Validation;
    
    log(entry.level, entry.message, "", 0, "", entry.category, entry.metadata);
}

bool AdvancedLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->minLevel;
}

AdvancedLogger& AdvancedLogger::getGlobal() {
    static AdvancedLogger globalLogger;
    return globalLogger;
}

void AdvancedLogger::setGlobal(std::shared_ptr<AdvancedLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

AdvancedLogger::LogAnalysis AdvancedLogger::analyzeLogs(const std::vector<EnhancedLogEntry>& entries) const {
    LogAnalysis analysis;
    analysis.totalEntries = entries.size();
    
    for (const auto& entry : entries) {
        if (static_cast<int>(entry.level) < 6) { // Ensure index is valid
            analysis.entriesByLevel[static_cast<int>(entry.level)]++;
        }
        
        std::string categoryStr;
        switch (entry.category) {
            case LogCategory::System: categoryStr = "System"; break;
            case LogCategory::Neural: categoryStr = "Neural"; break;
            case LogCategory::Simulation: categoryStr = "Simulation"; break;
            case LogCategory::Performance: categoryStr = "Performance"; break;
            case LogCategory::Memory: categoryStr = "Memory"; break;
            case LogCategory::Configuration: categoryStr = "Configuration"; break;
            case LogCategory::Validation: categoryStr = "Validation"; break;
            case LogCategory::World: categoryStr = "World"; break;
            case LogCategory::Agent: categoryStr = "Agent"; break;
            case LogCategory::Development: categoryStr = "Development"; break;
            default: categoryStr = "Custom"; break;
        }
        analysis.entriesByCategory[categoryStr]++;
        
        analysis.entriesByFile[entry.file]++;
        
        if (entry.isPerformanceEvent) {
            analysis.performanceEvents++;
        }
        
        // Aggregate metrics
        for (const auto& metric : entry.metrics) {
            analysis.averageMetrics[metric.first] += metric.second;
        }
        
        analysis.totalProcessingTime += entry.processingTime;
        
        // Track earliest and latest timestamps
        if (analysis.totalEntries == 0 || entry.timestamp < analysis.earliestEntry) {
            analysis.earliestEntry = entry.timestamp;
        }
        if (entry.timestamp > analysis.latestEntry) {
            analysis.latestEntry = entry.timestamp;
        }
    }
    
    // Calculate average metrics
    for (auto& metric : analysis.averageMetrics) {
        metric.second /= analysis.totalEntries;
    }
    
    return analysis;
}

bool AdvancedLogger::exportLogs(const std::string& outputPath, LogFormat format,
                               const std::shared_ptr<LogFilter>& filter) {
    std::vector<EnhancedLogEntry> entries;
    
    // In a real implementation, this would retrieve logs from storage
    // For brevity, this is a placeholder
    
    std::string extension;
    switch (format) {
        case LogFormat::PlainText: extension = ".txt"; break;
        case LogFormat::JSON: extension = ".json"; break;
        case LogFormat::CSV: extension = ".csv"; break;
        case LogFormat::XML: extension = ".xml"; break;
        case LogFormat::HTML: extension = ".html"; break;
    }
    
    std::string fullPath = outputPath + extension;
    std::ofstream file(fullPath);
    
    if (!file.is_open()) {
        return false;
    }
    
    // Write logs (simplified for brevity)
    for (const auto& entry : entries) {
        file << entry.message << "\n";
    }
    
    file.close();
    return true;
}

AdvancedLogger::CorrelationResult AdvancedLogger::correlateLogs(
    const std::vector<EnhancedLogEntry>& entries,
    const std::string& correlationType,
    const std::unordered_map<std::string, std::string>& options) {
    CorrelationResult result;
    result.correlationType = correlationType;
    
    // Simplified implementation - would need actual correlation logic
    for (size_t i = 0; i < entries.size(); ++i) {
        result.correlatedEntries.push_back(i);
    }
    
    // Calculate correlation strength (simplified)
    result.correlationStrength = entries.size() > 1 ? 0.85 : 1.0;
    
    return result;
}

AdvancedLogger::TroubleshootingResult AdvancedLogger::troubleshootFromLogs(
    const std::vector<EnhancedLogEntry>& entries,
    const std::string& problemDescription) {
    TroubleshootingResult result;
    
    // Simplified troubleshooting logic
    for (const auto& entry : entries) {
        if (entry.level >= LogLevel::Error || entry.level == LogLevel::Warning) {
            result.identifiedIssues.push_back("Issue at " + getCurrentTimestamp() + ": " + entry.message);
            
            // Categorize issues
            if (entry.category == LogCategory::Neural) {
                result.issueCategories["neural"].push_back(entry.message);
            } else if (entry.category == LogCategory::Memory) {
                result.issueCategories["memory"].push_back(entry.message);
            } else if (entry.category == LogCategory::Performance) {
                result.issueCategories["performance"].push_back(entry.message);
            } else {
                result.issueCategories["other"].push_back(entry.message);
            }
        }
    }
    
    // Suggest solutions based on issue types
    for (const auto& category : result.issueCategories) {
        if (category.first == "neural") {
            result.suggestedSolutions.push_back("Check neural network connections and weights");
        } else if (category.first == "memory") {
            result.suggestedSolutions.push_back("Monitor memory usage and consider garbage collection");
        } else if (category.first == "performance") {
            result.suggestedSolutions.push_back("Optimize performance-critical operations");
        }
    }
    
    return result;
}

// AdvancedLogStream implementation
AdvancedLogStream::AdvancedLogStream(LogLevel level, const std::string& file, int line,
                                    const std::string& function, LogCategory category,
                                    const std::unordered_map<std::string, std::string>& metadata)
    : level_(level), file_(file), line_(line), function_(function),
      category_(category), agentId_(0), regionId_(0), simulationStep_(0) {
    metadata_ = metadata;
}

AdvancedLogStream::~AdvancedLogStream() {
    nlm::AdvancedLogger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_, metadata_);
}

AdvancedLogStream& AdvancedLogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withMetric(const std::string& key, double value) {
    metrics_[key] = value;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withAgentId(uint64_t agentId) {
    agentId_ = agentId;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withRegionId(uint64_t regionId) {
    regionId_ = regionId;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withSimulationStep(uint64_t step) {
    simulationStep_ = step;
    return *this;
}

} // namespace nlm
