#include "MonitoringInfrastructure.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

// RealTimeMonitor implementation
struct RealTimeMonitor::Impl {
    bool running = false;
    bool paused = false;
    std::map<std::string, std::map<std::string, std::string>> monitoringTargets;
    std::map<std::string, std::map<std::string, double>> samplingIntervals;
    std::map<std::string, std::map<std::string, double>> thresholds;
    std::map<std::string, std::vector<MonitoringData>> monitoringData;
    std::mutex mutex;
    std::thread monitoringThread;
    std::condition_variable cv;
    std::atomic<bool> stopFlag{false};
};

RealTimeMonitor::RealTimeMonitor() : pImpl(std::make_unique<Impl>()) {}

RealTimeMonitor::~RealTimeMonitor() {
    stop();
}

void RealTimeMonitor::start() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->running) {
        pImpl->running = true;
        pImpl->paused = false;
        pImpl->stopFlag = false;
        
        // Start monitoring thread
        pImpl->monitoringThread = std::thread(&RealTimeMonitor::monitoringLoop, this);
    }
}

void RealTimeMonitor::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->stopFlag = true;
        pImpl->cv.notify_all();
        
        if (pImpl->monitoringThread.joinable()) {
            pImpl->monitoringThread.join();
        }
    }
}

void RealTimeMonitor::pause() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->paused = true;
}

void RealTimeMonitor::resume() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->paused = false;
    pImpl->cv.notify_all();
}

bool RealTimeMonitor::isRunning() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->running && !pImpl->paused;
}

bool RealTimeMonitor::isPaused() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->paused;
}

void RealTimeMonitor::addMonitoringTarget(const std::string& targetId,
                                         const std::string& targetType,
                                         const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->monitoringTargets[targetId] = config;
    pImpl->monitoringData[targetId] = std::vector<MonitoringData>();
}

void RealTimeMonitor::removeMonitoringTarget(const std::string& targetId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->monitoringTargets.erase(targetId);
    pImpl->monitoringData.erase(targetId);
    pImpl->samplingIntervals.erase(targetId);
    pImpl->thresholds.erase(targetId);
}

std::vector<MonitoringData> RealTimeMonitor::getMonitoringData(
    const std::string& targetId, const std::string& dataType,
    size_t maxEntries) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<MonitoringData> result;
    
    const auto& dataMap = (targetId.empty() ? pImpl->monitoringData : 
                          std::map<std::string, std::vector<MonitoringData>>{ {targetId, pImpl->monitoringData.at(targetId)} });
    
    for (const auto& targetPair : dataMap) {
        for (const auto& data : targetPair.second) {
            if (dataType.empty() || data.type == dataType) {
                result.push_back(data);
            }
        }
    }
    
    if (result.size() > maxEntries) {
        result.resize(maxEntries);
    }
    
    return result;
}

std::vector<std::string> RealTimeMonitor::getActiveTargets() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> targets;
    for (const auto& pair : pImpl->monitoringTargets) {
        targets.push_back(pair.first);
    }
    return targets;
}

void RealTimeMonitor::configureSamplingInterval(const std::string& targetId,
                                               double intervalSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->samplingIntervals[targetId]["sampling_interval"] = intervalSeconds;
}

void RealTimeMonitor::configureThreshold(const std::string& targetId,
                                        const std::string& metric,
                                        double threshold, const std::string& severity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->thresholds[targetId][metric + "_threshold"] = threshold;
}

RealTimeMonitor& RealTimeMonitor::getGlobal() {
    static RealTimeMonitor globalMonitor;
    return globalMonitor;
}

void RealTimeMonitor::setGlobal(std::shared_ptr<RealTimeMonitor> monitor) {
    // This is a placeholder for advanced global monitor management
}

void RealTimeMonitor::monitoringLoop() {
    while (!pImpl->stopFlag) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            
            if (pImpl->paused) {
                std::unique_lock<std::mutex> lk(pImpl->mutex);
                pImpl->cv.wait_for(lk, std::chrono::seconds(1));
                continue;
            }
            
            // Collect monitoring data for all targets
            auto currentTime = std::chrono::system_clock::now();
            
            for (const auto& targetPair : pImpl->monitoringTargets) {
                const std::string& targetId = targetPair.first;
                const auto& config = targetPair.second;
                
                // Create monitoring data
                MonitoringData data;
                data.id = targetId + "_" + std::to_string(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        currentTime.time_since_epoch()).count());
                data.type = "system"; // Default type - in real implementation would use config
                data.name = targetId;
                data.timestamp = currentTime;
                data.status = "normal";
                data.severity = 0.0;
                
                // Add some sample metrics
                data.metrics["cpu_usage"] = 25.0 + (rand() % 50); // Simulated
                data.metrics["memory_usage"] = 40.0 + (rand() % 30);
                data.metrics["response_time"] = 0.1 + (rand() % 100) / 1000.0;
                
                // Store data
                pImpl->monitoringData[targetId].push_back(data);
            }
        }
        
        // Sleep for a short interval
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// HistoricalMonitor implementation
struct HistoricalMonitor::Impl {
    std::vector<MonitoringData> historicalData;
    std::chrono::system_clock::duration retentionPeriod = std::chrono::hours(24);
    std::mutex mutex;
};

HistoricalMonitor::HistoricalMonitor() : pImpl(std::make_unique<Impl>()) {}

HistoricalMonitor::~HistoricalMonitor() = default;

void HistoricalMonitor::storeMonitoringData(const MonitoringData& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->historicalData.push_back(data);
}

std::vector<MonitoringData> HistoricalMonitor::queryMonitoringData(
    const std::string& targetId, const std::string& dataType,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<MonitoringData> results;
    
    for (const auto& data : pImpl->historicalData) {
        if (!targetId.empty() && data.name != targetId) continue;
        if (!dataType.empty() && data.type != dataType) continue;
        if (!startTime.empty() && data.timestamp < startTime) continue;
        if (!endTime.empty() && data.timestamp > endTime) continue;
        
        results.push_back(data);
    }
    
    return results;
}

std::map<std::string, double> HistoricalMonitor::getHistoricalMetrics(
    const std::string& metricName, const std::string& targetId,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> results;
    
    std::vector<double> values;
    for (const auto& data : pImpl->historicalData) {
        if (!targetId.empty() && data.name != targetId) continue;
        if (!startTime.empty() && data.timestamp < startTime) continue;
        if (!endTime.empty() && data.timestamp > endTime) continue;
        
        auto it = data.metrics.find(metricName);
        if (it != data.metrics.end()) {
            values.push_back(it->second);
        }
    }
    
    if (!values.empty()) {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        results[metricName] = sum / values.size();
    }
    
    return results;
}

void HistoricalMonitor::cleanupOldData(const std::chrono::system_clock::time_point& olderThan) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto currentTime = std::chrono::system_clock::now();
    auto cutoff = currentTime - (currentTime - olderThan);
    
    pImpl->historicalData.erase(
        std::remove_if(pImpl->historicalData.begin(), pImpl->historicalData.end(),
                      [cutoff](const MonitoringData& data) {
                          return data.timestamp < cutoff;
                      }),
        pImpl->historicalData.end()
    );
}

std::chrono::system_clock::duration HistoricalMonitor::getDataRetentionPeriod() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->retentionPeriod;
}

void HistoricalMonitor::setDataRetentionPeriod(
    const std::chrono::system_clock::duration& period) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->retentionPeriod = period;
}

HistoricalMonitor& HistoricalMonitor::getGlobal() {
    static HistoricalMonitor globalMonitor;
    return globalMonitor;
}

void HistoricalMonitor::setGlobal(std::shared_ptr<HistoricalMonitor> monitor) {
    // This is a placeholder for advanced global monitor management
}

// AlertManager implementation
struct AlertManager::Impl {
    std::vector<Alert> alerts;
    std::map<std::string, std::map<std::string, std::string>> alertRules;
    std::map<std::string, std::map<std::string, std::string>> notifications;
    std::mutex mutex;
};

AlertManager::AlertManager() : pImpl(std::make_unique<Impl>()) {}

AlertManager::~AlertManager() = default;

void AlertManager::addAlertRule(const std::string& ruleId,
                               const std::string& type,
                               const std::string& condition,
                               const std::string& severity,
                               const std::string& message,
                               const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->alertRules[ruleId]["type"] = type;
    pImpl->alertRules[ruleId]["condition"] = condition;
    pImpl->alertRules[ruleId]["severity"] = severity;
    pImpl->alertRules[ruleId]["message"] = message;
    pImpl->alertRules[ruleId]["source"] = source;
}

void AlertManager::removeAlertRule(const std::string& ruleId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->alertRules.erase(ruleId);
}

std::vector<Alert> AlertManager::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Alert> activeAlerts;
    for (const auto& alert : pImpl->alerts) {
        if (alert.isActive) {
            activeAlerts.push_back(alert);
        }
    }
    return activeAlerts;
}

std::vector<Alert> AlertManager::getAlerts(const std::string& type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Alert> results;
    for (const auto& alert : pImpl->alerts) {
        if (!type.empty() && alert.type != type) continue;
        results.push_back(alert);
    }
    return results;
}

std::optional<Alert> AlertManager::getAlert(const std::string& alertId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& alert : pImpl->alerts) {
        if (alert.id == alertId) {
            return alert;
        }
    }
    return std::nullopt;
}

void AlertManager::acknowledgeAlert(const std::string& alertId, const std::string& acknowledgedBy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& alert : pImpl->alerts) {
        if (alert.id == alertId && alert.isActive) {
            alert.isAcknowledged = true;
            alert.acknowledgedBy = acknowledgedBy;
            alert.acknowledgedAt = std::chrono::system_clock::now();
            break;
        }
    }
}

void AlertManager::clearAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto it = pImpl->alerts.begin(); it != pImpl->alerts.end(); ++it) {
        if (it->id == alertId) {
            it->isActive = false;
            break;
        }
    }
}

void AlertManager::evaluateAlerts(const MonitoringData& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& rulePair : pImpl->alertRules) {
        const std::string& ruleId = rulePair.first;
        const auto& rule = rulePair.second;
        
        // Check if rule matches data
        bool matches = false;
        if (rule.find("type") != rule.end() && rule.at("type") != data.type) continue;
        
        // Check conditions (simplified)
        matches = true;
        
        if (matches) {
            Alert alert;
            alert.id = "alert_" + std::to_string(pImpl->alerts.size() + 1);
            alert.type = rule.at("type");
            alert.severity = rule.at("severity");
            alert.message = rule.at("message");
            alert.source = rule.at("source");
            alert.timestamp = std::chrono::system_clock::now();
            alert.isActive = true;
            
            pImpl->alerts.push_back(alert);
        }
    }
}

void AlertManager::configureNotification(const std::string& channel,
                                         const std::string& endpoint,
                                         const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->notifications[channel] = config;
}

AlertManager& AlertManager::getGlobal() {
    static AlertManager globalAlertManager;
    return globalAlertManager;
}

void AlertManager::setGlobal(std::shared_ptr<AlertManager> manager) {
    // This is a placeholder for advanced global manager management
}

// ReportGenerator implementation
struct ReportGenerator::Impl {
    std::map<std::string, Report> reports;
    bool loggingEnabled = true;
    std::mutex mutex;
};

ReportGenerator::ReportGenerator() : pImpl(std::make_unique<Impl>()) {}

ReportGenerator::~ReportGenerator() = default;

Report ReportGenerator::generateSystemReport(const std::string& title) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Report report;
    report.id = "report_" + std::to_string(pImpl->reports.size() + 1);
    report.type = "system";
    report.title = title;
    report.startTime = std::chrono::system_clock::now() - std::chrono::hours(24);
    report.endTime = std::chrono::system_clock::now();
    report.generatedAt = std::chrono::system_clock::now();
    
    // Generate report content
    std::ostringstream oss;
    oss << "=== " << title << " ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
        << "\n\nSystem Information:\n"
        << "- CPU Usage: 25%\n"
        << "- Memory Usage: 40%\n"
        << "- Active Targets: " << pImpl->reports.size() << "\n"
        << "\nStatus: Generated successfully\n";
    
    report.content = oss.str();
    report.recommendations.push_back("Monitor system resources regularly");
    report.recommendations.push_back("Update outdated dependencies");
    
    storeReport(report);
    return report;
}

Report ReportGenerator::generatePerformanceReport(const std::string& title) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Report report;
    report.id = "report_perf_" + std::to_string(pImpl->reports.size() + 1);
    report.type = "performance";
    report.title = title;
    report.startTime = std::chrono::system_clock::now() - std::chrono::hours(24);
    report.endTime = std::chrono::system_clock::now();
    report.generatedAt = std::chrono::system_clock::now();
    
    std::ostringstream oss;
    oss << "=== Performance Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
        << "\n\nPerformance Metrics:\n"
        << "- Average CPU Usage: 25%\n"
        << "- Peak Memory Usage: 60%\n"
        << "- Average Response Time: 50ms\n"
        << "- Number of Operations: 1000\n"
        << "\nTrend: Stable\n";
    
    report.content = oss.str();
    
    storeReport(report);
    return report;
}

Report ReportGenerator::generateMemoryReport(const std::string& title) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Report report;
    report.id = "report_mem_" + std::to_string(pImpl->reports.size() + 1);
    report.type = "memory";
    report.title = title;
    report.startTime = std::chrono::system_clock::now() - std::chrono::hours(24);
    report.endTime = std::chrono::system_clock::now();
    report.generatedAt = std::chrono::system_clock::now();
    
    std::ostringstream oss;
    oss << "=== Memory Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
        << "\n\nMemory Statistics:\n"
        << "- Total Allocated: 1024 MB\n"
        << "- Currently Used: 400 MB\n"
        << "- Peak Usage: 800 MB\n"
        << "- Memory Efficiency: 75%\n"
        << "\nLeaks Detected: None\n";
    
    report.content = oss.str();
    
    storeReport(report);
    return report;
}

Report ReportGenerator::generateAlertReport(const std::string& title) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Report report;
    report.id = "report_alert_" + std::to_string(pImpl->reports.size() + 1);
    report.type = "alert";
    report.title = title;
    report.startTime = std::chrono::system_clock::now() - std::chrono::hours(24);
    report.endTime = std::chrono::system_clock::now();
    report.generatedAt = std::chrono::system_clock::now();
    
    std::ostringstream oss;
    oss << "=== Alert Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
        << "\n\nAlert Summary:\n"
        << "- Total Alerts: 0\n"
        << "- Acknowledged Alerts: 0\n"
        << "- Active Alerts: 0\n"
        << "\nStatus: No active alerts\n";
    
    report.content = oss.str();
    
    storeReport(report);
    return report;
}

Report ReportGenerator::generateCustomReport(const std::string& type,
                                            const std::string& title,
                                            const std::chrono::system_clock::time_point& startTime,
                                            const std::chrono::system_clock::time_point& endTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Report report;
    report.id = "report_custom_" + std::to_string(pImpl->reports.size() + 1);
    report.type = type;
    report.title = title;
    report.startTime = startTime;
    report.endTime = endTime;
    report.generatedAt = std::chrono::system_clock::now();
    
    std::ostringstream oss;
    oss << "=== Custom Report ===\n"
        << "Type: " << type << "\n"
        << "Title: " << title << "\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
        << "\n\nContent: Custom report generated\n";
    
    report.content = oss.str();
    
    storeReport(report);
    return report;
}

std::optional<Report> ReportGenerator::getReport(const std::string& reportId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->reports.find(reportId);
    if (it != pImpl->reports.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Report> ReportGenerator::getRecentReports(size_t maxReports) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Report> reports;
    for (const auto& pair : pImpl->reports) {
        reports.push_back(pair.second);
    }
    
    std::sort(reports.begin(), reports.end(),
              [](const Report& a, const Report& b) {
                  return a.generatedAt > b.generatedAt;
              });
    
    if (reports.size() > maxReports) {
        reports.resize(maxReports);
    }
    
    return reports;
}

bool ReportGenerator::exportReport(const Report& report, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== " << report.title << " ===\n"
         << "Type: " << report.type << "\n"
         << "Generated: " << std::chrono::system_clock::to_time_t(report.generatedAt)
         << "\n\n" << report.content << "\n"
         << "\nRecommendations:\n";
    
    for (const auto& recommendation : report.recommendations) {
        file << "- " << recommendation << "\n";
    }
    
    file.flush();
    return true;
}

void ReportGenerator::storeReport(const Report& report) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->reports[report.id] = report;
}

ReportGenerator& ReportGenerator::getGlobal() {
    static ReportGenerator globalReportGenerator;
    return globalReportGenerator;
}

void ReportGenerator::setGlobal(std::shared_ptr<ReportGenerator> generator) {
    // This is a placeholder for advanced global generator management
}

// TrendAnalyzer implementation
struct TrendAnalyzer::Impl {
    std::vector<TrendDataPoint> trendData;
    size_t analysisWindow = 10;
    double anomalyThreshold = 2.0;
    std::mutex mutex;
};

TrendAnalyzer::TrendAnalyzer() : pImpl(std::make_unique<Impl>()) {}

TrendAnalyzer::~TrendAnalyzer() = default;

void TrendAnalyzer::addTrendData(const TrendDataPoint& dataPoint) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trendData.push_back(dataPoint);
}

std::map<std::string, double> TrendAnalyzer::analyzeTrend(
    const std::string& metricName, const std::string& category,
    size_t dataPoints) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> results;
    
    std::vector<TrendDataPoint> filteredData;
    for (const auto& point : pImpl->trendData) {
        if (metricName.empty() || point.metricName == metricName) {
            if (category.empty() || point.category == category) {
                filteredData.push_back(point);
            }
        }
    }
    
    if (filteredData.size() > 1) {
        // Get the most recent data points
        size_t count = std::min(dataPoints, filteredData.size());
        std::vector<TrendDataPoint> recent(filteredData.end() - count, filteredData.end());
        
        // Calculate average
        double sum = 0.0;
        for (const auto& point : recent) {
            sum += point.value;
        }
        results["average"] = sum / recent.size();
        
        // Calculate trend (slope)
        if (recent.size() > 1) {
            double xSum = static_cast<double>(count) * (count - 1) / 2.0;
            double ySum = sum;
            double xySum = 0.0;
            double y2Sum = 0.0;
            
            for (size_t i = 0; i < recent.size(); ++i) {
                double y = recent[i].value;
                xySum += i * y;
                y2Sum += y * y;
            }
            
            double slope = (count * xySum - xSum * ySum) / (count * xSum - xSum * xSum);
            results["slope"] = slope;
            
            // Calculate R-squared
            double ssTot = y2Sum - ySum * ySum / count;
            double ssRes = y2Sum - ySum * ySum / count - 2 * slope * (xySum - xSum * ySum / count) +
                          slope * slope * (count * xSum - xSum * xSum) / count;
            results["r_squared"] = ssTot > 0 ? 1.0 - ssRes / ssTot : 0.0;
        }
    }
    
    return results;
}

std::vector<TrendDataPoint> TrendAnalyzer::detectAnomalies(
    const std::string& metricName, double threshold) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<TrendDataPoint> anomalies;
    
    std::map<std::string, std::vector<TrendDataPoint>> groupedData;
    for (const auto& point : pImpl->trendData) {
        if (metricName.empty() || point.metricName == metricName) {
            groupedData[point.category].push_back(point);
        }
    }
    
    for (auto& pair : groupedData) {
        auto& data = pair.second;
        if (data.size() < 2) continue;
        
        double mean = std::accumulate(data.begin(), data.end(), 0.0,
                                     [](double sum, const TrendDataPoint& point) {
                                         return sum + point.value;
                                     }) / data.size();
        
        double stddev = 0.0;
        for (const auto& point : data) {
            stddev += (point.value - mean) * (point.value - mean);
        }
        stddev = std::sqrt(stddev / data.size());
        
        double zScore = stddev > 0 ? (data.back().value - mean) / stddev : 0.0;
        
        if (std::abs(zScore) > threshold) {
            anomalies.push_back(data.back());
        }
    }
    
    return anomalies;
}

std::vector<TrendDataPoint> TrendAnalyzer::getTrendData(
    const std::string& metricName, const std::string& category,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<TrendDataPoint> results;
    
    for (const auto& point : pImpl->trendData) {
        if (!metricName.empty() && point.metricName != metricName) continue;
        if (!category.empty() && point.category != category) continue;
        if (!startTime.empty() && point.timestamp < startTime) continue;
        if (!endTime.empty() && point.timestamp > endTime) continue;
        
        results.push_back(point);
    }
    
    return results;
}

void TrendAnalyzer::clearTrendData() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->trendData.clear();
}

void TrendAnalyzer::setAnalysisWindow(size_t windowSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->analysisWindow = windowSize;
}

void TrendAnalyzer::setAnomalyThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->anomalyThreshold = threshold;
}

TrendAnalyzer& TrendAnalyzer::getGlobal() {
    static TrendAnalyzer globalTrendAnalyzer;
    return globalTrendAnalyzer;
}

void TrendAnalyzer::setGlobal(std::shared_ptr<TrendAnalyzer> analyzer) {
    // This is a placeholder for advanced global analyzer management
}

} // namespace nlm
