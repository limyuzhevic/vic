#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <functional>
#include "../core/Logger/Logger.hpp"
#include "DebugConfig.hpp"

namespace nlm {

// Forward declarations
class AlertSystem;
class DebugConfig;
class Monitor;

// Alert callback function type
using AlertCallback = std::function<void(const Alert&)>;

// Alert system interface
class IAlertSystem {
public:
    virtual ~IAlertSystem() = default;
    virtual void initialize(std::shared_ptr<DebugConfig> config) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual void addAlertCallback(const AlertCallback& callback) = 0;
    virtual void removeAlertCallback(const AlertCallback& callback) = 0;
    virtual void createAlert(const Alert& alert) = 0;
    virtual void resolveAlert(const std::string& alertId) = 0;
    virtual void clearAlerts() = 0;
    virtual std::vector<Alert> getActiveAlerts() const = 0;
    virtual std::vector<Alert> getAllAlerts() const = 0;
    virtual size_t getAlertCount() const = 0;
    virtual size_t getActiveAlertCount() const = 0;
};

// Alert system implementation
class AlertSystem : public IAlertSystem {
public:
    AlertSystem();
    ~AlertSystem() override;
    
    // Disable copying, enable moving
    AlertSystem(const AlertSystem&) = delete;
    AlertSystem& operator=(const AlertSystem&) = delete;
    AlertSystem(AlertSystem&&) noexcept;
    AlertSystem& operator=(AlertSystem&&) noexcept;
    
    void initialize(std::shared_ptr<DebugConfig> config) override;
    void start() override;
    void stop() override;
    void update() override;
    void addAlertCallback(const AlertCallback& callback) override;
    void removeAlertCallback(const AlertCallback& callback) override;
    void createAlert(const Alert& alert) override;
    void resolveAlert(const std::string& alertId) override;
    void clearAlerts() override;
    std::vector<Alert> getActiveAlerts() const override;
    std::vector<Alert> getAllAlerts() const override;
    size_t getAlertCount() const override;
    size_t getActiveAlertCount() const override;
    
    // Alert thresholds and conditions
    void setAlertThreshold(const std::string& metric, double threshold);
    void setAlertCondition(const std::string& condition, bool (*conditionFunc)(const PerformanceMetrics&));
    
    // Alert filtering
    void addAlertFilter(const std::string& metric, AlertSeverity minSeverity);
    void removeAlertFilter(const std::string& metric);
    
    // Alert categories
    enum class AlertCategory {
        Performance,
        Memory,
        Neural,
        System,
        Custom
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
