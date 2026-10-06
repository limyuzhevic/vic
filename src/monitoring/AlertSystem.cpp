#include "AlertSystem.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace nlm {

AlertSystem::AlertSystem(const AlertConfig& config) : config_(config) {}

bool AlertSystem::addRecipient(const AlertRecipient& recipient) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (recipients_.find(recipient.id) != recipients_.end()) {
        return false; // Recipient already exists
    }
    
    recipients_[recipient.id] = recipient;
    return true;
}

bool AlertSystem::removeRecipient(const std::string& recipientId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    return recipients_.erase(recipientId) > 0;
}

AlertRecipient* AlertSystem::getRecipient(const std::string& recipientId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = recipients_.find(recipientId);
    if (it == recipients_.end()) {
        return nullptr;
    }
    
    return &it->second;
}

bool AlertSystem::hasRecipient(const std::string& recipientId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return recipients_.find(recipientId) != recipients_.end();
}

bool AlertSystem::addAlertHandler(const std::string& name, AlertHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (alertHandlers_.find(name) != alertHandlers_.end()) {
        return false; // Handler already exists
    }
    
    alertHandlers_[name] = std::move(handler);
    return true;
}

bool AlertSystem::removeAlertHandler(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    return alertHandlers_.erase(name) > 0;
}

bool AlertSystem::processAlert(const AlertData& alert) {
    if (!isEnabled()) {
        return false;
    }
    
    // Check cooldown
    if (!checkCooldown(alert)) {
        return false; // Still in cooldown period
    }
    
    // Check concurrent alert limit
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (alerts_.size() >= config_.maxConcurrentAlerts) {
        // Remove oldest alerts to make room
        size_t removeCount = alerts_.size() - config_.maxConcurrentAlerts + 1;
        alerts_.erase(alerts_.begin(), alerts_.begin() + removeCount);
    }
    
    // Add to alert list
    alerts_.push_back(alert);
    
    // Update statistics
    stats_.totalAlerts++;
    stats_.lastAlertTime = alert.timestamp;
    
    switch (alert.level) {
        case AlertLevel::INFO: stats_.alertsByLevel[0]++; break;
        case AlertLevel::WARNING: stats_.alertsByLevel[1]++; break;
        case AlertLevel::ERROR: stats_.alertsByLevel[2]++; break;
        case AlertLevel::CRITICAL: stats_.alertsByLevel[3]++; break;
    }
    
    switch (alert.type) {
        case AlertType::PERFORMANCE_DEGRADATION: stats_.alertsByType[0]++; break;
        case AlertType::MEMORY_EXHAUSTION: stats_.alertsByType[1]++; break;
        case AlertType::CPU_OVERLOAD: stats_.alertsByType[2]++; break;
        case AlertType::SPIKE_ANOMALY: stats_.alertsByType[3]++; break;
        case AlertType::MEMORY_ANOMALY: stats_.alertsByType[4]++; break;
        case AlertType::NEURAL_ACTIVITY_CHANGE: stats_.alertsByType[5]++; break;
        case AlertType::CUSTOM: break;
    }
    
    // Send to all recipients that should receive this alert
    for (const auto& pair : recipients_) {
        const auto& recipient = pair.second;
        
        if (!recipient.isEnabled()) {
            continue; // Skip disabled recipients
        }
        
        if (static_cast<int>(alert.level) < static_cast<int>(recipient.minLevel)) {
            continue; // Alert level too low for this recipient
        }
        
        sendToRecipient(recipient, alert);
    }
    
    // Call alert handlers
    for (const auto& pair : alertHandlers_) {
        pair.second(alert);
    }
    
    return true;
}

std::vector<AlertData> AlertSystem::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AlertData> result;
    result.reserve(std::min(count, alerts_.size()));
    
    if (count >= alerts_.size()) {
        result = alerts_;
    } else {
        auto start = alerts_.rbegin();
        std::advance(start, alerts_.size() - count);
        result.assign(start, alerts_.rend());
    }
    
    return result;
}

std::vector<AlertData> AlertSystem::getAlertsByLevel(AlertLevel level, size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AlertData> result;
    
    for (const auto& alert : alerts_) {
        if (alert.level == level) {
            result.push_back(alert);
        }
        
        if (result.size() >= count) {
            break;
        }
    }
    
    return result;
}

std::vector<AlertData> AlertSystem::getAlertsByType(AlertType type, size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AlertData> result;
    
    for (const auto& alert : alerts_) {
        if (alert.type == type) {
            result.push_back(alert);
        }
        
        if (result.size() >= count) {
            break;
        }
    }
    
    return result;
}

void AlertSystem::cleanupOldAlerts() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::now();
    auto cutoffTime = now - config_.cooldownPeriod;
    
    auto newEnd = std::remove_if(alerts_.begin(), alerts_.end(),
                                 [cutoffTime](const AlertData& alert) {
                                     return alert.timestamp < cutoffTime;
                                 });
    alerts_.erase(newEnd, alerts_.end());
}

AlertSystem::AlertStats AlertSystem::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

bool AlertSystem::sendToRecipient(const AlertRecipient& recipient, const AlertData& alert) {
    switch (recipient.type) {
        case RecipientType::CONSOLE:
            return sendToConsole(alert);
        case RecipientType::LOG_FILE:
            return sendToFile(alert);
        case RecipientType::EMAIL:
            // TODO: Implement email sending
            return false;
        case RecipientType::WEBHOOK:
            // TODO: Implement webhook sending
            return false;
        case RecipientType::DASHBOARD:
            // TODO: Implement dashboard sending
            return false;
        case RecipientType::CUSTOM:
            // TODO: Implement custom recipient sending
            return false;
        default:
            return false;
    }
}

bool AlertSystem::sendToConsole(const AlertData& alert) {
    if (!config_.enableConsoleAlerts) {
        return false;
    }
    
    std::string levelStr;
    switch (alert.level) {
        case AlertLevel::INFO: levelStr = "INFO"; break;
        case AlertLevel::WARNING: levelStr = "WARNING"; break;
        case AlertLevel::ERROR: levelStr = "ERROR"; break;
        case AlertLevel::CRITICAL: levelStr = "CRITICAL"; break;
    }
    
    std::string typeStr;
    switch (alert.type) {
        case AlertType::PERFORMANCE_DEGRADATION: typeStr = "PERFORMANCE_DEGRADATION"; break;
        case AlertType::MEMORY_EXHAUSTION: typeStr = "MEMORY_EXHAUSTION"; break;
        case AlertType::CPU_OVERLOAD: typeStr = "CPU_OVERLOAD"; break;
        case AlertType::SPIKE_ANOMALY: typeStr = "SPIKE_ANOMALY"; break;
        case AlertType::MEMORY_ANOMALY: typeStr = "MEMORY_ANOMALY"; break;
        case AlertType::NEURAL_ACTIVITY_CHANGE: typeStr = "NEURAL_ACTIVITY_CHANGE"; break;
        case AlertType::CUSTOM: typeStr = "CUSTOM"; break;
    }
    
    std::cout << "[" << levelStr << "] " << typeStr << ": " << alert.message
              << " (metric: " << alert.metricName << " = " << alert.metricValue << ")" << std::endl;
    
    return true;
}

bool AlertSystem::sendToFile(const AlertData& alert) {
    if (!config_.enableFileAlerts) {
        return false;
    }
    
    try {
        std::ofstream file(config_.logFilePath, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        file << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
             << " [" << static_cast<int>(alert.level) << "] " << alert.message
             << " (metric: " << alert.metricName << " = " << alert.metricValue << ")" << std::endl;
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AlertSystem::checkCooldown(const AlertData& alert) {
    // Check if same alert type and level is in cooldown
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string cooldownKey = std::to_string(static_cast<int>(alert.type)) + "_" + 
                              std::to_string(static_cast<int>(alert.level));
    
    auto it = cooldowns_.find(cooldownKey);
    if (it != cooldowns_.end()) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        
        if (elapsed < config_.cooldownPeriod.count()) {
            return false; // Still in cooldown
        }
    }
    
    // Update cooldown
    cooldowns_[cooldownKey] = std::chrono::system_clock::now();
    return true;
}

std::string AlertSystem::generateAlertId() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "alert_" << time_t_now << "_" << rand();
    
    return ss.str();
}

} // namespace nlm
