// Enhanced error handling implementation
// Implementation of advanced error handling and logging system

#include "EnhancedErrorHandling.hpp"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <queue>
#include <condition_variable>
#include <unordered_set>
#include <regex>
#include <json.hpp> // For JSON export (assuming nlohmann/json is available)

using json = nlohmann::json;

namespace nlm {
namespace error {

// ErrorContext implementation
void ErrorContext::addParameter(const std::string& name, const std::string& value) {
    parameters_[name] = value;
}

void ErrorContext::addParameter(const std::string& name, int value) {
    parameters_[name] = std::to_string(value);
}

void ErrorContext::addParameter(const std::string& name, double value) {
    parameters_[name] = std::to_string(value);
}

void ErrorContext::addParameter(const std::string& name, bool value) {
    parameters_[name] = value ? "true" : "false";
}

// ExceptionHandler implementation
namespace {

// Type-erased exception handler registration
template<typename ExceptionType>
void registerHandlerInternal(
    std::unordered_map<std::string, std::function<RecoveryAction(const std::exception&)>>& handlers,
    std::function<RecoveryAction(const ExceptionType&)> handler) {
    
    std::string typeName = typeid(ExceptionType).name();
    handlers[typeName] = [handler](const std::exception& ex) -> RecoveryAction {
        return handler(static_cast<const ExceptionType&>(ex));
    };
}

// Type-erased exception handling
template<typename ExceptionType>
RecoveryAction handleExceptionInternal(
    const ExceptionType& exception,
    std::unordered_map<std::string, std::function<RecoveryAction(const std::exception&)>>& handlers,
    ExceptionHandler::Statistics& stats) {
    
    std::lock_guard<std::mutex> lock(std::mutex_); // Using global mutex for simplicity
    stats.total_exceptions++;
    
    std::string typeName = typeid(ExceptionType).name();
    auto it = handlers.find(typeName);
    if (it != handlers.end()) {
        stats.handled_exceptions++;
        stats.exception_counts[typeName]++;
        return it->second(exception);
    }
    
    return ExceptionHandler::RecoveryAction::Abort;
}

} // namespace anonymous

template<typename ExceptionType>
void ExceptionHandler::registerHandler(std::function<RecoveryAction(const ExceptionType&)> handler) {
    registerHandlerInternal(handler, handlers_);
}

template<typename ExceptionType>
RecoveryAction ExceptionHandler::handle(const ExceptionType& exception) {
    return handleExceptionInternal(exception, handlers_, stats_);
}

void ExceptionHandler::logException(const std::exception& exception, const ErrorContext& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Create log message
    std::ostringstream oss;
    oss << "Exception: " << exception.what() << "\n";
    oss << "Operation: " << context.getOperation() << "\n";
    oss << "Component: " << context.getComponent() << "\n";
    oss << "Parameters: ";
    const auto& params = context.getParameters();
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (it != params.begin()) oss << ", ";
        oss << it->first << "=" << it->second;
    }
    oss << "\n";
    oss << "Timestamp: " << std::chrono::system_clock::to_time_t(context.getTimestamp());
    
    // Update statistics
    stats_.total_exceptions++;
    
    // Log to global logger (simplified)
    NLM_LOG_ERROR(oss.str());
}

// LogStream implementation
ErrorStream::ErrorStream(const std::string& operation, const std::string& component)
    : context_(operation, component) {}

ErrorStream::~ErrorStream() {
    if (!stream_.str().empty()) {
        context_.setMessage(stream_.str());
        globalExceptionHandler.logException(context_, context_);
    }
}

} // namespace error

// AsyncLogger implementation
namespace logging {

struct AsyncLogger::Impl {
    std::queue<EnhancedLogEntry> logQueue;
    std::queue<EnhancedLogEntry> processedQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::thread> workerThreads;
    std::atomic<bool> running{true};
    std::atomic<size_t> queueSize{0};
    size_t maxQueueSize;
    size_t workerThreadsCount;
    
    LogLevel level;
    std::string format;
    std::function<bool(const EnhancedLogEntry&)> filter;
    std::vector<std::function<void(const EnhancedLogEntry&)>> handlers;
    
    struct Stats {
        size_t totalProcessed = 0;
        size_t totalDropped = 0;
        double averageProcessingTime = 0.0;
    } stats;
    
    Impl(size_t maxQueueSize, size_t workerThreadsCount)
        : maxQueueSize(maxQueueSize), workerThreadsCount(workerThreadsCount), level(LogLevel::Info) {}
    
    ~Impl() {
        running = false;
        queueCondition.notify_all();
        for (auto& thread : workerThreads) {
            thread.join();
        }
    }
    
    void workerFunction() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this]() {
                return !logQueue.empty() || !running;
            });
            
            if (!running && logQueue.empty()) {
                break;
            }
            
            if (!logQueue.empty()) {
                auto entry = std::move(logQueue.front());
                logQueue.pop();
                queueSize--;
                
                lock.unlock();
                
                // Apply filter
                if (filter && !filter(entry)) {
                    lock.lock();
                    continue;
                }
                
                // Process entry
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Apply handlers
                for (auto& handler : handlers) {
                    handler(entry);
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto processingTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
                
                std::lock_guard<std::mutex> statsLock(queueMutex);
                stats.totalProcessed++;
                stats.averageProcessingTime = (stats.averageProcessingTime * (stats.totalProcessed - 1) + processingTime) / stats.totalProcessed;
                
                lock.lock();
            }
        }
    }
};

AsyncLogger::AsyncLogger(size_t queue_size, size_t worker_threads)
    : pImpl(std::make_unique<Impl>(queue_size, worker_threads)) {
    
    for (size_t i = 0; i < worker_threads; ++i) {
        pImpl->workerThreads.emplace_back([this]() {
            pImpl->workerFunction();
        });
    }
}

AsyncLogger::~AsyncLogger() = default;

void AsyncLogger::log(const EnhancedLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    
    if (pImpl->queueSize >= pImpl->maxQueueSize) {
        pImpl->stats.totalDropped++;
        return;
    }
    
    pImpl->logQueue.push(entry);
    pImpl->queueSize++;
    pImpl->queueCondition.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

void AsyncLogger::setFilter(std::function<bool(const EnhancedLogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void AsyncLogger::addHandler(std::function<void(const EnhancedLogEntry&)> handler) {
    pImpl->handlers.push_back(std::move(handler));
}

void AsyncLogger::flush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty();
    });
}

AsyncLogger::QueueStats AsyncLogger::getQueueStats() const {
    QueueStats stats;
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    stats.queue_size = pImpl->maxQueueSize;
    stats.pending_items = pImpl->queueSize;
    stats.processed_items = pImpl->stats.totalProcessed;
    stats.average_processing_time_ms = pImpl->stats.averageProcessingTime;
    return stats;
}

} // namespace logging

// LogAggregator implementation
void logging::LogAggregator::addEntry(const EnhancedLogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.push_back(entry);
}

logging::LogAggregator::AggregatedStats logging::LogAggregator::getStats() const {
    AggregatedStats stats;
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (entries_.empty()) {
        return stats;
    }
    
    stats.total_entries = entries_.size();
    stats.oldest_entry = entries_.front().timestamp;
    stats.newest_entry = entries_.back().timestamp;
    
    for (const auto& entry : entries_) {
        stats.level_counts[entry.level]++;
        // Extract component from function name or message
        std::string component = "unknown";
        if (!entry.function.empty()) {
            // Extract function name (last part before :: or last word)
            size_t colon_pos = entry.function.find_last_of("::");
            if (colon_pos != std::string::npos) {
                component = entry.function.substr(colon_pos + 2);
            } else {
                component = entry.function;
            }
        }
        stats.component_counts[component]++;
        
        if (entry.level >= LogLevel::Error) {
            stats.error_messages.push_back(entry.message);
        }
    }
    
    return stats;
}

void logging::LogAggregator::exportToFile(const std::string& filepath, const std::string& format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (format == "json") {
        json json_data;
        json_data["entries"] = json::array();
        
        for (const auto& entry : entries_) {
            json entry_json;
            entry_json["level"] = static_cast<int>(entry.level);
            entry_json["message"] = entry.message;
            entry_json["file"] = entry.file;
            entry_json["line"] = entry.line;
            entry_json["function"] = entry.function;
            entry_json["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
            entry_json["thread_id"] = entry.thread_id;
            
            json_data["entries"].push_back(entry_json);
        }
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << json_data.dump(2);
        }
    } else {
        // Plain text format
        std::ofstream file(filepath);
        if (file.is_open()) {
            for (const auto& entry : entries_) {
                file << "[" << static_cast<int>(entry.level) << "] "
                     << std::chrono::system_clock::to_time_t(entry.timestamp) << " "
                     << entry.message << "\n";
            }
        }
    }
}

std::vector<logging::EnhancedLogEntry> logging::LogAggregator::filter(
    std::function<bool(const EnhancedLogEntry&)> predicate) const {
    std::vector<EnhancedLogEntry> result;
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& entry : entries_) {
        if (predicate(entry)) {
            result.push_back(entry);
        }
    }
    
    return result;
}

// PerformanceLogger implementation
logging::PerformanceLogger::PerformanceLogger(std::shared_ptr<IEnhancedLogger> logger)
    : logger_(std::move(logger)) {}

void logging::PerformanceLogger::logPerformance(
    const std::string& operation,
    double duration_ms,
    size_t memory_usage_bytes,
    const std::string& context) {
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Performance: " + operation + " took " + std::to_string(duration_ms) + " ms";
    if (memory_usage_bytes > 0) {
        entry.message += " (Memory: " + std::to_string(memory_usage_bytes) + " bytes)";
    }
    if (!context.empty()) {
        entry.message += " [" + context + "]";
    }
    entry.timestamp = std::chrono::system_clock::now();
    
    // Add metadata
    entry.metadata["operation"] = operation;
    entry.metadata["duration_ms"] = std::to_string(duration_ms);
    if (memory_usage_bytes > 0) {
        entry.metadata["memory_bytes"] = std::to_string(memory_usage_bytes);
    }
    if (!context.empty()) {
        entry.metadata["context"] = context;
    }
    
    logger_->log(entry);
}

void logging::PerformanceLogger::startMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    start_times_[operation] = std::chrono::system_clock::now();
}

void logging::PerformanceLogger::stopMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = start_times_.find(operation);
    if (it != start_times_.end()) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - it->second).count();
        
        logPerformance(operation, duration);
        
        start_times_.erase(it);
    }
}

logging::PerformanceLogger::PerformanceStats logging::PerformanceLogger::getStatistics() const {
    PerformanceStats stats;
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : start_times_) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - pair.second).count();
        stats.average_durations_ms[pair.first] += duration;
        stats.call_counts[pair.first]++;
        stats.total_duration_ms += duration;
        stats.total_calls++;
    }
    
    // Calculate averages
    for (auto& pair : stats.average_durations_ms) {
        pair.second /= stats.call_counts[pair.first];
    }
    
    if (stats.total_calls > 0) {
        stats.total_duration_ms /= stats.total_calls;
    }
    
    return stats;
}

} // namespace nlm