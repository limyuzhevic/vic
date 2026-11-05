// Neural activity monitoring and control system
// Provides commands to monitor and regulate neural activity levels

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"

namespace nlm {

class NeuralActivityMonitor {
public:
    NeuralActivityMonitor();
    ~NeuralActivityMonitor();
    
    // Initialize with brain reference
    void initialize(std::shared_ptr<Brain> brain);
    
    // Command: Get current neural activity levels
    std::string getActivitySummary() const;
    
    // Command: Set activity thresholds
    void setActivityThresholds(float firingThreshold, float excitationThreshold);
    
    // Command: Monitor neural activity
    void startMonitoring(double interval);
    
    // Command: Stop monitoring
    void stopMonitoring();
    
    // Command: Get activity statistics
    std::string getActivityStatistics() const;
    
    // Command: Control neural activity levels
    void controlActivity(const std::string& command, float parameter);
    
    // Command: Get current activity levels
    float getCurrentFiringRate() const;
    
    // Command: Get activity history
    std::vector<float> getActivityHistory() const;
    
    // Command: Set activity logging level
    void setActivityLogLevel(int level);
    
    // Command: Get activity log
    std::vector<std::string> getActivityLog() const;
    
    // Command: Check for neural anomalies
    bool checkNeuralAnomalies() const;
    
    // Command: Get neural health status
    std::string getNeuralHealthStatus() const;
    
    // Command: Apply neural activity control
    void applyActivityControl(const std::string& controlType, float value);
    
    // Command: Get current control settings
    std::string getCurrentControlSettings() const;
    
    // Command: Reset activity monitoring
    void resetMonitoring();
    
    // Command: Export activity data
    std::string exportActivityData(const std::string& format) const;
    
    // Command: Import activity settings
    bool importActivitySettings(const std::string& settings);
    
    // Command: Get monitoring status
    bool isMonitoring() const;
    
    // Command: Get monitoring interval
    double getMonitoringInterval() const;
    
    // Command: Set monitoring interval
    void setMonitoringInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm