#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Debug level enumeration
enum class DebugLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Debug message structure
struct DebugMessage {
    DebugLevel level;
    std::string source;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    int threadId;
    std::string file;
    int line;
    
    DebugMessage(DebugLevel lvl, const std::string& src, const std::string& msg,
                 const std::string& file = "", int line = 0)
        : level(lvl), source(src), message(msg), timestamp(std::chrono::system_clock::now()),
          threadId(0), file(file), line(line) {}
};

// Memory allocation tracking
struct MemoryAllocation {
    size_t size;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    
    MemoryAllocation(size_t sz, const std::string& file = "", int line = 0)
        : size(sz), file(file), line(line), timestamp(std::chrono::system_clock::now()) {}
};

// Performance measurement
struct PerformanceMetric {
    std::string name;
    std::chrono::duration<double> duration;
    int calls;
    double minTime;
    double maxTime;
    double averageTime;
    
    PerformanceMetric(const std::string& name) : name(name), calls(0), minTime(0.0), maxTime(0.0), averageTime(0.0) {}
};

// Network statistics
struct NetworkStats {
    size_t totalNeurons;
    size_t totalSynapses;
    float averageFiringRate;
    float excitationInhibitionRatio;
    std::vector<size_t> neuronDistributionByType;
    std::vector<size_t> spikeCountsByRegion;
    
    NetworkStats() : totalNeurons(0), totalSynapses(0), averageFiringRate(0.0), excitationInhibitionRatio(0.0) {}
};

// Debug interface
class DebugInterface {
public:
    virtual ~DebugInterface() = default;
    
    virtual void log(const DebugMessage& message) = 0;
    virtual void memoryAllocated(const MemoryAllocation& alloc) = 0;
    virtual void memoryDeallocated(size_t size) = 0;
    virtual void performanceMeasured(const PerformanceMetric& metric) = 0;
    
    virtual void setLevel(DebugLevel level) = 0;
    virtual DebugLevel getLevel() const = 0;
    
    virtual void startProfiling(const std::string& name) = 0;
    virtual void stopProfiling(const std::string& name) = 0;
    
    virtual NetworkStats getNetworkStats() const = 0;
    virtual std::vector<MemoryAllocation> getMemoryAllocations() const = 0;
    virtual std::vector<PerformanceMetric> getPerformanceMetrics() const = 0;
    
    virtual void saveDebugInfo(const std::string& filename) = 0;
    virtual void loadDebugInfo(const std::string& filename) = 0;
};

// Console debug implementation
class ConsoleDebug : public DebugInterface {
public:
    ConsoleDebug() : currentLevel(DebugLevel::Info) {}
    
    void log(const DebugMessage& message) override {
        if (message.level < currentLevel) return;
        
        std::string levelStr;
        switch (message.level) {
            case DebugLevel::Debug: levelStr = "DEBUG"; break;
            case DebugLevel::Info: levelStr = "INFO"; break;
            case DebugLevel::Warning: levelStr = "WARN"; break;
            case DebugLevel::Error: levelStr = "ERROR"; break;
            case DebugLevel::Critical: levelStr = "CRIT"; break;
        }
        
        std::cout << "[" << levelStr << "] " << message.source << ": " 
                  << message.message << std::endl;
        
        if (message.level >= DebugLevel::Error) {
            std::cerr << "[ERROR CONTEXT] File: " << message.file 
                      << ", Line: " << message.line << std::endl;
        }
    }
    
    void memoryAllocated(const MemoryAllocation& alloc) override {
        std::cout << "[MEMORY] Allocated " << alloc.size << " bytes at " 
                  << alloc.file << ":" << alloc.line << std::endl;
    }
    
    void memoryDeallocated(size_t size) override {
        std::cout << "[MEMORY] Deallocated " << size << " bytes" << std::endl;
    }
    
    void performanceMeasured(const PerformanceMetric& metric) override {
        std::cout << "[PERF] " << metric.name << ": " 
                  << metric.averageTime * 1000 << " ms avg (" 
                  << metric.calls << " calls)" << std::endl;
    }
    
    void setLevel(DebugLevel level) override {
        currentLevel = level;
    }
    
    DebugLevel getLevel() const override {
        return currentLevel;
    }
    
    void startProfiling(const std::string& name) override {
        // Start timing a profiling session
        profilingSessions[name] = std::chrono::system_clock::now();
    }
    
    void stopProfiling(const std::string& name) override {
        auto it = profilingSessions.find(name);
        if (it != profilingSessions.end()) {
            auto duration = std::chrono::system_clock::now() - it->second;
            double seconds = std::chrono::duration<double>(duration).count();
            
            auto& metric = performanceMetrics[name];
            metric.name = name;
            metric.duration = duration;
            metric.calls++;
            metric.minTime = (metric.calls == 1) ? seconds : std::min(metric.minTime, seconds);
            metric.maxTime = (metric.calls == 1) ? seconds : std::max(metric.maxTime, seconds);
            metric.averageTime = (metric.minTime + metric.maxTime) / 2.0; // Simplified
            
            profilingSessions.erase(it);
        }
    }
    
    NetworkStats getNetworkStats() const override {
        NetworkStats stats;
        // In a real implementation, this would collect network statistics
        // from the brain system
        return stats;
    }
    
    std::vector<MemoryAllocation> getMemoryAllocations() const override {
        return memoryAllocations;
    }
    
    std::vector<PerformanceMetric> getPerformanceMetrics() const override {
        std::vector<PerformanceMetric> metrics;
        for (const auto& pair : performanceMetrics) {
            metrics.push_back(pair.second);
        }
        return metrics;
    }
    
    void saveDebugInfo(const std::string& filename) override {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Debug Information" << std::endl;
            file << "=================" << std::endl;
            file << "Debug Level: " << static_cast<int>(currentLevel) << std::endl;
            file << "Messages: " << messages.size() << std::endl;
            file << "Memory Allocations: " << memoryAllocations.size() << std::endl;
            file << "Performance Metrics: " << performanceMetrics.size() << std::endl;
            file.close();
        }
    }
    
    void loadDebugInfo(const std::string& filename) override {
        // In a real implementation, this would load debug information from file
        // For now, it's a placeholder
    }
    
    void addMessage(const DebugMessage& message) {
        messages.push_back(message);
    }
    
    void addMemoryAllocation(const MemoryAllocation& alloc) {
        memoryAllocations.push_back(alloc);
    }
    
private:
    DebugLevel currentLevel;
    std::vector<DebugMessage> messages;
    std::vector<MemoryAllocation> memoryAllocations;
    std::map<std::string, PerformanceMetric> performanceMetrics;
    std::map<std::string, std::chrono::system_clock::time_point> profilingSessions;
};

// Global debug instance
class DebugManager {
public:
    static DebugManager& getInstance() {
        static DebugManager instance;
        return instance;
    }
    
    void initialize(DebugInterface* debugInterface) {
        if (instance_) {
            delete instance_;
        }
        instance_ = debugInterface;
    }
    
    DebugInterface* getInstance() {
        return instance_;
    }
    
    void log(DebugLevel level, const std::string& source, const std::string& message,
             const std::string& file = "", int line = 0) {
        if (instance_) {
            DebugMessage msg(level, source, message, file, line);
            instance_->log(msg);
            if (instance_->getLevel() <= level) {
                static_cast<ConsoleDebug*>(instance_)->addMessage(msg);
            }
        }
    }
    
    void memoryAllocated(size_t size, const std::string& file = "", int line = 0) {
        if (instance_) {
            MemoryAllocation alloc(size, file, line);
            instance_->memoryAllocated(alloc);
            static_cast<ConsoleDebug*>(instance_)->addMemoryAllocation(alloc);
        }
    }
    
    void memoryDeallocated(size_t size) {
        if (instance_) {
            instance_->memoryDeallocated(size);
        }
    }
    
    void startProfiling(const std::string& name) {
        if (instance_) {
            instance_->startProfiling(name);
        }
    }
    
    void stopProfiling(const std::string& name) {
        if (instance_) {
            instance_->stopProfiling(name);
        }
    }
    
    void setDebugLevel(DebugLevel level) {
        if (instance_) {
            instance_->setLevel(level);
        }
    }
    
private:
    DebugManager() {}
    static DebugInterface* instance_;
};

DebugInterface* DebugManager::instance_ = nullptr;

// Convenience functions for easy debugging
inline void DebugLog(DebugLevel level, const std::string& message) {
    DebugManager::getInstance().log(level, "Global", message);
}

inline void DebugLogInfo(const std::string& message) {
    DebugManager::getInstance().log(DebugLevel::Info, "Global", message);
}

inline void DebugLogWarning(const std::string& message) {
    DebugManager::getInstance().log(DebugLevel::Warning, "Global", message);
}

inline void DebugLogError(const std::string& message, const std::string& file = "", int line = 0) {
    DebugManager::getInstance().log(DebugLevel::Error, "Global", message, file, line);
}

inline void DebugLogCritical(const std::string& message) {
    DebugManager::getInstance().log(DebugLevel::Critical, "Global", message);
}

// Macro for easy debugging
#define DEBUG_LOG(level, message) nlm::DebugLog(level, message)
#define DEBUG_LOG_INFO(message) nlm::DebugLogInfo(message)
#define DEBUG_LOG_WARNING(message) nlm::DebugLogWarning(message)
#define DEBUG_LOG_ERROR(message) nlm::DebugLogError(message, __FILE__, __LINE__)

} // namespace nlm