// Implementation of EnhancedStatusReporter for NLM

#include "EnhancedStatusReporter.hpp"
#include <thread>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct EnhancedStatusReporter::Impl {
    std::chrono::milliseconds updateInterval;
    bool includeResourceMetrics;
    bool includeBrainMetrics;
    bool includePerformanceMetrics;
    bool useColors;
    bool monitoringEnabled;
    
    // Component status
    std::unordered_map<std::string, SystemComponentStatus> componentStatus;
    
    // Resource monitoring
    ResourceMetrics currentResourceMetrics;
    bool resourceMonitoringActive{false};
    std::thread resourceMonitorThread;
    std::atomic<bool> resourceMonitorRunning{false};
    
    // Brain state
    BrainStateMetrics currentBrainMetrics;
    std::shared_ptr<Brain> brain;
    
    // Configuration validation
    std::vector<ValidationResult> validationResults;
    
    // Performance metrics
    PerformanceMetrics currentPerformanceMetrics;
    std::vector<std::pair<std::string, double>> performanceMeasurements;
    
    // Live monitoring
    std::vector<MonitoringDataPoint> monitoringData;
    
    // Alert thresholds
    std::map<std::string, double> alertThresholds;
    
    // Alerts
    std::vector<Alert> alerts;
    std::function<void(const Alert&)> alertCallback;
    
    // Dashboard
    bool dashboardRunning{false};
    std::thread dashboardThread;
    std::atomic<bool> dashboardThreadRunning{false};
    std::chrono::seconds dashboardUpdateInterval{5};
    
    // Performance statistics
    std::vector<double> cpuUsageHistory;
    std::vector<double> memoryUsageHistory;
    std::chrono::system_clock::time_point lastUpdateTime;
};

EnhancedStatusReporter::EnhancedStatusReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->updateInterval = std::chrono::milliseconds(1000);
    pImpl->includeResourceMetrics = true;
    pImpl->includeBrainMetrics = true;
    pImpl->includePerformanceMetrics = true;
    pImpl->useColors = true;
    pImpl->monitoringEnabled = false;
    
    // Set default alert thresholds
    pImpl->alertThresholds = {
        {"cpu_usage", 80.0},     // Alert when CPU usage exceeds 80%
        {"memory_usage", 90.0},  // Alert when memory usage exceeds 90%
        {"throughput", 1000.0},  // Alert when throughput is low
        {"latency", 100.0}       // Alert when latency is high
    };
}

EnhancedStatusReporter::~EnhancedStatusReporter() {
    stopResourceMonitoring();
    stopDashboard();
}

void EnhancedStatusReporter::setUpdateInterval(std::chrono::milliseconds interval) {
    pImpl->updateInterval = interval;
}

void EnhancedStatusReporter::setIncludeResourceMetrics(bool include) {
    pImpl->includeResourceMetrics = include;
}

void EnhancedStatusReporter::setIncludeBrainMetrics(bool include) {
    pImpl->includeBrainMetrics = include;
}

void EnhancedStatusReporter::setIncludePerformanceMetrics(bool include) {
    pImpl->includePerformanceMetrics = include;
}

void EnhancedStatusReporter::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

void EnhancedStatusReporter::setMonitoringEnabled(bool enabled) {
    pImpl->monitoringEnabled = enabled;
}

void EnhancedStatusReporter::setAlertThresholds(const std::map<std::string, double>& thresholds) {
    pImpl->alertThresholds = thresholds;
}

void EnhancedStatusReporter::addComponentStatus(const SystemComponentStatus& status) {
    pImpl->componentStatus[status.name] = status;
}

void EnhancedStatusReporter::updateComponentStatus(const std::string& name, bool operational,
                                                   const std::string& status, double performance,
                                                   const std::string& details) {
    if (pImpl->componentStatus.find(name) == pImpl->componentStatus.end()) {
        pImpl->componentStatus[name] = SystemComponentStatus(name, operational, status);
    } else {
        pImpl->componentStatus[name].isOperational = operational;
        pImpl->componentStatus[name].status = status;
        pImpl->componentStatus[name].performance = performance;
    }
    
    pImpl->componentStatus[name].lastUpdate = std::chrono::system_clock::now();
    pImpl->componentStatus[name].details = details;
}

void EnhancedStatusReporter::removeComponentStatus(const std::string& name) {
    pImpl->componentStatus.erase(name);
}

void EnhancedStatusReporter::clearComponentStatus() {
    pImpl->componentStatus.clear();
}

void EnhancedStatusReporter::updateResourceMetrics(const ResourceMetrics& metrics) {
    pImpl->currentResourceMetrics = metrics;
    pImpl->currentResourceMetrics.timestamp = std::chrono::system_clock::now();
    
    // Update history for statistics
    pImpl->cpuUsageHistory.push_back(metrics.cpuUsage);
    pImpl->memoryUsageHistory.push_back(metrics.memoryUsage);
    
    // Keep history limited
    if (pImpl->cpuUsageHistory.size() > 100) {
        pImpl->cpuUsageHistory.erase(pImpl->cpuUsageHistory.begin());
        pImpl->memoryUsageHistory.erase(pImpl->memoryUsageHistory.begin());
    }
    
    pImpl->lastUpdateTime = std::chrono::system_clock::now();
}

void EnhancedStatusReporter::startResourceMonitoring(const std::chrono::milliseconds& interval) {
    if (pImpl->resourceMonitoringActive) return;
    
    pImpl->resourceMonitoringActive = true;
    pImpl->resourceMonitorRunning = true;
    
    pImpl->resourceMonitorThread = std::thread([this]() {
        while (pImpl->resourceMonitorRunning) {
            // Simulate resource monitoring - in real implementation this would read actual system metrics
            ResourceMetrics metrics;
            metrics.cpuUsage = 50.0 + (rand() % 30); // Simulated 50-80% CPU usage
            metrics.memoryUsage = 200.0 + (rand() % 300); // Simulated 200-500MB memory
            metrics.memoryAvailable = 1000.0 - metrics.memoryUsage;
            metrics.memoryReserved = 100.0;
            metrics.threadCount = 10 + (rand() % 20);
            metrics.openFileHandles = 50 + (rand() % 30);
            
            updateResourceMetrics(metrics);
            
            std::this_thread::sleep_for(pImpl->updateInterval);
        }
    });
}

void EnhancedStatusReporter::stopResourceMonitoring() {
    if (!pImpl->resourceMonitoringActive) return;
    
    pImpl->resourceMonitorRunning = false;
    if (pImpl->resourceMonitorThread.joinable()) {
        pImpl->resourceMonitorThread.join();
    }
    pImpl->resourceMonitoringActive = false;
}

void EnhancedStatusReporter::updateBrainState(const BrainStateMetrics& metrics) {
    pImpl->currentBrainMetrics = metrics;
    pImpl->currentBrainMetrics.lastUpdate = std::chrono::system_clock::now();
}

void EnhancedStatusReporter::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void EnhancedStatusReporter::addValidationResult(const ValidationResult& result) {
    pImpl->validationResults.push_back(result);
}

void EnhancedStatusReporter::addValidationError(const std::string& section, const std::string& error) {
    ValidationResult result(section, false);
    result.errors.push_back(error);
    addValidationResult(result);
}

void EnhancedStatusReporter::addValidationWarning(const std::string& section, const std::string& warning) {
    ValidationResult result(section, true);
    result.warnings.push_back(warning);
    addValidationResult(result);
}

void EnhancedStatusReporter::clearValidationResults() {
    pImpl->validationResults.clear();
}

void EnhancedStatusReporter::updatePerformanceMetrics(const PerformanceMetrics& metrics) {
    pImpl->currentPerformanceMetrics = metrics;
    pImpl->currentPerformanceMetrics.timestamp = std::chrono::system_clock::now();
}

void EnhancedStatusReporter::addPerformanceMeasurement(const std::string& operation, double duration,
                                                       size_t memoryUsed) {
    pImpl->performanceMeasurements.emplace_back(operation, duration);
    
    // Update performance metrics
    static double totalDuration = 0.0;
    static size_t totalOperations = 0;
    
    totalDuration += duration;
    totalOperations++;
    
    pImpl->currentPerformanceMetrics.throughput = (totalOperations > 0) ? totalOperations / totalDuration : 0.0;
    pImpl->currentPerformanceMetrics.latency = duration; // This should be average
    pImpl->currentPerformanceMetrics.reliability = 1.0; // Placeholder
}

void EnhancedStatusReporter::clearPerformanceMetrics() {
    pImpl->performanceMeasurements.clear();
}

void EnhancedStatusReporter::addMonitoringDataPoint(const MonitoringDataPoint& dataPoint) {
    pImpl->monitoringData.push_back(dataPoint);
}

void EnhancedStatusReporter::addMonitoringDataPoint(const std::string& component, const std::string& metric,
                                                   double value, const std::string& unit,
                                                   const std::map<std::string, std::string>& tags) {
    MonitoringDataPoint dataPoint(component, metric, value, unit);
    dataPoint.tags = tags;
    addMonitoringDataPoint(dataPoint);
}

void EnhancedStatusReporter::clearMonitoringData() {
    pImpl->monitoringData.clear();
}

std::vector<MonitoringDataPoint> EnhancedStatusReporter::getMonitoringData(const std::string& component,
                                                                          const std::string& metric,
                                                                          const std::chrono::system_clock::time_point& since) const {
    std::vector<MonitoringDataPoint> result;
    
    for (const auto& dataPoint : pImpl->monitoringData) {
        if (!component.empty() && dataPoint.component != component) continue;
        if (!metric.empty() && dataPoint.metric != metric) continue;
        if (dataPoint.timestamp >= since) {
            result.push_back(dataPoint);
        }
    }
    
    return result;
}

std::string EnhancedStatusReporter::generateSystemStatusReport() const {
    std::stringstream ss;
    
    ss << "=== NLM System Status Report ===" << std::endl;
    ss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    // Component status
    ss << "--- Component Status ---" << std::endl;
    for (const auto& pair : pImpl->componentStatus) {
        const auto& status = pair.second;
        ss << "Component: " << status.name << std::endl;
        ss << "  Status: " << status.status << std::endl;
        ss << "  Operational: " << (status.isOperational ? "Yes" : "No") << std::endl;
        ss << "  Performance: " << std::fixed << std::setprecision(1) << status.performance * 100 << "%" << std::endl;
        ss << "  Last Update: " << std::chrono::system_clock::to_time_t(status.lastUpdate) << std::endl;
        ss << "  Details: " << status.details << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string EnhancedStatusReporter::generateBrainStateReport() const {
    std::stringstream ss;
    
    ss << "=== Brain State Report ===" << std::endl;
    ss << std::endl;
    
    ss << "Neurons: " << pImpl->currentBrainMetrics.neuronCount << std::endl;
    ss << "Synapses: " << pImpl->currentBrainMetrics.synapseCount << std::endl;
    ss << "Active Neurons: " << pImpl->currentBrainMetrics.firingCount << std::endl;
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
       << pImpl->currentBrainMetrics.averageFiringRate << " Hz" << std::endl;
    ss << "Average Membrane Potential: " << std::fixed << std::setprecision(4)
       << pImpl->currentBrainMetrics.averageMembranePotential << " mV" << std::endl;
    ss << "Average Input Current: " << std::fixed << std::setprecision(4)
       << pImpl->currentBrainMetrics.averageInputCurrent << " nA" << std::endl;
    ss << "Energy Consumption: " << std::fixed << std::setprecision(2)
       << pImpl->currentBrainMetrics.energyConsumption << " arbitrary units" << std::endl;
    ss << "Computational Load: " << std::fixed << std::setprecision(1)
       << pImpl->currentBrainMetrics.computationalLoad * 100 << "%" << std::endl;
    ss << "Last Update: " << std::chrono::system_clock::to_time_t(pImpl->currentBrainMetrics.lastUpdate) << std::endl;
    
    return ss.str();
}

std::string EnhancedStatusReporter::generateResourceReport() const {
    std::stringstream ss;
    
    ss << "=== Resource Usage Report ===" << std::endl;
    ss << std::endl;
    
    ss << "CPU Usage: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.cpuUsage << "%" << std::endl;
    ss << "Memory Usage: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.memoryUsage << " MB" << std::endl;
    ss << "Memory Available: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.memoryAvailable << " MB" << std::endl;
    ss << "Memory Reserved: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.memoryReserved << " MB" << std::endl;
    ss << "Disk Usage: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.diskUsage << " MB" << std::endl;
    ss << "Network Usage: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.networkUsage << " MB/s" << std::endl;
    ss << "Threads: " << pImpl->currentResourceMetrics.threadCount << std::endl;
    ss << "Open File Handles: " << pImpl->currentResourceMetrics.openFileHandles << std::endl;
    ss << "Last Updated: " << std::chrono::system_clock::to_time_t(pImpl->currentResourceMetrics.timestamp) << std::endl;
    
    return ss.str();
}

std::string EnhancedStatusReporter::generateConfigurationValidationReport() const {
    std::stringstream ss;
    
    ss << "=== Configuration Validation Report ===" << std::endl;
    ss << std::endl;
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (const auto& result : pImpl->validationResults) {
        ss << "Section: " << result.section << std::endl;
        ss << "Status: " << (result.isValid ? "VALID" : "INVALID") << std::endl;
        
        if (result.isValid) {
            validCount++;
            if (!result.warnings.empty()) {
                ss << "Warnings:" << std::endl;
                for (const auto& warning : result.warnings) {
                    ss << "  - " << warning << std::endl;
                }
            }
        } else {
            invalidCount++;
            if (!result.errors.empty()) {
                ss << "Errors:" << std::endl;
                for (const auto& error : result.errors) {
                    ss << "  - " << error << std::endl;
                }
            }
        }
        
        ss << "Timestamp: " << std::chrono::system_clock::to_time_t(result.timestamp) << std::endl;
        ss << std::endl;
    }
    
    ss << "Summary:" << std::endl;
    ss << "  Valid Sections: " << validCount << std::endl;
    ss << "  Invalid Sections: " << invalidCount << std::endl;
    ss << "  Total Sections: " << pImpl->validationResults.size() << std::endl;
    
    return ss.str();
}

std::string EnhancedStatusReporter::generatePerformanceReport() const {
    std::stringstream ss;
    
    ss << "=== Performance Report ===" << std::endl;
    ss << std::endl;
    
    ss << "CPU Performance: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.cpuPerformance * 100 << "%" << std::endl;
    ss << "Memory Efficiency: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.memoryEfficiency * 100 << "%" << std::endl;
    ss << "Throughput: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.throughput << " ops/sec" << std::endl;
    ss << "Latency: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.latency << " ms" << std::endl;
    ss << "Reliability: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.reliability * 100 << "%" << std::endl;
    ss << "Last Updated: " << std::chrono::system_clock::to_time_t(pImpl->currentPerformanceMetrics.timestamp) << std::endl;
    
    return ss.str();
}

std::string EnhancedStatusReporter::generateLiveMonitoringReport() const {
    std::stringstream ss;
    
    ss << "=== Live Monitoring Report ===" << std::endl;
    ss << std::endl;
    
    // Group data points by component
    std::unordered_map<std::string, std::vector<MonitoringDataPoint>> dataByComponent;
    for (const auto& dataPoint : pImpl->monitoringData) {
        dataByComponent[dataPoint.component].push_back(dataPoint);
    }
    
    for (const auto& pair : dataByComponent) {
        const auto& component = pair.first;
        const auto& dataPoints = pair.second;
        
        ss << "Component: " << component << std::endl;
        
        // Calculate statistics for each metric
        std::unordered_map<std::string, std::vector<double>> metricValues;
        for (const auto& dataPoint : dataPoints) {
            metricValues[dataPoint.metric].push_back(dataPoint.value);
        }
        
        for (const auto& metricPair : metricValues) {
            const auto& metric = metricPair.first;
            const auto& values = metricPair.second;
            
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double avg = (values.size() > 0) ? sum / values.size() : 0.0;
            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            
            ss << "  " << metric << ": " << std::fixed << std::setprecision(2) << avg 
               << " (min: " << min << ", max: " << max << ")" << std::endl;
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

std::string EnhancedStatusReporter::generateSummaryReport() const {
    std::stringstream ss;
    
    ss << "=== NLM System Summary ===" << std::endl;
    ss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    ss << "System Overview:" << std::endl;
    ss << "  Components: " << pImpl->componentStatus.size() << std::endl;
    ss << "  Validation Results: " << pImpl->validationResults.size() << std::endl;
    ss << "  Performance Measurements: " << pImpl->performanceMeasurements.size() << std::endl;
    ss << "  Monitoring Data Points: " << pImpl->monitoringData.size() << std::endl;
    ss << "  Active Alerts: " << pImpl->alerts.size() << std::endl;
    ss << std::endl;
    
    ss << "Resource Usage:" << std::endl;
    ss << "  CPU: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.cpuUsage << "%" << std::endl;
    ss << "  Memory: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.memoryUsage << " MB" << std::endl;
    ss << std::endl;
    
    ss << "Brain State:" << std::endl;
    ss << "  Neurons: " << pImpl->currentBrainMetrics.neuronCount << std::endl;
    ss << "  Synapses: " << pImpl->currentBrainMetrics.synapseCount << std::endl;
    ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << pImpl->currentBrainMetrics.averageFiringRate << " Hz" << std::endl;
    ss << std::endl;
    
    ss << "Performance:" << std::endl;
    ss << "  Throughput: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.throughput << " ops/sec" << std::endl;
    ss << "  Latency: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.latency << " ms" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

void EnhancedStatusReporter::checkAlerts() {
    // Detect resource alerts
    detectResourceAlerts();
    
    // Detect brain alerts
    detectBrainAlerts();
    
    // Detect performance alerts
    detectPerformanceAlerts();
}

std::vector<EnhancedStatusReporter::Alert> EnhancedStatusReporter::getAlerts() const {
    return pImpl->alerts;
}

void EnhancedStatusReporter::clearAlerts() {
    pImpl->alerts.clear();
}

void EnhancedStatusReporter::setAlertCallback(const std::function<void(const Alert&)>& callback) {
    pImpl->alertCallback = callback;
}

void EnhancedStatusReporter::startDashboard(std::chrono::seconds updateInterval) {
    if (pImpl->dashboardRunning) return;
    
    pImpl->dashboardUpdateInterval = updateInterval;
    pImpl->dashboardRunning = true;
    pImpl->dashboardThreadRunning = true;
    
    pImpl->dashboardThread = std::thread([this]() {
        while (pImpl->dashboardThreadRunning) {
            renderDashboard();
            std::this_thread::sleep_for(pImpl->dashboardUpdateInterval);
        }
    });
}

void EnhancedStatusReporter::stopDashboard() {
    if (!pImpl->dashboardRunning) return;
    
    pImpl->dashboardThreadRunning = false;
    if (pImpl->dashboardThread.joinable()) {
        pImpl->dashboardThread.join();
    }
    pImpl->dashboardRunning = false;
}

bool EnhancedStatusReporter::isDashboardRunning() const {
    return pImpl->dashboardRunning;
}

std::string EnhancedStatusReporter::generateDashboardOutput() const {
    std::stringstream ss;
    
    ss << "=== NLM Status Dashboard ===" << std::endl;
    ss << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    // Component status summary
    ss << "Components (" << pImpl->componentStatus.size() << "):" << std::endl;
    int operationalCount = 0;
    for (const auto& pair : pImpl->componentStatus) {
        if (pair.second.isOperational) {
            operationalCount++;
        }
    }
    ss << "  Operational: " << operationalCount << "/" << pImpl->componentStatus.size() << std::endl;
    ss << "  Status: " << operationalCount * 100 / pImpl->componentStatus.size() << "% healthy" << std::endl;
    ss << std::endl;
    
    // Resource status
    ss << "Resources:" << std::endl;
    ss << "  CPU: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.cpuUsage << "%" << std::endl;
    ss << "  Memory: " << std::fixed << std::setprecision(1) << pImpl->currentResourceMetrics.memoryUsage << "/" 
       << pImpl->currentResourceMetrics.memoryAvailable + pImpl->currentResourceMetrics.memoryReserved << " MB (" 
       << std::fixed << std::setprecision(1) << (pImpl->currentResourceMetrics.memoryUsage / 
               (pImpl->currentResourceMetrics.memoryAvailable + pImpl->currentResourceMetrics.memoryReserved) * 100) << "% used)" << std::endl;
    ss << std::endl;
    
    // Brain status
    ss << "Brain:" << std::endl;
    ss << "  Neurons: " << pImpl->currentBrainMetrics.neuronCount << std::endl;
    ss << "  Synapses: " << pImpl->currentBrainMetrics.synapseCount << std::endl;
    ss << "  Activity: " << pImpl->currentBrainMetrics.firingCount << " firing" << std::endl;
    ss << std::endl;
    
    // Performance status
    ss << "Performance:" << std::endl;
    ss << "  Throughput: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.throughput << " ops/sec" << std::endl;
    ss << "  Latency: " << std::fixed << std::setprecision(2) << pImpl->currentPerformanceMetrics.latency << " ms" << std::endl;
    ss << std::endl;
    
    // Alerts
    if (!pImpl->alerts.empty()) {
        ss << "Alerts (" << pImpl->alerts.size() << "):" << std::endl;
        for (const auto& alert : pImpl->alerts) {
            ss << "  [" << getSeverityColor(alert.severity) << alert.severity << "\033[0m] " << alert.message << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

bool EnhancedStatusReporter::exportStatusReport(const std::string& filepath, const std::string& format,
                                                const std::string& reportType) const {
    std::stringstream ss;
    
    if (reportType == "comprehensive") {
        ss << generateSystemStatusReport() << std::endl;
        ss << generateBrainStateReport() << std::endl;
        ss << generateResourceReport() << std::endl;
        ss << generateConfigurationValidationReport() << std::endl;
        ss << generatePerformanceReport() << std::endl;
        ss << generateLiveMonitoringReport() << std::endl;
    } else if (reportType == "system") {
        ss << generateSystemStatusReport();
    } else if (reportType == "brain") {
        ss << generateBrainStateReport();
    } else if (reportType == "resource") {
        ss << generateResourceReport();
    } else if (reportType == "validation") {
        ss << generateConfigurationValidationReport();
    } else if (reportType == "performance") {
        ss << generatePerformanceReport();
    } else if (reportType == "monitoring") {
        ss << generateLiveMonitoringReport();
    } else {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    file << ss.str();
    
    return true;
}

bool EnhancedStatusReporter::validateConfiguration() const {
    return std::all_of(pImpl->validationResults.begin(), pImpl->validationResults.end(),
                       [](const ValidationResult& result) { return result.isValid; });
}

std::string EnhancedStatusReporter::getConfigurationSummary() const {
    std::stringstream ss;
    
    int totalSections = static_cast<int>(pImpl->validationResults.size());
    int validSections = std::count_if(pImpl->validationResults.begin(), pImpl->validationResults.end(),
                                     [](const ValidationResult& result) { return result.isValid; });
    int invalidSections = totalSections - validSections;
    
    ss << "Configuration Summary:" << std::endl;
    ss << "  Total Sections: " << totalSections << std::endl;
    ss << "  Valid Sections: " << validSections << std::endl;
    ss << "  Invalid Sections: " << invalidSections << std::endl;
    ss << "  Overall Status: " << (invalidSections == 0 ? "VALID" : "INVALID") << std::endl;
    
    return ss.str();
}

double EnhancedStatusReporter::getAverageCPUUsage() const {
    if (pImpl->cpuUsageHistory.empty()) return 0.0;
    
    double sum = std::accumulate(pImpl->cpuUsageHistory.begin(), pImpl->cpuUsageHistory.end(), 0.0);
    return sum / pImpl->cpuUsageHistory.size();
}

double EnhancedStatusReporter::getPeakCPUUsage() const {
    if (pImpl->cpuUsageHistory.empty()) return 0.0;
    
    return *std::max_element(pImpl->cpuUsageHistory.begin(), pImpl->cpuUsageHistory.end());
}

double EnhancedStatusReporter::getAverageMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) return 0.0;
    
    double sum = std::accumulate(pImpl->memoryUsageHistory.begin(), pImpl->memoryUsageHistory.end(), 0.0);
    return sum / pImpl->memoryUsageHistory.size();
}

double EnhancedStatusReporter::getPeakMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) return 0.0;
    
    return *std::max_element(pImpl->memoryUsageHistory.begin(), pImpl->memoryUsageHistory.end());
}

void EnhancedStatusReporter::detectResourceAlerts() {
    // Check CPU usage
    if (pImpl->alertThresholds.count("cpu_usage") > 0) {
        double cpuThreshold = pImpl->alertThresholds["cpu_usage"];
        if (pImpl->currentResourceMetrics.cpuUsage > cpuThreshold) {
            Alert alert;
            alert.component = "Resource";
            alert.metric = "cpu_usage";
            alert.severity = (pImpl->currentResourceMetrics.cpuUsage > cpuThreshold * 1.5) ? "critical" : 
                           (pImpl->currentResourceMetrics.cpuUsage > cpuThreshold * 1.2) ? "warning" : "info";
            alert.message = "CPU usage is " + std::to_string(pImpl->currentResourceMetrics.cpuUsage) + 
                          "% (threshold: " + std::to_string(cpuThreshold) + "%)";
            alert.timestamp = std::chrono::system_clock::now();
            alert.value = pImpl->currentResourceMetrics.cpuUsage;
            alert.threshold = cpuThreshold;
            
            pImpl->alerts.push_back(alert);
            if (pImpl->alertCallback) {
                pImpl->alertCallback(alert);
            }
        }
    }
    
    // Check memory usage
    if (pImpl->alertThresholds.count("memory_usage") > 0) {
        double memoryThreshold = pImpl->alertThresholds["memory_usage"];
        double memoryUsageMB = pImpl->currentResourceMetrics.memoryUsage;
        double memoryThresholdMB = memoryThreshold;
        
        if (memoryUsageMB > memoryThresholdMB) {
            Alert alert;
            alert.component = "Resource";
            alert.metric = "memory_usage";
            alert.severity = (memoryUsageMB > memoryThresholdMB * 1.5) ? "critical" : 
                           (memoryUsageMB > memoryThresholdMB * 1.2) ? "warning" : "info";
            alert.message = "Memory usage is " + std::to_string(memoryUsageMB) + 
                          " MB (threshold: " + std::to_string(memoryThresholdMB) + " MB)";
            alert.timestamp = std::chrono::system_clock::now();
            alert.value = memoryUsageMB;
            alert.threshold = memoryThresholdMB;
            
            pImpl->alerts.push_back(alert);
            if (pImpl->alertCallback) {
                pImpl->alertCallback(alert);
            }
        }
    }
}

void EnhancedStatusReporter::detectBrainAlerts() {
    // Check firing rate alerts
    if (pImpl->currentBrainMetrics.averageFiringRate > 100.0) {
        Alert alert;
        alert.component = "Brain";
        alert.metric = "firing_rate";
        alert.severity = pImpl->currentBrainMetrics.averageFiringRate > 200.0 ? "critical" : "warning";
        alert.message = "Average firing rate is " + std::to_string(pImpl->currentBrainMetrics.averageFiringRate) + 
                      " Hz (may indicate hyperexcitability)";
        alert.timestamp = std::chrono::system_clock::now();
        alert.value = pImpl->currentBrainMetrics.averageFiringRate;
        alert.threshold = 100.0;
        
        pImpl->alerts.push_back(alert);
        if (pImpl->alertCallback) {
            pImpl->alertCallback(alert);
        }
    }
}

void EnhancedStatusReporter::detectPerformanceAlerts() {
    // Check throughput alerts
    if (pImpl->alertThresholds.count("throughput") > 0) {
        double throughputThreshold = pImpl->alertThresholds["throughput"];
        if (pImpl->currentPerformanceMetrics.throughput < throughputThreshold) {
            Alert alert;
            alert.component = "Performance";
            alert.metric = "throughput";
            alert.severity = pImpl->currentPerformanceMetrics.throughput < throughputThreshold * 0.5 ? "critical" : "warning";
            alert.message = "Throughput is " + std::to_string(pImpl->currentPerformanceMetrics.throughput) + 
                          " ops/sec (threshold: " + std::to_string(throughputThreshold) + " ops/sec)";
            alert.timestamp = std::chrono::system_clock::now();
            alert.value = pImpl->currentPerformanceMetrics.throughput;
            alert.threshold = throughputThreshold;
            
            pImpl->alerts.push_back(alert);
            if (pImpl->alertCallback) {
                pImpl->alertCallback(alert);
            }
        }
    }
    
    // Check latency alerts
    if (pImpl->alertThresholds.count("latency") > 0) {
        double latencyThreshold = pImpl->alertThresholds["latency"];
        if (pImpl->currentPerformanceMetrics.latency > latencyThreshold) {
            Alert alert;
            alert.component = "Performance";
            alert.metric = "latency";
            alert.severity = pImpl->currentPerformanceMetrics.latency > latencyThreshold * 2.0 ? "critical" : "warning";
            alert.message = "Latency is " + std::to_string(pImpl->currentPerformanceMetrics.latency) + 
                          " ms (threshold: " + std::to_string(latencyThreshold) + " ms)";
            alert.timestamp = std::chrono::system_clock::now();
            alert.value = pImpl->currentPerformanceMetrics.latency;
            alert.threshold = latencyThreshold;
            
            pImpl->alerts.push_back(alert);
            if (pImpl->alertCallback) {
                pImpl->alertCallback(alert);
            }
        }
    }
}

void EnhancedStatusReporter::renderDashboard() const {
    // Implementation for dashboard rendering
    // For now, just generate and output the dashboard
    std::string dashboardOutput = generateDashboardOutput();
    if (!dashboardOutput.empty()) {
        std::cout << dashboardOutput << std::endl;
    }
}

void EnhancedStatusReporter::renderComponentStatus(std::stringstream& ss, const SystemComponentStatus& status) const {
    ss << "  " << status.name << ": " << status.status << " (" 
       << (status.isOperational ? "operational" : "not operational") << ")";
}

void EnhancedStatusReporter::renderResourceMetrics(std::stringstream& ss, const ResourceMetrics& metrics) const {
    ss << "  CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%" << std::endl;
    ss << "  Memory: " << std::fixed << std::setprecision(1) << metrics.memoryUsage << "/" 
       << metrics.memoryAvailable + metrics.memoryReserved << " MB" << std::endl;
}

void EnhancedStatusReporter::renderBrainState(std::stringstream& ss, const BrainStateMetrics& metrics) const {
    ss << "  Neurons: " << metrics.neuronCount << std::endl;
    ss << "  Synapses: " << metrics.synapseCount << std::endl;
    ss << "  Firing: " << metrics.firingCount << std::endl;
    ss << "  Avg Firing Rate: " << std::fixed << std::setprecision(2) << metrics.averageFiringRate << " Hz" << std::endl;
}

void EnhancedStatusReporter::formatTimestamp(std::stringstream& ss, std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
}

std::string EnhancedStatusReporter::getSeverityColor(const std::string& severity) const {
    if (severity == "critical") {
        return "\033[91m"; // Red
    } else if (severity == "warning") {
        return "\033[93m"; // Yellow
    } else if (severity == "info") {
        return "\033[94m"; // Blue
    } else {
        return "\033[0m"; // Default
    }
}

bool EnhancedStatusReporter::checkThreshold(const std::string& metric, double value, double threshold) const {
    return value > threshold;
}

} // namespace nlm
