#include "AlertSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace nlm {

struct AlertSystem::Impl {
    std::shared_ptr<DebugConfig> config;
    std::atomic<bool> isRunning;
    std::atomic<bool> isInitialized;
    std::vector<AlertCallback> alertCallbacks;
    std::vector<Alert> alerts;
    std::vector<Alert> resolvedAlerts;\n    std::map<std::string, double> alertThresholds;
    std::map<std::string, bool (*)(const PerformanceMetrics&)> alertConditions;
    std::map<std::string, AlertSeverity> alertFilters;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    size_t maxAlerts;
    bool persistAlerts;
    std::string alertLogFile;
    
    Impl() : isRunning(false), isInitialized(false), maxAlerts(1000), persistAlerts(true) {
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
    }
    
    ~Impl() {
        // Persist alerts if enabled
        if (persistAlerts) {
            persistAlertsToFile();
        }
    }
    
    void persistAlertsToFile() {
        if (alertLogFile.empty()) {
            return;
        }
        
        try {
            std::ofstream file(alertLogFile, std::ios::app);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open alert log file: " + alertLogFile);
                return;
            }
            
            for (const auto& alert : alerts) {
                file << formatAlert(alert) << "\n";
            }
            
            file.close();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error persisting alerts to file: " + std::string(e.what()));
        }
    }
    
    std::string formatAlert(const Alert& alert) const {
        std::stringstream ss;
        auto timePoint = alert.timestamp;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
        
        ss << "[" << duration.count() << "ms] ";
        ss << "[" << static_cast<int>(alert.severity) << "] ";
        ss << "[" << alert.id << "] ";
        ss << "[" << alert.source << "] ";
        ss << alert.message;
        
        if (!alert.metadata.empty()) {
            ss << " | Metadata: ";
            bool first = true;
            for (const auto& [key, value] : alert.metadata) {
                if (!first) ss << ", ";
                ss << key << "=" << value;
                first = false;
            }
        }
        
        ss << " | Active: " << (alert.isActive ? "Yes" : "No");
        
        return ss.str();
    }
    
    AlertSeverity getSeverityFromString(const std::string& severityStr) const {
        if (severityStr == "debug") return AlertSeverity::Debug;
        if (severityStr == "info") return AlertSeverity::Info;
        if (severityStr == "warning") return AlertSeverity::Warning;
        if (severityStr == "error") return AlertSeverity::Error;
        if (severityStr == "critical") return AlertSeverity::Critical;
        return AlertSeverity::Info;
    }
    
    std::string getSeverityString(AlertSeverity severity) const {
        switch (severity) {
            case AlertSeverity::Debug: return "debug";
            case AlertSeverity::Info: return "info";
            case AlertSeverity::Warning: return "warning";
            case AlertSeverity::Error: return "error";
            case AlertSeverity::Critical: return "critical";
            default: return "info";
        }
    }
};

AlertSystem::AlertSystem() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("AlertSystem created");
}

AlertSystem::~AlertSystem() {
    NLM_LOG_DEBUG("AlertSystem destroyed");
}

AlertSystem::AlertSystem(AlertSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

AlertSystem& AlertSystem::operator=(AlertSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void AlertSystem::initialize(std::shared_ptr<DebugConfig> config) {
    pImpl->config = config;
    pImpl->isInitialized = true;
    
    // Set default configuration
    if (config) {
        pImpl->alertSystemEnabled = config->isAlertSystemEnabled();
        pImpl->maxAlerts = config->isDebugSessionManagementEnabled() ? 10000 : 1000;
        pImpl->persistAlerts = config->isSessionRecordingEnabled();
        
        // Set default alert thresholds
        pImpl->setAlertThreshold("cpu_usage", config->getAlertThreshold("cpu_usage"));
        pImpl->setAlertThreshold("memory_usage", config->getAlertThreshold("memory_usage"));
        pImpl->setAlertThreshold("simulation_time", config->getAlertThreshold("simulation_time"));
    }
    
    NLM_LOG_INFO("AlertSystem initialized");
}

void AlertSystem::start() {
    if (!pImpl->isInitialized) {
        NLM_LOG_WARNING("AlertSystem not initialized, skipping start");
        return;
    }
    
    pImpl->isRunning = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("AlertSystem started");
}

void AlertSystem::stop() {
    pImpl->isRunning = false;
    NLM_LOG_INFO("AlertSystem stopped");
}

void AlertSystem::update() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    
    // Check alert conditions and create alerts if needed
    for (const auto& [metric, threshold] : pImpl->alertThresholds) {
        double currentValue = getCurrentMetricValue(metric);
        if (currentValue > threshold) {
            createThresholdAlert(metric, currentValue, threshold);
        }
    }
    
    // Check custom conditions
    for (const auto& [conditionName, conditionFunc] : pImpl->alertConditions) {
        PerformanceMetrics metrics = getCurrentPerformanceMetrics();
        if (conditionFunc(metrics)) {
            createConditionAlert(conditionName, metrics);
        }
    }
}

void AlertSystem::addAlertCallback(const AlertCallback& callback) {
    pImpl->alertCallbacks.push_back(callback);
    NLM_LOG_DEBUG("Alert callback added");
}

void AlertSystem::removeAlertCallback(const AlertCallback& callback) {
    auto it = std::find(pImpl->alertCallbacks.begin(), pImpl->alertCallbacks.end(), callback);
    if (it != pImpl->alertCallbacks.end()) {
        pImpl->alertCallbacks.erase(it);
        NLM_LOG_DEBUG("Alert callback removed");
    }
}

void AlertSystem::createAlert(const Alert& alert) {
    if (!pImpl->isAlertSystemEnabled()) {
        return;
    }
    
    // Check if alert already exists
    for (const auto& existingAlert : pImpl->alerts) {
        if (existingAlert.id == alert.id && existingAlert.isActive) {
            NLM_LOG_DEBUG("Alert already active: " + alert.id);
            return;
        }
    }
    
    // Add alert
    Alert newAlert = alert;
    newAlert.timestamp = std::chrono::steady_clock::now();
    newAlert.isActive = true;
    
    pImpl->alerts.push_back(newAlert);
    
    // Notify callbacks
    for (const auto& callback : pImpl->alertCallbacks) {
        try {
            callback(newAlert);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in alert callback: " + std::string(e.what()));
        }
    }
    
    // Log alert
    std::string severityStr;
    switch (newAlert.severity) {
        case AlertSeverity::Debug: severityStr = "DEBUG"; break;
        case AlertSeverity::Info: severityStr = "INFO"; break;
        case AlertSeverity::Warning: severityStr = "WARNING"; break;
        case AlertSeverity::Error: severityStr = "ERROR"; break;
        case AlertSeverity::Critical: severityStr = "CRITICAL"; break;
    }
    
    NLM_LOG_INFO("ALERT: " + severityStr + " - " + newAlert.id + ": " + newAlert.message);
    
    // Check if alert should be auto-resolved
    if (newAlert.severity <= AlertSeverity::Warning) {
        // Low severity alerts auto-resolve after 5 minutes
        std::thread([this, alertId = newAlert.id]() {
            std::this_thread::sleep_for(std::chrono::minutes(5));
            resolveAlert(alertId);
        }).detach();
    }
}

void AlertSystem::resolveAlert(const std::string& alertId) {
    auto it = std::find_if(pImpl->alerts.begin(), pImpl->alerts.end(),
                          [&alertId](const Alert& alert) { return alert.id == alertId && alert.isActive; });
    
    if (it != pImpl->alerts.end()) {
        it->isActive = false;
        pImpl->resolvedAlerts.push_back(*it);
        pImpl->alerts.erase(it);
        
        NLM_LOG_INFO("Alert resolved: " + alertId);
        
        // Notify callbacks
        Alert resolvedAlert = *it;
        for (const auto& callback : pImpl->alertCallbacks) {
            try {
                callback(resolvedAlert);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in alert callback: " + std::string(e.what()));
            }
        }
    }
}

void AlertSystem::clearAlerts() {
    pImpl->alerts.clear();
    NLM_LOG_INFO("All alerts cleared");
}

std::vector<Alert> AlertSystem::getActiveAlerts() const {
    std::vector<Alert> activeAlerts;
    for (const auto& alert : pImpl->alerts) {
        if (alert.isActive) {
            activeAlerts.push_back(alert);
        }
    }
    return activeAlerts;
}

std::vector<Alert> AlertSystem::getAllAlerts() const {
    std::vector<Alert> allAlerts = pImpl->alerts;
    allAlerts.insert(allAlerts.end(), pImpl->resolvedAlerts.begin(), pImpl->resolvedAlerts.end());
    return allAlerts;
}

size_t AlertSystem::getAlertCount() const {
    return pImpl->alerts.size() + pImpl->resolvedAlerts.size();
}

size_t AlertSystem::getActiveAlertCount() const {
    return pImpl->alerts.size();
}

void AlertSystem::setAlertThreshold(const std::string& metric, double threshold) {
    pImpl->alertThresholds[metric] = threshold;
    NLM_LOG_INFO("Alert threshold set for " + metric + ": " + std::to_string(threshold));
}

void AlertSystem::setAlertCondition(const std::string& condition, bool (*conditionFunc)(const PerformanceMetrics&)) {
    pImpl->alertConditions[condition] = conditionFunc;
    NLM_LOG_INFO("Alert condition set: " + condition);
}

void AlertSystem::addAlertFilter(const std::string& metric, AlertSeverity minSeverity) {
    pImpl->alertFilters[metric] = minSeverity;
    NLM_LOG_INFO("Alert filter added for " + metric + " with min severity: " + std::to_string(static_cast<int>(minSeverity)));
}

void AlertSystem::removeAlertFilter(const std::string& metric) {
    auto it = pImpl->alertFilters.find(metric);
    if (it != pImpl->alertFilters.end()) {
        pImpl->alertFilters.erase(it);
        NLM_LOG_INFO("Alert filter removed for " + metric);
    }
}

// Helper methods
PerformanceMetrics AlertSystem::getCurrentPerformanceMetrics() const {
    PerformanceMetrics metrics;
    metrics.timestamp = std::chrono::steady_clock::now();
    
    // In a real implementation, this would collect actual metrics from the system
    // For now, return default values
    metrics.cpuUsage = 50.0;
    metrics.memoryUsage = 0.5;
    metrics.allocatedMemory = 1024 * 1024 * 100; // 100MB
    metrics.deallocatedMemory = 1024 * 1024 * 50;  // 50MB
    metrics.activeThreads = 4;
    metrics.simulationTime = 1000.0;
    metrics.totalSpikeCount = 5000;
    metrics.averageFiringRate = 10.0;
    
    return metrics;
}

double AlertSystem::getCurrentMetricValue(const std::string& metric) const {
    // In a real implementation, this would get the current value of the metric
    // For now, return a simulated value
    if (metric == "cpu_usage") {
        return 50.0 + (std::rand() % 30); // 50-80%
    } else if (metric == "memory_usage") {
        return 0.5 + (std::rand() % 30) / 100.0; // 0.5-0.8
    } else if (metric == "simulation_time") {
        return 1000.0 + (std::rand() % 1000); // 1000-2000
    }
    return 0.0;
}

void AlertSystem::createThresholdAlert(const std::string& metric, double currentValue, double threshold) {
    Alert alert;
    alert.id = "threshold_" + metric + "_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    alert.severity = currentValue > threshold * 2.0 ? AlertSeverity::Critical :
                     currentValue > threshold * 1.5 ? AlertSeverity::Error : AlertSeverity::Warning;
    alert.message = metric + " usage exceeded threshold: " + std::to_string(currentValue) + 
                    " > " + std::to_string(threshold);
    alert.source = "AlertSystem";
    alert.metadata["metric"] = metric;
    alert.metadata["current_value"] = std::to_string(currentValue);
    alert.metadata["threshold"] = std::to_string(threshold);
    
    createAlert(alert);
}

void AlertSystem::createConditionAlert(const std::string& conditionName, const PerformanceMetrics& metrics) {
    Alert alert;
    alert.id = "condition_" + conditionName + "_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    alert.severity = AlertSeverity::Warning;
    alert.message = "Alert condition triggered: " + conditionName;
    alert.source = "AlertSystem";
    alert.metadata["condition"] = conditionName;
    
    createAlert(alert);
}

} // namespace nlm
