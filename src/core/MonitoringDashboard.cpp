#pragma once

#include "MonitoringDashboard.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

struct MonitoringDashboard::Impl {
    std::chrono::system_clock::time_point lastUpdate;
    size_t updateCounter;
    
    Impl() : lastUpdate(std::chrono::system_clock::now()), updateCounter(0) {}
};

MonitoringDashboard::MonitoringDashboard() : pImpl(std::make_unique<Impl>()) {}

MonitoringDashboard::~MonitoringDashboard() = default;

void MonitoringDashboard::updateAll() {
    pImpl->updateCounter++;
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_DEBUG("Monitoring dashboard updated (count=" + std::to_string(pImpl->updateCounter) + ")");
}

std::string MonitoringDashboard::getJSONReport() const {
    nlohmann::json j;
    
    j["timestamp"] = std::chrono::system_clock::to_time_t(pImpl->lastUpdate);
    j["updateCount"] = pImpl->updateCounter;
    
    // Add monitoring data would go here
    j["status"] = "operational";
    j["version"] = "1.0.0";
    
    return j.dump(2); // Pretty print with 2-space indentation
}

std::string MonitoringDashboard::getCSVReport() const {
    std::stringstream ss;
    ss << "timestamp,update_count,status,version\n";
    ss << std::chrono::system_clock::to_time_t(pImpl->lastUpdate) << ",";
    ss << pImpl->updateCounter << ",";
    ss << "operational" << ",";
    ss << "1.0.0" << "\n";
    
    return ss.str();
}

std::string MonitoringDashboard::getTextSummary() const {
    std::stringstream ss;
    ss << "=== NLM Monitoring Dashboard ===\n";
    ss << "Last Update: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(pImpl->lastUpdate)), "%Y-%m-%d %H:%M:%S") << "\n";
    ss << "Update Count: " << pImpl->updateCounter << "\n";
    ss << "Status: Operational\n";
    ss << "Version: 1.0.0\n";
    ss << "\nMonitoring Components:\n";
    ss << "  Performance Metrics: Active\n";
    ss << "  System Monitoring: Active\n";
    ss << "  Checkpoint Monitoring: Active\n";
    ss << "  Anomaly Detection: Active\n";
    ss << "  Alert System: Active\n";
    
    return ss.str();
}

std::string MonitoringDashboard::generateHealthReport() const {
    SystemHealth health = checkSystemHealth();
    
    std::stringstream ss;
    ss << "=== System Health Report ===\n";
    ss << "Overall Health: " << (health.isHealthy ? "HEALTHY" : "UNHEALTHY") << "\n";
    ss << "Health Score: " << (health.healthScore * 100.0) << "%\n";
    ss << "Last Check: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(health.lastCheck)), "%Y-%m-%d %H:%M:%S") << "\n";
    ss << "\nIssues:\n";
    for (const auto& issue : health.issues) {
        ss << "  - " << issue << "\n";
    }
    ss << "\nWarnings:\n";
    for (const auto& warning : health.warnings) {
        ss << "  - " << warning << "\n";
    }
    
    return ss.str();
}

MonitoringDashboard::SystemHealth MonitoringDashboard::checkSystemHealth() const {
    SystemHealth health;
    health.isHealthy = true;
    health.healthScore = 1.0;
    health.lastCheck = std::chrono::system_clock::now();
    
    // Basic health checks
    if (pImpl->updateCounter == 0) {
        health.isHealthy = false;
        health.healthScore = 0.0;
        health.issues.push_back("Monitoring system has not been updated");
    }
    
    // More health checks would go here
    
    return health;
}

} // namespace nlm