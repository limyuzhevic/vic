#pragma once

#include "Logger.hpp"
#include <map>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Set log format
    void setFormat(const std::string& format);
    void setTimestampFormat(const std::string& format);
    void setIncludeLocationInfo(bool include);
    
    // Rotating file support
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(int maxFiles);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const LogEntry& entry) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

// Async logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(std::shared_ptr<ILogger> targetLogger, size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Control queue size and behavior
    void setQueueSize(size_t size);
    void setFlushInterval(double interval); // seconds
    
    // Block until queue is empty
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance monitoring logger
class PerformanceLogger : public ILogger {
public:
    PerformanceLogger(std::shared_ptr<ILogger> targetLogger, 
                     const std::string& category = "default");
    ~PerformanceLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking
    void startTiming(const std::string& operation);
    void stopTiming(const std::string& operation);
    double getAverageTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    
    // Get performance statistics
    std::map<std::string, double> getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
