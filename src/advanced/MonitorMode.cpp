// NLM Advanced Features - Monitor Mode Implementation
// System monitoring mode for real-time performance analysis and alerts

#include "MonitorMode.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>

namespace nlm {
namespace advanced {

// Implementation of MonitorMode

struct MonitorMode::Impl {
    MonitorConfig config;
    bool active;
    bool collecting;
    std::vector<SystemMetrics> systemMetricsHistory;
    std::vector<PerformanceMetrics> performanceMetricsHistory;
    std::vector<SystemAlert> alerts;
    SystemStatistics systemStatistics;
    PerformanceStatistics performanceStatistics;
    
    std::thread collectionThread;
    std::atomic<bool> stopCollection;
    std::mutex dataMutex;
    std::mutex configMutex;
    std::condition_variable dataCondition;
    
    std::function<bool(const SystemMetrics&)>) metricFilter;
    std::function<void(const SystemMetrics&)>) metricCallback;
    std::function<bool(const SystemAlert&)>) alertFilter;
    std::function<void(const SystemAlert&)>) alertCallback;
    std::function<void(const SystemMetrics&, const PerformanceMetrics&)>) dataCallback;
    std::function<void(const std::string&)>) errorCallback;
    
    Impl() : active(false), collecting(false), stopCollection(false) {
        systemStatistics.firstSampleTime = std::chrono::system_clock::now();
        performanceStatistics.firstSampleTime = std::chrono::system_clock::now();
    }
    
    ~Impl() {
        stop();
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (active) {
            return;
        }
        
        active = true;
        collecting = true;
        stopCollection = false;
        
        // Start collection thread
        collectionThread = std::thread(&Impl::collectionWorker, this);
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(configMutex);
        
        if (!active) {
            return;
        }
        
        collecting = false;
        stopCollection = true;
        
        // Wake up collection thread
        dataCondition.notify_all();
        
        // Wait for collection thread to finish
        if (collectionThread.joinable()) {
            collectionThread.join();
        }
        
        active = false;
    }
    
    void collectionWorker() {
        while (!stopCollection.load() && collecting.load()) {
            std::unique_lock<std::mutex> lock(dataMutex);
            
            // Collect system metrics
            SystemMetrics systemMetrics = collectSystemMetrics();
            
            // Collect performance metrics
            PerformanceMetrics performanceMetrics = collectPerformanceMetrics();
            
            // Store metrics
            systemMetricsHistory.push_back(systemMetrics);
            performanceMetricsHistory.push_back(performanceMetrics);
            
            // Update statistics
            updateStatistics(systemMetrics);
            updateStatistics(performanceMetrics);
            
            // Check for alerts
            checkAndGenerateAlerts(systemMetrics);
            
            // Call callbacks
            if (metricCallback) {
                metricCallback(systemMetrics);
            }
            
            if (dataCallback) {
                dataCallback(systemMetrics, performanceMetrics);
            }
            
            // Notify condition variable
            dataCondition.notify_all();
            
            // Wait for next collection interval
            std::chrono::duration<double> sleepDuration(config.monitoringInterval);
            std::chrono::milliseconds sleepMs(static_cast<long>(sleepDuration.count() * 1000));
            
            if (dataCondition.wait_for(lock, sleepMs, 
                                       [this]() { return stopCollection.load() || !collecting.load(); })) {
                continue;
            }
        }
    }
    
    SystemMetrics collectSystemMetrics() const {
        SystemMetrics metrics;
        
        metrics.cpuUsage = calculateCPUUsage();
        metrics.memoryUsage = calculateMemoryUsage();
        metrics.memoryAvailable = getMemoryAvailable();
        metrics.diskUsage = calculateDiskUsage();
        metrics.networkUpload = calculateNetworkUpload();
        metrics.networkDownload = calculateNetworkDownload();
        metrics.activeProcesses = getActiveProcessCount();
        metrics.threads = getThreadCount();
        getLoadAverage(metrics.loadAverage);
        metrics.temperature = getTemperature();
        getBatteryInfo(metrics.batteryLevel, metrics.batteryCharging);
        
        return metrics;
    }
    
    PerformanceMetrics collectPerformanceMetrics() const {
        PerformanceMetrics metrics;
        
        metrics.processingSpeed = getProcessingSpeed();
        metrics.throughput = getThroughput();
        metrics.responseTime = getResponseTime();
        metrics.errorRate = getErrorRate();
        metrics.cpuTime = getCPUTime();
        metrics.userTime = getUserTime();
        metrics.contextSwitches = getContextSwitches();
        metrics.pageFaults = getPageFaults();
        
        return metrics;
    }
    
    double calculateCPUUsage() const {
        // Implementation would read system-specific CPU metrics
        // This is a placeholder implementation
        static double cpuUsage = 0.0;
        return cpuUsage;
    }
    
    double calculateMemoryUsage() const {
        // Implementation would read system-specific memory metrics
        // This is a placeholder implementation
        static double memoryUsage = 0.0;
        return memoryUsage;
    }
    
    double getMemoryAvailable() const {
        // Implementation would read system-specific memory metrics
        return 0.0; // Placeholder
    }
    
    double calculateDiskUsage() const {
        // Implementation would read system-specific disk metrics
        return 0.0; // Placeholder
    }
    
    double calculateNetworkUpload() const {
        // Implementation would read system-specific network metrics
        return 0.0; // Placeholder
    }
    
    double calculateNetworkDownload() const {
        // Implementation would read system-specific network metrics
        return 0.0; // Placeholder
    }
    
    uint32_t getActiveProcessCount() const {
        // Implementation would read system-specific process metrics
        return 0; // Placeholder
    }
    
    uint32_t getThreadCount() const {
        // Implementation would read system-specific thread metrics
        return 0; // Placeholder
    }
    
    void getLoadAverage(double load[3]) const {
        // Implementation would read system-specific load metrics
        for (int i = 0; i < 3; ++i) load[i] = 0.0; // Placeholder
    }
    
    double getTemperature() const {
        // Implementation would read system-specific temperature metrics
        return 0.0; // Placeholder
    }
    
    void getBatteryInfo(double& level, bool& charging) const {
        // Implementation would read system-specific battery metrics
        level = 0.0; charging = false; // Placeholder
    }
    
    double getProcessingSpeed() const {
        // Implementation would calculate system processing speed
        return 0.0; // Placeholder
    }
    
    double getThroughput() const {
        // Implementation would calculate system throughput
        return 0.0; // Placeholder
    }
    
    double getResponseTime() const {
        // Implementation would calculate system response time
        return 0.0; // Placeholder
    }
    
    double getErrorRate() const {
        // Implementation would calculate system error rate
        return 0.0; // Placeholder
    }
    
    double getCPUTime() const {
        // Implementation would calculate CPU time
        return 0.0; // Placeholder
    }
    
    double getUserTime() const {
        // Implementation would calculate user time
        return 0.0; // Placeholder
    }
    
    uint64_t getContextSwitches() const {
        // Implementation would get context switch count
        return 0; // Placeholder
    }
    
    uint64_t getPageFaults() const {
        // Implementation would get page fault count
        return 0; // Placeholder
    }
    
    void updateStatistics(const SystemMetrics& metrics) {
        systemStatistics.totalSamples++;
        
        // Update averages
        systemStatistics.averageCPUUsage = ((systemStatistics.averageCPUUsage * (systemStatistics.totalSamples - 1)) + metrics.cpuUsage) / systemStatistics.totalSamples;
        systemStatistics.averageMemoryUsage = ((systemStatistics.averageMemoryUsage * (systemStatistics.totalSamples - 1)) + metrics.memoryUsage) / systemStatistics.totalSamples;
        systemStatistics.averageDiskUsage = ((systemStatistics.averageDiskUsage * (systemStatistics.totalSamples - 1)) + metrics.diskUsage) / systemStatistics.totalSamples;
        systemStatistics.averageNetworkUpload = ((systemStatistics.averageNetworkUpload * (systemStatistics.totalSamples - 1)) + metrics.networkUpload) / systemStatistics.totalSamples;
        systemStatistics.averageNetworkDownload = ((systemStatistics.averageNetworkDownload * (systemStatistics.totalSamples - 1)) + metrics.networkDownload) / systemStatistics.totalSamples;
        
        // Update peaks
        systemStatistics.peakCPUUsage = std::max(systemStatistics.peakCPUUsage, metrics.cpuUsage);
        systemStatistics.peakMemoryUsage = std::max(systemStatistics.peakMemoryUsage, metrics.memoryUsage);
        systemStatistics.peakDiskUsage = std::max(systemStatistics.peakDiskUsage, metrics.diskUsage);
        systemStatistics.peakNetworkUpload = std::max(systemStatistics.peakNetworkUpload, metrics.networkUpload);
        systemStatistics.peakNetworkDownload = std::max(systemStatistics.peakNetworkDownload, metrics.networkDownload);
        
        // Update timestamps
        auto now = std::chrono::system_clock::now();
        systemStatistics.lastSampleTime = now;
        
        if (systemStatistics.totalSamples == 1) {
            systemStatistics.firstSampleTime = now;
        }
        
        // Calculate sample rate
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - systemStatistics.firstSampleTime);
        if (duration.count() > 0) {
            systemStatistics.sampleRatePerMinute = (static_cast<double>(systemStatistics.totalSamples) / duration.count()) * 60.0;
        }
        
        // Update most frequent alert
        if (!alerts.empty()) {
            std::map<std::string, uint64_t> alertFrequency;
            for (const auto& alert : alerts) {
                alertFrequency[alert.message]++;
            }
            
            uint64_t maxCount = 0;
            std::string mostFrequent;
            for (const auto& pair : alertFrequency) {
                if (pair.second > maxCount) {
                    maxCount = pair.second;
                    mostFrequent = pair.first;
                }
            }
            
            if (maxCount > systemStatistics.mostFrequentAlertCount) {
                systemStatistics.mostFrequentAlertCount = maxCount;
                systemStatistics.mostFrequentAlert = mostFrequent;
            }
        }
        
        // Update recent alerts
        systemStatistics.recentAlerts.clear(); // Simplified: keep only current alerts
    }
    
    void updateStatistics(const PerformanceMetrics& metrics) {
        performanceStatistics.totalSamples++;
        
        // Update averages
        performanceStatistics.averageProcessingSpeed = ((performanceStatistics.averageProcessingSpeed * (performanceStatistics.totalSamples - 1)) + metrics.processingSpeed) / performanceStatistics.totalSamples;
        performanceStatistics.averageThroughput = ((performanceStatistics.averageThroughput * (performanceStatistics.totalSamples - 1)) + metrics.throughput) / performanceStatistics.totalSamples;
        performanceStatistics.averageResponseTime = ((performanceStatistics.averageResponseTime * (performanceStatistics.totalSamples - 1)) + metrics.responseTime) / performanceStatistics.totalSamples;
        performanceStatistics.averageErrorRate = ((performanceStatistics.averageErrorRate * (performanceStatistics.totalSamples - 1)) + metrics.errorRate) / performanceStatistics.totalSamples;
        performanceStatistics.averageCPUTime = ((performanceStatistics.averageCPUTime * (performanceStatistics.totalSamples - 1)) + metrics.cpuTime) / performanceStatistics.totalSamples;
        performanceStatistics.averageUserTime = ((performanceStatistics.averageUserTime * (performanceStatistics.totalSamples - 1)) + metrics.userTime) / performanceStatistics.totalSamples;
        
        // Update timestamps
        auto now = std::chrono::system_clock::now();
        performanceStatistics.lastSampleTime = now;
        
        if (performanceStatistics.totalSamples == 1) {
            performanceStatistics.firstSampleTime = now;
        }
        
        // Calculate sample rate
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - performanceStatistics.firstSampleTime);
        if (duration.count() > 0) {
            performanceStatistics.sampleRatePerMinute = (static_cast<double>(performanceStatistics.totalSamples) / duration.count()) * 60.0;
        }
    }
    
    void checkAndGenerateAlerts(const SystemMetrics& metrics) {
        // Check CPU usage alert
        if (config.enableCPUMonitoring && metrics.cpuUsage > config.alertThresholdCPU) {
            SystemAlert alert;
            alert.id = generateAlertId();
            alert.source = "System Monitor";
            alert.message = "High CPU usage detected: " + std::to_string(metrics.cpuUsage) + "%";
            alert.severity = "WARNING";
            alert.category = "CPU";
            alert.resolved = false;
            
            if (shouldGenerateAlert(alert.category, metrics.cpuUsage)) {
                addAlert(alert);
            }
        }
        
        // Check memory usage alert
        if (config.enableMemoryMonitoring && metrics.memoryUsage > config.alertThresholdMemory) {
            SystemAlert alert;
            alert.id = generateAlertId();
            alert.source = "System Monitor";
            alert.message = "High memory usage detected: " + std::to_string(metrics.memoryUsage) + "%";
            alert.severity = "WARNING";
            alert.category = "Memory";
            alert.resolved = false;
            
            if (shouldGenerateAlert(alert.category, metrics.memoryUsage)) {
                addAlert(alert);
            }
        }
        
        // Check disk usage alert
        if (config.enableDiskMonitoring && metrics.diskUsage > config.alertThresholdDisk) {
            SystemAlert alert;
            alert.id = generateAlertId();
            alert.source = "System Monitor";
            alert.message = "High disk usage detected: " + std::to_string(metrics.diskUsage) + "%";
            alert.severity = "WARNING";
            alert.category = "Disk";
            alert.resolved = false;
            
            if (shouldGenerateAlert(alert.category, metrics.diskUsage)) {
                addAlert(alert);
            }
        }
        
        // Check network usage alert
        double totalNetworkUsage = metrics.networkUpload + metrics.networkDownload;
        if (config.enableNetworkMonitoring && totalNetworkUsage > config.alertThresholdNetwork) {
            SystemAlert alert;
            alert.id = generateAlertId();
            alert.source = "System Monitor";
            alert.message = "High network usage detected: " + std::to_string(totalNetworkUsage) + " MB/s";
            alert.severity = "INFO";
            alert.category = "Network";
            alert.resolved = false;
            
            if (shouldGenerateAlert(alert.category, totalNetworkUsage)) {
                addAlert(alert);
            }
        }
        
        // Check process count alert
        if (config.enableProcessMonitoring && metrics.activeProcesses > 1000) { // Threshold example
            SystemAlert alert;
            alert.id = generateAlertId();
            alert.source = "System Monitor";
            alert.message = "High process count detected: " + std::to_string(metrics.activeProcesses);
            alert.severity = "INFO";
            alert.category = "Processes";
            alert.resolved = false;
            
            if (shouldGenerateAlert(alert.category, static_cast<double>(metrics.activeProcesses))) {
                addAlert(alert);
            }
        }
    }
    
    bool shouldGenerateAlert(const std::string& category, double value) const {
        // Check if alert should be generated based on category and value
        auto it = config.thresholdOverrides.find(category);
        if (it != config.thresholdOverrides.end()) {
            return value > it->second;
        }
        
        // Default threshold check
        if (category == "CPU") return value > config.alertThresholdCPU;
        if (category == "Memory") return value > config.alertThresholdMemory;
        if (category == "Disk") return value > config.alertThresholdDisk;
        if (category == "Network") return value > config.alertThresholdNetwork;
        
        return false;
    }
    
    std::string generateAlertId() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000, 99999);
        
        std::time_t now = std::time(nullptr);
        char timeBuf[20];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y%m%d%H%M%S", std::localtime(&now));
        
        return std::string(timeBuf) + "_" + std::to_string(dis(gen));
    }
    
    std::string formatTime(double seconds) const {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        std::ostringstream oss;
        if (hours > 0) oss << hours << "h ";
        oss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
        return oss.str();
    }
    
    std::string formatBytes(size_t bytes) const {
        if (bytes < 1024) return std::to_string(bytes) + " B";
        if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
        if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
    
    std::string getTimeString(const std::chrono::system_clock::time_point& time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        return ss.str();
    }
};

MonitorMode::MonitorMode() : pImpl(std::make_unique<Impl>()) {
}

MonitorMode::~MonitorMode() {
    stop();
}

void MonitorMode::configure(const MonitorConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config = config;
}

MonitorConfig MonitorMode::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->config;
}

bool MonitorMode::start() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    if (pImpl->active) {
        return false;
    }
    
    pImpl->start();
    return true;
}

void MonitorMode::stop() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->stop();
}

bool MonitorMode::isActive() const {
    return pImpl->active;
}

bool MonitorMode::isCollecting() const {
    return pImpl->collecting;
}

SystemMetrics MonitorMode::collectSystemMetrics() const {
    return pImpl->collectSystemMetrics();
}

PerformanceMetrics MonitorMode::collectPerformanceMetrics() const {
    return pImpl->collectPerformanceMetrics();
}

void MonitorMode::recordMetrics(const SystemMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->systemMetricsHistory.push_back(metrics);
    pImpl->updateStatistics(metrics);
}

void MonitorMode::recordMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->performanceMetricsHistory.push_back(metrics);
    pImpl->updateStatistics(metrics);
}

std::vector<SystemMetrics> MonitorMode::getSystemMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->systemMetricsHistory;
}

std::vector<SystemMetrics> MonitorMode::getSystemMetricsByTimeRange(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<SystemMetrics> result;
    
    for (const auto& metrics : pImpl->systemMetricsHistory) {
        if (metrics.timestamp >= start && metrics.timestamp <= end) {
            result.push_back(metrics);
        }
    }
    
    return result;
}

std::vector<PerformanceMetrics> MonitorMode::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->performanceMetricsHistory;
}

SystemMetrics MonitorMode::getLatestSystemMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->systemMetricsHistory.empty()) {
        return SystemMetrics();
    }
    
    return pImpl->systemMetricsHistory.back();
}

PerformanceMetrics MonitorMode::getLatestPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->performanceMetricsHistory.empty()) {
        return PerformanceMetrics();
    }
    
    return pImpl->performanceMetricsHistory.back();
}

std::vector<SystemAlert> MonitorMode::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->alerts;
}

std::vector<SystemAlert> MonitorMode::getAlertsBySeverity(const std::string& severity) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<SystemAlert> result;
    
    for (const auto& alert : pImpl->alerts) {
        if (alert.severity == severity) {
            result.push_back(alert);
        }
    }
    
    return result;
}

std::vector<SystemAlert> MonitorMode::getAlertsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<SystemAlert> result;
    
    for (const auto& alert : pImpl->alerts) {
        if (alert.category == category) {
            result.push_back(alert);
        }
    }
    
    return result;
}

std::vector<SystemAlert> MonitorMode::getAlertsBySource(const std::string& source) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<SystemAlert> result;
    
    for (const auto& alert : pImpl->alerts) {
        if (alert.source == source) {
            result.push_back(alert);
        }
    }
    
    return result;
}

SystemAlert MonitorMode::getAlert(const std::string& alertId) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const auto& alert : pImpl->alerts) {
        if (alert.id == alertId) {
            return alert;
        }
    }
    
    return SystemAlert(); // Return empty alert if not found
}

void MonitorMode::addAlert(const SystemAlert& alert) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->alerts.push_back(alert);
    
    // Call alert callback
    if (pImpl->alertCallback) {
        pImpl->alertCallback(alert);
    }
}

void MonitorMode::resolveAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (auto& alert : pImpl->alerts) {
        if (alert.id == alertId) {
            alert.resolved = true;
            break;
        }
    }
}

void MonitorMode::clearAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->alerts.clear();
}

SystemStatistics MonitorMode::getSystemStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->systemStatistics;
}

double MonitorMode::getAverageCPUUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->systemStatistics.averageCPUUsage;
}

double MonitorMode::getPeakCPUUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->systemStatistics.peakCPUUsage;
}

double MonitorMode::getAverageMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->systemStatistics.averageMemoryUsage;
}

double MonitorMode::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->systemStatistics.peakMemoryUsage;
}

std::string MonitorMode::generateSystemReport() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "=== NLM System Monitoring Report ===" << std::endl;
    oss << "Total Samples: " << pImpl->systemStatistics.totalSamples << std::endl;
    oss << "Sample Rate: " << pImpl->systemStatistics.sampleRatePerMinute << " samples/minute" << std::endl;
    oss << "Average CPU Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.averageCPUUsage << "%" << std::endl;
    oss << "Peak CPU Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.peakCPUUsage << "%" << std::endl;
    oss << "Average Memory Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.averageMemoryUsage << "%" << std::endl;
    oss << "Peak Memory Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.peakMemoryUsage << "%" << std::endl;
    oss << "Average Disk Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.averageDiskUsage << "%" << std::endl;
    oss << "Peak Disk Usage: " << std::fixed << std::setprecision(1) << pImpl->systemStatistics.peakDiskUsage << "%" << std::endl;
    oss << std::endl;
    
    return oss.str();
}

std::string MonitorMode::generatePerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "=== NLM Performance Monitoring Report ===" << std::endl;
    oss << "Total Samples: " << pImpl->performanceStatistics.totalSamples << std::endl;
    oss << "Sample Rate: " << pImpl->performanceStatistics.sampleRatePerMinute << " samples/minute" << std::endl;
    oss << "Average Processing Speed: " << pImpl->performanceStatistics.averageProcessingSpeed << " ops/sec" << std::endl;
    oss << "Average Throughput: " << pImpl->performanceStatistics.averageThroughput << " items/sec" << std::endl;
    oss << "Average Response Time: " << std::fixed << std::setprecision(3) << pImpl->performanceStatistics.averageResponseTime << " sec" << std::endl;
    oss << "Average Error Rate: " << std::fixed << std::setprecision(3) << pImpl->performanceStatistics.averageErrorRate << "%" << std::endl;
    oss << std::endl;
    
    return oss.str();
}

std::string MonitorMode::generateAlertReport() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::ostringstream oss;
    oss << "=== NLM Alert Report ===" << std::endl;
    oss << "Total Alerts: " << pImpl->alerts.size() << std::endl;
    
    // Group alerts by severity
    std::map<std::string, uint64_t> severityCount;
    for (const auto& alert : pImpl->alerts) {
        severityCount[alert.severity]++;
    }
    
    for (const auto& pair : severityCount) {
        oss << pair.first << " Alerts: " << pair.second << std::endl;
    }
    
    oss << std::endl;
    
    // Show most frequent alert
    if (!pImpl->alerts.empty()) {
        std::map<std::string, uint64_t> alertFrequency;
        for (const auto& alert : pImpl->alerts) {
            alertFrequency[alert.message]++;
        }
        
        std::string mostFrequent;
        uint64_t maxCount = 0;
        for (const auto& pair : alertFrequency) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                mostFrequent = pair.first;
            }
        }
        
        if (!mostFrequent.empty()) {
            oss << "Most Frequent Alert: " << mostFrequent << " (" << maxCount << " occurrences)" << std::endl;
        }
    }
    
    return oss.str();
}

std::string MonitorMode::generateHealthReport() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "=== NLM System Health Report ===" << std::endl;
    oss << "Overall Health Status: " << getHealthStatus() << std::endl;
    oss << "CPU Health: " << getCPUHealth() << std::endl;
    oss << "Memory Health: " << getMemoryHealth() << std::endl;
    oss << "Disk Health: " << getDiskHealth() << std::endl;
    oss << "Network Health: " << getNetworkHealth() << std::endl;
    oss << "Process Health: " << getProcessHealth() << std::endl;
    oss << std::endl;
    
    // Add warnings for any high usage
    if (pImpl->systemStatistics.peakCPUUsage > 80.0) {
        oss << "WARNING: High CPU usage detected (peak: " << pImpl->systemStatistics.peakCPUUsage << "%)" << std::endl;
    }
    
    if (pImpl->systemStatistics.peakMemoryUsage > 90.0) {
        oss << "WARNING: High memory usage detected (peak: " << pImpl->systemStatistics.peakMemoryUsage << "%)" << std::endl;
    }
    
    if (pImpl->systemStatistics.peakDiskUsage > 85.0) {
        oss << "WARNING: High disk usage detected (peak: " << pImpl->systemStatistics.peakDiskUsage << "%)" << std::endl;
    }
    
    return oss.str();
}

std::string MonitorMode::getHealthStatus() const {
    // Determine overall health based on metrics
    auto latestMetrics = getLatestSystemMetrics();
    
    if (latestMetrics.cpuUsage > 90.0 || latestMetrics.memoryUsage > 95.0 || latestMetrics.diskUsage > 90.0) {
        return "CRITICAL";
    } else if (latestMetrics.cpuUsage > 80.0 || latestMetrics.memoryUsage > 85.0 || latestMetrics.diskUsage > 85.0) {
        return "WARNING";
    } else {
        return "HEALTHY";
    }
}

std::string MonitorMode::getCPUHealth() const {
    double cpuUsage = getAverageCPUUsage();
    if (cpuUsage < 50.0) {
        return "EXCELLENT";
    } else if (cpuUsage < 80.0) {
        return "GOOD";
    } else if (cpuUsage < 90.0) {
        return "WARNING";
    } else {
        return "CRITICAL";
    }
}

std::string MonitorMode::getMemoryHealth() const {
    double memUsage = getAverageMemoryUsage();
    if (memUsage < 70.0) {
        return "EXCELLENT";
    } else if (memUsage < 85.0) {
        return "GOOD";
    } else if (memUsage < 95.0) {
        return "WARNING";
    } else {
        return "CRITICAL";
    }
}

std::string MonitorMode::getDiskHealth() const {
    double diskUsage = getAverageDiskUsage();
    if (diskUsage < 70.0) {
        return "EXCELLENT";
    } else if (diskUsage < 85.0) {
        return "GOOD";
    } else if (diskUsage < 95.0) {
        return "WARNING";
    } else {
        return "CRITICAL";
    }
}

std::string MonitorMode::getNetworkHealth() const {
    double netUsage = getAverageNetworkUsage();
    if (netUsage < 50.0) {
        return "EXCELLENT";
    } else if (netUsage < 80.0) {
        return "GOOD";
    } else if (netUsage < 95.0) {
        return "WARNING";
    } else {
        return "CRITICAL";
    }
}

std::string MonitorMode::getProcessHealth() const {
    auto latestMetrics = getLatestSystemMetrics();
    uint32_t processCount = latestMetrics.activeProcesses;
    
    if (processCount < 100) {
        return "EXCELLENT";
    } else if (processCount < 500) {
        return "GOOD";
    } else if (processCount < 1000) {
        return "WARNING";
    } else {
        return "CRITICAL";
    }
}

double MonitorMode::getAverageNetworkUsage() const {
    // Calculate average network usage
    double totalUpload = 0.0;
    double totalDownload = 0.0;
    uint64_t count = 0;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    for (const auto& metrics : pImpl->systemMetricsHistory) {
        totalUpload += metrics.networkUpload;
        totalDownload += metrics.networkDownload;
        count++;
    }
    
    if (count == 0) return 0.0;
    
    double avgUpload = totalUpload / count;
    double avgDownload = totalDownload / count;
    return avgUpload + avgDownload; // Total average
}

bool MonitorMode::exportReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateSystemReport() << std::endl;
    file << generatePerformanceReport() << std::endl;
    file << generateAlertReport() << std::endl;
    file << generateHealthReport() << std::endl;
    
    file.close();
    
    return true;
}

bool MonitorMode::exportDetailedReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate detailed report with all metrics and alerts
    file << generateSystemReport() << std::endl;
    file << generatePerformanceReport() << std::endl;
    file << generateAlertReport() << std::endl;
    file << generateHealthReport() << std::endl;
    
    // Add detailed metrics data
    file << "\n=== Detailed Metrics Data ===" << std::endl;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    file << "System Metrics (" << pImpl->systemMetricsHistory.size() << " samples):" << std::endl;
    for (size_t i = 0; i < std::min(pImpl->systemMetricsHistory.size(), static_cast<size_t>(100)); ++i) {
        const auto& metrics = pImpl->systemMetricsHistory[i];
        file << "  Sample " << i << ": " << getTimeString(metrics.timestamp) << std::endl;
        file << "    CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%" << std::endl;
        file << "    Memory: " << std::fixed << std::setprecision(1) << metrics.memoryUsage << "%" << std::endl;
        file << "    Disk: " << std::fixed << std::setprecision(1) << metrics.diskUsage << "%" << std::endl;
        file << "    Network: " << std::fixed << std::setprecision(1) << (metrics.networkUpload + metrics.networkDownload) << " MB/s" << std::endl;
        file << "    Processes: " << metrics.activeProcesses << std::endl;
    }
    
    file << "\nPerformance Metrics (" << pImpl->performanceMetricsHistory.size() << " samples):" << std::endl;
    for (size_t i = 0; i < std::min(pImpl->performanceMetricsHistory.size(), static_cast<size_t>(100)); ++i) {
        const auto& metrics = pImpl->performanceMetricsHistory[i];
        file << "  Sample " << i << ": " << getTimeString(metrics.timestamp) << std::endl;
        file << "    Processing Speed: " << metrics.processingSpeed << " ops/sec" << std::endl;
        file << "    Throughput: " << metrics.throughput << " items/sec" << std::endl;
        file << "    Response Time: " << std::fixed << std::setprecision(3) << metrics.responseTime << " sec" << std::endl;
        file << "    Error Rate: " << std::fixed << std::setprecision(3) << metrics.errorRate << "%" << std::endl;
    }
    
    file.close();
    
    return true;
}

void MonitorMode::checkAndGenerateAlerts(const SystemMetrics& metrics) {
    pImpl->checkAndGenerateAlerts(metrics);
}

bool MonitorMode::shouldGenerateAlert(const std::string& category, double value) const {
    return pImpl->shouldGenerateAlert(category, value);
}

void MonitorMode::setMonitoringInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.monitoringInterval = interval;
}

void MonitorMode::setSamplingInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.samplingInterval = interval;
}

void MonitorMode::setThreshold(const std::string& category, double threshold) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.thresholdOverrides[category] = threshold;
}

void MonitorMode::enableCategory(const std::string& category, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    if (category == "CPU") pImpl->config.enableCPUMonitoring = enabled;
    else if (category == "Memory") pImpl->config.enableMemoryMonitoring = enabled;
    else if (category == "Disk") pImpl->config.enableDiskMonitoring = enabled;
    else if (category == "Network") pImpl->config.enableNetworkMonitoring = enabled;
    else if (category == "Processes") pImpl->config.enableProcessMonitoring = enabled;
    else if (category == "System") pImpl->config.enableSystemMonitoring = enabled;
}

void MonitorMode::addMonitoredProcess(const std::string& processName) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->config.monitoredProcesses.push_back(processName);
}

void MonitorMode::setMetricFilter(const std::function<bool(const SystemMetrics&)>& filter) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->metricFilter = filter;
}

void MonitorMode::setMetricCallback(const std::function<void(const SystemMetrics&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->metricCallback = callback;
}

void MonitorMode::setAlertFilter(const std::function<bool(const SystemAlert&)>& filter) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->alertFilter = filter;
}

void MonitorMode::setAlertCallback(const std::function<void(const SystemAlert&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->alertCallback = callback;
}

void MonitorMode::setDataCallback(const std::function<void(const SystemMetrics&, const PerformanceMetrics&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->dataCallback = callback;
}

void MonitorMode::setAlertCallback(const std::function<void(const SystemAlert&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->alertCallback = callback;
}

void MonitorMode::setErrorCallback(const std::function<void(const std::string&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->errorCallback = callback;
}

MonitorMode& MonitorMode::getInstance() {
    static MonitorMode instance;
    return instance;
}

void MonitorMode::resetInstance() {
    // This is primarily for testing
}

std::string MonitorMode::getTimeString(const std::chrono::system_clock::time_point& time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

} // namespace advanced
} // namespace nlm

// Include necessary headers
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <numeric>
#include <algorithm>

// MonitorMode implementation helper functions
namespace nlm {
namespace advanced {

// Initialize the async collection worker thread
void MonitorMode::Impl::collectionWorker() {
    while (!stopCollection.load() && collecting.load()) {
        std::unique_lock<std::mutex> lock(dataMutex);
        
        // Collect system metrics
        SystemMetrics systemMetrics = collectSystemMetrics();
        
        // Collect performance metrics
        PerformanceMetrics performanceMetrics = collectPerformanceMetrics();
        
        // Store metrics
        systemMetricsHistory.push_back(systemMetrics);
        performanceMetricsHistory.push_back(performanceMetrics);
        
        // Update statistics
        updateStatistics(systemMetrics);
        updateStatistics(performanceMetrics);
        
        // Check for alerts
        checkAndGenerateAlerts(systemMetrics);
        
        // Call callbacks
        if (metricCallback) {
            metricCallback(systemMetrics);
        }
        
        if (dataCallback) {
            dataCallback(systemMetrics, performanceMetrics);
        }
        
        // Notify condition variable
        dataCondition.notify_all();
        
        // Wait for next collection interval
        std::chrono::duration<double> sleepDuration(config.monitoringInterval);
        std::chrono::milliseconds sleepMs(static_cast<long>(sleepDuration.count() * 1000));
        
        if (dataCondition.wait_for(lock, sleepMs, 
                                   [this]() { return stopCollection.load() || !collecting.load(); })) {
            continue;
        }
    }
}

// Monitoring helper functions (placeholders)
double MonitorMode::Impl::calculateCPUUsage() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::calculateMemoryUsage() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getMemoryAvailable() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::calculateDiskUsage() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::calculateNetworkUpload() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::calculateNetworkDownload() const {
    return 0.0; // Placeholder
}

uint32_t MonitorMode::Impl::getActiveProcessCount() const {
    return 0; // Placeholder
}

uint32_t MonitorMode::Impl::getThreadCount() const {
    return 0; // Placeholder
}

void MonitorMode::Impl::getLoadAverage(double load[3]) const {
    for (int i = 0; i < 3; ++i) load[i] = 0.0;
}

double MonitorMode::Impl::getTemperature() const {
    return 0.0; // Placeholder
}

void MonitorMode::Impl::getBatteryInfo(double& level, bool& charging) const {
    level = 0.0; charging = false;
}

double MonitorMode::Impl::getProcessingSpeed() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getThroughput() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getResponseTime() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getErrorRate() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getCPUTime() const {
    return 0.0; // Placeholder
}

double MonitorMode::Impl::getUserTime() const {
    return 0.0; // Placeholder
}

uint64_t MonitorMode::Impl::getContextSwitches() const {
    return 0; // Placeholder
}

uint64_t MonitorMode::Impl::getPageFaults() const {
    return 0; // Placeholder
}

} // namespace advanced
} // namespace nlm

// Re-export key functions
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

// MonitorMode global macros
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