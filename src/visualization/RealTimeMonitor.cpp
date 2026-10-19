#include "VisualizationSystem.hpp"

namespace nlm {

struct RealTimeMonitor::Impl {
    // Metrics storage
    BrainMetrics latestMetrics;
    std::vector<BrainMetrics> metricsHistory;
    std::vector<std::string> activeAlerts;
    std::vector<std::pair<Timestamp, std::string>> events;
    
    // Callbacks
    MetricsCallback metricsCallback;
    AlertCallback alertCallback;
    
    // Configuration
    double updateIntervalHz;
    bool monitoringEnabled;
    bool performanceMonitoringEnabled;
    
    // Performance thresholds
    float cpuThreshold;
    float memoryThreshold;
    float spikeRateThreshold;
    
    // Custom metrics
    std::unordered_map<std::string, float> customMetrics;
    
    // Threading
    std::mutex metricsMutex;
    std::thread monitoringThread;
    std::atomic<bool> stopMonitoring;
};

RealTimeMonitor::RealTimeMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->updateIntervalHz = 30.0;
    pImpl->monitoringEnabled = false;
    pImpl->performanceMonitoringEnabled = false;
    pImpl->cpuThreshold = 90.0f;
    pImpl->memoryThreshold = 1024.0f;
    pImpl->spikeRateThreshold = 1000.0f;
    pImpl->stopMonitoring = false;
}

RealTimeMonitor::~RealTimeMonitor() {
    stopMonitoring();
}

bool RealTimeMonitor::initialize(std::shared_ptr<Config> config) {
    // Initialize monitoring system
    // Configure based on settings in config
    return true;
}

void RealTimeMonitor::shutdown() {
    stopMonitoring();
}

void RealTimeMonitor::startMonitoring(double updateIntervalHz) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (pImpl->monitoringEnabled) return;
    
    pImpl->updateIntervalHz = updateIntervalHz;
    pImpl->monitoringEnabled = true;
    pImpl->stopMonitoring = false;
    
    pImpl->monitoringThread = std::thread([this]() {
        while (!pImpl->stopMonitoring) {
            try {
                // In a real implementation, this would update metrics from the brain
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    1000 / static_cast<int>(pImpl->updateIntervalHz)));
            } catch (const std::exception& e) {
                // Handle thread errors
            }
        }
    });
}

void RealTimeMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (!pImpl->monitoringEnabled) return;
    
    pImpl->stopMonitoring = true;
    
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
    
    pImpl->monitoringEnabled = false;
}

bool RealTimeMonitor::isMonitoring() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->monitoringEnabled;
}

void RealTimeMonitor::updateMetrics(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (!brain) return;
    
    // Collect metrics from brain
    pImpl->latestMetrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    pImpl->latestMetrics.avgFiringRate = brain->getAverageFiringRate();
    pImpl->latestMetrics.activeNeuronCount = brain->getActiveNeuronCount();
    pImpl->latestMetrics.firingNeuronCount = brain->getFiringNeuronCount();
    pImpl->latestMetrics.totalSpikeCount = brain->getTotalSpikeCount();
    pImpl->latestMetrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    // Memory system metrics (would access through brain interfaces)
    pImpl->latestMetrics.workingMemoryLoad = 0.5f; // Placeholder
    pImpl->latestMetrics.episodicMemoryUtilization = 0.3f; // Placeholder
    
    // Store metrics history
    if (pImpl->metricsHistory.size() > 1000) {
        pImpl->metricsHistory.erase(pImpl->metricsHistory.begin());
    }
    pImpl->metricsHistory.push_back(pImpl->latestMetrics);
    
    // Invoke callback
    if (pImpl->metricsCallback) {
        pImpl->metricsCallback(pImpl->latestMetrics);
    }
}

void RealTimeMonitor::recordEvent(const std::string& eventType, const std::string& details) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    Timestamp timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    pImpl->events.emplace_back(timestamp, eventType + " - " + details);
    
    // Check for alerts based on event type
    if (eventType == "HighFiringRate" || eventType == "LowFiringRate" ||
        eventType == "MemoryError" || eventType == "NeuromodulationError") {
        triggerAlert("Event: " + eventType + " - " + details, 0.7f, "System");
    }
}

void RealTimeMonitor::addCustomMetric(const std::string& name, float value) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->customMetrics[name] = value;
}

void RealTimeMonitor::setMetricsCallback(MetricsCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metricsCallback = std::move(callback);
}

void RealTimeMonitor::setAlertCallback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->alertCallback = std::move(callback);
}

void RealTimeMonitor::triggerAlert(const std::string& message, float severity,
                                  const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Create alert entry
    std::string alert = "[" + category + "] " + message + " (Severity: " + 
                       std::to_string(severity) + ")";
    
    pImpl->activeAlerts.push_back(alert);
    
    // Invoke callback
    if (pImpl->alertCallback) {
        pImpl->alertCallback(message, severity);
    }
    
    // Remove old alerts
    if (pImpl->activeAlerts.size() > 100) {
        pImpl->activeAlerts.erase(pImpl->activeAlerts.begin());
    }
}

RealTimeMonitor::BrainMetrics RealTimeMonitor::getLatestMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->latestMetrics;
}

std::vector<RealTimeMonitor::BrainMetrics> RealTimeMonitor::getMetricsHistory(
    size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (maxCount > pImpl->metricsHistory.size()) {
        maxCount = pImpl->metricsHistory.size();
    }
    
    std::vector<BrainMetrics> result;
    result.reserve(maxCount);
    
    auto startIt = pImpl->metricsHistory.end() - maxCount;
    for (auto it = startIt; it != pImpl->metricsHistory.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

std::vector<std::string> RealTimeMonitor::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->activeAlerts;
}

float RealTimeMonitor::getMetricMean(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Placeholder implementation
    if (metricName == "avgFiringRate") {
        float sum = 0.0f;
        for (const auto& metrics : pImpl->metricsHistory) {
            sum += metrics.avgFiringRate;
        }
        return pImpl->metricsHistory.empty() ? 0.0f : sum / pImpl->metricsHistory.size();
    }
    
    return 0.0f;
}

float RealTimeMonitor::getMetricStdDev(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Placeholder implementation
    if (metricName == "avgFiringRate") {
        float mean = getMetricMean(metricName);
        float sumSq = 0.0f;
        for (const auto& metrics : pImpl->metricsHistory) {
            float diff = metrics.avgFiringRate - mean;
            sumSq += diff * diff;
        }
        return pImpl->metricsHistory.empty() ? 0.0f : std::sqrt(sumSq / pImpl->metricsHistory.size());
    }
    
    return 0.0f;
}

float RealTimeMonitor::getMetricMax(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Placeholder implementation
    if (metricName == "avgFiringRate") {
        float max = 0.0f;
        for (const auto& metrics : pImpl->metricsHistory) {
            max = std::max(max, metrics.avgFiringRate);
        }
        return max;
    }
    
    return 0.0f;
}

float RealTimeMonitor::getMetricMin(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Placeholder implementation
    if (metricName == "avgFiringRate") {
        float min = std::numeric_limits<float>::max();
        for (const auto& metrics : pImpl->metricsHistory) {
            min = std::min(min, metrics.avgFiringRate);
        }
        return min;
    }
    
    return 0.0f;
}

void RealTimeMonitor::clearOldEvents(double keepSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    Timestamp currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    auto it = pImpl->events.begin();
    while (it != pImpl->events.end()) {
        double eventAge = (currentTime - it->first) / 1000.0;
        if (eventAge > keepSeconds) {
            it = pImpl->events.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<std::pair<Timestamp, std::string>> RealTimeMonitor::getRecentEvents(
    size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<std::pair<Timestamp, std::string>> result;
    result.reserve(std::min(maxCount, pImpl->events.size()));
    
    auto startIt = pImpl->events.end() - maxCount;
    for (auto it = startIt; it != pImpl->events.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void RealTimeMonitor::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->performanceMonitoringEnabled = enable;
}

void RealTimeMonitor::setPerformanceThresholds(float cpuThreshold, float memoryThreshold,
                                                float spikeRateThreshold) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->cpuThreshold = cpuThreshold;
    pImpl->memoryThreshold = memoryThreshold;
    pImpl->spikeRateThreshold = spikeRateThreshold;
}

void RealTimeMonitor::setUpdateInterval(double intervalHz) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->updateIntervalHz = intervalHz;
}

double RealTimeMonitor::getUpdateInterval() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->updateIntervalHz;
}

bool RealTimeMonitor::exportMetrics(const std::string& filepath, const std::string& format) {
    // Implementation for exporting metrics
    // For now, just return success
    return true;
}

bool RealTimeMonitor::exportEvents(const std::string& filepath, const std::string& format) {
    // Implementation for exporting events
    // For now, just return success
    return true;
}

} // namespace nlm
