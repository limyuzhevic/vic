pragma once

#include "MemoryManager.hpp"
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>

namespace nlm {

// Memory threshold configuration
struct MemoryThresholdConfig {
    size_t warningThreshold;      // Warning when usage exceeds this (bytes)
    size_t errorThreshold;        // Error when usage exceeds this (bytes)
    float warningPercent;         // Warning when usage exceeds this percentage
    float errorPercent;           // Error when usage exceeds this percentage
    std::string warningMessage;    // Message for warning threshold
    std::string errorMessage;      // Message for error threshold
    bool enableThresholds;        // Whether thresholds are enabled
    bool enablePercentThresholds; // Whether percentage-based thresholds are used
    bool enableAbsoluteThresholds; // Whether absolute-based thresholds are used
    
    MemoryThresholdConfig()
        : warningThreshold(100 * 1024 * 1024),     // 100MB
          errorThreshold(500 * 1024 * 1024),       // 500MB
          warningPercent(80.0f),                   // 80%
          errorPercent(90.0f),                     // 90%
          warningMessage("Memory usage warning: High memory usage detected"),
          errorMessage("Memory usage error: Critical memory usage exceeded"),
          enableThresholds(true),
          enablePercentThresholds(true),
          enableAbsoluteThresholds(true) {}
};

// Memory alert information
struct MemoryAlert {
    uint64_t timestamp;
    size_t currentUsage;
    size_t totalMemory;
    float usagePercent;
    std::string alertType;     // "WARNING" or "ERROR"
    std::string message;
    std::string threadId;
    
    MemoryAlert(size_t usage, size_t total, float percent, const std::string& type, const std::string& msg)
        : timestamp(getCurrentTimestamp()), currentUsage(usage), totalMemory(total),
          usagePercent(percent), alertType(type), message(msg), threadId(getCurrentThreadId()) {}
};

// Memory usage trend information
struct MemoryTrend {
    uint64_t timestamp;
    size_t memoryUsage;
    size_t allocationCount;
    size_t deallocationCount;
    float growthRate;  // bytes per second
    std::string trendDirection; // "increasing", "stable", "decreasing"
    
    MemoryTrend(size_t usage, size_t allocCount, size_t deallocCount, float growth)
        : timestamp(getCurrentTimestamp()), memoryUsage(usage), allocationCount(allocCount),
          deallocationCount(deallocCount), growthRate(growth), trendDirection("stable") {}
};

// Memory monitoring callback
using MemoryAlertCallback = std::function<void(const MemoryAlert&)>
;
using MemoryTrendCallback = std::function<void(const MemoryTrend&)>
;

// Memory monitoring system
class MemoryMonitor {
private:
    std::mutex mutex;
    std::condition_variable cv;
    bool running;
    std::thread monitoringThread;
    
    // Configuration
    MemoryThresholdConfig config;
    
    // State tracking
    std::vector<MemoryAlert> recentAlerts;
    std::vector<MemoryTrend> trends;
    std::map<std::string, size_t> componentMemoryUsage;
    size_t previousTotalMemory;
    uint64_t lastCheckTimestamp;
    
    // Callbacks
    std::vector<MemoryAlertCallback> alertCallbacks;
    std::vector<MemoryTrendCallback> trendCallbacks;
    
    // Alert throttling
    uint64_t lastAlertTimestamps[2]; // 0 = warning, 1 = error
    static constexpr uint64_t ALERT_THROTTLE_MS = 60000; // 1 minute between same alerts
    
public:
    MemoryMonitor() : running(false), previousTotalMemory(0), lastCheckTimestamp(0) {
        lastAlertTimestamps[0] = 0;
        lastAlertTimestamps[1] = 0;
        setDefaultThresholds();
    }
    
    ~MemoryMonitor() {
        stop();
    }
    
    void start() {
        if (!running) {
            running = true;
            monitoringThread = std::thread(&MemoryMonitor::monitoringWorker, this);
        }
    }
    
    void stop() {
        if (running) {
            running = false;
            cv.notify_one();
            if (monitoringThread.joinable()) {
                monitoringThread.join();
            }
        }
    }
    
    // Configuration methods
    void setThresholds(size_t warningAbs, size_t errorAbs,
                       float warningPercent = 0.0f, float errorPercent = 0.0f) {
        std::lock_guard<std::mutex> lock(mutex);
        config.warningThreshold = warningAbs;
        config.errorThreshold = errorAbs;
        config.warningPercent = warningPercent;
        config.errorPercent = errorPercent;
        config.enableAbsoluteThresholds = (warningPercent > 0.0f || errorPercent > 0.0f);
    }
    
    void setThresholdMessages(const std::string& warning, const std::string& error) {
        std::lock_guard<std::mutex> lock(mutex);
        config.warningMessage = warning;
        config.errorMessage = error;
    }
    
    void enableThresholds(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        config.enableThresholds = enable;
    }
    
    void registerComponent(const std::string& name, size_t initialMemory = 0) {
        std::lock_guard<std::mutex> lock(mutex);
        componentMemoryUsage[name] = initialMemory;
    }
    
    void updateComponentMemory(const std::string& name, size_t memory) {
        std::lock_guard<std::mutex> lock(mutex);
        componentMemoryUsage[name] = memory;
    }
    
    void addAlertCallback(MemoryAlertCallback callback) {
        std::lock_guard<std::mutex> lock(mutex);
        alertCallbacks.push_back(callback);
    }
    
    void addTrendCallback(MemoryTrendCallback callback) {
        std::lock_guard<std::mutex> lock(mutex);
        trendCallbacks.push_back(callback);
    }
    
    std::string getStatusReport() const {
        std::stringstream ss;
        ss << "=== Memory Monitor Status ===\n";
        ss << "Thresholds enabled: " << (config.enableThresholds ? "yes" : "no") << "\n";
        ss << "Warning threshold: " << (config.enableAbsoluteThresholds ? config.warningThreshold : config.warningPercent * 100) << " bytes\n";
        ss << "Error threshold: " << (config.enableAbsoluteThresholds ? config.errorThreshold : config.errorPercent * 100) << " bytes\n\n";
        
        ss << "=== Component Memory Usage ===\n";
        for (const auto& pair : componentMemoryUsage) {
            ss << "  " << pair.first << ": " << pair.second << " bytes\n";
        }
        
        ss << "\n=== Recent Alerts ===\n";
        if (recentAlerts.empty()) {
            ss << "  No recent alerts\n";
        } else {
            size_t displayCount = std::min(recentAlerts.size(), static_cast<size_t>(10));
            for (size_t i = 0; i < displayCount; ++i) {
                const auto& alert = recentAlerts[i];
                ss << "  [" << alert.timestamp << "] " << alert.alertType << ": " << alert.message << " (" << alert.currentUsage << " bytes, " << alert.usagePercent << "% total)\n";
            }
        }
        
        ss << "\n=== Memory Trends ===\n";
        if (trends.empty()) {
            ss << "  No trend data available\n";
        } else {
            size_t displayCount = std::min(trends.size(), static_cast<size_t>(5));
            for (size_t i = 0; i < displayCount; ++i) {
                const auto& trend = trends[i];
                ss << "  [" << trend.timestamp << "] Memory: " << trend.memoryUsage << " bytes, "
                   << trend.growthRate << " bytes/s growth, "
                   << "direction: " << trend.trendDirection << "\n";
            }
        }
        
        return ss.str();
    }
    
    std::vector<MemoryAlert> getRecentAlerts(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t displayCount = std::min(count, recentAlerts.size());
        return std::vector<MemoryAlert>(recentAlerts.end() - displayCount, recentAlerts.end());
    }
    
    std::vector<MemoryTrend> getRecentTrends(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t displayCount = std::min(count, trends.size());
        return std::vector<MemoryTrend>(trends.end() - displayCount, trends.end());
    }
    
    void forceCheck() {
        std::lock_guard<std::mutex> lock(mutex);
        performMemoryCheck();
    }
    
private:
    void monitoringWorker() {
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            
            // Wait for the next check interval
            cv.wait_for(lock, std::chrono::milliseconds(1000)); // Check every second
            
            if (!running) break;
            
            performMemoryCheck();
        }
    }
    
    void performMemoryCheck() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t currentMemory = MemoryManager::MemoryTracker::getInstance().getTotalAllocated();
        size_t peakMemory = MemoryManager::MemoryTracker::getInstance().getPeakAllocated();
        size_t activeAllocations = MemoryManager::MemoryTracker::getInstance().getActiveAllocations();
        size_t allocationCount = MemoryManager::MemoryTracker::getInstance().getAllocationCount();
        size_t deallocationCount = MemoryManager::MemoryTracker::getInstance().getDeallocationCount();
        
        uint64_t currentTimestamp = getCurrentTimestamp();
        
        // Calculate growth rate
        float growthRate = 0.0f;
        if (lastCheckTimestamp > 0) {
            size_t memoryGrowth = currentMemory - previousTotalMemory;
            uint64_t timeElapsed = currentTimestamp - lastCheckTimestamp;
            if (timeElapsed > 0) {
                growthRate = static_cast<float>(memoryGrowth) / timeElapsed * 1000000.0f; // bytes per second
            }
        }
        
        // Update trend
        MemoryTrend trend(currentMemory, allocationCount, deallocationCount, growthRate);
        if (growthRate > 1000.0f) {  // 1KB/s
            trend.trendDirection = "increasing";
        } else if (growthRate < -1000.0f) {
            trend.trendDirection = "decreasing";
        } else {
            trend.trendDirection = "stable";
        }
        
        // Add trend to history
        trends.push_back(trend);
        if (trends.size() > 1000) {
            trends.erase(trends.begin(), trends.begin() + (trends.size() - 1000));
        }
        
        // Update system memory (simulated)
        size_t totalMemory = 2 * 1024 * 1024 * 1024; // 2GB (simulated)
        
        // Check thresholds
        checkThresholds(currentMemory, peakMemory, totalMemory);
        
        // Update state
        previousTotalMemory = currentMemory;
        lastCheckTimestamp = currentTimestamp;
        
        // Notify trend callbacks
        for (const auto& callback : trendCallbacks) {
            callback(trend);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (duration.count() > 100) {  // Log slow checks
            NLM_LOG_WARNING("Memory check took " << duration.count() << "ms")
        }
    }
    
    void checkThresholds(size_t currentMemory, size_t peakMemory, size_t totalMemory) {
        float currentPercent = static_cast<float>(currentMemory) / totalMemory * 100.0f;
        
        // Check error threshold
        bool errorAlert = false;
        bool warningAlert = false;
        
        if (config.enableThresholds) {
            if (config.enableAbsoluteThresholds) {
                if (currentMemory >= config.errorThreshold) {
                    errorAlert = true;
                } else if (currentMemory >= config.warningThreshold) {
                    warningAlert = true;
                }
            }
            
            if (config.enablePercentThresholds) {
                if (currentPercent >= config.errorPercent) {
                    errorAlert = true;
                } else if (currentPercent >= config.warningPercent) {
                    warningAlert = true;
                }
            }
        }
        
        // Throttle error alerts
        if (errorAlert) {
            uint64_t now = getCurrentTimestamp();
            if (now - lastAlertTimestamps[1] >= ALERT_THROTTLE_MS) {
                createAndRecordAlert(currentMemory, totalMemory, currentPercent, "ERROR", config.errorMessage);
                lastAlertTimestamps[1] = now;
            }
        }
        
        // Throttle warning alerts
        if (warningAlert) {
            uint64_t now = getCurrentTimestamp();
            if (now - lastAlertTimestamps[0] >= ALERT_THROTTLE_MS) {
                createAndRecordAlert(currentMemory, totalMemory, currentPercent, "WARNING", config.warningMessage);
                lastAlertTimestamps[0] = now;
            }
        }
    }
    
    void createAndRecordAlert(size_t currentMemory, size_t totalMemory, float percent,
                              const std::string& type, const std::string& baseMessage) {
        std::string message = baseMessage;
        if (type == "ERROR") {
            message = "CRITICAL: " + message;
        }
        
        // Add additional context
        message += " (" + std::to_string(currentMemory) + " bytes, " + std::to_string(percent) + "% total)";
        
        MemoryAlert alert(currentMemory, totalMemory, percent, type, message);
        
        // Add to recent alerts
        recentAlerts.push_back(alert);
        if (recentAlerts.size() > 1000) {
            recentAlerts.erase(recentAlerts.begin(), recentAlerts.begin() + (recentAlerts.size() - 1000));
        }
        
        // Notify alert callbacks
        for (const auto& callback : alertCallbacks) {
            callback(alert);
        }
        
        // Log the alert
        if (type == "ERROR") {
            NLM_LOG_ERROR(message);
        } else {
            NLM_LOG_WARNING(message);
        }
    }
    
    void setDefaultThresholds() {
        config.warningThreshold = 100 * 1024 * 1024;  // 100MB
        config.errorThreshold = 500 * 1024 * 1024;    // 500MB
        config.warningPercent = 80.0f;               // 80%
        config.errorPercent = 90.0f;                 // 90%
        config.warningMessage = "Memory usage warning: High memory usage detected";
        config.errorMessage = "Memory usage error: Critical memory usage exceeded";
        config.enableThresholds = true;
        config.enablePercentThresholds = true;
        config.enableAbsoluteThresholds = true;
    }
    
    static uint64_t getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    static std::string getCurrentThreadId() {
        auto id = std::this_thread::get_id();
        return std::to_string(std::hash<std::thread::id>{}(id));
    }
};

// Global memory monitor instance
inline MemoryMonitor& getMemoryMonitor() {
    static MemoryMonitor monitor;
    return monitor;
}

// Convenience functions for memory monitoring
inline void startMemoryMonitoring(size_t warningThreshold = 100 * 1024 * 1024,
                                 size_t errorThreshold = 500 * 1024 * 1024) {
    MemoryMonitor& monitor = getMemoryMonitor();
    monitor.setThresholds(warningThreshold, errorThreshold);
    monitor.start();
}

inline void stopMemoryMonitoring() {
    getMemoryMonitor().stop();
}

inline void updateComponentMemory(const std::string& name, size_t memory) {
    getMemoryMonitor().updateComponentMemory(name, memory);
}

inline void addMemoryAlertCallback(MemoryAlertCallback callback) {
    getMemoryMonitor().addAlertCallback(callback);
}

inline void addMemoryTrendCallback(MemoryTrendCallback callback) {
    getMemoryMonitor().addTrendCallback(callback);
}

inline std::string getMemoryMonitorStatus() {
    return getMemoryMonitor().getStatusReport();
}

inline void forceMemoryCheck() {
    getMemoryMonitor().forceCheck();
}

} // namespace nlm
