#pragma once

#include "Logger.hpp"
#include <string>
#include <map>
#include <memory>

namespace nlm {

class LogManager {
public:
    static LogManager& getInstance();
    
    // Disable copying
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    // Add a logger with a name for easy management
    void addLogger(const std::string& name, std::shared_ptr<ILogger> logger);
    
    // Get logger by name
    std::shared_ptr<ILogger> getLogger(const std::string& name);
    
    // Remove logger by name
    void removeLogger(const std::string& name);
    
    // Get all logger names
    std::vector<std::string> getLoggerNames() const;
    
    // Set level for specific logger
    void setLoggerLevel(const std::string& name, LogLevel level);
    
    // Get level for specific logger
    LogLevel getLoggerLevel(const std::string& name) const;
    
    // Log configuration change
    void logConfigChange(const std::string& key, const std::string& oldValue, const std::string& newValue);
    
    // Log neural operation
    void logNeuralOperation(const std::string& operation, int neuronId, float value, bool success);
    
    // Log performance metrics
    void logPerformance(const std::string& metric, double value, const std::string& unit);
    
    // Log memory usage
    void logMemoryUsage(size_t bytes, size_t peakBytes);
    
    // Flush all loggers
    void flushAll();
    
    // Close all loggers
    void closeAll();
    
    // Check if any logger is at or below specified level
    bool isEnabled(LogLevel level) const;
    
    // Clear all loggers
    void clearAllLoggers();
    
private:
    LogManager();
    ~LogManager();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm