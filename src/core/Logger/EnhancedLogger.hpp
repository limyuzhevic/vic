// Enhanced logging system with performance monitoring and debugging
#pragma once

#include "Logger.hpp"
#include <map>
#include <unordered_map>
#include <chrono>
#include <string>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Enhanced log categories for better organization
enum class LogCategory {
    Core = 0,
    Brain = 1,
    Memory = 2,
    Plasticity = 3,
    Development = 4,
    Neuromodulation = 5,
    Prediction = 6,
    Cognition = 7,
    Sensory = 8,
    Motor = 9,
    Environment = 10,
    Experiment = 11,
    Performance = 12,
    Debug = 13,
    User = 14
};

// Performance metrics structure
struct PerformanceMetrics {
    std::string name;
    double executionTimeMs;
    uint64_t memoryUsageBytes;
    uint64_t callCount;
    double averageTimeMs;
    double minTimeMs;
    double maxTimeMs;
    std::chrono::system_clock::time_point lastMeasured;
    
    PerformanceMetrics(const std::string& n) : name(n), executionTimeMs(0.0), 
        memoryUsageBytes(0), callCount(0), averageTimeMs(0.0), 
        minTimeMs(0.0), maxTimeMs(0.0), lastMeasured(std::chrono::system_clock::now()) {}
    
    void update(double timeMs, uint64_t memoryBytes) {
        executionTimeMs = timeMs;
        memoryUsageBytes = memoryBytes;
        callCount++;
        if (callCount == 1) {
            averageTimeMs = minTimeMs = maxTimeMs = timeMs;
        } else {
            averageTimeMs = ((averageTimeMs * (callCount - 1)) + timeMs) / callCount;
            minTimeMs = std::min(minTimeMs, timeMs);
            maxTimeMs = std::max(maxTimeMs, timeMs);
        }
        lastMeasured = std::chrono::system_clock::now();
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Name: " << name << "\n";
        oss << "  Calls: " << callCount << "\n";
        oss << "  Avg Time: " << std::fixed << std::setprecision(3) << averageTimeMs << " ms\n";
        oss << "  Min Time: " << minTimeMs << " ms\n";
        oss << "  Max Time: " << maxTimeMs << " ms\n";
        oss << "  Memory: " << memoryUsageBytes << " bytes\n";
        return oss.str();
    }
};

// Enhanced log entry with additional metadata
struct EnhancedLogEntry {
    LogLevel level;
    LogCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string threadName;
    std::map<std::string, std::string> metadata;
    
    EnhancedLogEntry(LogLevel lvl, LogCategory cat, const std::string& msg,
                     const std::string& file = "", int line = 0, 
                     const std::string& function = "")
        : level(lvl), category(cat), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        // Set thread name for easier debugging
        #ifdef _WIN32
        char threadNameBuf[256];
        DWORD threadId = GetCurrentThreadId();
        if (GetThreadDescription(threadId, &threadNameBuf) == S_OK) {
            threadName = threadNameBuf;
        } else {
            threadName = std::to_string(threadId);
        }
        #else
        threadName = std::to_string(threadId);
        #endif
    }
};

// Performance timer for measuring execution time
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name, LogCategory category = LogCategory::Performance)
        : name_(name), category_(category), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
        
        // Log the performance metric
        std::string message = "Performance measurement: " + name_ + " took " + 
                            std::to_string(duration) + " ms";
        NLM_LOG_STREAM_CATEGORY(LogLevel::Info, category_, message);
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
    }
    
private:
    std::string name_;
    LogCategory category_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Enhanced logging stream with metadata support
class EnhancedLogStream {
public:
    EnhancedLogStream(LogLevel level, LogCategory category, 
                     const std::string& file, int line, 
                     const std::string& function)
        : level_(level), category_(category), file_(file), line_(line), 
          function_(function), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~EnhancedLogStream() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_).count();
        
        std::ostringstream oss;
        oss << oss_;
        
        // Add performance metadata if duration is significant
        if (duration > 1000) { // Log if more than 1ms
            oss << " [Execution: " << duration << " us]";
        }
        
        nlm::Logger::getGlobal().log(level_, oss.str(), file_, line_, function_);
    }
    
    template<typename T>
    EnhancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    EnhancedLogStream& withMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
        return *this;
    }
    
    EnhancedLogStream& withThreadInfo() {
        threadInfo_ = true;
        return *this;
    }
    
private:
    LogLevel level_;
    LogCategory category_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::map<std::string, std::string> metadata_;
    bool threadInfo_;
};

// Performance monitoring utility
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void recordMetric(const std::string& name, double timeMs, uint64_t memoryBytes = 0,
                      LogCategory category = LogCategory::Performance) {
        auto it = metrics_.find(name);
        if (it == metrics_.end()) {
            metrics_[name] = PerformanceMetrics(name);
        }
        
        metrics_[name].update(timeMs, memoryBytes);
        
        // Log significant performance events
        if (timeMs > 100.0) { // Log if more than 100ms
            NLM_LOG_STREAM_CATEGORY(LogLevel::Warning, category, 
                "Performance warning: " + name + " took " + std::to_string(timeMs) + " ms");
        }
    }
    
    const PerformanceMetrics& getMetric(const std::string& name) const {
        static PerformanceMetrics emptyMetric("empty");
        auto it = metrics_.find(name);
        return (it != metrics_.end()) ? it->second : emptyMetric;
    }
    
    std::map<std::string, PerformanceMetrics> getAllMetrics() const {
        return metrics_;
    }
    
    void clear() {
        metrics_.clear();
    }
    
    std::string getReport() const {
        std::ostringstream oss;
        oss << "=== Performance Report ===\n";
        oss << "Total metrics: " << metrics_.size() << "\n\n";
        
        for (const auto& [name, metric] : metrics_) {
            oss << metric.getSummary() << "\n";
        }
        
        return oss.str();
    }
    
private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    std::map<std::string, PerformanceMetrics> metrics_;
};

// Enhanced logging macros
#define NLM_LOG_STREAM_CATEGORY(level, category, file, line, function) \
    nlm::EnhancedLogStream(level, category, file, line, function)

#define NLM_PERFORMANCE_TIMER(name, category) \
    nlm::PerformanceTimer performanceTimer_##__LINE__(name, category)

// Convenience functions for common logging categories
inline EnhancedLogStream debugBrain(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
    return EnhancedLogStream(LogLevel::Debug, LogCategory::Brain, file, line, function) << message;
}

inline EnhancedLogStream infoBrain(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
    return EnhancedLogStream(LogLevel::Info, LogCategory::Brain, file, line, function) << message;
}

inline EnhancedLogStream warningBrain(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
    return EnhancedLogStream(LogLevel::Warning, LogCategory::Brain, file, line, function) << message;
}

inline EnhancedLogStream errorBrain(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
    return EnhancedLogStream(LogLevel::Error, LogCategory::Brain, file, line, function) << message;
}

// Performance monitoring convenience functions
inline void recordPerformanceMetric(const std::string& name, double timeMs, 
                                     uint64_t memoryBytes = 0, 
                                     LogCategory category = LogCategory::Performance) {
    PerformanceMonitor::getInstance().recordMetric(name, timeMs, memoryBytes, category);
}

inline std::string getPerformanceReport() {
    return PerformanceMonitor::getInstance().getReport();
}

// Debug utilities
inline std::string getStackTrace() {
    // Simplified stack trace - in production, use platform-specific stack trace generation
    return "Stack trace not implemented in this build";
}

inline std::string getMemoryInfo() {
    // Simplified memory info - in production, use platform-specific memory APIs
    return "Memory info not implemented in this build";
}

inline void debugMemoryLeakDetection(const std::string& objectName, uint64_t address) {
    NLM_LOG_STREAM_CATEGORY(LogLevel::Debug, LogCategory::Debug,
        "Memory leak detection: " + objectName + " at address " + std::to_string(address),
        __FILE__, __LINE__, __func__);
}

inline void debugThreadActivity(const std::string& threadName, const std::string& activity) {
    NLM_LOG_STREAM_CATEGORY(LogLevel::Debug, LogCategory::Debug,
        "Thread activity: " + threadName + " - " + activity,
        __FILE__, __LINE__, __func__);
}

} // namespace nlm

// Integration with existing macros
#define NLM_LOG_DEBUG_CATEGORY(msg, category) \
    nlm::EnhancedLogStream(nlm::LogLevel::Debug, category, __FILE__, __LINE__, __func__) << msg

#define NLM_LOG_INFO_CATEGORY(msg, category) \
    nlm::EnhancedLogStream(nlm::LogLevel::Info, category, __FILE__, __LINE__, __func__) << msg

#define NLM_LOG_WARNING_CATEGORY(msg, category) \
    nlm::EnhancedLogStream(nlm::LogLevel::Warning, category, __FILE__, __LINE__, __func__) << msg

#define NLM_LOG_ERROR_CATEGORY(msg, category) \
    nlm::EnhancedLogStream(nlm::LogLevel::Error, category, __FILE__, __LINE__, __func__) << msg

// Performance timing macro
#define NLM_PERFORMANCE_MEASURE(name) \
    nlm::PerformanceTimer performanceTimer_##__LINE__(name)