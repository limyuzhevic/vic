#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <set>
#include <queue>
#include <iomanip>
#include <sstream>
#include "NLMExceptions.hpp"
#include "DebugInfrastructure.hpp"
#include "DiagnosticTools.hpp"

namespace nlm {

// Monitoring data structures
struct MonitoringData {
    std::string id;
    std::string type;           // "system", "component", "performance", "memory", "io", "network"
    std::string name;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> properties;
    std::string status;         // "normal", "warning", "error", "critical"
    double severity;            // 0.0 to 1.0
    std::string description;
    std::vector<std::string> alerts;
    bool isAlert = false;
};

// Alert information
struct Alert {
    std::string id;
    std::string type;
    std::string severity;       // "low", "medium", "high", "critical"
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::map<std::string, std::string> context;
    bool isActive = true;
    bool isAcknowledged = false;
    std::string acknowledgedBy;
    std::chrono::system_clock::time_point acknowledgedAt;
};

// Report information
struct Report {
    std::string id;
    std::string type;           // "system", "performance", "memory", "trend"
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string title;
    std::string content;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> recommendations;
    std::chrono::system_clock::time_point generatedAt;
};

// Trend data point
struct TrendDataPoint {
    std::chrono::system_clock::time_point timestamp;
    std::string metricName;
    double value;
    std::string category;
    std::string unit;
};

// RealTimeMonitor - Real-time monitoring infrastructure
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Monitoring control
    void start();
    void stop();
    void pause();
    void resume();
    
    // Check monitoring state
    bool isRunning() const;
    bool isPaused() const;
    
    // Add monitoring target
    void addMonitoringTarget(const std::string& targetId,
                            const std::string& targetType,
                            const std::map<std::string, std::string>& config);
    
    // Remove monitoring target
    void removeMonitoringTarget(const std::string& targetId);
    
    // Get monitoring data
    std::vector<MonitoringData> getMonitoringData(const std::string& targetId = "",
                                                 const std::string& dataType = "",
                                                 size_t maxEntries = 100) const;
    
    // Get active targets
    std::vector<std::string> getActiveTargets() const;
    
    // Configure monitoring
    void configureSamplingInterval(const std::string& targetId, double intervalSeconds);
    void configureThreshold(const std::string& targetId, const std::string& metric,
                           double threshold, const std::string& severity = "warning");
    
    // Get global real-time monitor instance
    static RealTimeMonitor& getGlobal();
    
    // Set global real-time monitor
    static void setGlobal(std::shared_ptr<RealTimeMonitor> monitor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// HistoricalMonitor - Historical data storage and retrieval
class HistoricalMonitor {
public:
    HistoricalMonitor();
    ~HistoricalMonitor();
    
    // Store monitoring data
    void storeMonitoringData(const MonitoringData& data);
    
    // Query historical data
    std::vector<MonitoringData> queryMonitoringData(const std::string& targetId = "",
                                                   const std::string& dataType = "",
                                                   const std::chrono::system_clock::time_point& startTime = {},
                                                   const std::chrono::system_clock::time_point& endTime = {}) const;
    
    // Get historical metrics
    std::map<std::string, double> getHistoricalMetrics(const std::string& metricName,
                                                      const std::string& targetId = "",
                                                      const std::chrono::system_clock::time_point& startTime = {},
                                                      const std::chrono::system_clock::time_point& endTime = {}) const;
    
    // Clean up old data
    void cleanupOldData(const std::chrono::system_clock::time_point& olderThan);
    
    // Get data retention period
    std::chrono::system_clock::duration getDataRetentionPeriod() const;
    
    // Set data retention period
    void setDataRetentionPeriod(const std::chrono::system_clock::duration& period);
    
    // Get global historical monitor instance
    static HistoricalMonitor& getGlobal();
    
    // Set global historical monitor
    static void setGlobal(std::shared_ptr<HistoricalMonitor> monitor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// AlertManager - Alert management and notification
class AlertManager {
public:
    AlertManager();
    ~AlertManager();
    
    // Add alert rule
    void addAlertRule(const std::string& ruleId,
                     const std::string& type,
                     const std::string& condition,
                     const std::string& severity = "warning",
                     const std::string& message = "",
                     const std::string& source = "");
    
    // Remove alert rule
    void removeAlertRule(const std::string& ruleId);
    
    // Get alert rules
    std::vector<Alert> getActiveAlerts() const;
    std::vector<Alert> getAlerts(const std::string& type = "") const;
    std::optional<Alert> getAlert(const std::string& alertId) const;
    
    // Acknowledge alert
    void acknowledgeAlert(const std::string& alertId, const std::string& acknowledgedBy);
    
    // Clear alert
    void clearAlert(const std::string& alertId);
    
    // Check monitoring data against alert rules
    void evaluateAlerts(const MonitoringData& data);
    
    // Configure notification
    void configureNotification(const std::string& channel,
                               const std::string& endpoint,
                               const std::map<std::string, std::string>& config);
    
    // Get global alert manager instance
    static AlertManager& getGlobal();
    
    // Set global alert manager
    static void setGlobal(std::shared_ptr<AlertManager> manager);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ReportGenerator - Diagnostic report generation
class ReportGenerator {
public:
    ReportGenerator();
    ~ReportGenerator();
    
    // Generate report
    Report generateSystemReport(const std::string& title = "System Report");
    Report generatePerformanceReport(const std::string& title = "Performance Report");
    Report generateMemoryReport(const std::string& title = "Memory Report");
    Report generateAlertReport(const std::string& title = "Alert Report");
    
    // Generate custom report
    Report generateCustomReport(const std::string& type,
                               const std::string& title,
                               const std::chrono::system_clock::time_point& startTime,
                               const std::chrono::system_clock::time_point& endTime);
    
    // Get report by ID
    std::optional<Report> getReport(const std::string& reportId) const;
    
    // Get recent reports
    std::vector<Report> getRecentReports(size_t maxReports = 10) const;
    
    // Export report
    bool exportReport(const Report& report, const std::string& filepath) const;
    
    // Store report
    void storeReport(const Report& report);
    
    // Get global report generator instance
    static ReportGenerator& getGlobal();
    
    // Set global report generator
    static void setGlobal(std::shared_ptr<ReportGenerator> generator);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// TrendAnalyzer - Performance trend analysis
class TrendAnalyzer {
public:
    TrendAnalyzer();
    ~TrendAnalyzer();
    
    // Add trend data point
    void addTrendData(const TrendDataPoint& dataPoint);
    
    // Analyze trends
    std::map<std::string, double> analyzeTrend(const std::string& metricName,
                                              const std::string& category = "",
                                              size_t dataPoints = 10) const;
    
    // Detect anomalies
    std::vector<TrendDataPoint> detectAnomalies(const std::string& metricName,
                                               double threshold = 2.0) const;
    
    // Get trend data
    std::vector<TrendDataPoint> getTrendData(const std::string& metricName = "",
                                            const std::string& category = "",
                                            const std::chrono::system_clock::time_point& startTime = {},
                                            const std::chrono::system_clock::time_point& endTime = {}) const;
    
    // Clear trend data
    void clearTrendData();
    
    // Set analysis parameters
    void setAnalysisWindow(size_t windowSize);
    void setAnomalyThreshold(double threshold);
    
    // Get global trend analyzer instance
    static TrendAnalyzer& getGlobal();
    
    // Set global trend analyzer
    static void setGlobal(std::shared_ptr<TrendAnalyzer> analyzer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
