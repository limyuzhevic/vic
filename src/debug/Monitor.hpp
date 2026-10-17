#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include "../core/Logger/Logger.hpp"
#include "DebugConfig.hpp"
#include "Profiler.hpp"
#include "AlertSystem.hpp"
#include "Visualizer.hpp"

namespace nlm {

// Neural activity monitoring data
struct NeuralActivityData {
    std::chrono::steady_clock::time_point timestamp;
    std::string neuronId;
    float membranePotential;
    float firingRate;
    float synapticInput;
    std::vector<float> neurotransmitterLevels;
    std::map<std::string, float> neuromodulatorLevels;
};

// System health metrics
struct SystemHealthMetrics {
    std::chrono::steady_clock::time_point timestamp;
    double cpuUsage;
    double memoryUsage;
    size_t memoryAllocated;
    size_t memoryDeallocated;
    size_t activeThreads;
    double simulationTime;
    size_t totalSpikeCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    std::map<std::string, double> customMetrics;
};

// Monitoring event
struct MonitoringEvent {
    std::chrono::steady_clock::time_point timestamp;
    std::string eventId;
    std::string eventType;
    std::string source;
    std::map<std::string, std::string> metadata;
    AlertSeverity severity;
    bool isProcessed;
};

// Monitoring system interface
class IMonitor {
public:
    virtual ~IMonitor() = default;
    virtual void initialize(std::shared_ptr<DebugConfig> config) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual void recordNeuralActivity(const NeuralActivityData& activity) = 0;
    virtual void recordSystemMetrics(const SystemHealthMetrics& metrics) = 0;
    virtual void recordEvent(const MonitoringEvent& event) = 0;
    virtual std::vector<NeuralActivityData> getRecentNeuralActivity(size_t count = 100) const = 0;
    virtual std::vector<SystemHealthMetrics> getRecentSystemMetrics(size_t count = 100) const = 0;
    virtual std::vector<MonitoringEvent> getRecentEvents(size_t count = 100) const = 0;
    virtual std::map<std::string, double> getCurrentMetrics() const = 0;
    virtual std::string getSystemStatus() const = 0;
    virtual void clearHistory() = 0;
    virtual void setMonitoringCallback(const std::function<void(const MonitoringEvent&)>& callback) = 0;
};

// Monitoring system implementation
class Monitor : public IMonitor {
public:
    Monitor();
    ~Monitor() override;
    
    // Disable copying, enable moving
    Monitor(const Monitor&) = delete;
    Monitor& operator=(const Monitor&) = delete;
    Monitor(Monitor&&) noexcept;
    Monitor& operator=(Monitor&&) noexcept;
    
    void initialize(std::shared_ptr<DebugConfig> config) override;
    void start() override;
    void stop() override;
    void update() override;
    void recordNeuralActivity(const NeuralActivityData& activity) override;
    void recordSystemMetrics(const SystemHealthMetrics& metrics) override;
    void recordEvent(const MonitoringEvent& event) override;
    std::vector<NeuralActivityData> getRecentNeuralActivity(size_t count = 100) const override;
    std::vector<SystemHealthMetrics> getRecentSystemMetrics(size_t count = 100) const override;
    std::vector<MonitoringEvent> getRecentEvents(size_t count = 100) const override;
    std::map<std::string, double> getCurrentMetrics() const override;
    std::string getSystemStatus() const override;
    void clearHistory() override;
    void setMonitoringCallback(const std::function<void(const MonitoringEvent&)>& callback) override;
    
    // Event-driven monitoring
    void enableEventDrivenMonitoring(bool enabled);
    bool isEventDrivenMonitoringEnabled() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enabled);
    bool isRealTimeMonitoringEnabled() const;
    
    // Alert integration
    void setAlertSystem(std::shared_ptr<AlertSystem> alertSystem);
    void enableAlerting(bool enabled);
    bool isAlertingEnabled() const;
    
    // Visualization integration
    void setVisualizer(std::shared_ptr<Visualizer> visualizer);
    void enableVisualization(bool enabled);
    bool isVisualizationEnabled() const;
    
    // Neural activity analysis
    void analyzeNeuralActivityPatterns();
    void detectAbnormalNeuralActivity();
    
    // Performance monitoring
    void analyzePerformanceTrends();
    void detectPerformanceIssues();
    
    // System health monitoring
    void analyzeSystemHealth();
    void detectSystemIssues();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
