#pragma once

#include "LogEntry.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <thread>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

class AsyncLogger : public ILogDestination {
public:
    AsyncLogger(size_t maxQueueSize = 10000, size_t workerThreads = 2);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void flush() override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    
    void addDestination(std::shared_ptr<ILogDestination> destination);
    void setAsync(bool enabled);
    void setBatchSize(size_t batchSize);
    void setFlushInterval(std::chrono::milliseconds interval);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm