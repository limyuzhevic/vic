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

namespace nlm {

class ConsoleLogger : public ILogger, public IAsyncLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    explicit ConsoleLogger(const LoggingConfig& config);
    ~ConsoleLogger() override;
    
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
    
    // Console-specific methods
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void setBufferSize(size_t bufferSize);
    size_t getBufferSize() const;
    void enableLogRotation(size_t maxFileSize, size_t maxFiles);
    void setCustomPattern(const std::string& pattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void processAsyncLogs();
    void rotateLogFile();
    std::string formatEntry(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

} // namespace nlm