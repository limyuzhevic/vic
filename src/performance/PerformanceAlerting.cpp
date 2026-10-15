#include "PerformanceAlerting.hpp"
#include <fstream>
#include <algorithm>

namespace nlm {

PerformanceAlerting::PerformanceAlerting() {
}

PerformanceAlerting::~PerformanceAlerting() {
}

void PerformanceAlerting::addCondition(const std::string& name, const AlertCondition& condition) {
    std::lock_guard<std::mutex> lock(conditions_mutex);
    conditions[name] = condition;
}

void PerformanceAlerting::removeCondition(const std::string& name) {
    std::lock_guard<std::mutex> lock(conditions_mutex);
    conditions.erase(name);
}

void PerformanceAlerting::updateCondition(const std::string& name, const AlertCondition& condition) {
    std::lock_guard<std::mutex> lock(conditions_mutex);
    conditions[name] = condition;
}

void PerformanceAlerting::evaluate(const PerformanceMetrics& metrics, uint64_t timestamp_ns) {
    std::lock_guard<std::mutex> lock1(conditions_mutex);
    std::lock_guard<std::mutex> lock2(alerts_mutex);
    
    for (const auto& pair : conditions) {
        const auto& name = pair.first;
        const auto& condition = pair.second;
        
        bool triggered = false;
        
        switch (condition.type) {
            case AlertCondition::ConditionType::CPU_USAGE_ABOVE:
                triggered = metrics.cpu_usage_percent > condition.threshold;
                break;
            case AlertCondition::ConditionType::MEMORY_USAGE_ABOVE:
                triggered = metrics.memory_usage_mb > condition.threshold;
                break;
            case AlertCondition::ConditionType::SPIKE_RATE_ABOVE:
                triggered = metrics.average_firing_rate > condition.threshold;
                break;
            case AlertCondition::ConditionType::SIMULATION_SLOW:
                triggered = metrics.simulation_time_ms > condition.threshold;
                break;
            case AlertCondition::ConditionType::THREAD_COUNT_ABOVE:
                triggered = static_cast<double>(metrics.thread_count) > condition.threshold;
                break;
        }
        
        if (triggered) {
            condition.current_failures++;
            if (condition.current_failures >= condition.consecutive_failures) {
                PerformanceAlert alert;
                alert.type = static_cast<PerformanceAlert::AlertType>(condition.type);
                alert.condition = name;
                alert.threshold_value = condition.threshold;
                alert.current_value = (condition.type == AlertCondition::ConditionType::CPU_USAGE_ABOVE) ? metrics.cpu_usage_percent :
                                       (condition.type == AlertCondition::ConditionType::MEMORY_USAGE_ABOVE) ? metrics.memory_usage_mb :
                                       (condition.type == AlertCondition::ConditionType::SPIKE_RATE_ABOVE) ? metrics.average_firing_rate :
                                       (condition.type == AlertCondition::ConditionType::SIMULATION_SLOW) ? metrics.simulation_time_ms :
                                       static_cast<double>(metrics.thread_count);
                alert.severity = condition.severity;
                alert.message = condition.message;
                alert.timestamp_ns = timestamp_ns;
                alerts.push_back(alert);
                
                if (alert_callback) {
                    alert_callback(alert);
                }
            }
        } else {
            condition.current_failures = 0;
        }
    }
}

std::vector<PerformanceAlert> PerformanceAlerting::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(alerts_mutex);
    return alerts;
}

void PerformanceAlerting::clearAlerts() {
    std::lock_guard<std::mutex> lock(alerts_mutex);
    alerts.clear();
}

void PerformanceAlerting::setAlertCallback(std::function<void(const PerformanceAlert&)> callback) {
    std::lock_guard<std::mutex> lock(conditions_mutex);
    alert_callback = callback;
}

void PerformanceAlerting::exportConditions(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "Performance Alert Conditions" << std::endl;
    file << "===========================" << std::endl << std::endl;
    
    for (const auto& pair : conditions) {
        const auto& name = pair.first;
        const auto& condition = pair.second;
        
        file << "Condition: " << name << std::endl;
        file << "  Type: ";
        switch (condition.type) {
            case AlertCondition::ConditionType::CPU_USAGE_ABOVE: file << "CPU_USAGE_ABOVE"; break;
            case AlertCondition::ConditionType::MEMORY_USAGE_ABOVE: file << "MEMORY_USAGE_ABOVE"; break;
            case AlertCondition::ConditionType::SPIKE_RATE_ABOVE: file << "SPIKE_RATE_ABOVE"; break;
            case AlertCondition::ConditionType::SIMULATION_SLOW: file << "SIMULATION_SLOW"; break;
            case AlertCondition::ConditionType::THREAD_COUNT_ABOVE: file << "THREAD_COUNT_ABOVE"; break;
        }
        file << std::endl;
        file << "  Threshold: " << condition.threshold << std::endl;
        file << "  Severity: " << condition.severity << std::endl;
        file << "  Message: " << condition.message << std::endl;
        file << "  Consecutive failures required: " << condition.consecutive_failures << std::endl;
        file << std::endl;
    }
    
    file.close();
}

void PerformanceAlerting::importConditions(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    conditions.clear();
    
    std::string line;
    std::string current_name;
    AlertCondition condition;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line.find("Condition: ") == 0) {
            if (!current_name.empty()) {
                conditions[current_name] = condition;
            }
            current_name = line.substr(11);
            condition = AlertCondition();
        } else if (line.find("  Type: ") == 0) {
            std::string type_str = line.substr(8);
            if (type_str == "CPU_USAGE_ABOVE") condition.type = AlertCondition::ConditionType::CPU_USAGE_ABOVE;
            else if (type_str == "MEMORY_USAGE_ABOVE") condition.type = AlertCondition::ConditionType::MEMORY_USAGE_ABOVE;
            else if (type_str == "SPIKE_RATE_ABOVE") condition.type = AlertCondition::ConditionType::SPIKE_RATE_ABOVE;
            else if (type_str == "SIMULATION_SLOW") condition.type = AlertCondition::ConditionType::SIMULATION_SLOW;
            else if (type_str == "THREAD_COUNT_ABOVE") condition.type = AlertCondition::ConditionType::THREAD_COUNT_ABOVE;
        } else if (line.find("  Threshold: ") == 0) {
            condition.threshold = std::stod(line.substr(13));
        } else if (line.find("  Severity: ") == 0) {
            condition.severity = line.substr(12);
        } else if (line.find("  Message: ") == 0) {
            condition.message = line.substr(11);
        } else if (line.find("  Consecutive failures required: ") == 0) {
            condition.consecutive_failures = std::stoi(line.substr(33));
        }
    }
    
    if (!current_name.empty()) {
        conditions[current_name] = condition;
    }
    
    file.close();
}

} // namespace nlm