#include "PerformanceDashboard.hpp"
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <cmath>

namespace nlm {

PerformanceDashboard::PerformanceDashboard(const PerformanceDashboardConfig& config) 
    : config_(config),
      currentSessionName(""),
      monitoringActive(false),
      shouldStopMonitoring(true),
      parentProfiler(nullptr) {
    
    outputDir = "./profiling_output/dashboard/";
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }
    
    if (config_.enableRealTimeDisplay) {
        startMonitoring();
    }
}

PerformanceDashboard::~PerformanceDashboard() {
    stopMonitoring();
    endSession();
}

void PerformanceDashboard::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (monitoringActive) {
        return;
    }
    
    monitoringActive = true;
    shouldStopMonitoring = false;
    
    monitoringThread = std::thread(&PerformanceDashboard::sampleMetrics, this);
    
    std::cout << "PerformanceDashboard started monitoring" << std::endl;
}

void PerformanceDashboard::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!monitoringActive) {
        return;
    }
    
    shouldStopMonitoring = true;
    monitoringActive = false;
    
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
    
    std::cout << "PerformanceDashboard stopped monitoring" << std::endl;
}

void PerformanceDashboard::sampleMetrics() {
    while (!shouldStopMonitoring) {
        updateMetrics();
        checkAlerts();
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.refreshInterval));
    }
}

void PerformanceDashboard::updateMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    // Simulate real-time metrics collection
    // In a real implementation, this would query actual system metrics
    
    // Update current metrics
    currentMetrics.timestamp = std::chrono::steady_clock::now();
    
    // Simulate metrics (in real implementation, this would read from profiler)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> cpuDist(10.0, 90.0);
    static std::uniform_real_distribution<> memDist(0.5, 2.0); // GB
    static std::uniform_real_distribution<> ioDist(100000000, 500000000); // 100MB/s - 500MB/s
    static std::uniform_real_distribution<> respDist(0.001, 0.05); // 1-50ms
    static std::uniform_real_distribution<> eventDist(100, 1000); // events per second
    
    currentMetrics.cpuUsage = cpuDist(gen);
    currentMetrics.memoryUsage = memDist(gen) * 1024 * 1024 * 1024; // Convert to bytes
    currentMetrics.memoryEfficiency = 80.0 + (std::rand() % 20); // 80-100%
    currentMetrics.iOThroughput = ioDist(gen);
    currentMetrics.responseTime = respDist(gen);
    currentMetrics.eventRate = eventDist(gen);
    currentMetrics.activeEvents = static_cast<size_t>(eventDist(gen) / 10); // ~10% active
    currentMetrics.totalEvents = currentMetrics.activeEvents + static_cast<size_t>(eventDist(gen) * 10);
    currentMetrics.systemStatus = getSystemStatus(currentMetrics.cpuUsage, 
                                                 (currentMetrics.memoryUsage / currentMetrics.memoryEfficiency));
    
    // Update metrics history
    metricsHistory.push_back(currentMetrics);
    
    // Cleanup old metrics
    if (metricsHistory.size() > 1000) {
        metricsHistory.erase(metricsHistory.begin());
    }
    
    updateStatistics();
}

void PerformanceDashboard::monitorCPUUsage() {
    // This would be called from sampleMetrics in a real implementation
}

void PerformanceDashboard::monitorMemoryUsage() {
    // This would be called from sampleMetrics in a real implementation
}

void PerformanceDashboard::monitorIOUsage() {
    // This would be called from sampleMetrics in a real implementation
}

void PerformanceDashboard::monitorEventRate() {
    // This would be called from sampleMetrics in a real implementation
}

void PerformanceDashboard::monitorSystemStatus() {
    // This would be called from sampleMetrics in a real implementation
}

void PerformanceDashboard::updateStatistics() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (metricsHistory.empty()) {
        return;
    }
    
    double sumCPU = 0.0;
    double sumMemory = 0.0;
    double sumResponseTime = 0.0;
    
    for (const auto& metric : metricsHistory) {
        sumCPU += metric.cpuUsage;
        sumMemory += metric.memoryUsage;
        sumResponseTime += metric.responseTime;
    }
    
    size_t count = metricsHistory.size();
    
    totalCPUUsage = sumCPU / count;
    totalMemoryUsage = sumMemory / count;
    totalResponseTime = sumResponseTime / count;
    totalEvents = metricsHistory.back().totalEvents;
    samplesProcessed = count;
}

void PerformanceDashboard::checkAlerts() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    // Check CPU alert
    if (currentMetrics.cpuUsage >= config_.cpuAlertThreshold) {
        PerformanceAlert alert("cpu_high_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()),
            "cpu", "warning",
            "CPU usage is high: " + std::to_string(currentMetrics.cpuUsage) + "%",
            currentMetrics.cpuUsage, config_.cpuAlertThreshold);
        alert.remediationAction = "Consider reducing workload or scaling resources";
        addAlert(alert);
    }
    
    // Check memory alert
    double memoryUsagePercent = (currentMetrics.memoryUsage / currentMetrics.memoryEfficiency) * 100.0;
    if (memoryUsagePercent >= config_.memoryAlertThreshold) {
        PerformanceAlert alert("memory_high_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()),
            "memory", "critical",
            "Memory usage is high: " + std::to_string(memoryUsagePercent) + "%",
            memoryUsagePercent, config_.memoryAlertThreshold);
        alert.remediationAction = "Free up memory or increase system memory";
        addAlert(alert);
    }
    
    // Check I/O alert
    if (currentMetrics.iOThroughput >= config_.ioAlertThreshold) {
        PerformanceAlert alert("io_high_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()),
            "io", "warning",
            "I/O throughput is high: " + std::to_string(currentMetrics.iOThroughput) + " bytes/s",
            currentMetrics.iOThroughput, config_.ioAlertThreshold);
        alert.remediationAction = "Monitor I/O bottlenecks";
        addAlert(alert);
    }
    
    // Check response time alert
    if (currentMetrics.responseTime >= config_.responseTimeAlertThreshold) {
        PerformanceAlert alert("response_slow_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()),
            "response", "warning",
            "Response time is slow: " + std::to_string(currentMetrics.responseTime) + "s",
            currentMetrics.responseTime, config_.responseTimeAlertThreshold);
        alert.remediationAction = "Optimize performance and consider caching";
        addAlert(alert);
    }
}

void PerformanceDashboard::addAlert(const PerformanceAlert& alert) {
    std::lock_guard<std::mutex> lock(mutex_);
    alerts.push_back(alert);
    std::cout << "ALERT: " << alert.message << std::endl;
}

std::vector<PerformanceAlert> PerformanceDashboard::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceAlert> activeAlerts;
    for (const auto& alert : alerts) {
        if (!alert.acknowledged) {
            activeAlerts.push_back(alert);
        }
    }
    
    return activeAlerts;
}

std::vector<PerformanceAlert> PerformanceDashboard::getAlertsByType(const std::string& alertType) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceAlert> typeAlerts;
    for (const auto& alert : alerts) {
        if (alert.alertType == alertType && !alert.acknowledged) {
            typeAlerts.push_back(alert);
        }
    }
    
    return typeAlerts;
}

void PerformanceDashboard::acknowledgeAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& alert : alerts) {
        if (alert.alertId == alertId) {
            alert.acknowledged = true;
            break;
        }
    }
}

void PerformanceDashboard::clearAlerts() {
    std::lock_guard<std::mutex> lock(mutex_);
    alerts.clear();
}

void PerformanceDashboard::displayDashboard() const {
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "PERFORMANCE DASHBOARD - " << currentSessionName << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (!metricsHistory.empty()) {
        const auto& latestMetrics = metricsHistory.back();
        
        std::cout << "System Status: " << latestMetrics.systemStatus << std::endl;
        std::cout << "CPU Usage: " << std::fixed << std::setprecision(2) << latestMetrics.cpuUsage << "%" << std::endl;
        std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << latestMetrics.memoryUsage / (1024 * 1024) << " MB ("
                  << latestMetrics.memoryEfficiency << "% efficient)" << std::endl;
        std::cout << "I/O Throughput: " << std::fixed << std::setprecision(2) << latestMetrics.iOThroughput / (1024 * 1024) << " MB/s" << std::endl;
        std::cout << "Response Time: " << std::fixed << std::setprecision(6) << latestMetrics.responseTime << "s" << std::endl;
        std::cout << "Event Rate: " << std::fixed << std::setprecision(2) << latestMetrics.eventRate << " events/s" << std::endl;
        std::cout << "Active Events: " << latestMetrics.activeEvents << std::endl;
        std::cout << std::endl;
    }
    
    auto activeAlerts = getActiveAlerts();
    if (!activeAlerts.empty()) {
        std::cout << "Active Alerts (" << activeAlerts.size() << "):" << std::endl;
        for (const auto& alert : activeAlerts) {
            std::cout << "  " << alert.severity << ": " << alert.message << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cout << "No active alerts." << std::endl;
        std::cout << std::endl;
    }
}

bool PerformanceDashboard::generateHTMLReport(const std::string& filename) const {
    std::string outputFile = filename.empty() ? getDefaultFilename("dashboard.html") : filename;
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for HTML report" << std::endl;
        return false;
    }
    
    file << "<!DOCTYPE html>\n";
    file << "<html>\n";
    file << "<head>\n";
    file << "  <title>Performance Dashboard - " << currentSessionName << "</title>\n";
    file << "  <style>\n";
    file << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << "    .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }\n";
    file << "    .metric { margin: 10px 0; padding: 10px; background-color: #f9f9f9; border-radius: 3px; }\n";
    file << "    .alert { border-left: 4px solid #ff9800; padding: 10px; margin: 10px 0; background-color: #fff3e0; }\n";
    file << "    .critical { border-left: 4px solid #f44336; }\n";
    file << "    .warning { border-left: 4px solid #ff9800; }\n";
    file << "    .good { border-left: 4px solid #4caf50; }\n";
    file << "    table { border-collapse: collapse; width: 100%; }\n";
    file << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "    th { background-color: #f2f2f2; }\n";
    file << "  </style>\n";
    file << "</head>\n";
    file << "<body>\n";
    
    generateDashboardHTML(file);
    
    file << "</body>\n";
    file << "</html>\n";
    
    file.close();
    return true;
}

void PerformanceDashboard::generateDashboardHTML(std::ofstream& file) const {
    if (!metricsHistory.empty()) {
        const auto& latestMetrics = metricsHistory.back();
        
        file << "<div class=\"header\">\n";
        file << "  <h1>Performance Dashboard - " << currentSessionName << "</h1>\n";
        file << "  <p>Last updated: " << std::chrono::system_clock::now() << "</p>\n";
        file << "</div>\n\n";
        
        // System overview
        file << "<div class=\"metric\">\n";
        file << "  <h2>System Status</h2>\n";
        file << "  <p>Status: <strong>" << latestMetrics.systemStatus << "</strong></p>\n";
        file << "</div>\n\n";
        
        // Performance metrics
        file << "<div class=\"metric\">\n";
        file << "  <h2>Performance Metrics</h2>\n";
        file << "  <table>\n";
        file << "    <tr><th>Metric</th><th>Value</th><th>Status</th></tr>\n";
        file << "    <tr><td>CPU Usage</td><td>" << std::fixed << std::setprecision(2) << latestMetrics.cpuUsage << "%</td>";
        
        if (latestMetrics.cpuUsage < 70.0) file << "<td class=\"good\">Good</td>\n";
        else if (latestMetrics.cpuUsage < 85.0) file << "<td class=\"warning\">Warning</td>\n";
        else file << "<td class=\"critical\">Critical</td>\n";
        
        file << "    </tr>\n";
        
        double memoryUsagePercent = (latestMetrics.memoryUsage / latestMetrics.memoryEfficiency) * 100.0;
        file << "    <tr><td>Memory Usage</td><td>" << std::fixed << std::setprecision(2) << memoryUsagePercent << "%</td>";
        
        if (memoryUsagePercent < 80.0) file << "<td class=\"good\">Good</td>\n";
        else if (memoryUsagePercent < 90.0) file << "<td class=\"warning\">Warning</td>\n";
        else file << "<td class=\"critical\">Critical</td>\n";
        
        file << "    </tr>\n";
        
        file << "    <tr><td>I/O Throughput</td><td>" << std::fixed << std::setprecision(2) << latestMetrics.iOThroughput / (1024 * 1024) << " MB/s</td>";
        
        if (latestMetrics.iOThroughput < 500000000) file << "<td class=\"good\">Good</td>\n";
        else if (latestMetrics.iOThroughput < 1000000000) file << "<td class=\"warning\">Warning</td>\n";
        else file << "<td class=\"critical\">Critical</td>\n";
        
        file << "    </tr>\n";
        
        file << "    <tr><td>Response Time</td><td>" << std::fixed << std::setprecision(6) << latestMetrics.responseTime << "s</td>";
        
        if (latestMetrics.responseTime < 0.01) file << "<td class=\"good\">Good</td>\n";
        else if (latestMetrics.responseTime < 0.05) file << "<td class=\"warning\">Warning</td>\n";
        else file << "<td class=\"critical\">Critical</td>\n";
        
        file << "    </tr>\n";
        
        file << "  </table>\n";
        file << "</div>\n\n";
    }
}

bool PerformanceDashboard::generateJSONReport(const std::string& filename) const {
    std::string outputFile = filename.empty() ? getDefaultFilename("dashboard.json") : filename;
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for JSON report" << std::endl;
        return false;
    }
    
    // Generate JSON representation
    file << "{\n";
    file << "  \"session_name\": \"" << currentSessionName << "\",\n";
    file << "  \"timestamp\": \"" << std::chrono::system_clock::now() << "\",\n";
    file << "  \"system_status\": \"" << (metricsHistory.empty() ? "unknown" : metricsHistory.back().systemStatus) << "\",\n";
    
    if (!metricsHistory.empty()) {
        const auto& latest = metricsHistory.back();
        file << "  \"metrics\": {\n";
        file << "    \"cpu_usage\": " << std::fixed << std::setprecision(2) << latest.cpuUsage << ",\n";
        file << "    \"memory_usage\": " << std::fixed << std::setprecision(2) << latest.memoryUsage << ",\n";
        file << "    \"memory_efficiency\": " << std::fixed << std::setprecision(2) << latest.memoryEfficiency << ",\n";
        file << "    \"io_throughput\": " << std::fixed << std::setprecision(2) << latest.iOThroughput << ",\n";
        file << "    \"response_time\": " << std::fixed << std::setprecision(6) << latest.responseTime << ",\n";
        file << "    \"event_rate\": " << std::fixed << std::setprecision(2) << latest.eventRate << ",\n";
        file << "    \"active_events\": " << latest.activeEvents << ",\n";
        file << "    \"total_events\": " << latest.totalEvents << "\n";
        file << "  },\n";
    }
    
    // Add alerts
    file << "  \"alerts\": [\n";
    
    auto activeAlerts = getActiveAlerts();;
    for (size_t i = 0; i < activeAlerts.size(); ++i) {
        const auto& alert = activeAlerts[i];
        file << "    {\"id\": \"" << alert.alertId << "\", \"type\": \"" << alert.alertType
             << "\", \"severity\": \"" << alert.severity << "\", \"message\": \"" << alert.message
             << "\", \"current_value\": " << std::fixed << std::setprecision(2) << alert.currentValue
             << ", \"threshold\": " << std::fixed << std::setprecision(2) << alert.thresholdValue
             << ", \"acknowledged\": " << (alert.acknowledged ? "true" : "false") << "\"";
        
        if (i < activeAlerts.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool PerformanceDashboard::generateTextReport(const std::string& filename) const {
    std::string outputFile = filename.empty() ? getDefaultFilename("dashboard.txt") : filename;
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for text report" << std::endl;
        return false;
    }
    
    displayDashboard();
    
    // Add more detailed text report
    file << std::string(80, '=') << std::endl;
    file << "DETAILED STATISTICS:" << std::endl;
    file << "  Total Samples: " << samplesProcessed << std::endl;
    file << "  Average CPU Usage: " << std::fixed << std::setprecision(2) << totalCPUUsage << "%" << std::endl;
    file << "  Average Memory Usage: " << std::fixed << std::setprecision(2) << totalMemoryUsage / (1024 * 1024) << " MB" << std::endl;
    file << "  Average Response Time: " << std::fixed << std::setprecision(6) << totalResponseTime << "s" << std::endl;
    
    file.close();
    return true;
}

std::vector<RealTimeMetrics> PerformanceDashboard::getMetricsHistory(size_t count) const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    std::vector<RealTimeMetrics> history(metricsHistory.rbegin(), metricsHistory.rend());
    
    if (history.size() > count) {
        history.erase(history.begin() + count, history.end());
    }
    
    return history;
}

double PerformanceDashboard::calculateAverageCPUUsage() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (metricsHistory.empty()) {
        return 0.0;
    }
    
    double totalCPU = 0.0;
    for (const auto& metric : metricsHistory) {
        totalCPU += metric.cpuUsage;
    }
    
    return totalCPU / metricsHistory.size();
}

double PerformanceDashboard::calculateAverageMemoryUsage() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (metricsHistory.empty()) {
        return 0.0;
    }
    
    double totalMemory = 0.0;
    for (const auto& metric : metricsHistory) {
        totalMemory += metric.memoryUsage;
    }
    
    return totalMemory / metricsHistory.size();
}

double PerformanceDashboard::calculateAverageResponseTime() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (metricsHistory.empty()) {
        return 0.0;
    }
    
    double totalResponseTime = 0.0;
    for (const auto& metric : metricsHistory) {
        totalResponseTime += metric.responseTime;
    }
    
    return totalResponseTime / metricsHistory.size();
}

void PerformanceDashboard::startSession(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    endSession(); // End previous session if any
    
    currentSessionName = sessionName;
    sessionStartTime = std::chrono::steady_clock::now();
    
    // Reset statistics
    totalCPUUsage = 0.0;
    totalMemoryUsage = 0.0;
    totalResponseTime = 0.0;
    totalEvents = 0;
    samplesProcessed = 0;
    metricsHistory.clear();
    alerts.clear();
    
    std::cout << "PerformanceDashboard session started: " << sessionName << std::endl;
}

void PerformanceDashboard::endSession() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!currentSessionName.empty()) {
        // Stop monitoring
        stopMonitoring();
        
        // Generate reports
        if (config_.enableRealTimeDisplay) {
            generateTextReport();
        }
        
        std::cout << "PerformanceDashboard session ended: " << currentSessionName << std::endl;
        std::cout << "Total samples: " << samplesProcessed << std::endl;
        
        currentSessionName.clear();
    }
}

std::string PerformanceDashboard::getSystemStatus(double cpuUsage, double memoryUsage) const {
    if (cpuUsage < 70.0 && memoryUsage < 80.0) {
        return "normal";
    } else if (cpuUsage < 85.0 && memoryUsage < 90.0) {
        return "warning";
    } else {
        return "critical";
    }
}

bool PerformanceDashboard::validateConfiguration() const {
    if (config_.refreshInterval <= 0) return false;
    if (config_.cpuAlertThreshold < 0 || config_.cpuAlertThreshold > 100) return false;
    if (config_.memoryAlertThreshold < 0 || config_.memoryAlertThreshold > 100) return false;
    if (config_.ioAlertThreshold < 0) return false;
    if (config_.responseTimeAlertThreshold < 0) return false;
    
    return true;
}

std::string PerformanceDashboard::getDefaultFilename(const std::string& extension) const {
    if (currentSessionName.empty()) {
        return "dashboard." + extension;
    }
    
    return currentSessionName + "." + extension;
}

} // namespace nlm
