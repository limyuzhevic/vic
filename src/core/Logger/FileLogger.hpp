#pragma once

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>
#include <deque>
#include <shared_mutex>
#include <algorithm>
#include <map>
#include <condition_variable>
#include <sstream>
#include <zlib.h>
#include <bzlib.h>

namespace nlm {

class FileLogger : public ILogger, public IAsyncLogger {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    explicit FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    explicit FileLogger(const LoggingConfig& config);
    ~FileLogger() override;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isLevelEnabled(LogLevel level) const override;
    void setCategoryFilter(const std::string& category, bool enabled) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void flush() override;
    LoggerStats getStats() const override;
    void setFormatter(std::function<std::string(const LogEntry&)> formatter) override;
    
    // IAsyncLogger interface
    void logAsync(const LogEntry& entry) override;
    void setWorkerThreadCount(size_t count) override;
    void waitForCompletion() override;
    
    // File-specific methods
    void setFilepath(const std::string& filepath);
    void enableCompression(bool enable);
    void setCompressionLevel(int level);
    void setMaximumFileSize(size_t maxSize);
    void setMaximumBackupFiles(size_t maxFiles);
    void enableLogRotation(bool enable);
    void setCustomPattern(const std::string& pattern);
    void setBufferSize(size_t bufferSize);
    
    // Performance optimization methods
    void enableMemoryMapping(bool enable);
    void enableAsyncCompression(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void processAsyncLogs();
    void compressFile(const std::string& filepath);
    void rotateLogs();
    std::string formatEntry(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTimestamp() const;
};

} // namespace nlm