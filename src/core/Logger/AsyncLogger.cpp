#include "Logger.hpp"
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>

namespace nlm {

struct AsyncLogger::Impl {
    std::vector<std::thread> workers;
    std::queue<LogEntry> logQueue;
    std::queue<LogEntry> backupQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::atomic<bool> running{true};
    std::atomic<size_t> processedCount{0};
    std::atomic<size_t> droppedCount{0};
    
    LogLevel level = LogLevel::Info;
    std::shared_ptr<LogFormatter> formatter;
    std::unordered_map<std::string, bool> categoryEnabled;
    
    // Performance tracking
    std::chrono::steady_clock::time_point lastStatsReport;
    uint64_t logsProcessedSinceLastReport = 0;
};

AsyncLogger::AsyncLogger(size_t threadCount, size_t queueSize)
    : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters (all enabled by default)
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
    
        // Start worker threads
        for (size_t i = 0; i < threadCount; ++i) {
            pImpl->workers.emplace_back([this]() {
                this->workerThread();
            });
        }
}

AsyncLogger::~AsyncLogger() {
    stop();
}

void AsyncLogger::log(const LogEntry& entry) {
    // Check level and category
    if (entry.level < pImpl->level) {
        PerformanceMetrics::recordLog(entry, true);
        return;
    }
    
    if (!isCategoryEnabled(entry.category)) {
        PerformanceMetrics::recordLog(entry, true);
        return;
    }
    
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    
    // Check queue size and drop if necessary (simplified: always accept for now)
    pImpl->logQueue.push(entry);
    lock.unlock();
    
    pImpl->queueCV.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setCategoryFilter(const std::string& category) {
    // Simple implementation: enable/disable specific category
    for (auto& pair : pImpl->categoryEnabled) {
        pair.second = (pair.first == category);
    }
}

void AsyncLogger::addCategory(const std::string& category) {
    pImpl->categoryEnabled[category] = true;
}

void AsyncLogger::removeCategory(const std::string& category) {
    pImpl->categoryEnabled.erase(category);
}

bool AsyncLogger::isCategoryEnabled(const std::string& category) const {
    auto it = pImpl->categoryEnabled.find(category);
    return (it != pImpl->categoryEnabled.end()) && it->second;
}

void AsyncLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> AsyncLogger::getFormatter() const {
    return pImpl->formatter;
}

void AsyncLogger::stop() {
    if (!pImpl->running.exchange(false)) {
        return; // Already stopped
    }
    
    pImpl->queueCV.notify_all();
    
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void AsyncLogger::workerThread() {
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        
        // Wait for log or stop signal
        if (pImpl->logQueue.empty()) {
            auto status = pImpl->queueCV.wait_for(lock, std::chrono::milliseconds(100),
                                                [this]() { return !pImpl->running || !pImpl->logQueue.empty(); });
            if (!status && !pImpl->running) {
                break;
            }
            continue;
        }
        
        // Process log entry
        LogEntry entry = std::move(pImpl->logQueue.front());
        pImpl->logQueue.pop();
        lock.unlock();
        
    // In a real implementation, this would actually log to the real logger
    // For now, just track metrics
    auto startTime = std::chrono::steady_clock::now();
    
    // Simulate log processing
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    
    auto endTime = std::chrono::steady_clock::now();
    auto logTime = std::chrono::duration<double, std::micro> (endTime - startTime).count();
    
    pImpl->processedCount++;
    pImpl->logsProcessedSinceLastReport++;
    
    PerformanceMetrics::recordLog(entry, false, logTime);
}

} // namespace nlm
