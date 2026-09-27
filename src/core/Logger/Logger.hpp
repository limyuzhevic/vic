#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stacktrace>
#include <numeric>
#include <cmath>
#include <chrono>
#include <thread>

namespace nlm {

// Enhanced log severity levels with neural-specific levels
enum class LogLevel {
    // Basic levels
    Off = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    
    // Neural-specific levels
    NeuralEvent = 10,
    Spike = 11,
    SynapseChange = 12,
    Plasticity = 13,
    MemoryUpdate = 14,
    Development = 15,
    
    // Performance levels
    PerformanceStart = 20,
    PerformanceEnd = 21,
    Throughput = 22,
    ResourceUsage = 23,
    
    // Debug levels
    Trace = 30,
    Profile = 31,
    MemoryAlloc = 32,
    MemoryDealloc = 33,
    Breakpoint = 34
};

// Log entry with enhanced context information
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Neural event context
    std::string neuronId;
    std::string synapseId;
    std::string regionId;
    std::string populationId;
    
    // Performance metrics
    double durationMs = 0.0;
    size_t memoryDelta = 0;
    
    // Additional metadata
    std::unordered_map<std::string, std::string> metadata;
    
    // Stack trace
    std::vector<std::string> stackTrace;
};

// Log pattern for filtering
struct LogPattern {
    std::regex regex;
    LogLevel minLevel;
    std::string component;
    bool enabled = true;
};

// Log configuration
struct LogConfig {
    // General settings
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool enableStructuredLogging = true;
    std::string structuredFormat = "json";
    
    // Performance settings
    bool enableProfiling = true;
    bool enableMemoryTracking = true;
    bool enableTimingMetrics = true;
    
    // Filtering
    std::vector<LogPattern> filters;
    std::unordered_set<std::string> enabledComponents;
    
    // Output
    std::string logFile;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    int maxFiles = 5;
    bool rotateOnStart = false;
    
    // Memory tracking
    bool enableMemoryLeakDetection = true;
    double memoryThresholdMB = 100.0;
    
    // Neural logging
    bool enableNeuralEventLogging = true;
    bool enableSpikeLogging = true;
    bool enablePlasticityLogging = true;
    bool enableDevelopmentLogging = true;
};

// Logger sink interface
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void configure(const LogConfig& config) = 0;
};

// Base logger implementation with advanced features
class BaseLogger : public ILogSink {
protected:
    LogLevel level_ = LogLevel::Info;
    std::mutex mutex_;
    bool useColors_ = true;
    
    // Performance tracking
    std::chrono::system_clock::time_point lastProfileTime_;
    double totalTimeSpent_ = 0.0;
    int logCount_ = 0;
    
    // Memory tracking
    std::unordered_map<void*, size_t> allocations_;
    std::chrono::system_clock::time_point memoryTrackingStart_;
    
    // Pattern matching
    std::vector<LogPattern> patterns_;
    
public:
    virtual ~BaseLogger() = default;
    
    void log(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check level
        if (entry.level < level_) return;
        
        // Apply pattern matching
        if (!matchesPatterns(entry)) return;
        
        // Track performance
        logCount_++;
        if (config_.enableTimingMetrics) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(now - entry.timestamp).count();
            totalTimeSpent_ += duration;
        }
        
        // Memory tracking
        if (config_.enableMemoryTracking) {
            trackMemoryUsage(entry);
        }
        
        // Output to sink
        writeEntry(entry);
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        return level_;
    }
    
    virtual void configure(const LogConfig& config) override {
        config_ = config;
        useColors_ = config.useColors;
    }
    
    // Add pattern filter
    void addPattern(const LogPattern& pattern) {
        std::lock_guard<std::mutex> lock(mutex_);
        patterns_.push_back(pattern);
    }
    
    // Check if entry matches patterns
    virtual bool matchesPatterns(const LogEntry& entry) {
        for (const auto& pattern : patterns_) {
            if (!pattern.enabled) continue;
            
            // Check component
            if (!pattern.component.empty() && 
                entry.metadata.find("component") == entry.metadata.end()) {
                continue;
            }
            
            // Check level
            if (entry.level < pattern.minLevel) continue;
            
            // Check regex
            if (std::regex_search(entry.message, pattern.regex)) {
                return true;
            }
        }
        
        return patterns_.empty();
    }
    
    // Performance tracking
    double getAverageLogTime() const {
        return logCount_ > 0 ? totalTimeSpent_ / logCount_ : 0.0;
    }
    
    int getLogCount() const { return logCount_; }
    
    // Memory leak detection
    void trackMemoryUsage(const LogEntry& entry) {
        if (entry.memoryDelta != 0) {
            if (entry.memoryDelta > 0) {
                allocations_[nullptr] += entry.memoryDelta;
            } else {
                if (allocations_.find(nullptr) != allocations_.end()) {
                    allocations_[nullptr] += entry.memoryDelta;
                    if (allocations_[nullptr] <= 0) {
                        // Memory leak detected
                        std::string leakMsg = "MEMORY LEAK DETECTED: " + 
                                            std::to_string(-allocations_[nullptr]) + " bytes not freed";
                        LogEntry leakEntry;
                        leakEntry.level = LogLevel::Critical;
                        leakEntry.message = leakMsg;
                        leakEntry.metadata["type"] = "leak_detection";
                        writeEntry(leakEntry);
                    }
                }
            }
        }
    }
    
protected:
    virtual void writeEntry(const LogEntry& entry) = 0;
    
    LogConfig config_;
};

class EnhancedLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Configuration
    void configure(const LogConfig& config);
    LogConfig getConfig() const;
    
    // Log sinks management
    void addSink(std::shared_ptr<ILogSink> sink);
    void removeSink(const std::string& name);
    void clearSinks();
    
    // Log methods with enhanced context
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Neural-specific logging methods
    void logNeuralEvent(const std::string& message, const std::string& neuronId = "",
                       const std::string& synapseId = "", const std::string& regionId = "",
                       const std::string& populationId = "", double duration = 0.0);
    
    void logSpike(const std::string& neuronId, float voltage, 
                  const std::string& regionId = "", const std::string& populationId = "",
                  double duration = 0.0);
    
    void logSynapseChange(const std::string& synapseId, const std::string& preNeuronId,
                         const std::string& postNeuronId, float weightChange,
                         const std::string& plasticityType = "STDP", double duration = 0.0);
    
    void logPlasticity(const std::string& neuronId, const std::string& regionId,
                      const std::string& plasticityType, float magnitude, double duration = 0.0);
    
    void logMemoryUpdate(const std::string& neuronId, size_t memoryDelta,
                        const std::string& memoryType = "working", double duration = 0.0);
    
    void logDevelopment(const std::string& neuronId, const std::string& regionId,
                       const std::string& stage, double duration = 0.0);
    
    // Performance logging
    void startTiming(const std::string& operation);
    void endTiming(const std::string& operation);
    void logThroughput(const std::string& operation, size_t count, double timeMs);
    void logResourceUsage(double cpuUsage, size_t memoryMB, size_t memoryAllocated);
    
    // Debug and profiling
    void logWithStackTrace(const std::string& message, const std::string& file = "",
                          int line = 0, const std::string& function = "");
    void logBreakpoint(const std::string& location, const std::string& condition = "",
                      const std::string& message = "");
    void logProfile(const std::string& operation, double timeMs, 
                   size_t memoryDelta = 0, int callCount = 1);
    void logMemoryAlloc(const void* ptr, size_t size, const std::string& location = "");
    void logMemoryDealloc(const void* ptr, size_t size, const std::string& location = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0,
             const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Level-specific convenience methods
    void neuralEvent(const std::string& message, const std::string& neuronId = "",
                   const std::string& synapseId = "", const std::string& regionId = "",
                   const std::string& populationId = "", double duration = 0.0);
    
    void spike(const std::string& neuronId, float voltage, 
              const std::string& regionId = "", const std::string& populationId = "",
              double duration = 0.0);
    
    void synapseChange(const std::string& synapseId, const std::string& preNeuronId,
                      const std::string& postNeuronId, float weightChange,
                      const std::string& plasticityType = "STDP", double duration = 0.0);
    
    void plasticity(const std::string& neuronId, const std::string& regionId,
                   const std::string& plasticityType, float magnitude, double duration = 0.0);
    
    void memoryUpdate(const std::string& neuronId, size_t memoryDelta,
                     const std::string& memoryType = "working", double duration = 0.0);
    
    void development(const std::string& neuronId, const std::string& regionId,
                    const std::string& stage, double duration = 0.0);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
};

// Convenience macros for easy usage
#define NLM_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Neural-specific macros
#define NLM_LOG_NEURAL(msg, neuronId) nlm::EnhancedLogger::getGlobal().neuralEvent(msg, neuronId)
#define NLM_LOG_SPIKE(neuronId, voltage) nlm::EnhancedLogger::getGlobal().spike(neuronId, voltage)
#define NLM_LOG_SYNAPSE(synapseId, pre, post, change) nlm::EnhancedLogger::getGlobal().synapseChange(synapseId, pre, post, change)
#define NLM_LOG_PLASTICITY(neuronId, type, magnitude) nlm::EnhancedLogger::getGlobal().plasticity(neuronId, "", type, magnitude)
#define NLM_LOG_MEMORY(neuronId, delta) nlm::EnhancedLogger::getGlobal().memoryUpdate(neuronId, delta)
#define NLM_LOG_DEVELOPMENT(neuronId, stage) nlm::EnhancedLogger::getGlobal().development(neuronId, "", stage)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::unordered_map<std::string, std::string>& metadata = {});
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Helper for adding metadata
    LogStream& withMetadata(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

#endif // LOGGER_HPP
