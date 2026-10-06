#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

// Alert levels
enum class AlertLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Alert types
enum class AlertType {
    PERFORMANCE_DEGRADATION,
    MEMORY_EXHAUSTION,
    CPU_OVERLOAD,
    SPIKE_ANOMALY,
    MEMORY_ANOMALY,
    NEURAL_ACTIVITY_CHANGE,
    CUSTOM
};

// Alert recipient types
enum class RecipientType {
    CONSOLE,
    EMAIL,
    WEBHOOK,
    LOG_FILE,
    DASHBOARD,
    CUSTOM
};

// Alert recipient configuration
struct AlertRecipient {
    std::string id;
    RecipientType type;
    std::string address;  // email, webhook URL, file path, etc.
    AlertLevel minLevel;  // Only alert at or above this level
    bool isEnabled;
    
    AlertRecipient(const std::string& id, RecipientType type, const std::string& address,
                   AlertLevel minLevel = AlertLevel::WARNING)
        : id(id), type(type), address(address), minLevel(minLevel), isEnabled(true) {}
};

// Alert configuration
struct AlertConfig {
    bool enableAlerts;                    // Enable alert system
    double thresholdMultiplier;           // Multiplier for statistical thresholds
    std::chrono::seconds cooldownPeriod;  // Cooldown period between similar alerts
    size_t maxConcurrentAlerts;           // Maximum concurrent alerts
    bool enableEmailAlerts;               // Enable email alerts
    bool enableConsoleAlerts;             // Enable console alerts
    bool enableFileAlerts;                // Enable file logging alerts
    bool enableDashboardAlerts;           // Enable dashboard alerts
    std::string emailServer;              // Email server settings
    std::string emailFrom;                // Email from address
    std::string logFilePath;              // Path for alert log file
    
    AlertConfig()
        : enableAlerts(true), thresholdMultiplier(3.0), cooldownPeriod(60),
          maxConcurrentAlerts(10), enableEmailAlerts(false),
          enableConsoleAlerts(true), enableFileAlerts(true),
          enableDashboardAlerts(true) {}
};

// Alert data
struct AlertData {
    std::string id;
    AlertType type;
    AlertLevel level;
    std::string message;
    std::string metricName;
    double metricValue;
    double thresholdValue;
    double deviation;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    AlertData(const std::string& id, AlertType type, AlertLevel level,
              const std::string& message, const std::string& metricName,
              double metricValue, double thresholdValue = 0.0)
        : id(id), type(type), level(level), message(message),
          metricName(metricName), metricValue(metricValue),
          thresholdValue(thresholdValue), deviation(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Alert handler function
using AlertHandler = std::function<bool(const AlertData& alert)>;

class AlertSystem {
public:
    AlertSystem(const AlertConfig& config = AlertConfig());
    ~AlertSystem() = default;
    
    // Configure alert system
    void configure(const AlertConfig& config) { config_ = config; }
    
    // Add alert recipient
    bool addRecipient(const AlertRecipient& recipient);
    
    // Remove alert recipient
    bool removeRecipient(const std::string& recipientId);
    
    // Get recipient
    AlertRecipient* getRecipient(const std::string& recipientId);
    
    // Check if recipient exists
    bool hasRecipient(const std::string& recipientId) const;
    
    // Add alert handler
    bool addAlertHandler(const std::string& name, AlertHandler handler);
    
    // Remove alert handler
    bool removeAlertHandler(const std::string& name);
    
    // Process alert
    bool processAlert(const AlertData& alert);
    
    // Get recent alerts
    std::vector<AlertData> getRecentAlerts(size_t count = 100) const;
    
    // Get alerts by level
    std::vector<AlertData> getAlertsByLevel(AlertLevel level, size_t count = 100) const;
    
    // Get alerts by type
    std::vector<AlertData> getAlertsByType(AlertType type, size_t count = 100) const;
    
    // Clear old alerts
    void cleanupOldAlerts();
    
    // Get alert statistics
    struct AlertStats {
        size_t totalAlerts;
        size_t alertsByLevel[4];  // INFO, WARNING, ERROR, CRITICAL
        size_t alertsByType[6];   // PERFORMANCE_DEGRADATION, MEMORY_EXHAUSTION, etc.
        std::chrono::system_clock::time_point lastAlertTime;
        
        AlertStats() : totalAlerts(0) {
            std::fill(std::begin(alertsByLevel), std::end(alertsByLevel), 0);
            std::fill(std::begin(alertsByType), std::end(alertsByType), 0);
        }
    };
    
    AlertStats getStats() const;
    
    // Check if alert system is enabled
    bool isEnabled() const { return config_.enableAlerts; }
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    // Send alert to recipient
    bool sendToRecipient(const AlertRecipient& recipient, const AlertData& alert);
    
    // Send alert to console
    bool sendToConsole(const AlertData& alert);
    
    // Send alert to file
    bool sendToFile(const AlertData& alert);
    
    // Check cooldown
    bool checkCooldown(const AlertData& alert);
    
    // Generate alert ID
    std::string generateAlertId() const;
    
    AlertConfig config_;
    mutable std::mutex mutex_;
    
    // Alert recipients
    std::unordered_map<std::string, AlertRecipient> recipients_;
    
    // Alert handlers
    std::unordered_map<std::string, AlertHandler> alertHandlers_;
    
    // Active alerts
    std::vector<AlertData> alerts_;
    
    // Cooldown tracking
    std::unordered_map<std::string, std::chrono::system_clock::time_point> cooldowns_;
    
    // Statistics
    mutable AlertStats stats_;
};

} // namespace nlm
