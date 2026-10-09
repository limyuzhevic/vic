#include "PerformanceAlertSystem.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace profiling {

// PerformanceAlertSystem implementation

void PerformanceAlertSystem::addAlertRule(const std::string& name, AlertType type, double threshold,
                                         const std::string& message, const std::string& severity) {
    AlertRule rule;
    rule.name = name;
    rule.type = type;
    rule.threshold = threshold;
    rule.message = message;
    rule.severity = severity;
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->alertRules.push_back(rule);
}

void PerformanceAlertSystem::removeAlertRule(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->alertRules.erase(
        std::remove_if(pImpl_->alertRules.begin(), pImpl_->alertRules.end(),
                      [&name](const AlertRule& rule) { return rule.name == name; }),
        pImpl_->alertRules.end()
    );
}

void PerformanceAlertSystem::clearAlertRules() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->alertRules.clear();
}

void PerformanceAlertSystem::evaluateAlerts(const PerformanceProfiler::PerformanceSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    // Clear previous active alerts
    for (auto& alert : alerts_) {
        alert.isActive = false;
    }
    
    // Evaluate alert rules based on snapshot data
    for (const auto& rule : pImpl_->alertRules) {
        bool alertTriggered = false;
        
        switch (rule.type) {
            case AlertType::HighCPUUsage:
                alertTriggered = (snapshot.cpuUsage > rule.threshold);
                break;
            case AlertType::LowFPS:
                // Calculate FPS from snapshot
                double fps = (snapshot.step > 0) ? (snapshot.step / snapshot.timestamp) : 0.0;
                alertTriggered = (fps < rule.threshold);
                break;
            case AlertType::PerformanceRegression:
                // Compare with baseline
                alertTriggered = false; // Simplified
                break;
            case AlertType::ResourceExhaustion:
                // Check memory usage
                alertTriggered = (snapshot.memoryUsage > rule.threshold);
                break;
            default:
                alertTriggered = false;
                break;
        }
        
        if (alertTriggered) {
            Alert alert;
            alert.id = "alert_" + std::to_string(alerts_.size());
            alert.type = rule.type;
            alert.message = rule.message;
            alert.severity = std::stod(rule.severity);
            alert.timestamp = snapshot.timestamp;
            alert.step = snapshot.step;
            alert.isActive = true;
            
            alerts_.push_back(alert);
            
            // Trigger notification if callback is set
            if (pImpl_->notificationCallback) {
                pImpl_->notificationCallback(alert);
            }
        }
    }
}

void PerformanceAlertSystem::addAlert(const Alert& alert) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    alerts_.push_back(alert);
    
    if (pImpl_->notificationCallback) {
        pImpl_->notificationCallback(alert);
    }
}

void PerformanceAlertSystem::acknowledgeAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    auto it = std::find_if(alerts_.begin(), alerts_.end(),
                          [&alertId](const Alert& alert) { return alert.id == alertId; });
    
    if (it != alerts_.end()) {
        it->isActive = false;
    }
}

void PerformanceAlertSystem::resolveAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    auto it = std::remove_if(alerts_.begin(), alerts_.end(),
                           [&alertId](const Alert& alert) { return alert.id == alertId; });
    
    if (it != alerts_.end()) {
        alerts_.erase(it, alerts_.end());
    }
}

const std::vector<PerformanceAlertSystem::Alert>& PerformanceAlertSystem::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    std::vector<Alert> activeAlerts;
    for (const auto& alert : alerts_) {
        if (alert.isActive) {
            activeAlerts.push_back(alert);
        }
    }
    
    return activeAlerts;
}

const std::vector<PerformanceAlertSystem::Alert>& PerformanceAlertSystem::getAllAlerts() const {
    return alerts_;
}

size_t PerformanceAlertSystem::getActiveAlertCount() const {
    return getActiveAlerts().size();
}

void PerformanceAlertSystem::setNotificationCallback(std::function<void(const Alert&)>
                                                    callback) {
    pImpl_->notificationCallback = std::move(callback);
}

void PerformanceAlertSystem::setAlertLevel(AlertType type, const std::string& level) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->alertLevels[type] = level;
}

std::string PerformanceAlertSystem::getAlertLevel(AlertType type) const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    auto it = pImpl_->alertLevels.find(type);
    if (it != pImpl_->alertLevels.end()) {
        return it->second;
    }
    
    return "warning";
}

bool PerformanceAlertSystem::exportAlertReport(const std::string& filepath) const {
    // TODO: Implement alert report export
    return false;
}

void PerformanceAlertSystem::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

} // namespace profiling
} // namespace nlm
