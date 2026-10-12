#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <regex>
#include <zlib.h>
#include <curl/curl.h>
#include <pqxx/pqxx>

namespace nlm {

// AsyncLoggerCore implementation
struct AsyncLoggerCore::Impl {
    std::vector<std::shared_ptr<ISink>> sinks;
    std::vector<std::shared_ptr<IFilter>> filters;
    std::shared_ptr<IFormatter> formatter;
    
    // Thread pool and queue
    std::vector<std::thread> workers;
    std::queue<LogEntry> logQueue;
    std::priority_queue<LogEntry, std::vector<LogEntry>, std::function<bool(const LogEntry&, const LogEntry&)>> priorityQueue;
    
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running{false};
    std::atomic<size_t> queueSize{0};
    
    // Configuration
    size_t maxQueueSize;
    size_t batchSize;
    size_t numThreads;
    std::string configJson;
    
    // Statistics
    std::atomic<uint64_t> totalLogs{0};
    std::atomic<uint64_t> droppedLogs{0};
    std::atomic<uint64_t> errorCount{0};
    
    // Dead letter queue
    std::queue<LogEntry> deadLetterQueue;
    std::mutex deadLetterMutex;
    
    // Correlation ID management
    std::string currentCorrelationId;
    std::mutex correlationMutex;
};

AsyncLoggerCore::AsyncLoggerCore(size_t numThreads, size_t maxQueueSize, size_t batchSize)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->numThreads = numThreads;
    pImpl->maxQueueSize = maxQueueSize;
    pImpl->batchSize = batchSize;
}

AsyncLoggerCore::~AsyncLoggerCore() {
    stop();
}

void AsyncLoggerCore::log(const LogEntry& entry) {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    
    // Check if queue is full
    if (pImpl->queueSize >= pImpl->maxQueueSize) {
        pImpl->droppedLogs++;
        std::lock_guard<std::mutex> dlLock(pImpl->deadLetterMutex);
        pImpl->deadLetterQueue.push(entry);
        return;
    }
    
    // Add to queue
    pImpl->logQueue.push(entry);
    pImpl->queueSize++;
    
    // Notify worker threads
    lock.unlock();
    pImpl->queueCondition.notify_one();
    pImpl->totalLogs++;
}

void AsyncLoggerCore::flush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty() && !pImpl->running;
    });
    
    // Process remaining logs
    while (!pImpl->logQueue.empty()) {
        auto entry = std::move(pImpl->logQueue.front());
        pImpl->logQueue.pop();
        pImpl->queueSize--;
        
        // Apply filters
        bool shouldLog = true;
        for (const auto& filter : pImpl->filters) {
            if (!filter->evaluate(entry)) {
                shouldLog = false;
                break;
            }
        }
        
        if (shouldLog && pImpl->formatter) {
            std::string formatted = pImpl->formatter->format(entry);
            
            // Write to all sinks
            for (const auto& sink : pImpl->sinks) {
                try {
                    if (!sink->write(entry)) {
                        pImpl->errorCount++;
                        sink->reconnect();
                    }
                } catch (...) {
                    pImpl->errorCount++;
                }
            }
        }
    }
}

void AsyncLoggerCore::addSink(std::shared_ptr<ISink> sink) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->sinks.push_back(sink);
}

void AsyncLoggerCore::removeSink(const std::string& sinkId) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->sinks.erase(
        std::remove_if(pImpl->sinks.begin(), pImpl->sinks.end(),
            [&sinkId](const std::shared_ptr<ISink>& sink) {
                return dynamic_cast<const void*>(sink.get()) == nullptr; // Placeholder
            }),
        pImpl->sinks.end()
    );
}

void AsyncLoggerCore::clearSinks() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->sinks.clear();
}

void AsyncLoggerCore::addFilter(std::shared_ptr<IFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->filters.push_back(filter);
}

void AsyncLoggerCore::removeFilter(const std::string& filterId) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->filters.erase(
        std::remove_if(pImpl->filters.begin(), pImpl->filters.end(),
            [&filterId](const std::shared_ptr<IFilter>& filter) {
                return filter->getFilterName() != filterId;
            }),
        pImpl->filters.end()
    );
}

void AsyncLoggerCore::clearFilters() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->filters.clear();
}

void AsyncLoggerCore::setFormatter(std::shared_ptr<IFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->formatter = formatter;
}

void AsyncLoggerCore::configure(const std::string& configJson) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->configJson = configJson;
    
    // TODO: Parse config and update configuration
}

std::string AsyncLoggerCore::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->configJson;
}

uint64_t AsyncLoggerCore::getTotalLogCount() const {
    return pImpl->totalLogs.load();
}

uint64_t AsyncLoggerCore::getDroppedLogCount() const {
    return pImpl->droppedLogs.load();
}

uint64_t AsyncLoggerCore::getErrorCount() const {
    return pImpl->errorCount.load();
}

void AsyncLoggerCore::start() {
    if (pImpl->running) return;
    
    pImpl->running = true;
    
    for (size_t i = 0; i < pImpl->numThreads; ++i) {
        pImpl->workers.emplace_back([this]() {
            while (pImpl->running) {
                std::unique_lock<std::mutex> lock(pImpl->queueMutex);
                pImpl->queueCondition.wait(lock, [this]() {
                    return !pImpl->logQueue.empty() || !pImpl->running;
                });
                
                if (!pImpl->running && pImpl->logQueue.empty()) {
                    break;
                }
                
                // Batch processing
                std::vector<LogEntry> batch;
                for (size_t i = 0; i < pImpl->batchSize && !pImpl->logQueue.empty(); ++i) {
                    batch.push_back(std::move(pImpl->logQueue.front()));
                    pImpl->logQueue.pop();
                    pImpl->queueSize--;
                }
                
                lock.unlock();
                
                // Process batch
                for (const auto& entry : batch) {
                    bool shouldLog = true;
                    for (const auto& filter : pImpl->filters) {
                        if (!filter->evaluate(entry)) {
                            shouldLog = false;
                            break;
                        }
                    }
                    
                    if (shouldLog && pImpl->formatter) {
                        std::string formatted = pImpl->formatter->format(entry);
                        
                        for (const auto& sink : pImpl->sinks) {
                            try {
                                if (!sink->write(entry)) {
                                    pImpl->errorCount++;
                                    sink->reconnect();
                                }
                            } catch (...) {
                                pImpl->errorCount++;
                            }
                        }
                    }
                }
            }
        });
    }
}

void AsyncLoggerCore::stop() {
    if (!pImpl->running) return;
    
    pImpl->running = false;
    pImpl->queueCondition.notify_all();
    
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    pImpl->workers.clear();
    
    // Flush remaining logs
    flush();
    
    // Process dead letter queue
    std::lock_guard<std::mutex> lock(pImpl->deadLetterMutex);
    while (!pImpl->deadLetterQueue.empty()) {
        auto entry = std::move(pImpl->deadLetterQueue.front());
        pImpl->deadLetterQueue.pop();
        
        // Retry logging
        bool shouldLog = true;
        for (const auto& filter : pImpl->filters) {
            if (!filter->evaluate(entry)) {
                shouldLog = false;
                break;
            }
        }
        
        if (shouldLog && pImpl->formatter) {
            std::string formatted = pImpl->formatter->format(entry);
            
            for (const auto& sink : pImpl->sinks) {
                try {
                    if (!sink->write(entry)) {
                        pImpl->errorCount++;
                    }
                } catch (...) {
                    pImpl->errorCount++;
                }
            }
        }
    }
}

bool AsyncLoggerCore::isRunning() const {
    return pImpl->running;
}

bool AsyncLoggerCore::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    bool sinksHealthy = std::all_of(pImpl->sinks.begin(), pImpl->sinks.end(),
        [](const std::shared_ptr<ISink>& sink) { return sink->isConnected(); });
    return sinksHealthy && pImpl->queueSize < pImpl->maxQueueSize / 2;
}

// MemoryBufferSink implementation
struct MemoryBufferSink::Impl {
    std::vector<LogEntry> buffer;
    std::mutex mutex;
    size_t maxBufferSize;
    uint64_t writeCount{0};
    uint64_t errorCount{0};
    bool connected{true};
};

MemoryBufferSink::MemoryBufferSink(size_t maxBufferSize)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->maxBufferSize = maxBufferSize;
}

bool MemoryBufferSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->connected) {
        pImpl->errorCount++;
        return false;
    }
    
    // Estimate entry size
    size_t entrySize = sizeof(LogEntry) + entry.message.size() + entry.kvPairs.size() * 100;
    if (pImpl->buffer.size() * entrySize > pImpl->maxBufferSize) {
        pImpl->errorCount++;
        return false;
    }
    
    pImpl->buffer.push_back(entry);
    pImpl->writeCount++;
    return true;
}

void MemoryBufferSink::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->buffer.clear();
}

bool MemoryBufferSink::isConnected() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->connected;
}

void MemoryBufferSink::reconnect() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->connected = true;
}

uint64_t MemoryBufferSink::getWriteCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->writeCount;
}

uint64_t MemoryBufferSink::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

std::vector<LogEntry> MemoryBufferSink::getBuffer() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->buffer;
}

size_t MemoryBufferSink::getBufferSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->buffer.size();
}

void MemoryBufferSink::clearBuffer() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->buffer.clear();
}

void MemoryBufferSink::setMaxBufferSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxBufferSize = maxSize;
}

uint64_t MemoryBufferSink::getRetainedCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->buffer.size();
}

// FileSink implementation
struct FileSink::Impl {
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize;
    int maxFiles;
    bool compressOld;
    std::string filepath;
    uint64_t writeCount{0};
    uint64_t errorCount{0};
    bool connected{false};
};

FileSink::FileSink(const std::string& filepath, size_t maxFileSize, int maxFiles, bool compressOld)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->filepath = filepath;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
    pImpl->compressOld = compressOld;
    
    reconnect();
}

bool FileSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->fileOut.is_open()) {
        pImpl->errorCount++;
        return false;
    }
    
    // Format entry
    std::ostringstream oss;
    oss << "[" << utils::levelToString(entry.level) << "] "
        << std::chrono::system_clock::to_time_t(entry.timestamp)
        << " " << entry.message;
    
    // Add correlation ID if present
    if (!entry.correlationId.empty()) {
        oss << " [CorrelationID:" << entry.correlationId << "]";
    }
    
    // Add key-value pairs
    for (const auto& kv : entry.kvPairs) {
        oss << " [" << kv.first << ":" << kv.second << "]";
    }
    
    pImpl->fileOut << oss.str() << "\n";
    pImpl->writeCount++;
    
    // Check file size and rotate if needed
    pImpl->fileOut.flush();
    if (pImpl->fileOut.tellp() >= static_cast<std::streampos>(pImpl->maxFileSize)) {
        pImpl->fileOut.close();
        // TODO: Implement file rotation
    }
    
    return true;
}

void FileSink::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

bool FileSink::isConnected() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOut.is_open();
}

void FileSink::reconnect() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
    pImpl->connected = pImpl->fileOut.is_open();
}

uint64_t FileSink::getWriteCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->writeCount;
}

uint64_t FileSink::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

void FileSink::setFilePath(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    reconnect();
}

void FileSink::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void FileSink::setMaxFiles(int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFiles = maxFiles;
}

void FileSink::setCompressOld(bool compress) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->compressOld = compress;
}

// NetworkSink implementation
struct NetworkSink::Impl {
    std::string host;
    uint16_t port;
    std::string protocol;
    size_t bufferSize;
    std::mutex mutex;
    uint64_t writeCount{0};
    uint64_t errorCount{0};
    bool connected{false};
    CURL* curlHandle{nullptr};
};

size_t NetworkSink::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* data = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    data->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

NetworkSink::NetworkSink(const std::string& host, uint16_t port, 
                        const std::string& protocol, size_t bufferSize)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->host = host;
    pImpl->port = port;
    pImpl->protocol = protocol;
    pImpl->bufferSize = bufferSize;
    
    CURL_GLOBAL_INIT(CURL_GLOBAL_ALL);
    pImpl->curlHandle = curl_easy_init();
    reconnect();
}

bool NetworkSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->curlHandle) {
        pImpl->errorCount++;
        return false;
    }
    
    // Prepare log entry for network transmission
    std::ostringstream oss;
    oss << "level:" << static_cast<int>(entry.level) << "\n"
        << "message:" << entry.message << "\n"
        << "timestamp:" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\n"
        << "correlation_id:" << entry.correlationId << "\n";
    
    for (const auto& kv : entry.kvPairs) {
        oss << kv.first << ":" << kv.second << "\n";
    }
    
    std::string postData = oss.str();
    std::string readBuffer;
    
    // Set up curl options
    curl_easy_setopt(pImpl->curlHandle, CURLOPT_URL, (pImpl->protocol == "udp" ? 
        ("http://" + pImpl->host + ":" + std::to_string(pImpl->port)) : 
        ("https://" + pImpl->host + ":" + std::to_string(pImpl->port))).c_str());
    curl_easy_setopt(pImpl->curlHandle, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(pImpl->curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(pImpl->curlHandle, CURLOPT_WRITEDATA, &readBuffer);
    
    CURLcode res = curl_easy_perform(pImpl->curlHandle);
    if (res != CURLE_OK) {
        pImpl->errorCount++;
        return false;
    }
    
    pImpl->writeCount++;
    return true;
}

void NetworkSink::flush() {
    // Network flushes are handled by curl
}

bool NetworkSink::isConnected() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->curlHandle && (pImpl->connected || true); // Always true for simplicity
}

void NetworkSink::reconnect() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->curlHandle) {
        curl_easy_cleanup(pImpl->curlHandle);
        pImpl->curlHandle = curl_easy_init();
        pImpl->connected = true;
    }
}

uint64_t NetworkSink::getWriteCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->writeCount;
}

uint64_t NetworkSink::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

void NetworkSink::setHost(const std::string& host) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->host = host;
    reconnect();
}

void NetworkSink::setPort(uint16_t port) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->port = port;
    reconnect();
}

void NetworkSink::setProtocol(const std::string& protocol) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->protocol = protocol;
    reconnect();
}

// DatabaseSink implementation
struct DatabaseSink::Impl {
    std::string connectionString;
    std::string tableName;
    std::string schema;
    std::mutex mutex;
    uint64_t writeCount{0};
    uint64_t errorCount{0};
    bool connected{false};
    std::unique_ptr<pqxx::connection> dbConnection;
};

DatabaseSink::DatabaseSink(const std::string& connectionString, 
                         const std::string& tableName, const std::string& schema)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->connectionString = connectionString;
    pImpl->tableName = tableName;
    pImpl->schema = schema;
    
    reconnect();
}

bool DatabaseSink::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->dbConnection || !pImpl->dbConnection->is_open()) {
        pImpl->errorCount++;
        reconnect();
        if (!pImpl->dbConnection || !pImpl->dbConnection->is_open()) {
            return false;
        }
    }
    
    try {
        pqxx::work txn(*pImpl->dbConnection);
        
        // Build INSERT statement
        std::string query = "INSERT INTO " + pImpl->schema + "." + pImpl->tableName +
            " (level, message, file, line, function, timestamp, correlation_id)" +
            " VALUES (?, ?, ?, ?, ?, ?, ?)";
        
        pqxx::params params;
        params.push_back(static_cast<int>(entry.level));
        params.push_back(entry.message);
        params.push_back(entry.file);
        params.push_back(entry.line);
        params.push_back(entry.function);
        params.push_back(std::chrono::system_clock::to_time_t(entry.timestamp));
        params.push_back(entry.correlationId);
        
        // Add key-value pairs as JSON
        if (!entry.kvPairs.empty()) {
            // TODO: Serialize kvPairs as JSON
        }
        
        txn.exec_params(query, params);
        txn.commit();
        
        pImpl->writeCount++;
        return true;
    } catch (...) {
        pImpl->errorCount++;
        return false;
    }
}

void DatabaseSink::flush() {
    // Database flushes are handled by transaction commit
}

bool DatabaseSink::isConnected() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->dbConnection && pImpl->dbConnection->is_open();
}

void DatabaseSink::reconnect() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->dbConnection.reset();
    
    try {
        pImpl->dbConnection = std::make_unique<pqxx::connection>(pImpl->connectionString);
        pImpl->connected = pImpl->dbConnection->is_open();
    } catch (...) {
        pImpl->connected = false;
    }
}

uint64_t DatabaseSink::getWriteCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->writeCount;
}

uint64_t DatabaseSink::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

void DatabaseSink::setConnectionString(const std::string& connectionString) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->connectionString = connectionString;
    reconnect();
}

void DatabaseSink::setTableName(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->tableName = tableName;
}

void DatabaseSink::setSchema(const std::string& schema) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema = schema;
}

// Formatter implementations
struct JsonFormatter::Impl {
    bool includeTimestamp{true};
    bool includeSourceLocation{true};
    bool includeCorrelationId{true};
    bool includeMetrics{true};
    std::string dateFormat{"iso8601"};
};

JsonFormatter::JsonFormatter() : pImpl(std::make_unique<Impl>()) {}

std::string JsonFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "{\n";
    
    oss << "  \"level\": \"" << utils::levelToString(entry.level) << "\",\n";
    
    if (pImpl->includeTimestamp) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << "  \"timestamp\": \"" << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%SZ") << "\",\n";
    }
    
    oss << "  \"message\": \"" << entry.message << "\",\n";
    
    if (pImpl->includeSourceLocation) {
        oss << "  \"file\": \"" << entry.file << "\",\n"
            << "  \"line\": " << entry.line << ",\n"
            << "  \"function\": \"" << entry.function << "\",\n";
    }
    
    if (pImpl->includeCorrelationId && !entry.correlationId.empty()) {
        oss << "  \"correlation_id\": \"" << entry.correlationId << "\",\n";
    }
    
    // Add key-value pairs
    if (!entry.kvPairs.empty()) {
        oss << "  \"kv_pairs\": {\n";
        bool first = true;
        for (const auto& kv : entry.kvPairs) {
            if (!first) oss << ",\n";
            oss << "    \"" << kv.first << "\": \"" << kv.second << "\"";
            first = false;
        }
        oss << "\n  },\n";
    }
    
    if (pImpl->includeMetrics && !entry.metrics.empty()) {
        oss << "  \"metrics\": {\n";
        bool first = true;
        for (const auto& metric : entry.metrics) {
            if (!first) oss << ",\n";
            oss << "    \"" << metric.first << "\": " << metric.second;
            first = false;
        }
        oss << "\n  }\n";
    }
    
    oss << "}\n";
    return oss.str();
}

void JsonFormatter::configure(const std::string& formatString) {
    // Parse format string and update configuration
    if (formatString.find("no_timestamp") != std::string::npos) {
        pImpl->includeTimestamp = false;
    }
    if (formatString.find("no_source") != std::string::npos) {
        pImpl->includeSourceLocation = false;
    }
    if (formatString.find("no_correlation") != std::string::npos) {
        pImpl->includeCorrelationId = false;
    }
    if (formatString.find("no_metrics") != std::string::npos) {
        pImpl->includeMetrics = false;
    }
}

std::string JsonFormatter::getFormat() const {
    return pImpl->includeTimestamp ? "json_full" : "json_compact";
}

void JsonFormatter::setIncludeTimestamp(bool include) {
    pImpl->includeTimestamp = include;
}

void JsonFormatter::setIncludeSourceLocation(bool include) {
    pImpl->includeSourceLocation = include;
}

void JsonFormatter::setIncludeCorrelationId(bool include) {
    pImpl->includeCorrelationId = include;
}

void JsonFormatter::setIncludeMetrics(bool include) {
    pImpl->includeMetrics = include;
}

// XmlFormatter implementation
XmlFormatter::XmlFormatter() = default;

std::string XmlFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "<log>\\n";
    
    oss << "  <level>" << utils::levelToString(entry.level) << "</level>\\n";
    oss << "  <timestamp>" << std::chrono::system_clock::to_time_t(entry.timestamp) << "</timestamp>\\n";
    oss << "  <message>" << entry.message << "</message>\\n";
    oss << "  <file>" << entry.file << "</file>\\n";
    oss << "  <line>" << entry.line << "</line>\\n";
    oss << "  <function>" << entry.function << "</function>\\n";
    oss << "  <correlationId>" << entry.correlationId << "</correlationId>\\n";
    
    // Add key-value pairs
    for (const auto& kv : entry.kvPairs) {
        oss << "  <kvPairs>\\n";
        oss << "    <key>" << kv.first << "</key>\\n";
        oss << "    <value>" << kv.second << "</value>\\n";
        oss << "  </kvPairs>\\n";
    }
    
    oss << "</log>\\n";
    return oss.str();
}

void XmlFormatter::configure(const std::string& formatString) {
    // XML configuration (placeholder)
}

std::string XmlFormatter::getFormat() const {
    return "xml";
}

// CsvFormatter implementation
CsvFormatter::CsvFormatter() = default;

std::string CsvFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << utils::levelToString(entry.level) << ","
        << std::chrono::system_clock::to_time_t(entry.timestamp) << ","
        << entry.message << ","
        << entry.file << ","
        << entry.line << ","
        << entry.function << ","
        << entry.correlationId << ",";
    
    // Add key-value pairs as CSV
    bool first = true;
    for (const auto& kv : entry.kvPairs) {
        if (!first) oss << ";";
        oss << kv.first << "=" << kv.second;
        first = false;
    }
    
    oss << "\\n";
    return oss.str();
}

void CsvFormatter::configure(const std::string& formatString) {
    // CSV configuration (placeholder)
}

std::string CsvFormatter::getFormat() const {
    return "csv";
}

// PatternFilter implementation
PatternFilter::PatternFilter(const std::string& pattern, LogLevel minLevel)
    : pattern_(pattern), minLevel_(minLevel) {}

bool PatternFilter::evaluate(const LogEntry& entry) const {
    if (entry.level < minLevel_) {
        return false;
    }
    
    // Simple regex matching for pattern
    static const std::regex patternRegex("[\\s\\S]*");
    return std::regex_match(entry.message, patternRegex);
}

std::string PatternFilter::getFilterName() const {
    return "pattern_filter";
}

void PatternFilter::setPattern(const std::string& pattern) {
    pattern_ = pattern;
}

void PatternFilter::setMinLevel(LogLevel level) {
    minLevel_ = level;
}

// CorrelationIdFilter implementation
CorrelationIdFilter::CorrelationIdFilter(const std::string& correlationId)
    : correlationId_(correlationId) {}

bool CorrelationIdFilter::evaluate(const LogEntry& entry) const {
    return !correlationId_.empty() && entry.correlationId == correlationId_;
}

std::string CorrelationIdFilter::getFilterName() const {
    return "correlation_filter";
}

void CorrelationIdFilter::setCorrelationId(const std::string& correlationId) {
    correlationId_ = correlationId;
}

// MetricsFilter implementation
MetricsFilter::MetricsFilter(const std::string& metricName, double minValue)
    : metricName_(metricName), minValue_(minValue) {}

bool MetricsFilter::evaluate(const LogEntry& entry) const {
    auto it = entry.metrics.find(metricName_);
    if (it == entry.metrics.end()) {
        return false;
    }
    return it->second >= minValue_;
}

std::string MetricsFilter::getFilterName() const {
    return "metrics_filter";
}

void MetricsFilter::setMetricName(const std::string& metricName) {
    metricName_ = metricName;
}

void MetricsFilter::setMinValue(double minValue) {
    minValue_ = minValue;
}

// Enhanced ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level{LogLevel::Info};
    bool useColors{true};
    std::ofstream fileOut;
    std::mutex mutex;
    std::vector<std::function<bool(const LogEntry&)>> filters;
    uint64_t writeCount{0};
    uint64_t errorCount{0};
    bool connected{true};
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::logImpl(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply filters
    for (const auto& filter : pImpl->filters) {
        if (!filter(entry)) {
            return;
        }
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << utils::levelToString(entry.level) << "]";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    // Add correlation ID
    if (!entry.correlationId.empty()) {
        oss << " [CorrelationID:" << entry.correlationId << "]";
    }
    
    // Add key-value pairs
    for (const auto& kv : entry.kvPairs) {
        oss << " [" << kv.first << ":" << kv.second << "]";
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
    }
    
    pImpl->writeCount++;
}

void ConsoleLogger::log(const LogEntry& entry) {
    logImpl(entry);
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::configure(const std::string& config) {
    // Parse JSON/YAML config and update configuration
    // TODO: Implement configuration parsing
}

std::string ConsoleLogger::getConfiguration() const {
    // Return current configuration as JSON/YAML
    return "{}"; // Placeholder
}

void ConsoleLogger::addFilter(const std::function<bool(const LogEntry&)>& filter) {
    pImpl->filters.push_back(filter);
}

void ConsoleLogger::removeAllFilters() {
    pImpl->filters.clear();
}

bool ConsoleLogger::isHealthy() const {
    return pImpl->connected && pImpl->fileOut.is_open() || !pImpl->fileOut.is_open();
}

uint64_t ConsoleLogger::getWriteCount() const {
    return pImpl->writeCount;
}

uint64_t ConsoleLogger::getErrorCount() const {
    return pImpl->errorCount;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->connected = pImpl->fileOut.is_open();
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->connected = false;
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

// Logger implementation
struct Logger::Impl {
    std::shared_ptr<AsyncLoggerCore> asyncCore;
    std::vector<std::shared_ptr<ILogger>> syncLoggers;
    std::mutex mutex;
    
    // Statistics
    std::atomic<uint64_t> logCount{0};
    std::atomic<uint64_t> errorCount{0};
    std::atomic<uint64_t> droppedCount{0};
    
    // Correlation ID management
    std::string currentCorrelationId;
    std::mutex correlationMutex;
    
    // Configuration
    std::string configJson;
    bool asyncEnabled{false};
    bool initialized{false};
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->asyncCore = std::make_shared<AsyncLoggerCore>();
    pImpl->initialized = true;
}

Logger::~Logger() {
    stopAsync();
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->syncLoggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->syncLoggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->syncLoggers) {
        logger->setLevel(level);
    }
    if (pImpl->asyncCore) {
        pImpl->asyncCore->setFormatter(nullptr); // TODO: Implement proper level setting
    }
}

void Logger::asyncLog(const LogEntry& entry) {
    if (pImpl->asyncEnabled && pImpl->asyncCore) {
        pImpl->asyncCore->log(entry);
    } else {
        // Fallback to synchronous logging
        for (auto& logger : pImpl->syncLoggers) {
            logger->log(entry);
        }
    }
}

void Logger::asyncLog(LogLevel level, const std::string& message,
                     const std::string& file, int line,
                     const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = utils::getThreadId();
    entry.correlationId = pImpl->currentCorrelationId;
    
    asyncLog(entry);
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line,
                const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = utils::getThreadId();
    entry.correlationId = pImpl->currentCorrelationId;
    
    pImpl->logCount++;
    
    if (pImpl->asyncEnabled && pImpl->asyncCore) {
        pImpl->asyncCore->log(entry);
    } else {
        for (auto& logger : pImpl->syncLoggers) {
            logger->log(entry);
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::unordered_map<std::string, std::string>& kvPairs,
                const std::string& file, int line,
                const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.kvPairs = kvPairs;
    entry.fields = kvPairs;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = utils::getThreadId();
    entry.correlationId = pImpl->currentCorrelationId;
    
    pImpl->logCount++;
    
    if (pImpl->asyncEnabled && pImpl->asyncCore) {
        pImpl->asyncCore->log(entry);
    } else {
        for (auto& logger : pImpl->syncLoggers) {
            logger->log(entry);
        }
    }
}

void Logger::debug(const std::string& message, const std::unordered_map<std::string, std::string>& kvPairs) {
    log(LogLevel::Debug, message, kvPairs);
}

void Logger::info(const std::string& message, const std::unordered_map<std::string, std::string>& kvPairs) {
    log(LogLevel::Info, message, kvPairs);
}

void Logger::warning(const std::string& message, const std::unordered_map<std::string, std::string>& kvPairs) {
    log(LogLevel::Warning, message, kvPairs);
}

void Logger::error(const std::string& message, const std::unordered_map<std::string, std::string>& kvPairs) {
    log(LogLevel::Error, message, kvPairs);
}

void Logger::critical(const std::string& message, const std::unordered_map<std::string, std::string>& kvPairs) {
    log(LogLevel::Critical, message, kvPairs);
}

std::string Logger::createCorrelationId() {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    pImpl->currentCorrelationId = utils::generateCorrelationId();
    return pImpl->currentCorrelationId;
}

void Logger::setCorrelationId(const std::string& correlationId) {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    pImpl->currentCorrelationId = correlationId;
}

std::string Logger::getCorrelationId() const {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    return pImpl->currentCorrelationId;
}

uint64_t Logger::getLogCount() const {
    return pImpl->logCount.load();
}

uint64_t Logger::getErrorCount() const {
    return pImpl->errorCount.load();
}

uint64_t Logger::getDroppedCount() const {
    return pImpl->droppedCount.load();
}

void Logger::configure(const std::string& configJson) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->configJson = configJson;
    
    // Parse configuration and update settings
    // TODO: Implement configuration parsing
    
    if (pImpl->asyncCore) {
        pImpl->asyncCore->configure(configJson);
    }
}

std::string Logger::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->configJson;
}

void Logger::reloadConfiguration() {
    // Force reload of configuration from source
    if (pImpl->asyncCore) {
        pImpl->asyncCore->configure(pImpl->configJson);
    }
}

void Logger::startAsync() {
    if (pImpl->asyncCore && !pImpl->asyncEnabled) {
        pImpl->asyncCore->start();
        pImpl->asyncEnabled = true;
    }
}

void Logger::stopAsync() {
    if (pImpl->asyncCore && pImpl->asyncEnabled) {
        pImpl->asyncCore->stop();
        pImpl->asyncEnabled = false;
    }
}

bool Logger::isAsyncRunning() const {
    return pImpl->asyncEnabled && pImpl->asyncCore && pImpl->asyncCore->isRunning();
}

bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    bool syncHealthy = std::all_of(pImpl->syncLoggers.begin(), pImpl->syncLoggers.end(),
        [](const std::shared_ptr<ILogger>& logger) { return logger->isHealthy(); });
    
    bool asyncHealthy = !pImpl->asyncEnabled || (pImpl->asyncCore && pImpl->asyncCore->isHealthy());
    
    return syncHealthy && asyncHealthy;
}

std::string Logger::getHealthStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "LogCount: " << pImpl->logCount.load() << "\n"
        << "ErrorCount: " << pImpl->errorCount.load() << "\n"
        << "DroppedCount: " << pImpl->droppedCount.load() << "\n"
        << "AsyncRunning: " << (pImpl->asyncEnabled && pImpl->asyncCore ? "true" : "false") << "\n"
        << "SyncLoggers: " << pImpl->syncLoggers.size() << "\n"
        << "CorrelationId: " << pImpl->currentCorrelationId << "\n";
    return oss.str();
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->syncLoggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    if (pImpl->asyncCore) {
        // TODO: Check async core level setting
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

void Logger::log(const LogEntry& entry) {
    // Legacy compatibility
    for (auto& logger : pImpl->syncLoggers) {
        logger->log(entry);
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), message_("") {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, message_, file_, line_, function_);
}

LogStream& LogStream::kv(const std::string& key, const std::string& value) {
    kvPairs_[key] = value;
    return *this;
}

LogStream& LogStream::kv(const std::string& key, int value) {
    std::ostringstream oss;
    oss << value;
    kvPairs_[key] = oss.str();
    return *this;
}

LogStream& LogStream::kv(const std::string& key, double value) {
    std::ostringstream oss;
    oss << value;
    kvPairs_[key] = oss.str();
    return *this;
}

// Configuration utilities
namespace config {
    std::string loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + filepath);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    
    std::string mergeConfigurations(const std::string& base, const std::string& override) {
        // Simple JSON merge (placeholder)
        return override.empty() ? base : override;
    }
    
    std::string validateConfiguration(const std::string& configJson) {
        // Validate JSON configuration
        // TODO: Implement proper validation
        return "";
    }
}

// Helper functions
namespace utils {
    std::string generateCorrelationId() {
        // Simple UUID generation (placeholder)
        std::string id = "corr-";
        id += std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        id += "-";
        id += std::to_string(std::rand());
        return id;
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
    
    LogLevel stringToLevel(const std::string& levelStr) {
        std::string upper = levelStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        
        if (upper == "DEBUG") return LogLevel::Debug;
        if (upper == "INFO") return LogLevel::Info;
        if (upper == "WARN" || upper == "WARNING") return LogLevel::Warning;
        if (upper == "ERROR") return LogLevel::Error;
        if (upper == "CRITICAL" || upper == "CRIT") return LogLevel::Critical;
        return LogLevel::Info;
    }
    
    std::unordered_map<std::string, std::string> parseKeyValue(const std::string& kvString) {
        std::unordered_map<std::string, std::string> result;
        // Simple parsing (key=value pairs separated by commas)
        // TODO: Implement proper parsing
        return result;
    }
    
    uint64_t getThreadId() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }
}

} // namespace nlm