#include "ResourceProfiler.hpp"

namespace nlm {

// ResourceCounter implementation

// CPUMonitor implementation

CPUMonitor::CPUStats CPUMonitor::getStats() const {
    // In a real implementation, this would collect CPU statistics
    // using system-specific APIs
    CPUStats stats;
    stats.usagePercent = 0.0;
    stats.userTime = 0;
    stats.systemTime = 0;
    stats.idleTime = 0;
    stats.frequency = 0;
    stats.cores = std::thread::hardware_concurrency();
    stats.temperature = 0.0;
    stats.power = 0.0;
    stats.instructionsRetired = 0;
    stats.cycles = 0;
    stats.instructionsPerCycle = 0.0;
    stats.coreUsages = std::vector<double>(stats.cores, 0.0);
    return stats;
}

void CPUMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            history_.push_back(getStats());
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void CPUMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<CPUMonitor::CPUStats> CPUMonitor::getHistory() const {
    return history_;
}

// MemoryMonitor implementation

MemoryMonitor::MemoryStats MemoryMonitor::getStats() const {
    // In a real implementation, this would collect memory statistics
    MemoryStats stats;
    stats.totalBytes = 0;
    stats.usedBytes = 0;
    stats.freeBytes = 0;
    stats.cachedBytes = 0;
    stats.buffersBytes = 0;
    stats.usagePercent = 0.0;
    stats.availableBytes = 0;
    stats.mallocCount = 0;
    stats.mallocBytes = 0;
    stats.freeCount = 0;
    stats.freeBytes = 0;
    stats.reallocCount = 0;
    stats.reallocBytes = 0;
    return stats;
}

void MemoryMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            history_.push_back(getStats());
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void MemoryMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<MemoryMonitor::MemoryStats> MemoryMonitor::getHistory() const {
    return history_;
}

void MemoryMonitor::recordAllocation(uint64_t size, const std::string& label, const std::string& source) {
    ResourceAllocation allocation;
    allocation.label = label;
    allocation.size = size;
    allocation.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    allocation.source = source;
    allocation.threadId = std::this_thread::get_id();
    allocation.isFreed = false;
    allocation.allocationStack = ""; // Would capture stack trace
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    allocations_.push_back(allocation);
}

void MemoryMonitor::recordDeallocation(uint64_t size, const std::string& label) {
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    for (auto& allocation : allocations_) {
        if (!allocation.isFreed && allocation.label == label) {
            allocation.isFreed = true;
            allocation.freeTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            allocation.freeStack = ""; // Would capture stack trace
            break;
        }
    }
}

std::vector<ResourceAllocation> MemoryMonitor::getAllocations() const {
    return allocations_;
}

// NetworkMonitor implementation

NetworkMonitor::NetworkStats NetworkMonitor::getStats() const {
    // In a real implementation, this would collect network statistics
    NetworkStats stats;
    stats.uploadSpeed = 0.0;
    stats.downloadSpeed = 0.0;
    stats.uploadBytes = 0;
    stats.downloadBytes = 0;
    stats.uploadPackets = 0;
    stats.downloadPackets = 0;
    stats.uploadErrors = 0;
    stats.downloadErrors = 0;
    stats.activeConnections = {};
    stats.connectionCount = 0;
    stats.latency = 0.0;
    stats.jitter = 0.0;
    return stats;
}

void NetworkMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            history_.push_back(getStats());
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void NetworkMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<NetworkMonitor::NetworkStats> NetworkMonitor::getHistory() const {
    return history_;
}

// ThreadMonitor implementation

std::vector<ThreadMonitor::ThreadStats> ThreadMonitor::getStats() const {
    // In a real implementation, this would collect thread statistics
    std::vector<ThreadStats> stats;
    return stats;
}

void ThreadMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            std::vector<ThreadStats> currentStats = getStats();
            history_.push_back(currentStats);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void ThreadMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<std::vector<ThreadMonitor::ThreadStats>> ThreadMonitor::getHistory() const {
    return history_;
}

// GPUMonitor implementation

std::vector<GPUMonitor::GPUStats> GPUMonitor::getStats() const {
    // In a real implementation, this would collect GPU statistics
    std::vector<GPUStats> stats;
    return stats;
}

void GPUMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            auto currentStats = getStats();
            history_.push_back(currentStats);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void GPUMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<std::vector<GPUMonitor::GPUStats>> GPUMonitor::getHistory() const {
    return history_;
}

// IOMonitor implementation

std::vector<IOMonitor::IOStats> IOMonitor::getStats() const {
    // In a real implementation, this would collect I/O statistics
    std::vector<IOStats> stats;
    return stats;
}

void IOMonitor::startMonitoring(uint32_t intervalMs) {
    if (monitoring_) {
        return;
    }
    
    monitoringIntervalMs_ = intervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread([this]() {
        while (monitoring_) {
            auto currentStats = getStats();
            history_.push_back(currentStats);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(monitoringIntervalMs_));
        }
    });
}

void IOMonitor::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<std::vector<IOMonitor::IOStats>> IOMonitor::getHistory() const {
    return history_;
}

// MetricCollector implementation

void MetricCollector::recordMetric(const PerformanceMetric& metric) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    metrics_.push_back(metric);
    
    // Trim history if needed
    while (metrics_.size() > maxHistorySize_) {
        metrics_.pop_front();
    }
}

void MetricCollector::recordGauge(const std::string& name, double value,
                                 const std::string& label,
                                 const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetric metric;
    metric.name = name;
    metric.type = MetricType::Gauge;
    metric.value = value;
    metric.label = label;
    metric.tags = tags;
    metric.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    recordMetric(metric);
}

void MetricCollector::recordCounter(const std::string& name, uint64_t count,
                                    const std::string& label,
                                    const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetric metric;
    metric.name = name;
    metric.type = MetricType::Counter;
    metric.value = static_cast<double>(count);
    metric.label = label;
    metric.tags = tags;
    metric.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    recordMetric(metric);
}

void MetricCollector::recordHistogram(const std::string& name, double value,
                                      const std::string& label,
                                      const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetric metric;
    metric.name = name;
    metric.type = MetricType::Histogram;
    metric.value = value;
    metric.label = label;
    metric.tags = tags;
    metric.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    recordMetric(metric);
}

void MetricCollector::recordTimer(const std::string& name, double duration,
                                  const std::string& label,
                                  const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetric metric;
    metric.name = name;
    metric.type = MetricType::Timer;
    metric.value = duration;
    metric.label = label;
    metric.tags = tags;
    metric.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    recordMetric(metric);
}

std::vector<PerformanceMetric> MetricCollector::getMetric(const std::string& name) const {
    std::vector<PerformanceMetric> result;
    for (const auto& metric : metrics_) {
        if (metric.name == name) {
            result.push_back(metric);
        }
    }
    return result;
}

std::vector<PerformanceMetric> MetricCollector::getMetricsByType(MetricType type) const {
    std::vector<PerformanceMetric> result;
    for (const auto& metric : metrics_) {
        if (metric.type == type) {
            result.push_back(metric);
        }
    }
    return result;
}

std::vector<PerformanceMetric> MetricCollector::getMetricsByTag(
    const std::string& tagName,
    const std::string& tagValue) const {
    
    std::vector<PerformanceMetric> result;
    for (const auto& metric : metrics_) {
        auto it = metric.tags.find(tagName);
        if (it != metric.tags.end() && it->second == tagValue) {
            result.push_back(metric);
        }
    }
    return result;
}

std::vector<PerformanceMetric> MetricCollector::getAllMetrics() const {
    return std::vector<PerformanceMetric>(metrics_.begin(), metrics_.end());
}

MetricCollector::MetricStats MetricCollector::getMetricStats(const std::string& name) const {
    MetricStats stats;
    std::vector<double> values;
    
    for (const auto& metric : metrics_) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) {
        return stats;
    }
    
    stats.count = values.size();
    stats.sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats.min = *std::min_element(values.begin(), values.end());
    stats.max = *std::max_element(values.begin(), values.end());
    stats.mean = stats.sum / stats.count;
    stats.median = values[stats.count / 2];
    stats.stddev = calculateStdDev(values, stats.mean);
    stats.variance = stats.stddev * stats.stddev;
    
    // Calculate percentiles
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    stats.p95 = sorted[static_cast<size_t>(0.95 * stats.count)];
    stats.p99 = sorted[static_cast<size_t>(0.99 * stats.count)];
    stats.p999 = sorted[static_cast<size_t>(0.999 * stats.count)];
    
    stats.throughput = stats.count; // Would need time window
    
    return stats;
}

void MetricCollector::clearMetrics(const std::string& name) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    if (name.empty()) {
        metrics_.clear();
    } else {
        metrics_.erase(std::remove_if(metrics_.begin(), metrics_.end(),
                                     [&name](const PerformanceMetric& m) {
                                         return m.name == name;
                                     }),
                       metrics_.end());
    }
}

std::vector<std::string> MetricCollector::getMetricNames() const {
    std::vector<std::string> names;
    for (const auto& metric : metrics_) {
        if (std::find(names.begin(), names.end(), metric.name) == names.end()) {
            names.push_back(metric.name);
        }
    }
    return names;
}

std::unordered_map<std::string, double> MetricCollector::getLatestValues() const {
    std::unordered_map<std::string, double> latest;
    std::unordered_map<std::string, uint64_t> latestTimestamps;
    
    for (const auto& metric : metrics_) {
        auto it = latestTimestamps.find(metric.name);
        if (it == latestTimestamps.end() || metric.timestamp > it->second) {
            latest[metric.name] = metric.value;
            latestTimestamps[metric.name] = metric.timestamp;
        }
    }
    
    return latest;
}

// AlertManager implementation

void AlertManager::addAlert(const PerformanceAlert& alert) {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    alerts_.push_back(alert);
    alertsChanged_ = true;
}

void AlertManager::removeAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    alerts_.erase(std::remove_if(alerts_.begin(), alerts_.end(),
                               [&alertId](const PerformanceAlert& a) {
                                   return a.id == alertId;
                               }),
                 alerts_.end());
    alertsChanged_ = true;
}

void AlertManager::updateAlert(const PerformanceAlert& alert) {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    for (auto& existingAlert : alerts_) {
        if (existingAlert.id == alert.id) {
            existingAlert = alert;
            alertsChanged_ = true;
            return;
        }
    }
    
    alerts_.push_back(alert);
    alertsChanged_ = true;
}

std::optional<PerformanceAlert> AlertManager::getAlert(const std::string& alertId) const {
    for (const auto& alert : alerts_) {
        if (alert.id == alertId) {
            return alert;
        }
    }
    return std::nullopt;
}

std::vector<PerformanceAlert> AlertManager::getAllAlerts() const {
    return alerts_;
}

std::vector<PerformanceAlert> AlertManager::getAlertsBySeverity(
    const std::string& severity) const {
    
    std::vector<PerformanceAlert> result;
    for (const auto& alert : alerts_) {
        if (alert.severity == severity) {
            result.push_back(alert);
        }
    }
    
    return result;
}

std::vector<PerformanceAlert> AlertManager::getAlertsByMetric(
    const std::string& metricName) const {
    
    std::vector<PerformanceAlert> result;
    for (const auto& alert : alerts_) {
        if (alert.metricName == metricName) {
            result.push_back(alert);
        }
    }
    
    return result;
}

std::vector<PerformanceAlert> AlertManager::checkAlerts(const MetricCollector& collector) const {
    std::vector<PerformanceAlert> triggeredAlerts;
    
    for (const auto& alert : alerts_) {
        if (!alert.enabled) {
            continue;
        }
        
        // Get current metrics
        auto metrics = collector.getMetric(alert.metricName);
        if (metrics.empty()) {
            continue;
        }
        
        // Check alert condition
        bool conditionMet = false;
        for (const auto& metric : metrics) {
            if (checkCondition(metric, alert.condition)) {
                conditionMet = true;
                break;
            }
        }
        
        if (conditionMet) {
            // Check cooldown
            if (alert.lastTriggered == 0 ||
                (std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count() -
                 alert.lastTriggered) > alert.cooldown.count()) {
                
                PerformanceAlert triggered = alert;
                triggered.lastTriggered = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                triggered.triggerCount++;
                triggered.message = generateAlertMessage(triggered, metrics);
                
                triggeredAlerts.push_back(triggered);
                
                // Update in alerts
                for (auto& existingAlert : alerts_) {
                    if (existingAlert.id == triggered.id) {
                        existingAlert = triggered;
                        break;
                    }
                }
            }
        }
    }
    
    return triggeredAlerts;
}

void AlertManager::clearTriggeredAlerts() {
    for (auto& alert : alerts_) {
        alert.triggerCount = 0;
        alert.lastTriggered = 0;
    }
    alertsChanged_ = true;
}

void AlertManager::setAlertEnabled(const std::string& alertId, bool enabled) {
    for (auto& alert : alerts_) {
        if (alert.id == alertId) {
            alert.enabled = enabled;
            alertsChanged_ = true;
            return;
        }
    }
}

AlertManager::AlertStats AlertManager::getStats() const {
    AlertStats stats;
    stats.totalAlerts = alerts_.size();
    
    for (const auto& alert : alerts_) {
        if (alert.enabled) {
            stats.enabledAlerts++;
        }
        if (alert.triggerCount > 0) {
            stats.triggeredAlerts++;
        }
        stats.severityCounts[alert.severity]++;
    }
    
    return stats;
}

// PerformanceAnalyzer implementation

PerformanceAnalysis PerformanceAnalyzer::analyze(const MetricCollector& collector,
                                               const std::vector<std::string>& metricNames) {
    PerformanceAnalysis analysis;
    analysis.analysisName = "PerformanceAnalysis_" + 
                          std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
                              std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    analysis.description = "Performance analysis of system metrics";
    
    // Get all metrics or filter by names
    std::vector<PerformanceMetric> metrics;
    if (metricNames.empty()) {
        metrics = collector.getAllMetrics();
    } else {
        for (const auto& name : metricNames) {
            auto metric = collector.getMetric(name);
            metrics.insert(metrics.end(), metric.begin(), metric.end());
        }
    }
    
    analysis.metrics = metricNames;
    
    // Analyze metrics
    for (const auto& metric : metrics) {
        auto stats = collector.getMetricStats(metric.name);
        
        if (stats.count > 0) {
            double score = calculatePerformanceScore(stats);
            analysis.score += score;
            
            if (score < 50.0) {
                analysis.issues.push_back("Low performance for " + metric.name + " (score: " + 
                                        std::to_string(score) + ")");
            } else if (score < 80.0) {
                analysis.warnings.push_back("Moderate performance for " + metric.name + " (score: " + 
                                           std::to_string(score) + ")");
            }
        }
    }
    
    if (!metrics.empty()) {
        analysis.score /= metrics.size();
    }
    
    // Generate recommendations
    analysis.recommendations = generateRecommendations(analysis);
    
    // Calculate improvement potential
    analysis.improvementPotential = std::max(0.0, 100.0 - analysis.score);
    
    return analysis;
}

PerformanceTrend PerformanceAnalyzer::analyzeTrend(const std::vector<PerformanceMetric>& metrics,
                                                std::chrono::nanoseconds period) {
    
    PerformanceTrend trend;
    trend.period = period;
    
    if (metrics.empty()) {
        return trend;
    }
    
    // Group metrics by name
    std::unordered_map<std::string, std::vector<std::pair<uint64_t, double>>> groupedMetrics;
    for (const auto& metric : metrics) {
        groupedMetrics[metric.name].push_back(
            std::make_pair(metric.timestamp, metric.value));
    }
    
    // Analyze each metric
    for (const auto& pair : groupedMetrics) {
        if (pair.second.size() >= 2) {
            auto linearRegression = calculateLinearRegression(pair.second);
            double slope = linearRegression.first;
            double rSquared = linearRegression.second;
            
            trend.slope = slope;
            trend.rSquared = rSquared;
            
            if (slope > 0.001) {
                trend.direction = "increasing";
            } else if (slope < -0.001) {
                trend.direction = "decreasing";
            } else {
                trend.direction = "stable";
            }
            
            // Calculate average change
            double totalChange = 0.0;
            for (size_t i = 1; i < pair.second.size(); ++i) {
                totalChange += pair.second[i].second - pair.second[i-1].second;
            }
            
            trend.averageChange = totalChange / (pair.second.size() - 1);
            trend.dataPoints = pair.second;
        }
    }
    
    return trend;
}

std::vector<PerformanceRegression> PerformanceAnalyzer::detectRegressions(
    const std::vector<PerformanceBenchmark>& benchmarks,
    double regressionThresholdPercent) {
    
    std::vector<PerformanceRegression> regressions;
    
    for (const auto& benchmark : benchmarks) {
        if (benchmark.previousRuns.empty()) {
            continue;
        }
        
        // Compare with previous runs
        for (const auto& previousRun : benchmark.previousRuns) {
            double regression = (benchmark.executionTime - previousRun.executionTime) / 
                               previousRun.executionTime * 100.0;
            
            if (std::abs(regression) > regressionThresholdPercent) {
                PerformanceRegression reg;
                reg.metricName = benchmark.benchmarkName;
                reg.baselineValue = previousRun.executionTime;
                reg.currentValue = benchmark.executionTime;
                reg.regressionPercent = regression;
                reg.detectionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                
                // Determine severity
                if (std::abs(regression) > 10.0) {
                    reg.severity = "critical";
                } else if (std::abs(regression) > 5.0) {
                    reg.severity = "high";
                } else {
                    reg.severity = "medium";
                }
                
                reg.description = "Performance regression in " + benchmark.benchmarkName + 
                                 " (" + std::to_string(regression) + "%)";
                reg.suggestedFix = "Investigate and fix performance regression in " + 
                                  benchmark.benchmarkSuite;
                
                regressions.push_back(reg);
            }
        }
    }
    
    return regressions;
}

std::vector<std::string> PerformanceAnalyzer::generateRecommendations(
    const PerformanceAnalysis& analysis) {
    
    std::vector<std::string> recommendations;
    
    for (const auto& issue : analysis.issues) {
        // Generate recommendation based on issue
        recommendations.push_back("Address performance issue: " + issue);
    }
    
    for (const auto& warning : analysis.warnings) {
        // Generate recommendation based on warning
        recommendations.push_back("Improve performance for: " + warning);
    }
    
    if (analysis.improvementPotential > 20.0) {
        recommendations.push_back("Significant performance improvement potential detected (" + 
                                std::to_string(analysis.improvementPotential) + "%)");
    }
    
    return recommendations;
}

std::string PerformanceAnalyzer::analyzeMetricDistribution(
    const std::vector<PerformanceMetric>& metrics) {
    
    if (metrics.empty()) {
        return "No metrics to analyze";
    }
    
    std::stringstream ss;
    ss << "Metric distribution analysis for " << metrics.size() << " metrics:" << std::endl;
    
    std::unordered_map<std::string, std::vector<double>> metricValues;
    for (const auto& metric : metrics) {
        metricValues[metric.name].push_back(metric.value);
    }
    
    for (const auto& pair : metricValues) {
        ss << "  " << pair.first << ": " << pair.second.size() << " values" << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> PerformanceAnalyzer::identifyBottlenecks(
    const MetricCollector& collector) {
    
    std::vector<std::string> bottlenecks;
    
    auto metrics = collector.getAllMetrics();
    
    // Check for high latency
    for (const auto& metric : metrics) {
        if (metric.type == MetricType::Timer && metric.value > 100.0) {
            bottlenecks.push_back("High latency in " + metric.name + " (" + 
                                 std::to_string(metric.value) + "ms)");
        }
    }
    
    // Check for low throughput
    for (const auto& metric : metrics) {
        if (metric.type == MetricType::Counter) {
            double rate = metric.value / 1000.0; // operations per second
            if (rate < 1000.0) {
                bottlenecks.push_back("Low throughput in " + metric.name + " (" + 
                                     std::to_string(rate) + " ops/sec)");
            }
        }
    }
    
    return bottlenecks;
}

double PerformanceAnalyzer::calculatePerformanceScore(const MetricCollector::MetricStats& stats) {
    if (stats.count == 0) return 0.0;
    
    double score = 100.0;
    
    // Penalize high variance
    if (stats.variance > 10000.0) {
        score -= 50.0;
    }
    
    // Penalize low throughput
    if (stats.throughput < 100.0) {
        score -= 30.0;
    }
    
    // Penalize low efficiency
    if (stats.efficiency < 0.5) {
        score -= 20.0;
    }
    
    return std::max(0.0, score);
}

std::string PerformanceAnalyzer::gradePerformance(double score) {
    if (score >= 90.0) return "A";
    if (score >= 80.0) return "B";
    if (score >= 70.0) return "C";
    if (score >= 60.0) return "D";
    return "F";
}

std::pair<double, double> PerformanceAnalyzer::calculateLinearRegression(
    const std::vector<std::pair<uint64_t, double>>& data) {
    
    if (data.size() < 2) {
        return std::make_pair(0.0, 0.0);
    }
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    uint64_t n = data.size();
    
    for (const auto& point : data) {
        double x = static_cast<double>(point.first);
        double y = point.second;
        
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }
    
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0) {
        return std::make_pair(0.0, 0.0);
    }
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    double intercept = (sumY - slope * sumX) / n;
    
    // Calculate R-squared
    double meanY = sumY / n;
    double ssTotal = 0.0, ssResidual = 0.0;
    
    for (const auto& point : data) {
        double y = point.second;
        ssTotal += (y - meanY) * (y - meanY);
        ssResidual += (y - (slope * static_cast<double>(point.first) + intercept)) * 
                     (y - (slope * static_cast<double>(point.first) + intercept));
    }
    
    double rSquared = (ssTotal - ssResidual) / ssTotal;
    
    return std::make_pair(slope, rSquared);
}

// PerformanceReporter implementation

std::string PerformanceReporter::generateReport(const MetricCollector& collector,
                                                const AlertManager& alertManager,
                                                const PerformanceAnalyzer& analyzer,
                                                const ReportConfig& config) {
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    
    if (config.format == "text") {
        ss << generateTextReport(collector, alertManager, analyzer, config);
    } else if (config.format == "json") {
        ss << generateJSONReport(collector, alertManager, analyzer, config);
    } else if (config.format == "html") {
        ss << generateHTMLReport(collector, alertManager, analyzer, config);
    }
    
    return ss.str();
}

bool PerformanceReporter::saveReport(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

std::string PerformanceReporter::generateHTMLReport(const MetricCollector& collector,
                                                    const AlertManager& alertManager,
                                                    const PerformanceAnalyzer& analyzer,
                                                    const ReportConfig& config) {
    
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "  <title>Performance Report</title>" << std::endl;
    ss << "  <style>" << std::endl;
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    ss << "    table { border-collapse: collapse; width: 100%; }" << std::endl;
    ss << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    ss << "    th { background-color: #f2f2f2; }" << std::endl;
    ss << "    .critical { background-color: #ffcccc; }" << std::endl;
    ss << "    .warning { background-color: #ffffcc; }" << std::endl;
    ss << "    .good { background-color: #ccffcc; }" << std::endl;
    ss << "  </style>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "  <h1>Performance Report</h1>" << std::endl;
    ss << "  <p>Generated: " << std::chrono::system_clock::now() << "</p>" << std::endl;
    
    // Metrics section
    if (config.includeMetrics) {
        ss << "  <h2>Metrics</h2>" << std::endl;
        ss << "  <table>" << std::endl;
        ss << "    <tr><th>Metric</th><th>Value</th><th>Unit</th><th>Tags</th></tr>" << std::endl;
        
        auto metrics = collector.getAllMetrics();
        for (const auto& metric : metrics) {
            if (config.metricFilter.empty() || metric.name.find(config.metricFilter) != std::string::npos) {
                ss << "    <tr>" << std::endl;
                ss << "      <td>" << metric.name << "</td>" << std::endl;
                ss << "      <td>" << metric.value << "</td>" << std::endl;
                ss << "      <td>" << metric.unit << "</td>" << std::endl;
                ss << "      <td>";
                bool first = true;
                for (const auto& tag : metric.tags) {
                    if (!first) ss << ", ";
                    ss << tag.first << "=" << tag.second;
                    first = false;
                }
                ss << "</td>" << std::endl;
                ss << "    </tr>" << std::endl;
            }
        }
        
        ss << "  </table>" << std::endl;
    }
    
    // Alerts section
    if (config.includeAlerts) {
        ss << "  <h2>Alerts</h2>" << std::endl;
        ss << "  <table>" << std::endl;
        ss << "    <tr><th>ID</th><th>Name</th><th>Severity</th><th>Status</th><th>Count</th></tr>" << std::endl;
        
        auto alerts = alertManager.getAllAlerts();
        for (const auto& alert : alerts) {
            std::string status = alert.enabled ? "enabled" : "disabled";
            std::string rowClass = "";
            if (alert.severity == "critical") rowClass = "critical";
            else if (alert.severity == "warning") rowClass = "warning";
            else if (alert.severity == "info") rowClass = "good";
            
            ss << "    <tr class=\"" << rowClass << "\">" << std::endl;
            ss << "      <td>" << alert.id << "</td>" << std::endl;
            ss << "      <td>" << alert.name << "</td>" << std::endl;
            ss << "      <td>" << alert.severity << "</td>" << std::endl;
            ss << "      <td>" << status << "</td>" << std::endl;
            ss << "      <td>" << alert.triggerCount << "</td>" << std::endl;
            ss << "    </tr>" << std::endl;
        }
        
        ss << "  </table>" << std::endl;
    }
    
    // Analysis section
    if (config.includeAnalysis) {
        ss << "  <h2>Analysis</h2>" << std::endl;
        ss << "  <table>" << std::endl;
        ss << "    <tr><th>Metric</th><th>Score</th><th>Grade</th><th>Recommendations</th></tr>" << std::endl;
        
        auto analysis = analyzer.analyze(collector);
        ss << "    <tr>" << std::endl;
        ss << "      <td>Overall Performance</td>" << std::endl;
        ss << "      <td>" << analysis.score << "</td>" << std::endl;
        ss << "      <td>" << analysis.grade << "</td>" << std::endl;
        ss << "      <td>";
        for (const auto& recommendation : analysis.recommendations) {
            ss << "• " << recommendation << "<br>";
        }
        ss << "</td>" << std::endl;
        ss << "    </tr>" << std::endl;
        
        ss << "  </table>" << std::endl;
    }
    
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    
    return ss.str();
}

std::string PerformanceReporter::generateJSONReport(const MetricCollector& collector,
                                                    const AlertManager& alertManager,
                                                    const PerformanceAnalyzer& analyzer,
                                                    const ReportConfig& config) {
    
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"timestamp\": \"" << std::chrono::system_clock::now() << "\"," << std::endl;
    ss << "  \"metrics\": [" << std::endl;
    
    auto metrics = collector.getAllMetrics();
    for (size_t i = 0; i < metrics.size(); ++i) {
        const auto& metric = metrics[i];
        ss << "    {\"name\": \"" << metric.name << "\", \"type\": \"" << 
           (metric.type == MetricType::Gauge ? "gauge" :
            metric.type == MetricType::Counter ? "counter" :
            metric.type == MetricType::Histogram ? "histogram" :
            metric.type == MetricType::Timer ? "timer" : "custom") << "\", \"value\": " << 
           metric.value << ", \"label\": \"" << metric.label << "\"";
        
        if (i < metrics.size() - 1) ss << "," << std::endl;
    }
    
    ss << std::endl << "  ]," << std::endl;
    ss << "  \"alerts\": [" << std::endl;
    
    auto alerts = alertManager.getAllAlerts();
    for (size_t i = 0; i < alerts.size(); ++i) {
        const auto& alert = alerts[i];
        ss << "    {\"id\": \"" << alert.id << "\", \"name\": \"" << alert.name << 
           "\", \"severity\": \"" << alert.severity << "\", \"enabled\": " << 
           (alert.enabled ? "true" : "false") << ", \"triggerCount\": " << 
           alert.triggerCount << "}";
        
        if (i < alerts.size() - 1) ss << "," << std::endl;
    }
    
    ss << std::endl << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string PerformanceReporter::generateTextReport(const MetricCollector& collector,
                                                    const AlertManager& alertManager,
                                                    const PerformanceAnalyzer& analyzer,
                                                    const ReportConfig& config) {
    
    std::stringstream ss;
    ss << "Performance Report" << std::endl;
    ss << "=================" << std::endl;
    ss << std::endl;
    ss << "Generated: " << std::chrono::system_clock::now() << std::endl;
    ss << std::endl;
    
    // Metrics
    if (config.includeMetrics) {
        ss << "Metrics:" << std::endl;
        auto metrics = collector.getAllMetrics();
        for (const auto& metric : metrics) {
            ss << "  " << metric.name << ": " << metric.value << " " << metric.unit << std::endl;
        }
        ss << std::endl;
    }
    
    // Alerts
    if (config.includeAlerts) {
        ss << "Alerts:" << std::endl;
        auto alerts = alertManager.getAllAlerts();
        for (const auto& alert : alerts) {
            ss << "  " << alert.name << " (" << alert.severity << ") - " << alert.description << std::endl;
        }
        ss << std::endl;
    }
    
    // Analysis
    if (config.includeAnalysis) {
        ss << "Analysis:" << std::endl;
        auto analysis = analyzer.analyze(collector);
        ss << "  Overall Score: " << analysis.score << " (Grade: " << analysis.grade << ")" << std::endl;
        ss << "  Recommendations:" << std::endl;
        for (const auto& recommendation : analysis.recommendations) {
            ss << "    • " << recommendation << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceReporter::generateDashboardData(const MetricCollector& collector,
                                                       const AlertManager& alertManager,
                                                       const PerformanceAnalyzer& analyzer,
                                                       const ReportConfig& config) {
    
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"metrics\": [" << std::endl;
    
    auto metrics = collector.getAllMetrics();
    for (size_t i = 0; i < metrics.size(); ++i) {
        const auto& metric = metrics[i];
        ss << "    {\"name\": \"" << metric.name << "\", \"value\": " << 
           metric.value << ", \"type\": \"" << 
           (metric.type == MetricType::Gauge ? "gauge" :
            metric.type == MetricType::Counter ? "counter" :
            metric.type == MetricType::Histogram ? "histogram" :
            metric.type == MetricType::Timer ? "timer" : "custom") << 
           "\", \"timestamp\": " << metric.timestamp << "}";
        
        if (i < metrics.size() - 1) ss << "," << std::endl;
    }
    
    ss << std::endl << "  ]," << std::endl;
    ss << "  \"alerts\": [" << std::endl;
    
    auto alerts = alertManager.getAllAlerts();
    for (size_t i = 0; i < alerts.size(); ++i) {
        const auto& alert = alerts[i];
        ss << "    {\"id\": \"" << alert.id << "\", \"name\": \"" << alert.name << 
           "\", \"severity\": \"" << alert.severity << ", \"enabled\": " << 
           (alert.enabled ? "true" : "false") << "}";
        
        if (i < alerts.size() - 1) ss << "," << std::endl;
    }
    
    ss << std::endl << "  ]," << std::endl;
    
    // Add analysis data
    auto analysis = analyzer.analyze(collector);
    ss << "  \"analysis\": {\"score\": " << analysis.score << 
           ", \"grade\": \"" << analysis.grade << 
           "\", \"recommendations\": [";
    
    for (size_t i = 0; i < analysis.recommendations.size(); ++i) {
        ss << "\"" << analysis.recommendations[i] << "\"";
        if (i < analysis.recommendations.size() - 1) ss << ", ";
    }
    
    ss << "]}" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string PerformanceReporter::formatMetricValue(double value, const std::string& unit) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        ss << " " << unit;
    }
    return ss.str();
}

std::string PerformanceReporter::formatTimestamp(uint64_t timestampNs) {
    auto time = std::chrono::nanoseconds(timestampNs);
    auto systemTime = std::chrono::time_point<std::chrono::system_clock>(time);
    std::time_t time_t = std::chrono::system_clock::to_time_t(systemTime);
    return std::string(std::ctime(&time_t));
}

std::string PerformanceReporter::escapeHTML(const std::string& input) {
    std::string escaped = input;
    // Simple HTML escaping
    for (char& c : escaped) {
        switch (c) {
            case '&': c = "&amp;"; break;
            case '<': c = "&lt;"; break;
            case '>': c = "&gt;"; break;
            case '"': c = "&quot;"; break;
            case '\'': c = "&#39;"; break;
        }
    }
    return escaped;
}

std::string PerformanceReporter::generateCSS() {
    return "";
}

std::string PerformanceReporter::generateJavaScript() {
    return "";
}

// PerformanceMonitor implementation

PerformanceMonitor::PerformanceMonitor(const Config& config) : config_(config), monitoring_(false) {
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::configure(const Config& config) {
    std::lock_guard<std::mutex> lock(monitorMutex_);
    config_ = config;
    
    // Update component configurations
    if (config_.enableResourceProfiling) {
        // Update resource profiling interval
        // In a real implementation, this would update the monitoring thread
    }
}

void PerformanceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(monitorMutex_);
    
    if (monitoring_) {
        return;
    }
    
    monitoring_ = true;
    monitoringThread_ = std::thread([this]() { monitoringLoop(); });
}

void PerformanceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(monitorMutex_);
    
    monitoring_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceMonitor::recordMetric(const PerformanceMetric& metric) {
    metricCollector_.recordMetric(metric);
}

void PerformanceMonitor::recordGauge(const std::string& name, double value,
                                     const std::string& label,
                                     const std::unordered_map<std::string, std::string>& tags) {
    metricCollector_.recordGauge(name, value, label, tags);
}

void PerformanceMonitor::recordCounter(const std::string& name, uint64_t count,
                                       const std::string& label,
                                       const std::unordered_map<std::string, std::string>& tags) {
    metricCollector_.recordCounter(name, count, label, tags);
}

void PerformanceMonitor::recordHistogram(const std::string& name, double value,
                                         const std::string& label,
                                         const std::unordered_map<std::string, std::string>& tags) {
    metricCollector_.recordHistogram(name, value, label, tags);
}

void PerformanceMonitor::recordTimer(const std::string& name, double duration,
                                     const std::string& label,
                                     const std::unordered_map<std::string, std::string>& tags) {
    metricCollector_.recordTimer(name, duration, label, tags);
}

std::string PerformanceMonitor::generateReport() {
    return performanceReporter_.generateReport(metricCollector_, alertManager_,
                                               performanceAnalyzer_, config_);
}

bool PerformanceMonitor::saveReport(const std::string& filepath) {
    std::string report = generateReport();
    return performanceReporter_.saveReport(filepath.empty() ? "performance_report.txt" : filepath, report);
}

bool PerformanceMonitor::loadReport(const std::string& filepath) {
    // In a real implementation, this would load a saved report
    return true;
}

PerformanceAnalysis PerformanceMonitor::getPerformanceAnalysis() {
    return performanceAnalyzer_.analyze(metricCollector_);
}

std::vector<PerformanceAlert> PerformanceMonitor::getPerformanceAlerts() {
    return alertManager_.checkAlerts(metricCollector_);
}

std::vector<PerformanceBenchmark> PerformanceMonitor::getPerformanceBenchmarks() {
    return performanceBenchmarks_;
}

void PerformanceMonitor::addPerformanceBenchmark(const PerformanceBenchmark& benchmark) {
    performanceBenchmarks_.push_back(benchmark);
}

std::vector<PerformanceTrend> PerformanceMonitor::getPerformanceTrends() {
    return performanceTrends_;
}

std::vector<PerformanceRegression> PerformanceMonitor::getPerformanceRegressions() {
    return performanceRegressions_;
}

void PerformanceMonitor::setPerformanceAlert(const PerformanceAlert& alert) {
    alertManager_.addAlert(alert);
}

void PerformanceMonitor::removePerformanceAlert(const std::string& alertId) {
    alertManager_.removeAlert(alertId);
}

std::string PerformanceMonitor::getMonitoringStatus() const {
    std::stringstream ss;
    ss << "Performance Monitor Status:" << std::endl;
    ss << "  Monitoring: " << (monitoring_ ? "Enabled" : "Disabled") << std::endl;
    ss << "  Metrics Collected: " << metricCollector_.getMetricNames().size() << std::endl;
    ss << "  Active Alerts: " << alertManager_.getAllAlerts().size() << std::endl;
    ss << "  Resource Profiling: " << (config_.enableResourceProfiling ? "Enabled" : "Disabled") << std::endl;
    ss << "  Performance Analysis: " << (config_.enableAnalysis ? "Enabled" : "Disabled") << std::endl;
    ss << "  Reporting: " << (config_.enableReporting ? "Enabled" : "Disabled") << std::endl;
    return ss.str();
}

std::string PerformanceMonitor::getSystemInfo() const {
    // In a real implementation, this would collect system information
    return "System information not available in this implementation.";
}

std::string PerformanceMonitor::getResourceUsage() const {
    // In a real implementation, this would collect resource usage information
    return "Resource usage information not available in this implementation.";
}

std::vector<std::string> PerformanceMonitor::getPerformanceRecommendations() {
    auto analysis = getPerformanceAnalysis();
    return analysis.recommendations;
}

std::vector<std::string> PerformanceMonitor::getPerformanceIssues() {
    auto analysis = getPerformanceAnalysis();
    return analysis.issues;
}

std::vector<std::string> PerformanceMonitor::getPerformanceWarnings() {
    auto analysis = getPerformanceAnalysis();
    return analysis.warnings;
}

bool PerformanceMonitor::checkCondition(const PerformanceMetric& metric,
                                       const std::string& condition) {
    // Simple condition checking
    if (condition.find(">") != std::string::npos) {
        return metric.value > std::stod(condition.substr(condition.find(">") + 1));
    } else if (condition.find("<") != std::string::npos) {
        return metric.value < std::stod(condition.substr(condition.find("<") + 1));
    } else if (condition.find("==") != std::string::npos) {
        return metric.value == std::stod(condition.substr(condition.find("==") + 2));
    } else if (condition.find("!=") != std::string::npos) {
        return metric.value != std::stod(condition.substr(condition.find("!=") + 2));
    }
    
    return false;
}

std::string PerformanceMonitor::generateAlertMessage(const PerformanceAlert& alert,
                                                     const std::vector<PerformanceMetric>& metrics) {
    std::stringstream ss;
    ss << "Alert: " << alert.name << std::endl;
    ss << "  Metric: " << alert.metricName << std::endl;
    ss << "  Severity: " << alert.severity << std::endl;
    ss << "  Description: " << alert.description << std::endl;
    ss << "  Action: " << alert.action << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::monitoringLoop() {
    while (monitoring_) {
        updatePerformanceMetrics();
        checkAlerts();
        generatePerformanceAnalysis();
        updatePerformanceTrends();
        detectPerformanceRegressions();
        generatePerformanceReports();
        applyAutomaticOptimizations();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.monitoringIntervalMs));
    }
}

void PerformanceMonitor::updatePerformanceMetrics() {
    // Update metrics from collectors
    // This would update the metric collector with new measurements
}

void PerformanceMonitor::checkAlerts() {
    auto triggeredAlerts = alertManager_.checkAlerts(metricCollector_);
    for (const auto& alert : triggeredAlerts) {
        // Send notification if configured
        if (alert.action == "notify") {
            // Send notification
        }
    }
}

void PerformanceMonitor::generatePerformanceAnalysis() {
    // Generate and store performance analysis
    performanceAnalysis_ = performanceAnalyzer_.analyze(metricCollector_);
}

void PerformanceMonitor::updatePerformanceTrends() {
    // Calculate and store performance trends
    // This would calculate trends from historical data
}

void PerformanceMonitor::detectPerformanceRegressions() {
    // Detect performance regressions
    // This would compare current performance with baseline
}

void PerformanceMonitor::generatePerformanceReports() {
    // Generate performance reports if configured
    if (config_.enableReporting) {
        saveReport();
    }
}

void PerformanceMonitor::applyAutomaticOptimizations() {
    // Apply automatic optimizations if configured
    if (config_.enableAutoOptimization) {
        // Apply optimizations based on analysis results
    }
}

void PerformanceMonitor::loadAlertConfiguration() {
    // Load alert configuration from file
}

void PerformanceMonitor::loadOptimizationRules() {
    // Load optimization rules from file
}

} // namespace nlm