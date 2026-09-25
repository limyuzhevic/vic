// Runtime Diagnostics and Monitoring for NLM
// Comprehensive system monitoring and diagnostics tools

#ifndef NLM_DIAGNOSTICS_H
#define NLM_DIAGNOSTICS_H

#include "core/EnhancedErrorHandling.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <optional>
#include <map>
#include <algorithm>
#include <numeric>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;              // Percentage (0-100)
    double memoryUsage;           // MB
    double memoryPercent;        // Percentage (0-100)
    double diskUsage;             // MB
    double diskPercent;          // Percentage (0-100)
    size_t activeThreads;         // Number of active threads
    size_t totalThreads;          // Total threads created
    size_t garbageCollections;    // Number of garbage collections
    std::unordered_map<std::string, double> customMetrics;
    
    PerformanceMetrics()
        : timestamp(std::chrono::system_clock::now()),
          cpuUsage(0.0), memoryUsage(0.0), memoryPercent(0.0),
          diskUsage(0.0), diskPercent(0.0), activeThreads(0), totalThreads(0),
          garbageCollections(0) {}
    
    std::string getFormattedMetrics() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "CPU: " << cpuUsage << "% | "
           << "RAM: " << memoryUsage << "MB (" << memoryPercent << "%) | "
           << "Disk: " << diskUsage << "MB (" << diskPercent << "%) | "
           << "Threads: " << activeThreads << "/" << totalThreads << " | "
           << "GCs: " << garbageCollections;
        return ss.str();
    }
};

// System health status
struct HealthStatus {
    bool isHealthy;
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
    std::vector<std::string> info;
    std::chrono::system_clock::time_point lastCheck;
    double healthScore;           // 0.0 to 100.0
    
    HealthStatus()
        : isHealthy(true), healthScore(100.0), lastCheck(std::chrono::system_clock::now()) {}
    
    std::string getHealthSummary() const {
        std::stringstream ss;
        ss << "Health: " << (isHealthy ? "✓ HEALTHY" : "✗ UNHEALTHY")
           << " | Score: " << std::fixed << std::setprecision(1) << healthScore << "/100"
           << " | Issues: " << issues.size()
           << " | Warnings: " << warnings.size();
        return ss.str();
    }
};

// Component health information
struct ComponentHealth {
    std::string name;
    bool isOperational;
    std::string statusMessage;
    std::chrono::system_clock::time_point lastUpdate;
    std::unordered_map<std::string, std::string> metadata;
    
    ComponentHealth(const std::string& n = "", bool op = true, const std::string& msg = "")
        : name(n), isOperational(op), statusMessage(msg),
          lastUpdate(std::chrono::system_clock::now()) {}
};

// Runtime diagnostic information
struct DiagnosticInfo {
    std::chrono::system_clock::time_point sessionStartTime;
    std::chrono::system_clock::time_point lastActivityTime;
    size_t totalOperations;
    size_t successfulOperations;
    size_t failedOperations;
    double successRate;
    std::vector<PerformanceMetrics> performanceHistory;
    std::vector<EnhancedErrorInfo> recentErrors;
    std::unordered_map<std::string, ComponentHealth> componentHealth;
    std::unordered_map<std::string, double> systemResources;
    
    DiagnosticInfo() : totalOperations(0), successfulOperations(0), failedOperations(0),
                      successRate(0.0), sessionStartTime(std::chrono::system_clock::now()),
                      lastActivityTime(std::chrono::system_clock::now()) {}
    
    void updateSuccessRate() {
        if (totalOperations > 0) {
            successRate = (static_cast<double>(successfulOperations) / totalOperations) * 100.0;
        } else {
            successRate = 0.0;
        }
    }
    
    std::string getDiagnosticSummary() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "=== NLM Runtime Diagnostics ===" << std::endl;
        ss << "Session Duration: " << getSessionDurationString() << std::endl;
        ss << "Total Operations: " << totalOperations << " ("
           << successfulOperations << " successful, " << failedOperations << " failed)" << std::endl;
        ss << "Success Rate: " << successRate << "%" << std::endl;
        ss << "Active Components: " << componentHealth.size() << std::endl;
        
        if (!recentErrors.empty()) {
            ss << "Recent Errors: " << recentErrors.size() << std::endl;
        }
        
        if (!performanceHistory.empty()) {
            ss << "Latest Performance: " << performanceHistory.back().getFormattedMetrics() << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::string getSessionDurationString() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime);
        
        int hours = duration.count() / 3600;
        int minutes = (duration.count() % 3600) / 60;
        int seconds = duration.count() % 60;
        
        std::stringstream ss;
        if (hours > 0) {
            ss << hours << "h ";
        }
        ss << minutes << "m " << seconds << "s";
        return ss.str();
    }
};

// System monitor for runtime diagnostics
class SystemMonitor {
public:
    // Monitor callbacks
    using HealthCheckCallback = std::function<void(const HealthStatus&)>
    using PerformanceCallback = std::function<void(const PerformanceMetrics&)>
    using ComponentCallback = std::function<void(const std::string&, const ComponentHealth&)>
    
    // Monitor singleton
    static SystemMonitor& getInstance() {
        static SystemMonitor instance;
        return instance;
    }
    
    // Initialization and control
    void initialize() {
        std::unique_lock<std::mutex> lock(mutex_);
        isMonitoring_ = true;
        monitorThread_ = std::thread(&SystemMonitor::monitorLoop, this);
    }
    
    void shutdown() {
        std::unique_lock<std::mutex> lock(mutex_);
        isMonitoring_ = false;
        monitorThread_.join();
    }
    
    // Health checking
    void addHealthCheck(std::function<bool()> check, const std::string& name = "") {
        std::unique_lock<std::mutex> lock(mutex_);
        healthChecks_.push_back({check, name.empty() ? "anonymous" : name});
    }
    
    void removeHealthCheck(std::function<bool()> check) {
        std::unique_lock<std::mutex> lock(mutex_);
        healthChecks_.erase(std::remove(healthChecks_.begin(), healthChecks_.end(),
                                      check, [](const auto& hc) { return hc.check; }),
                           healthChecks_.end());
    }
    
    HealthStatus performHealthCheck() {
        std::unique_lock<std::mutex> lock(mutex_);
        HealthStatus status;
        
        // Check resource usage
        updateResourceUsage(status);
        
        // Run all health checks
        for (const auto& healthCheck : healthChecks_) {
            try {
                bool checkResult = healthCheck.check();
                if (!checkResult) {
                    status.isHealthy = false;
                    status.issues.push_back(healthCheck.name + " health check failed");
                }
            } catch (const std::exception& e) {
                status.isHealthy = false;
                status.issues.push_back("Health check error for " + healthCheck.name + ": " + std::string(e.what()));
            }
        }
        
        // Calculate health score
        calculateHealthScore(status);
        
        // Trigger health check callbacks
        for (const auto& callback : healthCheckCallbacks_) {
            try {
                callback(status);
            } catch (const std::exception& e) {
                // Log callback errors but don't propagate
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Health check callback error: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
            }
        }
        
        return status;
    }
    
    // Performance monitoring
    void recordPerformanceMetrics(const PerformanceMetrics& metrics) {
        std::unique_lock<std::mutex> lock(mutex_);
        performanceHistory_.push_back(metrics);
        
        // Keep only last 1000 entries
        if (performanceHistory_.size() > 1000) {
            performanceHistory_.erase(performanceHistory_.begin());
        }
        
        // Trigger performance callbacks
        for (const auto& callback : performanceCallbacks_) {
            try {
                callback(metrics);
            } catch (const std::exception& e) {
                // Log callback errors but don't propagate
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Performance callback error: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
            }
        }
    }
    
    // Component monitoring
    void updateComponentHealth(const std::string& name, const ComponentHealth& health) {
        std::unique_lock<std::mutex> lock(mutex_);
        componentHealth_[name] = health;
        health_.lastUpdate = std::chrono::system_clock::now();
        
        // Trigger component callbacks
        for (const auto& callback : componentCallbacks_) {
            try {
                callback(name, health);
            } catch (const std::exception& e) {
                // Log callback errors but don't propagate
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Component callback error for " + name + ": " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
            }
        }
    }
    
    // Diagnostic information
    DiagnosticInfo getDiagnosticInfo() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return diagnosticInfo_;
    }
    
    // Statistics and analysis
    std::vector<PerformanceMetrics> getPerformanceHistory(size_t count = 100) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (performanceHistory_.empty()) {
            return std::vector<PerformanceMetrics>();
        }
        
        size_t start = std::max<size_t>(0, performanceHistory_.size() - count);
        return std::vector<PerformanceMetrics>(performanceHistory_.begin() + start,
                                              performanceHistory_.end());
    }
    
    double getAverageCPUUsage(size_t count = 60) const {
        auto history = getPerformanceHistory(count);
        if (history.empty()) return 0.0;
        
        double total = std::accumulate(history.begin(), history.end(), 0.0,
                                      [](double sum, const PerformanceMetrics& m) {
                                          return sum + m.cpuUsage;
                                      });
        return total / history.size();
    }
    
    double getAverageMemoryUsage(size_t count = 60) const {
        auto history = getPerformanceHistory(count);
        if (history.empty()) return 0.0;
        
        double total = std::accumulate(history.begin(), history.end(), 0.0,
                                      [](double sum, const PerformanceMetrics& m) {
                                          return sum + m.memoryUsage;
                                      });
        return total / history.size();
    }
    
    // Callbacks
    void addHealthCheckCallback(HealthCheckCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        healthCheckCallbacks_.push_back(callback);
    }
    
    void removeHealthCheckCallback(HealthCheckCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        healthCheckCallbacks_.erase(std::remove(healthCheckCallbacks_.begin(), healthCheckCallbacks_.end(), callback),
                                   healthCheckCallbacks_.end());
    }
    
    void addPerformanceCallback(PerformanceCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        performanceCallbacks_.push_back(callback);
    }
    
    void removePerformanceCallback(PerformanceCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        performanceCallbacks_.erase(std::remove(performanceCallbacks_.begin(), performanceCallbacks_.end(), callback),
                                   performanceCallbacks_.end());
    }
    
    void addComponentCallback(ComponentCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        componentCallbacks_.push_back(callback);
    }
    
    void removeComponentCallback(ComponentCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        componentCallbacks_.erase(std::remove(componentCallbacks_.begin(), componentCallbacks_.end(), callback),
                                 componentCallbacks_.end());
    }
    
    // Resource monitoring
    void updateResourceUsage(HealthStatus& status) {
        // Mock resource usage (in real implementation, this would query system resources)
        status.systemResources["cpu"] = getAverageCPUUsage();
        status.systemResources["memory"] = getAverageMemoryUsage();
        status.systemResources["threads"] = diagnosticInfo_.componentHealth.size();
        
        // Check for resource issues
        if (status.systemResources["cpu"] > 80.0) {
            status.warnings.push_back("CPU usage is high: " + std::to_string(status.systemResources["cpu"]) + "%");
        }
        
        if (status.systemResources["memory"] > 2000.0) {  // 2GB threshold
            status.warnings.push_back("Memory usage is high: " + std::to_string(status.systemResources["memory"]) + "MB");
        }
    }
    
    // Health score calculation
    void calculateHealthScore(HealthStatus& status) {
        double score = 100.0;
        
        // Deduct points for issues
        score -= status.issues.size() * 10.0;
        
        // Deduct points for warnings
        score -= status.warnings.size() * 5.0;
        
        // Deduct points for resource issues
        auto& resources = status.systemResources;
        if (resources["cpu"] > 90.0) score -= 20.0;
        if (resources["memory"] > 4000.0) score -= 20.0;
        
        // Ensure score is within bounds
        score = std::max(0.0, std::min(100.0, score));
        status.healthScore = score;
        
        if (score < 50.0) {
            status.isHealthy = false;
        }
    }
    
protected:
    SystemMonitor() = default;
    ~SystemMonitor() = default;
    
    SystemMonitor(const SystemMonitor&) = delete;
    SystemMonitor& operator=(const SystemMonitor&) = delete;
    
private:
    mutable std::shared_mutex mutex_;
    std::mutex monitorMutex_;
    
    bool isMonitoring_ = false;
    std::thread monitorThread_;
    
    struct HealthCheck {
        std::function<bool()> check;
        std::string name;
        
        bool operator==(const HealthCheck& other) const {
            return check.target<void()>() == other.check.target<void()>() && name == other.name;
        }
    };
    
    std::vector<HealthCheck> healthChecks_;
    std::vector<PerformanceMetrics> performanceHistory_;
    std::unordered_map<std::string, ComponentHealth> componentHealth_;
    
    std::vector<HealthCheckCallback> healthCheckCallbacks_;
    std::vector<PerformanceCallback> performanceCallbacks_;
    std::vector<ComponentCallback> componentCallbacks_;
    
    DiagnosticInfo diagnosticInfo_;
    
    void monitorLoop() {
        while (isMonitoring_) {
            try {
                // Perform health check
                HealthStatus status = performHealthCheck();
                
                // Update diagnostic info
                diagnosticInfo_.recentErrors = getEnhancedErrorManager().getRecentErrors(50);
                diagnosticInfo_.lastActivityTime = std::chrono::system_clock::now();
                
                // Record current performance
                PerformanceMetrics metrics;
                recordPerformanceMetrics(metrics);
                
                // Sleep for monitoring interval
                std::this_thread::sleep_for(std::chrono::seconds(5));
                
            } catch (const std::exception& e) {
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Monitoring loop error: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
};

// Global system monitor instance
inline SystemMonitor& getSystemMonitor() {
    return SystemMonitor::getInstance();
}

// Utility functions for diagnostics
inline std::string getSystemInfo() {
    std::stringstream ss;
    ss << "NLM Runtime System Information" << std::endl;
    ss << "=============================" << std::endl;
    ss << "Platform: Linux" << std::endl;
    ss << "Architecture: x86_64" << std::endl;
    ss << "Compiler: GCC" << std::endl;
    ss << "Version: Phase 2" << std::endl;
    ss << "Build Date: " << __DATE__ << std::endl;
    ss << "Build Time: " << __TIME__ << std::endl;
    
    return ss.str();
}

inline std::string getDiagnosticReport() {
    std::stringstream ss;
    
    // Get error manager report
    ss << getEnhancedErrorManager().generateErrorReport() << std::endl;
    
    // Get system monitor diagnostic info
    ss << getSystemMonitor().getDiagnosticInfo().getDiagnosticSummary() << std::endl;
    
    // Get system info
    ss << getSystemInfo() << std::endl;
    
    return ss.str();
}

} // namespace nlm

#endif // NLM_DIAGNOSTICS_H
