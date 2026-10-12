#include "AlertManager.hpp"
#include <iostream>
#include <chrono>

namespace nlm {

struct AlertManager::Impl {
    bool monitoring;
    double updateInterval;
    double lastUpdateTime;
    bool notificationEnabled;
    std::string notificationChannel;
    size_t alertHistorySize;
    std::vector<std::string> activeAlerts;
    std::vector<std::string> recentAlerts;
    std::map<std::string, std::pair<double, AlertType>> alerts;
    
    Impl() : monitoring(false), updateInterval(1.0), lastUpdateTime(0.0),
             notificationEnabled(true), notificationChannel("console"), alertHistorySize(100) {}
};

AlertManager::AlertManager() : pImpl(std::make_unique<Impl>()) {}

bool AlertManager::initialize() {
    pImpl->monitoring = false;
    pImpl->activeAlerts.clear();
    pImpl->recentAlerts.clear();
    pImpl->alerts.clear();
    return true;
}

void AlertManager::startMonitoring() {
    pImpl->monitoring = true;
    pImpl->lastUpdateTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void AlertManager::stopMonitoring() {
    pImpl->monitoring = false;
}

void AlertManager::update() {
    if (!pImpl->monitoring) return;
    
    auto currentTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    pImpl->lastUpdateTime = currentTime;
    
    // Simulate alert updates
    pImpl->activeAlerts.clear();
    for (const auto& alert : pImpl->alerts) {
        pImpl->activeAlerts.push_back(alert.first + " (" + std::to_string(alert.second.first) + ")");
    }
    
    // Add to recent alerts if we have any
    if (!pImpl->activeAlerts.empty()) {
        pImpl->recentAlerts.push_back("Alert at " + std::to_string(static_cast<int>(currentTime)) + ": " + pImpl->activeAlerts.back());
        if (pImpl->recentAlerts.size() > pImpl->alertHistorySize) {
            pImpl->recentAlerts.erase(pImpl->recentAlerts.begin());
        }
    }
}

bool AlertManager::isMonitoring() const {
    return pImpl->monitoring;
}

void AlertManager::addThresholdAlert(const std::string& metric, double threshold, AlertType type) {
    pImpl->alerts[metric] = std::make_pair(threshold, type);
}

void AlertManager::addAnomalyAlert(const std::string& metric, std::function<bool()> detector) {
    // In a real implementation, would use the detector to check for anomalies
    // For now, just add as a threshold alert
    pImpl->alerts[metric] = std::make_pair(0.0, AlertType::Warning);
}

void AlertManager::removeAlert(const std::string& metric) {
    pImpl->alerts.erase(metric);
}

void AlertManager::clearAlerts() {
    pImpl->alerts.clear();
    pImpl->activeAlerts.clear();
}

std::vector<std::string> AlertManager::getActiveAlerts() const {
    return pImpl->activeAlerts;
}

std::vector<std::string> AlertManager::getRecentAlerts() const {
    return pImpl->recentAlerts;
}

double AlertManager::getLastUpdateTime() const {
    return pImpl->lastUpdateTime;
}

void AlertManager::setUpdateInterval(double interval) {
    pImpl->updateInterval = interval;
}

double AlertManager::getUpdateInterval() const {
    return pImpl->updateInterval;
}

void AlertManager::setNotificationEnabled(bool enable) {
    pImpl->notificationEnabled = enable;
}

void AlertManager::setNotificationChannel(const std::string& channel) {
    pImpl->notificationChannel = channel;
}

void AlertManager::setAlertHistorySize(size_t size) {
    pImpl->alertHistorySize = size;
}

} // namespace nlm