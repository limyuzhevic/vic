#pragma once

#include "ErrorHandler.hpp"
#include "LogEntry.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>

namespace nlm {

class LogManagerImpl {
public:
    LogManagerImpl() : globalLevel_(LogLevel::INFO), nextSequenceNumber_(0) {}
    
    void addLogger(const std::string& name, std::shared_ptr<ILogDestination> logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_[name] = logger;
        logger->setLevel(globalLevel_);
    }
    
    void removeLogger(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.erase(name);
    }
    
    void removeAllLoggers() {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.clear();
    }
    
    void setGlobalLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        globalLevel_ = level;
        for (auto& pair : loggers_) {
            pair.second->setLevel(level);
        }
    }
    
    LogLevel getGlobalLevel() const {
        return globalLevel_;
    }
    
    void log(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Add sequence number
        entry.sequenceNumber = nextSequenceNumber_++;
        
        // Check global level
        if (entry.level < globalLevel_) {
            return;
        }
        
        // Log to all loggers
        for (auto& pair : loggers_) {
            if (pair.second->isEnabled(entry.level)) {
                pair.second->log(entry);
            }
        }
    }
    
    void flushAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : loggers_) {
            pair.second->flush();
        }
    }
    
    std::vector<std::string> getLoggerNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(loggers_.size());
        for (const auto& pair : loggers_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void enableMemoryBuffer(std::shared_ptr<MemoryBuffer> buffer) {
        std::lock_guard<std::mutex> lock(mutex_);
        memoryBuffer_ = buffer;
    }
    
    void addMemoryEntry(const LogEntry& entry) {
        if (memoryBuffer_) {
            memoryBuffer_->addEntry(entry);
        }
    }
    
private:
    mutable std::mutex mutex_;
    LogLevel globalLevel_;
    std::unordered_map<std::string, std::shared_ptr<ILogDestination>> loggers_;
    std::shared_ptr<MemoryBuffer> memoryBuffer_;
    std::atomic<uint64_t> nextSequenceNumber_;
};

class LogManager {
public:
    static LogManager& getInstance() {
        static LogManager instance;
        return instance;
    }
    
    void addLogger(const std::string& name, std::shared_ptr<ILogDestination> logger) {
        pImpl_->addLogger(name, logger);
    }
    
    void removeLogger(const std::string& name) {
        pImpl_->removeLogger(name);
    }
    
    void removeAllLoggers() {
        pImpl_->removeAllLoggers();
    }
    
    void setGlobalLevel(LogLevel level) {
        pImpl_->setGlobalLevel(level);
    }
    
    LogLevel getGlobalLevel() const {
        return pImpl_->getGlobalLevel();
    }
    
    void log(const LogEntry& entry) {
        pImpl_->log(entry);
    }
    
    void flushAll() {
        pImpl_->flushAll();
    }
    
    std::vector<std::string> getLoggerNames() const {
        return pImpl_->getLoggerNames();n
    }
    
    void enableMemoryBuffer(std::shared_ptr<MemoryBuffer> buffer) {
        pImpl_->enableMemoryBuffer(buffer);
    }
    
    void addMemoryEntry(const LogEntry& entry) {
        pImpl_->addMemoryEntry(entry);
    }
    
private:
    LogManager() : pImpl_(std::make_unique<LogManagerImpl>()) {}
    ~LogManager() = default;
    
    std::unique_ptr<LogManagerImpl> pImpl_;
};

} // namespace nlm