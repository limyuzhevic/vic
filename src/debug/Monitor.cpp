#include "Monitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <set>
#ifdef __linux__
#include <filesystem>
#endif

namespace nlm {

struct Monitor::Impl {
    std::shared_ptr<DebugConfig> config;
    std::atomic<bool> isRunning;
    std::atomic<bool> isInitialized;
    
    // Monitoring data
    std::vector<NeuralActivityData> neuralActivityHistory;
    std::vector<SystemHealthMetrics> systemMetricsHistory;
    std::vector<MonitoringEvent> eventHistory;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Current metrics
    SystemHealthMetrics currentSystemMetrics;
    std::map<std::string, double> currentMetrics;
    
    // Configuration
    bool eventDrivenMonitoringEnabled;
    bool realTimeMonitoringEnabled;
    bool alertingEnabled;
    bool visualizationEnabled;
    size_t maxHistorySize;
    int updateInterval;
    
    // Integration
    std::shared_ptr<AlertSystem> alertSystem;
    std::shared_ptr<Visualizer> visualizer;
    std::function<void(const MonitoringEvent&)> monitoringCallback;
    
    // Statistics
    std::map<std::string, std::vector<double>> metricHistory;
    std::map<std::string, std::chrono::steady_clock::time_point> lastEventTimes;
    
    Impl() : isRunning(false), isInitialized(false), 
             eventDrivenMonitoringEnabled(true),
             realTimeMonitoringEnabled(true),
             alertingEnabled(true),
             visualizationEnabled(true),
             maxHistorySize(10000),
             updateInterval(100) // milliseconds
    {
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
        
        // Initialize current metrics
        currentSystemMetrics.timestamp = startTime;
        currentSystemMetrics.cpuUsage = 0.0;
        currentSystemMetrics.memoryUsage = 0.0;
        currentSystemMetrics.memoryAllocated = 0;
        currentSystemMetrics.memoryDeallocated = 0;
        currentSystemMetrics.activeThreads = std::thread::hardware_concurrency();
        currentSystemMetrics.simulationTime = 0.0;
        currentSystemMetrics.totalSpikeCount = 0;
        currentSystemMetrics.averageFiringRate = 0.0;
        currentSystemMetrics.excitationInhibitionRatio = 1.0;
    }
    
    ~Impl() {
        // Export monitoring data if needed
        exportMonitoringData();
    }
    
    void exportMonitoringData() {
        if (maxHistorySize > 0 && neuralActivityHistory.size() > maxHistorySize) {
            // Keep only the most recent entries
            size_t keepCount = maxHistorySize / 2;
            neuralActivityHistory.erase(neuralActivityHistory.begin(), 
                                       neuralActivityHistory.begin() + (neuralActivityHistory.size() - keepCount));
        }
        
        if (maxHistorySize > 0 && systemMetricsHistory.size() > maxHistorySize) {
            size_t keepCount = maxHistorySize / 2;
            systemMetricsHistory.erase(systemMetricsHistory.begin(), 
                                      systemMetricsHistory.begin() + (systemMetricsHistory.size() - keepCount));
        }
        
        if (maxHistorySize > 0 && eventHistory.size() > maxHistorySize) {
            size_t keepCount = maxHistorySize / 2;
            eventHistory.erase(eventHistory.begin(), 
                              eventHistory.begin() + (eventHistory.size() - keepCount));
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return std::to_string(duration.count());
    }
    
    double getNeuralActivityLevel(const NeuralActivityData& activity) const {
        // Calculate a normalized activity level based on membrane potential
        return std::min(1.0, std::max(0.0, (activity.membranePotential - (-70.0)) / (50.0 - (-70.0))));
    }
    
    AlertSeverity getEventSeverity(const std::string& eventType) const {
        if (eventType == "error" || eventType == "critical") return AlertSeverity::Critical;
        if (eventType == "warning") return AlertSeverity::Warning;
        if (eventType == "performance_issue") return AlertSeverity::Error;
        if (eventType == "neural_abnormal") return AlertSeverity::Warning;
        return AlertSeverity::Info;
    }
    
    void analyzeNeuralActivityPatterns() {
        if (neuralActivityHistory.size() < 10) {
            return;
        }
        
        // Group activities by neuron
        std::map<std::string, std::vector<float>> neuronActivities;
        for (const auto& activity : neuralActivityHistory) {
            neuronActivities[activity.neuronId].push_back(getNeuralActivityLevel(activity));
        }
        
        // Detect abnormal patterns
        for (const auto& [neuronId, activities] : neuronActivities) {
            if (activities.size() < 5) continue;
            
            double avgActivity = std::accumulate(activities.begin(), activities.end(), 0.0) / activities.size();
            
            // Check for abnormally high activity
            if (avgActivity > 0.8) {
                MonitoringEvent event;
                event.timestamp = std::chrono::steady_clock::now();
                event.eventId = "high_neural_activity_" + neuronId;
                event.eventType = "neural_abnormal";
                event.source = "Monitor";
                event.metadata["neuron_id"] = neuronId;
                event.metadata["average_activity"] = std::to_string(avgActivity);
                event.severity = AlertSeverity::Warning;
                event.isProcessed = false;
                
                recordEvent(event);
            }
        }
    }
    
    void detectAbnormalNeuralActivity() {
        if (neuralActivityHistory.size() < 20) {
            return;
        }
        
        // Look for sudden changes in activity
        std::map<std::string, std::vector<float>> neuronActivities;
        for (const auto& activity : neuralActivityHistory) {
            neuronActivities[activity.neuronId].push_back(activity.membranePotential);
        }
        
        for (const auto& [neuronId, potentials] : neuronActivities) {
            if (potentials.size() < 10) continue;
            
            // Check for unusual variance
            double mean = std::accumulate(potentials.begin(), potentials.end(), 0.0) / potentials.size();
            double variance = 0.0;
            for (float potential : potentials) {
                variance += (potential - mean) * (potential - mean);
            }
            variance /= potentials.size();
            double stdDev = std::sqrt(variance);
            
            if (stdDev > 20.0) { // Large standard deviation indicates abnormal activity
                MonitoringEvent event;
                event.timestamp = std::chrono::steady_clock::now();
                event.eventId = "abnormal_neural_variance_" + neuronId;
                event.eventType = "neural_abnormal";
                event.source = "Monitor";
                event.metadata["neuron_id"] = neuronId;
                event.metadata["mean_potential"] = std::to_string(mean);
                event.metadata["std_dev"] = std::to_string(stdDev);
                event.severity = AlertSeverity::Warning;
                event.isProcessed = false;
                
                recordEvent(event);
            }
        }
    }
    
    void analyzePerformanceTrends() {
        if (systemMetricsHistory.size() < 10) {
            return;
        }
        
        // Check for performance degradation
        for (const auto& metric : {"cpu_usage", "memory_usage", "simulation_time"}) {
            if (metricHistory.find(metric) == metricHistory.end()) {
                continue;
            }
            
            const auto& values = metricHistory[metric];
            if (values.size() < 10) continue;
            
            // Calculate recent trend (last 5 measurements vs previous 5)
            size_t recentSize = std::min<size_t>(5, values.size());
            std::vector<double> recent(values.end() - recentSize, values.end());
            std::vector<double> previous(values.end() - 2 * recentSize, values.end() - recentSize);
            
            if (previous.empty()) continue;
            
            double recentAvg = std::accumulate(recent.begin(), recent.end(), 0.0) / recent.size();
            double previousAvg = std::accumulate(previous.begin(), previous.end(), 0.0) / previous.size();
            
            double trend = (recentAvg - previousAvg) / previousAvg;
            
            if (std::abs(trend) > 0.2) { // 20% change
                MonitoringEvent event;
                event.timestamp = std::chrono::steady_clock::now();
                event.eventId = "performance_trend_" + metric;
                event.eventType = "performance_issue";
                event.source = "Monitor";
                event.metadata["metric"] = metric;
                event.metadata["previous_avg"] = std::to_string(previousAvg);
                event.metadata["recent_avg"] = std::to_string(recentAvg);
                event.metadata["trend"] = std::to_string(trend);
                event.severity = (trend > 0.2) ? AlertSeverity::Error : AlertSeverity::Warning;
                event.isProcessed = false;
                
                recordEvent(event);
            }
        }
    }
    
    void detectPerformanceIssues() {
        // Check for resource exhaustion
        if (currentSystemMetrics.cpuUsage > 90.0) {
            MonitoringEvent event;
            event.timestamp = std::chrono::steady_clock::now();
            event.eventId = "high_cpu_usage";
            event.eventType = "performance_issue";
            event.source = "Monitor";
            event.metadata["cpu_usage"] = std::to_string(currentSystemMetrics.cpuUsage);
            event.severity = AlertSeverity::Error;
            event.isProcessed = false;
            
            recordEvent(event);
        }
        
        if (currentSystemMetrics.memoryUsage > 0.9) {
            MonitoringEvent event;
            event.eventId = "high_memory_usage";
            event.eventType = "performance_issue";
            event.source = "Monitor";
            event.metadata["memory_usage"] = std::to_string(currentSystemMetrics.memoryUsage);
            event.severity = AlertSeverity::Error;
            event.isProcessed = false;
            
            recordEvent(event);
        }
        
        if (currentSystemMetrics.totalSpikeCount > 100000) { // Threshold can be configured
            MonitoringEvent event;
            event.timestamp = std::chrono::steady_clock::now();
            event.eventId = "high_spike_count";
            event.eventType = "performance_issue";
            event.source = "Monitor";
            event.metadata["spike_count"] = std::to_string(currentSystemMetrics.totalSpikeCount);
            event.severity = AlertSeverity::Warning;
            event.isProcessed = false;
            
            recordEvent(event);
        }
    }
    
    void analyzeSystemHealth() {
        // Check system health based on metrics
        double healthScore = 100.0;
        
        if (currentSystemMetrics.cpuUsage > 90.0) healthScore -= 20.0;
        if (currentSystemMetrics.memoryUsage > 0.9) healthScore -= 20.0;
        if (currentSystemMetrics.excitationInhibitionRatio < 0.5 || currentSystemMetrics.excitationInhibitionRatio > 2.0) healthScore -= 15.0;
        
        if (healthScore < 50.0) {
            MonitoringEvent event;
            event.timestamp = std::chrono::steady_clock::now();
            event.eventId = "poor_system_health";
            event.eventType = "system_issue";
            event.source = "Monitor";
            event.metadata["health_score"] = std::to_string(healthScore);
            event.metadata["cpu_usage"] = std::to_string(currentSystemMetrics.cpuUsage);
            event.metadata["memory_usage"] = std::to_string(currentSystemMetrics.memoryUsage);
            event.severity = AlertSeverity::Critical;
            event.isProcessed = false;
            
            recordEvent(event);
        }
    }
    
    void detectSystemIssues() {
        // Detect various system issues
        if (currentSystemMetrics.activeThreads > std::thread::hardware_concurrency() * 2) {
            MonitoringEvent event;
            event.timestamp = std::chrono::steady_clock::now();
            event.eventId = "high_thread_count";
            event.eventType = "system_issue";
            event.source = "Monitor";
            event.metadata["thread_count"] = std::to_string(currentSystemMetrics.activeThreads);
            event.metadata["hardware_concurrency"] = std::to_string(std::thread::hardware_concurrency());
            event.severity = AlertSeverity::Warning;
            event.isProcessed = false;
            
            recordEvent(event);
        }
    }
};

Monitor::Monitor() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Monitor created");
}

Monitor::~Monitor() {
    NLM_LOG_DEBUG("Monitor destroyed");
}

Monitor::Monitor(Monitor&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

Monitor& Monitor::operator=(Monitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Monitor::initialize(std::shared_ptr<DebugConfig> config) {
    pImpl->config = config;
    pImpl->isInitialized = true;
    
    // Load monitoring configuration from config
    if (config) {
        pImpl->eventDrivenMonitoringEnabled = config->isEventDrivenMonitoringEnabled();
        pImpl->realTimeMonitoringEnabled = config->isRealTimeMonitoringEnabled();
        pImpl->alertingEnabled = config->isAlertSystemEnabled();
        pImpl->visualizationEnabled = config->isVisualizationEnabled();
        pImpl->updateInterval = config->getRealTimeUpdateInterval();
    }
    
    NLM_LOG_INFO("Monitor initialized");
}

void Monitor::start() {
    if (!pImpl->isInitialized) {
        NLM_LOG_WARNING("Monitor not initialized, skipping start");
        return;
    }
    
    pImpl->isRunning = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("Monitor started");
}

void Monitor::stop() {
    pImpl->isRunning = false;
    NLM_LOG_INFO("Monitor stopped");
}

void Monitor::update() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    
    // Update current metrics
    updateCurrentMetrics();
    
    // Analyze data periodically
    static size_t analysisCount = 0;
    if (++analysisCount >= 10) {
        if (pImpl->config && pImpl->config->isNeuralActivityMonitoringEnabled()) {
            analyzeNeuralActivityPatterns();
            detectAbnormalNeuralActivity();
        }
        
        analyzePerformanceTrends();
        detectPerformanceIssues();
        
        analyzeSystemHealth();
        detectSystemIssues();
        
        analysisCount = 0;
    }
}

void Monitor::recordNeuralActivity(const NeuralActivityData& activity) {
    pImpl->neuralActivityHistory.push_back(activity);
    
    // Add to metrics history
    double activityLevel = pImpl->getNeuralActivityLevel(activity);
    pImpl->metricHistory["neural_activity_" + activity.neuronId].push_back(activityLevel);
    
    // Send to alert system if enabled
    if (pImpl->alertSystem && pImpl->alertingEnabled && activityLevel > pImpl->config->getNeuralActivityThreshold()) {
        Alert alert;
        alert.timestamp = std::chrono::steady_clock::now();
        alert.id = "neural_activity_" + activity.neuronId + "_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            alert.timestamp.time_since_epoch()).count());
        alert.severity = AlertSeverity::Warning;
        alert.message = "Neural activity level for neuron " + activity.neuronId + " exceeds threshold";
        alert.source = "Monitor";
        alert.metadata["neuron_id"] = activity.neuronId;
        alert.metadata["activity_level"] = std::to_string(activityLevel);
        alert.metadata["threshold"] = std::to_string(pImpl->config->getNeuralActivityThreshold());
        alert.isActive = true;
        
        pImpl->alertSystem->createAlert(alert);
    }
    
    // Send to visualization if enabled
    if (pImpl->visualizer && pImpl->visualizationEnabled) {
        // In a real implementation, this would send data to the visualizer
        // For now, just log it
        NLM_LOG_DEBUG("Neural activity recorded for neuron " + activity.neuronId);
    }
}

void Monitor::recordSystemMetrics(const SystemHealthMetrics& metrics) {
    pImpl->systemMetricsHistory.push_back(metrics);
    pImpl->currentSystemMetrics = metrics;
    
    // Update metric history
    pImpl->metricHistory["cpu_usage"].push_back(metrics.cpuUsage);
    pImpl->metricHistory["memory_usage"].push_back(metrics.memoryUsage);
    pImpl->metricHistory["total_spike_count"].push_back(static_cast<double>(metrics.totalSpikeCount));
    pImpl->metricHistory["average_firing_rate"].push_back(metrics.averageFiringRate);
    
    // Send to alert system if enabled
    if (pImpl->alertSystem && pImpl->alertingEnabled) {
        // Check various thresholds and create alerts if needed
        if (metrics.cpuUsage > 90.0) {
            Alert alert;
            alert.timestamp = std::chrono::steady_clock::now();
            alert.id = "high_cpu_usage_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                alert.timestamp.time_since_epoch()).count());
            alert.severity = AlertSeverity::Error;
            alert.message = "CPU usage exceeds 90%";
            alert.source = "Monitor";
            alert.metadata["cpu_usage"] = std::to_string(metrics.cpuUsage);
            alert.metadata["threshold"] = "90.0";
            alert.isActive = true;
            
            pImpl->alertSystem->createAlert(alert);
        }
        
        if (metrics.memoryUsage > 0.9) {
            Alert alert;
            alert.timestamp = std::chrono::steady_clock::now();
            alert.id = "high_memory_usage_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                alert.timestamp.time_since_epoch()).count());
            alert.severity = AlertSeverity::Error;
            alert.message = "Memory usage exceeds 90%";
            alert.source = "Monitor";
            alert.metadata["memory_usage"] = std::to_string(metrics.memoryUsage);
            alert.metadata["threshold"] = "0.9";
            alert.isActive = true;
            
            pImpl->alertSystem->createAlert(alert);
        }
    }
    
    // Send to monitoring callback
    if (pImpl->monitoringCallback) {
        MonitoringEvent event;
        event.timestamp = std::chrono::steady_clock::now();
        event.eventId = "system_metrics_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            event.timestamp.time_since_epoch()).count());
        event.eventType = "system_metrics";
        event.source = "Monitor";
        event.metadata["cpu_usage"] = std::to_string(metrics.cpuUsage);
        event.metadata["memory_usage"] = std::to_string(metrics.memoryUsage);
        event.metadata["spike_count"] = std::to_string(metrics.totalSpikeCount);
        event.severity = AlertSeverity::Info;
        event.isProcessed = false;
        
        pImpl->monitoringCallback(event);
    }
}

void Monitor::recordEvent(const MonitoringEvent& event) {
    pImpl->eventHistory.push_back(event);
    
    // Send to alert system
    if (pImpl->alertSystem && pImpl->alertingEnabled) {
        Alert alert;
        alert.timestamp = event.timestamp;
        alert.id = "monitoring_event_" + event.eventId;
        alert.severity = event.severity;
        alert.message = event.eventType + ": " + event.source + " - " + event.eventId;
        alert.source = "Monitor";
        alert.metadata["event_type"] = event.eventType;
        alert.metadata["source"] = event.source;
        alert.metadata["event_id"] = event.eventId;
        alert.isActive = true;
        
        pImpl->alertSystem->createAlert(alert);
    }
    
    // Send to monitoring callback
    if (pImpl->monitoringCallback) {
        pImpl->monitoringCallback(event);
    }
}

std::vector<NeuralActivityData> Monitor::getRecentNeuralActivity(size_t count) const {
    if (count > pImpl->neuralActivityHistory.size()) {
        count = pImpl->neuralActivityHistory.size();
    }
    
    std::vector<NeuralActivityData> recent;
    recent.reserve(count);
    
    auto start = pImpl->neuralActivityHistory.end() - count;
    for (auto it = start; it != pImpl->neuralActivityHistory.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

std::vector<SystemHealthMetrics> Monitor::getRecentSystemMetrics(size_t count) const {
    if (count > pImpl->systemMetricsHistory.size()) {
        count = pImpl->systemMetricsHistory.size();
    }
    
    std::vector<SystemHealthMetrics> recent;
    recent.reserve(count);
    
    auto start = pImpl->systemMetricsHistory.end() - count;
    for (auto it = start; it != pImpl->systemMetricsHistory.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

std::vector<MonitoringEvent> Monitor::getRecentEvents(size_t count) const {
    if (count > pImpl->eventHistory.size()) {
        count = pImpl->eventHistory.size();
    }
    
    std::vector<MonitoringEvent> recent;
    recent.reserve(count);
    
    auto start = pImpl->eventHistory.end() - count;
    for (auto it = start; it != pImpl->eventHistory.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

std::map<std::string, double> Monitor::getCurrentMetrics() const {
    std::map<std::string, double> metrics = pImpl->currentMetrics;
    
    // Add system metrics
    metrics["cpu_usage"] = pImpl->currentSystemMetrics.cpuUsage;
    metrics["memory_usage"] = pImpl->currentSystemMetrics.memoryUsage;
    metrics["total_spike_count"] = static_cast<double>(pImpl->currentSystemMetrics.totalSpikeCount);
    metrics["average_firing_rate"] = pImpl->currentSystemMetrics.averageFiringRate;
    metrics["excitation_inhibition_ratio"] = pImpl->currentSystemMetrics.excitationInhibitionRatio;
    
    return metrics;
}

std::string Monitor::getSystemStatus() const {
    std::stringstream ss;
    ss << "System Status:\n";
    ss << "==============\n\n";
    
    ss << "Uptime: " << std::chrono::duration<double, std::milli>(pImpl->lastUpdateTime - pImpl->startTime).count() << " ms\n";
    ss << "Neural Activity Samples: " << pImpl->neuralActivityHistory.size() << "\n";
    ss << "System Metric Samples: " << pImpl->systemMetricsHistory.size() << "\n";
    ss << "Event Samples: " << pImpl->eventHistory.size() << "\n";
    ss << "\n";
    
    ss << "Current Metrics:\n";
    auto metrics = getCurrentMetrics();
    for (const auto& [key, value] : metrics) {
        ss << "  " << key << ": " << value << "\n";
    }
    
    ss << "\n";
    
    // Add active alerts count if alert system is available
    if (pImpl->alertSystem) {
        size_t activeAlertCount = pImpl->alertSystem->getActiveAlertCount();
        ss << "Active Alerts: " << activeAlertCount << "\n";
    }
    
    return ss.str();
}

void Monitor::clearHistory() {
    pImpl->neuralActivityHistory.clear();
    pImpl->systemMetricsHistory.clear();
    pImpl->eventHistory.clear();
    pImpl->metricHistory.clear();
    NLM_LOG_INFO("Monitoring history cleared");
}

void Monitor::setMonitoringCallback(const std::function<void(const MonitoringEvent&)>& callback) {
    pImpl->monitoringCallback = callback;
    NLM_LOG_INFO("Monitoring callback set");
}

void Monitor::enableEventDrivenMonitoring(bool enabled) {
    pImpl->eventDrivenMonitoringEnabled = enabled;
    NLM_LOG_INFO("Event-driven monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

bool Monitor::isEventDrivenMonitoringEnabled() const {
    return pImpl->eventDrivenMonitoringEnabled;
}

void Monitor::enableRealTimeMonitoring(bool enabled) {
    pImpl->realTimeMonitoringEnabled = enabled;
    NLM_LOG_INFO("Real-time monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

bool Monitor::isRealTimeMonitoringEnabled() const {
    return pImpl->realTimeMonitoringEnabled;
}

void Monitor::setAlertSystem(std::shared_ptr<AlertSystem> alertSystem) {
    pImpl->alertSystem = alertSystem;
    NLM_LOG_INFO("Alert system set");
}

void Monitor::enableAlerting(bool enabled) {
    pImpl->alertingEnabled = enabled;
    NLM_LOG_INFO("Alerting " + std::string(enabled ? "enabled" : "disabled"));
}

bool Monitor::isAlertingEnabled() const {
    return pImpl->alertingEnabled;
}

void Monitor::setVisualizer(std::shared_ptr<Visualizer> visualizer) {
    pImpl->visualizer = visualizer;
    NLM_LOG_INFO("Visualizer set");
}

void Monitor::enableVisualization(bool enabled) {
    pImpl->visualizationEnabled = enabled;
    NLM_LOG_INFO("Visualization " + std::string(enabled ? "enabled" : "disabled"));
}

bool Monitor::isVisualizationEnabled() const {
    return pImpl->visualizationEnabled;
}

// Helper methods

void Monitor::updateCurrentMetrics() {
    // In a real implementation, this would collect actual metrics from the system
    // For now, generate simulated metrics
    static size_t spikeCount = 0;
    
    pImpl->currentSystemMetrics.timestamp = std::chrono::steady_clock::now();
    pImpl->currentSystemMetrics.cpuUsage = 50.0 + (std::rand() % 30); // 50-80%
    pImpl->currentSystemMetrics.memoryUsage = 0.5 + (std::rand() % 30) / 100.0; // 0.5-0.8
    pImpl->currentSystemMetrics.memoryAllocated = 1024 * 1024 * (100 + std::rand() % 400); // 100-500MB
    pImpl->currentSystemMetrics.memoryDeallocated = 1024 * 1024 * (50 + std::rand() % 200); // 50-250MB
    pImpl->currentSystemMetrics.activeThreads = 4 + (std::rand() % 8); // 4-12 threads
    pImpl->currentSystemMetrics.simulationTime = 1000.0 + (std::rand() % 2000); // 1000-3000
    pImpl->currentSystemMetrics.totalSpikeCount = spikeCount + (std::rand() % 1000); // Increment
    spikeCount = pImpl->currentSystemMetrics.totalSpikeCount;
    pImpl->currentSystemMetrics.averageFiringRate = 10.0 + (std::rand() % 20); // 10-30 Hz
    pImpl->currentSystemMetrics.excitationInhibitionRatio = 1.0 + (std::rand() % 100) / 50.0 - 1.0; // 0.8-2.0
}

} // namespace nlm
