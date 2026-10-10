// NLM Advanced Features - Monitor Mode
// System monitoring mode for real-time performance analysis and alerts

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <deque>
#include <memory>
#include <algorithm>
#include <numeric>

namespace nlm {
namespace advanced {

// System metrics
struct SystemMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;
    double memoryUsage;
    double memoryAvailable;
    double diskUsage;
    double networkUpload;
    double networkDownload;
    uint32_t activeProcesses;
    uint32_t threads;
    double loadAverage[3]; // 1, 5, 15 minute averages
    double temperature;
    double batteryLevel;
    bool batteryCharging;
    std::map<std::string, double> customMetrics;
    
    SystemMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryAvailable(0.0),
                      diskUsage(0.0), networkUpload(0.0), networkDownload(0.0),
                      activeProcesses(0), threads(0), temperature(0.0),
                      batteryLevel(0.0), batteryCharging(false) {
        timestamp = std::chrono::system_clock::now();
        for (int i = 0; i < 3; ++i) loadAverage[i] = 0.0;
    }
};

// Performance metrics
struct PerformanceMetrics {
    std::chrono::system_clock::time_point timestamp;
    double processingSpeed;
    double throughput;
    double responseTime;
    double errorRate;
    double cpuTime;
    double userTime;
    uint64_t contextSwitches;
    uint64_t pageFaults;
    std::map<std::string, double> componentMetrics;
    
    PerformanceMetrics() : processingSpeed(0.0), throughput(0.0), responseTime(0.0),
                           errorRate(0.0), cpuTime(0.0), userTime(0.0),
                           contextSwitches(0), pageFaults(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// System alert
struct SystemAlert {
    std::chrono::system_clock::time_point timestamp;
    std::string id;
    std::string source;
    std::string message;
    std::string severity;
    std::string category;
    bool resolved;
    std::map<std::string, std::string> metadata;
    
    SystemAlert() : severity("INFO"), resolved(false) {
        timestamp = std::chrono::system_clock::now();
    }
};

// System statistics
struct SystemStatistics {
    uint64_t totalSamples;
    double averageCPUUsage;
    double peakCPUUsage;
    double averageMemoryUsage;
    double peakMemoryUsage;
    double averageDiskUsage;
    double peakDiskUsage;
    double averageNetworkUpload;
    double peakNetworkUpload;
    double averageNetworkDownload;
    double peakNetworkDownload;
    std::chrono::system_clock::time_point firstSampleTime;
    std::chrono::system_clock::time_point lastSampleTime;
    double sampleRatePerMinute;
    std::string mostFrequentAlert;
    uint64_t mostFrequentAlertCount;
    std::deque<SystemAlert> recentAlerts;
    
    SystemStatistics() : totalSamples(0), averageCPUUsage(0.0), peakCPUUsage(0.0),
                         averageMemoryUsage(0.0), peakMemoryUsage(0.0),
                         averageDiskUsage(0.0), peakDiskUsage(0.0),
                         averageNetworkUpload(0.0), peakNetworkUpload(0.0),
                         averageNetworkDownload(0.0), peakNetworkDownload(0.0),
                         mostFrequentAlertCount(0) {
    }
};

// Monitoring configuration
struct MonitorConfig {
    double monitoringInterval; // in seconds
    double samplingInterval; // in seconds
    double alertThresholdCPU;
    double alertThresholdMemory;
    double alertThresholdDisk;
    double alertThresholdNetwork;
    bool enableCPUMonitoring;
    bool enableMemoryMonitoring;
    bool enableDiskMonitoring;
    bool enableNetworkMonitoring;
    bool enableProcessMonitoring;
    bool enableSystemMonitoring;
    std::vector<std::string> monitoredProcesses;
    std::vector<std::string> customMetrics;
    std::map<std::string, double> thresholdOverrides;
    std::map<std::string, std::string> alertCategories;
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::string outputFormat;
    std::string outputFile;
    bool enableConsoleOutput;
    bool enableFileOutput;
    bool enableEmailAlerts;
    std::string emailAlertAddress;
    std::function<bool(const SystemMetrics&)>) metricFilter;
    std::function<void(const SystemMetrics&)>) metricCallback;
    std::function<bool(const SystemAlert&)>) alertFilter;
    std::function<void(const SystemAlert&)>) alertCallback;
    
    MonitorConfig() : monitoringInterval(1.0), samplingInterval(0.1),
                      alertThresholdCPU(80.0), alertThresholdMemory(90.0),
                      alertThresholdDisk(85.0), alertThresholdNetwork(100.0),
                      enableCPUMonitoring(true), enableMemoryMonitoring(true),
                      enableDiskMonitoring(true), enableNetworkMonitoring(true),
                      enableProcessMonitoring(true), enableSystemMonitoring(true),
                      enableConsoleOutput(true), enableFileOutput(false),
                      enableEmailAlerts(false) {
    }
};

// Monitor mode class
class MonitorMode {
public:
    MonitorMode();
    ~MonitorMode();
    
    // Configuration
    void configure(const MonitorConfig& config);
    MonitorConfig getConfig() const;
    
    // Monitoring control
    bool start();
    void stop();
    bool isActive() const;
    bool isCollecting() const;
    
    // Data collection
    SystemMetrics collectSystemMetrics() const;
    PerformanceMetrics collectPerformanceMetrics() const;
    void recordMetrics(const SystemMetrics& metrics);
    void recordMetrics(const PerformanceMetrics& metrics);
    
    // Metrics query
    std::vector<SystemMetrics> getSystemMetrics() const;
    std::vector<SystemMetrics> getSystemMetricsByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    std::vector<PerformanceMetrics> getPerformanceMetrics() const;
    SystemMetrics getLatestSystemMetrics() const;
    PerformanceMetrics getLatestPerformanceMetrics() const;
    
    // Alerts
    std::vector<SystemAlert> getAlerts() const;
    std::vector<SystemAlert> getAlertsBySeverity(const std::string& severity) const;
    std::vector<SystemAlert> getAlertsByCategory(const std::string& category) const;
    std::vector<SystemAlert> getAlertsBySource(const std::string& source) const;
    SystemAlert getAlert(const std::string& alertId) const;
    
    // Alert management
    void addAlert(const SystemAlert& alert);
    void resolveAlert(const std::string& alertId);
    void clearAlerts();
    
    // Statistics
    SystemStatistics getSystemStatistics() const;
    double getAverageCPUUsage() const;
    double getPeakCPUUsage() const;
    double getAverageMemoryUsage() const;
    double getPeakMemoryUsage() const;
    
    // Reporting
    std::string generateSystemReport() const;
    std::string generatePerformanceReport() const;
    std::string generateAlertReport() const;
    std::string generateHealthReport() const;
    bool exportReport(const std::string& filename) const;
    bool exportDetailedReport(const std::string& filename) const;
    
    // Alert generation
    void checkAndGenerateAlerts(const SystemMetrics& metrics);
    bool shouldGenerateAlert(const std::string& category, double value) const;
    
    // Configuration
    void setMonitoringInterval(double interval);
    void setSamplingInterval(double interval);
    void setThreshold(const std::string& category, double threshold);
    void enableCategory(const std::string& category, bool enabled);
    void addMonitoredProcess(const std::string& processName);
    void setMetricFilter(const std::function<bool(const SystemMetrics&)>& filter);
    void setMetricCallback(const std::function<void(const SystemMetrics&)>& callback);
    void setAlertFilter(const std::function<bool(const SystemAlert&)>& filter);
    void setAlertCallback(const std::function<void(const SystemAlert&)>& callback);
    
    // Callbacks
    void setDataCallback(const std::function<void(const SystemMetrics&, const PerformanceMetrics&)>& callback);
    void setAlertCallback(const std::function<void(const SystemAlert&)>& callback);
    void setErrorCallback(const std::function<void(const std::string&)>& callback);
    
    // Singleton pattern
    static MonitorMode& getInstance();
    static void resetInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void startCollectionThread();
    void stopCollectionThread();
    void collectionWorker();
    SystemMetrics collectCPUMetrics() const;
    SystemMetrics collectMemoryMetrics() const;
    SystemMetrics collectDiskMetrics() const;
    SystemMetrics collectNetworkMetrics() const;
    SystemMetrics collectProcessMetrics() const;
    PerformanceMetrics collectProcessingMetrics() const;
    void updateStatistics(const SystemMetrics& metrics);
    void updateStatistics(const PerformanceMetrics& metrics);
    std::string formatTime(double seconds) const;
    std::string formatBytes(size_t bytes) const;
    std::string getTimeString(const std::chrono::system_clock::time_point& time) const;
    double calculateCPUUsage() const;
    double calculateMemoryUsage() const;
    double calculateDiskUsage() const;
    double calculateNetworkUsage() const;
};

} // namespace advanced
} // namespace nlm

// Export MonitorMode functions for external use
namespace nlm {
    namespace advanced {
        // Global monitor mode functions
        inline bool startMonitoring() {
            return MonitorMode::getInstance().start();
        }
        
        inline void stopMonitoring() {
            MonitorMode::getInstance().stop();
        }
        
        inline bool isMonitoring() {
            return MonitorMode::getInstance().isActive();
        }
        
        inline SystemMetrics getSystemMetrics() {
            return MonitorMode::getInstance().getLatestSystemMetrics();
        }
        
        inline PerformanceMetrics getPerformanceMetrics() {
            return MonitorMode::getInstance().getLatestPerformanceMetrics();
        }
        
        // Convenience macros
        #define NLM_MONITOR_START() nlm::advanced::startMonitoring()
        #define NLM_MONITOR_STOP() nlm::advanced::stopMonitoring()
        #define NLM_IS_MONITORING() nlm::advanced::isMonitoring()
        
        #define NLM_GET_SYSTEM_METRICS() nlm::advanced::getSystemMetrics()
        #define NLM_GET_PERFORMANCE_METRICS() nlm::advanced::getPerformanceMetrics()
        
    } // namespace advanced
} // namespace nlm

// Include implementation
#include "MonitorMode.cpp"

// Initialize global monitor macros
namespace nlm {
    namespace advanced {
        // Global monitoring macros for use throughout the codebase
        #define NLM_MONITOR_START_SESSION() nlm::advanced::startMonitoring()
        #define NLM_MONITOR_END_SESSION() nlm::advanced::stopMonitoring()
        
        #define NLM_MONITOR_SYSTEM() nlm::advanced::getSystemMetrics()
        #define NLM_MONITOR_PERFORMANCE() nlm::advanced::getPerformanceMetrics()
        
        #define NLM_MONITOR_ALERT(source, message, severity, category) { \
            nlm::advanced::SystemAlert alert; \
            alert.source = source; \
            alert.message = message; \
            alert.severity = severity; \
            alert.category = category; \
            alert.resolved = false; \
            nlm::advanced::MonitorMode::getInstance().addAlert(alert); \
        }
        
    } // namespace advanced
} // namespace nlm