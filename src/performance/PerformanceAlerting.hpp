#pragma once

/**
 * Performance Alerting System
 * 
 * Performance alerting and notification system for monitoring thresholds
 */

#include <string>
#include <chrono>
#include <functional>
#include <map>
#include <unordered_map>
#include <memory>
#include <vector>
#include "PerformanceMonitoring.hpp"

namespace nlm {

/**
 * Alert condition configuration
 */
struct AlertCondition {
    enum class ConditionType {
        CPU_USAGE_ABOVE,
        MEMORY_USAGE_ABOVE,
        SPIKE_RATE_ABOVE,
        SIMULATION_SLOW,
        THREAD_COUNT_ABOVE
    };
    
    ConditionType type;
    double threshold;
    std::string severity; // "WARNING", "ERROR", "CRITICAL"
    std::string message;
    uint32_t consecutive_failures = 1;
    uint32_t current_failures = 0;
};

/**
 * Performance alerting system
 */
class PerformanceAlerting {
private:
    std::map<std::string, AlertCondition> conditions;
    std::vector<PerformanceAlert> active_alerts;
    mutable std::mutex conditions_mutex;
    mutable std::mutex alerts_mutex;
    
    std::function<void(const PerformanceAlert&)> alert_callback;

public:
    PerformanceAlerting();
    ~PerformanceAlerting();
    
    // Condition management
    void addCondition(const std::string& name, const AlertCondition& condition);
    void removeCondition(const std::string& name);
    void updateCondition(const std::string& name, const AlertCondition& condition);
    
    // Alert evaluation
    void evaluate(const PerformanceMetrics& metrics, uint64_t timestamp_ns);
    std::vector<PerformanceAlert> getActiveAlerts() const;
    void clearAlerts();
    
    // Callback system
    void setAlertCallback(std::function<void(const PerformanceAlert&)> callback);
    
    // Utility
    void exportConditions(const std::string& filename) const;
    void importConditions(const std::string& filename);
};

} // namespace nlm