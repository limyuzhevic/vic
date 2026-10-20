#include "MonitoringManager.hpp"
#include "../brain/Brain.hpp"
#include "../performance/OptimizedBrain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace nlm {

namespace {
    // Helper functions for system metrics
    double getCurrentCPUUsage() {
        // Placeholder - would implement with platform-specific code
        // Could use /proc/stat on Linux, GetSystemTimes on Windows, etc.
        static double lastCpuUsage = 0.0;
        static auto lastTime = std::chrono::high_resolution_clock::now();
        
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
        
        if (elapsed < std::chrono::milliseconds(1000)) {
            return lastCpuUsage;
        }
        
        // Simulate CPU usage (would use actual system calls)
        lastCpuUsage = 45.0 + (rand() % 30); // Random between 45-75%
        lastTime = now;
        return lastCpuUsage;
    }
    
    double getCurrentMemoryUsageMB() {
        // Placeholder - would implement with platform-specific code
        // Could use /proc/meminfo on Linux, GlobalMemoryStatus on Windows, etc.
        static double lastMemoryUsage = 512.0;
        static auto lastTime = std::chrono::high_resolution_clock::now();
        
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
        
        if (elapsed < std::chrono::milliseconds(5000)) {
            return lastMemoryUsage;
        }
        
        // Simulate memory usage (would use actual system calls)
        lastMemoryUsage = 512.0 + (rand() % 200); // Random between 512-712 MB
        lastTime = now;
        return lastMemoryUsage;
    }
    
    double getCurrentMemoryUsagePercent() {
        double memMB = getCurrentMemoryUsageMB();
        // Assume 2GB total memory for this system
        return (memMB / 2048.0) * 100.0;
    }
}

struct MonitoringManager::Impl {
    std::unordered_map<std::string, MetricDefinition> metricDefinitions;
    std::unordered_map<std::string, std::vector<RealTimeMetric>> realTimeHistory;
    std::mutex metricsMutex;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> lastUpdateTimes;
    size_t updateCount = 0;
    std::chrono::high_resolution_clock::time_point startTime;
};

MonitoringManager::MonitoringManager() 
    : running_(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

MonitoringManager::~MonitoringManager() {
    stop();
}

bool MonitoringManager::initialize(const MonitoringConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    
    // Register default metrics based on config
    if (config_.enableRealTimeMetrics) {
        registerMetric(MetricDefinition("cpu_usage", "CPU utilization", "%", 
                                       [this]() { return getCurrentCPUUsage(); },
                                       std::chrono::milliseconds(1000), LogLevel::Info));
        registerMetric(MetricDefinition("memory_usage_mb", "Memory usage", "MB",
                                       [this]() { return getCurrentMemoryUsageMB(); },
                                       std::chrono::milliseconds(5000), LogLevel::Info));
    }
    
    if (config_.enablePerformanceMonitoring) {
        registerMetric(MetricDefinition("simulation_step", "Current simulation step", "steps",
                                       nullptr, std::chrono::milliseconds(100), LogLevel::Debug));
        registerMetric(MetricDefinition("simulation_time", "Current simulation time", "s",
                                       nullptr, std::chrono::milliseconds(100), LogLevel::Debug));
    }
    
    NLM_LOG_INFO("MonitoringManager initialized");
    return true;
}

void MonitoringManager::start() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        lastUpdateTime_ = std::chrono::high_resolution_clock::now();
        NLM_LOG_INFO("MonitoringManager started");
    }
}

void MonitoringManager::stop() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (running_) {
        running_ = false;
        NLM_LOG_INFO("MonitoringManager stopped");
    }
}

void MonitoringManager::configure(const MonitoringConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    NLM_LOG_INFO("MonitoringManager reconfigured");
}

void MonitoringManager::registerMetric(const MetricDefinition& metric) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->metricDefinitions[metric.name] = metric;
    NLM_LOG_INFO("Registered metric: " + metric.name + " - " + metric.description);
}

void MonitoringManager::registerNeuralMetrics(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // Register neural system metrics
    registerMetric(MetricDefinition("total_neurons", "Total neurons in brain", "neurons",
                                   [&brain]() { return static_cast<double>(brain.getTotalNeuronCount()); },
                                   std::chrono::milliseconds(500), LogLevel::Info));
    
    registerMetric(MetricDefinition("total_synapses", "Total synapses in brain", "synapses",
                                   [&brain]() { return static_cast<double>(brain.getTotalSynapseCount()); },
                                   std::chrono::milliseconds(500), LogLevel::Info));
    
    registerMetric(MetricDefinition("active_neurons", "Currently active neurons", "neurons",
                                   [&brain]() { return static_cast<double>(brain.getActiveNeuronCount()); },
                                   std::chrono::milliseconds(100), LogLevel::Info));
    
    registerMetric(MetricDefinition("firing_neurons", "Currently firing neurons (this step)", "neurons",
                                   [&brain]() { return static_cast<double>(brain.getFiringNeuronCount()); },
                                   std::chrono::milliseconds(100), LogLevel::Info));
    
    registerMetric(MetricDefinition("total_spikes", "Total spikes since simulation start", "spikes",
                                   [&brain]() { return static_cast<double>(brain.getTotalSpikeCount()); },
                                   std::chrono::milliseconds(1000), LogLevel::Debug));
    
    registerMetric(MetricDefinition("avg_firing_rate", "Average firing rate", "Hz",
                                   [&brain]() { return brain.getAverageFiringRate(); },
                                   std::chrono::milliseconds(500), LogLevel::Info));
    
    registerMetric(MetricDefinition("e_i_ratio", "Excitatory/Inhibitory ratio", "ratio",
                                   [&brain]() { return brain.getExcitationInhibitionRatio(); },
                                   std::chrono::milliseconds(1000), LogLevel::Info));
}

void MonitoringManager::registerPerformanceMetrics(const OptimizedBrain* optimizedBrain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (optimizedBrain) {
        registerMetric(MetricDefinition("perf_step_time_ms", "Average step time", "ms",
                                       [optimizedBrain]() { return optimizedBrain->getPerformanceStats().stepTimeMs; },
                                       std::chrono::milliseconds(100), LogLevel::Debug));
        
        registerMetric(MetricDefinition("perf_real_time_factor", "Real-time simulation factor", "ratio",
                                       [optimizedBrain]() { return optimizedBrain->getPerformanceStats().realTimeFactor; },
                                       std::chrono::milliseconds(100), LogLevel::Debug));
        
        registerMetric(MetricDefinition("perf_neurons_per_second", "Neurons processed per second", "neurons/s",
                                       [optimizedBrain]() { return optimizedBrain->getPerformanceStats().neuronsPerSecond; },
                                       std::chrono::milliseconds(100), LogLevel::Debug));
        
        registerMetric(MetricDefinition("perf_spikes_per_second", "Spikes processed per second", "spikes/s",
                                       [optimizedBrain]() { return optimizedBrain->getPerformanceStats().spikesPerSecond; },
                                       std::chrono::milliseconds(100), LogLevel::Debug));
    }
}

void MonitoringManager::registerSpikeMetrics(const std::vector<DetailedSpikeEvent>& spikes) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (!spikes.empty()) {
        registerMetric(MetricDefinition("spike_count_current_step", "Spikes in current step", "spikes",
                                       [spikes]() { return static_cast<double>(spikes.size()); },
                                       std::chrono::milliseconds(100), LogLevel::Debug));
    }
}

void MonitoringManager::update() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    update(std::nullopt, std::nullopt, std::nullopt);
}

void MonitoringManager::update(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime_);
    
    if (!running_ || elapsed < config_.updateInterval) {
        return;
    }
    
    lastUpdateTime_ = now;
    updateMetrics(brain, currentTime, currentStep);
    updateRealTimeMetrics();
    processMetrics();
    logMetrics();
    sendNotifications();
}

void MonitoringManager::updateMetrics(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    if (!config_.enablePerformanceMonitoring) {
        return;
    }
    
    // Update performance metrics
    performanceMetrics_.neuronCount = brain.getTotalNeuronCount();
    performanceMetrics_.synapseCount = brain.getTotalSynapseCount();
    performanceMetrics_.activeNeurons = brain.getActiveNeuronCount();
    performanceMetrics_.firingNeurons = brain.getFiringNeuronCount();
    performanceMetrics_.totalSpikes = brain.getTotalSpikeCount();
    performanceMetrics_.avgFiringRate = brain.getAverageFiringRate();
    performanceMetrics_.e_iRatio = brain.getExcitationInhibitionRatio();
    
    performanceMetrics_.currentStep = currentStep;
    performanceMetrics_.currentTime = currentTime;
    
    // Update system metrics
    performanceMetrics_.cpuUsage = getCurrentCPUUsage();
    performanceMetrics_.memoryUsageMB = getCurrentMemoryUsageMB();
    performanceMetrics_.memoryUsagePercent = getCurrentMemoryUsagePercent();
}

void MonitoringManager::updateRealTimeMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    realTimeMetrics_.clear();
    
    for (const auto& pair : pImpl->metricDefinitions) {
        const auto& metric = pair.second;
        
        // Check if enough time has passed since last update
        auto now = std::chrono::high_resolution_clock::now();
        auto lastUpdateIt = pImpl->lastUpdateTimes.find(metric.name);
        
        if (lastUpdateIt == pImpl->lastUpdateTimes.end()) {
            // First update
            pImpl->lastUpdateTimes[metric.name] = now;
        } else {
            auto lastUpdate = lastUpdateIt->second;
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
            
            if (elapsed < metric.updateInterval) {
                continue; // Skip this metric
            }
        }
        
        // Update metric
        double value = 0.0;
        if (metric.getValue) {
            value = metric.getValue();
        }
        
        pImpl->lastUpdateTimes[metric.name] = now;
        
        // Add to real-time metrics
        realTimeMetrics_.emplace_back(metric.name, value);
        
        // Add to history if tracking is enabled
        if (metric.trackHistory) {
            pImpl->realTimeHistory[metric.name].emplace_back(metric.name, value);
            
            // Keep only recent history (last 1000 entries per metric)
            auto& history = pImpl->realTimeHistory[metric.name];
            if (history.size() > 1000) {
                history.erase(history.begin(), history.begin() + (history.size() - 1000));
            }
        }
        
        pImpl->updateCount++;
    }
}

void MonitoringManager::processMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Calculate derived metrics
    if (performanceMetrics_.neuronCount > 0) {
        performanceMetrics_.pendingSpikes = pImpl->updateCount % 1000; // Simulated value
        performanceMetrics_.pendingDelayedSpikes = pImpl->updateCount % 500; // Simulated value
    }
    
    // Calculate performance metrics
    static auto lastStepTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStepTime);
    
    if (stepDuration.count() > 0) {
        performanceMetrics_.stepTimeMs = static_cast<double>(stepDuration.count());
        performanceMetrics_.realTimeFactor = (1000.0 / performanceMetrics_.stepTimeMs); // Simulated
        performanceMetrics_.neuronsPerSecond = (performanceMetrics_.neuronCount * 1000.0) / std::max(1.0, performanceMetrics_.stepTimeMs);
        performanceMetrics_.spikesPerSecond = (performanceMetrics_.totalSpikes * 1000.0) / std::max(1.0, performanceMetrics_.stepTimeMs);
    }
    
    lastStepTime = now;
}

void MonitoringManager::logMetrics() const {
    if (!config_.enableLogging) {
        return;
    }
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    for (const auto& pair : pImpl->metricDefinitions) {
        const auto& metric = pair.second;
        
        // Find the latest value
        std::optional<double> value = getMetricValue(metric.name);
        if (!value) {
            continue;
        }
        
        // Check if we should log this metric
        if (metric.logLevel < config_.minimumLogLevel) {
            continue;
        }
        
        std::string logMessage = metric.name + ": " + std::to_string(*value);
        if (!metric.unit.empty()) {
            logMessage += " " + metric.unit;
        }
        if (!metric.label.empty()) {
            logMessage += " (" + metric.label + ")";
        }
        
        switch (metric.logLevel) {
            case LogLevel::Debug:
                NLM_LOG_DEBUG(logMessage);
                break;
            case LogLevel::Info:
                NLM_LOG_INFO(logMessage);
                break;
            case LogLevel::Warning:
                NLM_LOG_WARNING(logMessage);
                break;
            case LogLevel::Error:
                NLM_LOG_ERROR(logMessage);
                break;
            case LogLevel::Critical:
                NLM_LOG_CRITICAL(logMessage);
                break;
        }
    }
}

void MonitoringManager::sendNotifications() const {
    if (!config_.enableNotifications || !config_.notificationCallback) {
        return;
    }
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    // Send notifications for significant changes
    static size_t lastTotalSpikes = 0;
    if (performanceMetrics_.totalSpikes != lastTotalSpikes) {
        if (performanceMetrics_.totalSpikes > 0 && (performanceMetrics_.totalSpikes % 1000) == 0) {
            config_.notificationCallback("Milestone reached: " + std::to_string(performanceMetrics_.totalSpikes) + " total spikes");
        }
        lastTotalSpikes = performanceMetrics_.totalSpikes;
    }
    
    // Send performance warning if CPU usage is high
    if (performanceMetrics_.cpuUsage > 80.0) {
        config_.notificationCallback("High CPU usage: " + std::to_string(performanceMetrics_.cpuUsage) + "%");
    }
    
    // Send memory warning if memory usage is high
    if (performanceMetrics_.memoryUsagePercent > 90.0) {
        config_.notificationCallback("High memory usage: " + std::to_string(performanceMetrics_.memoryUsagePercent) + "%");
    }
}

std::optional<double> MonitoringManager::getMetricValue(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    // First check real-time metrics
    for (const auto& metric : realTimeMetrics_) {
        if (metric.name == name) {
            return metric.value;
        }
    }
    
    // Check metric definitions for getters
    auto it = pImpl->metricDefinitions.find(name);
    if (it != pImpl->metricDefinitions.end() && it->second.getValue) {
        return it->second.getValue();
    }
    
    return std::optional<double>();
}

MonitoringStats MonitoringManager::getStats() const {
    MonitoringStats stats;
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    stats.totalMetrics = pImpl->metricDefinitions.size();
    stats.activeMetrics = realTimeMetrics_.size();
    stats.realTimeMetrics = realTimeMetrics_.size();
    stats.cpuUsageAvg = performanceMetrics_.cpuUsage; // In real implementation, would be averaged over time
    stats.memoryUsageAvg = performanceMetrics_.memoryUsageMB;
    stats.simulationSpeedAvg = performanceMetrics_.neuronsPerSecond;
    stats.totalSteps = performanceMetrics_.currentStep;
    stats.totalSimulationTime = performanceMetrics_.currentTime;
    stats.recordingStartTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - pImpl->startTime).count();
    
    return stats;
}

std::vector<std::string> MonitoringManager::getMetricNames() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(pImpl->metricDefinitions.size());
    for (const auto& pair : pImpl->metricDefinitions) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<RealTimeMetric> MonitoringManager::getRealTimeHistory(const std::string& name, size_t limit) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    auto it = pImpl->realTimeHistory.find(name);
    if (it == pImpl->realTimeHistory.end()) {
        return {};
    }
    
    std::vector<RealTimeMetric> result = it->second;
    
    if (limit > 0 && limit < result.size()) {
        auto start = result.end() - limit;
        result = std::vector<RealTimeMetric>(start, result.end());
    }
    
    return result;
}

std::string MonitoringManager::getFormattedMetrics() const {
    std::ostringstream oss;
    oss << "=== MonitoringManager Metrics ===\n";
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    for (const auto& pair : pImpl->metricDefinitions) {
        const auto& metric = pair.second;
        std::optional<double> value = getMetricValue(metric.name);
        
        oss << std::left << std::setw(30) << metric.name << ": ";
        if (value.has_value()) {
            oss << std::setw(10) << std::fixed << std::setprecision(3) << value.value();
            if (!metric.unit.empty()) {
                oss << " " << metric.unit;
            }
        } else {
            oss << "N/A";
        }
        oss << " - " << metric.description << "\n";
    }
    
    oss << "\n" << getStats().toString() << "\n";
    return oss.str();
}

std::string MonitoringManager::getFormattedPerformanceMetrics() const {
    std::ostringstream oss;
    oss << "=== Performance Metrics ===\n";
    oss << std::left << std::setw(30) << "CPU Usage:" << std::setw(10) << std::fixed << std::setprecision(1) << performanceMetrics_.cpuUsage << "%\n";
    oss << std::left << std::setw(30) << "Memory Usage:" << std::setw(10) << std::fixed << std::setprecision(1) << performanceMetrics_.memoryUsageMB << " MB (" << performanceMetrics_.memoryUsagePercent << "%)\n";
    oss << std::left << std::setw(30) << "Neurons:" << std::setw(10) << performanceMetrics_.neuronCount << "\n";
    oss << std::left << std::setw(30) << "Synapses:" << std::setw(10) << performanceMetrics_.synapseCount << "\n";
    oss << std::left << std::setw(30) << "Active Neurons:" << std::setw(10) << performanceMetrics_.activeNeurons << "\n";
    oss << std::left << std::setw(30) << "Firing Neurons:" << std::setw(10) << performanceMetrics_.firingNeurons << "\n";
    oss << std::left << std::setw(30) << "Total Spikes:" << std::setw(10) << performanceMetrics_.totalSpikes << "\n";
    oss << std::left << std::setw(30) << "Avg Firing Rate:" << std::setw(10) << std::fixed << std::setprecision(2) << performanceMetrics_.avgFiringRate << " Hz\n";
    oss << std::left << std::setw(30) << "E/I Ratio:" << std::setw(10) << std::fixed << std::setprecision(3) << performanceMetrics_.e_iRatio << "\n";
    oss << std::left << std::setw(30) << "Step Time:" << std::setw(10) << std::fixed << std::setprecision(2) << performanceMetrics_.stepTimeMs << " ms\n";
    oss << std::left << std::setw(30) << "Real-time Factor:" << std::setw(10) << std::fixed << std::setprecision(3) << performanceMetrics_.realTimeFactor << "\n";
    oss << std::left << std::setw(30) << "Current Step:" << std::setw(10) << performanceMetrics_.currentStep << "\n";
    oss << std::left << std::setw(30) << "Current Time:" << std::setw(10) << std::fixed << std::setprecision(3) << performanceMetrics_.currentTime << " s\n";
    
    return oss.str();
}

std::string MonitoringManager::getFormattedRealTimeMetrics() const {
    std::ostringstream oss;
    oss << "=== Real-time Metrics ===\n";
    oss << std::left << std::setw(30) << "Metric" << std::setw(15) << "Value" << std::setw(10) << "Time" << "\n";
    oss << std::string(65, '-') << "\n";
    
    for (const auto& metric : realTimeMetrics_) {
        auto timestamp = metric.timestamp;
        auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch());
        oss << std::left << std::setw(30) << metric.name << std::setw(15) << std::fixed << std::setprecision(3) << metric.value;
        oss << " " << timeSinceEpoch.count() << "ms\n";
    }
    
    return oss.str();
}

bool MonitoringManager::exportMetrics(const std::string& filename, const std::string& format) {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "csv") {
        return exportToCSV(filename);
    } else if (format == "json") {
        return exportToJSON(filename);
    } else if (format == "xml") {
        return exportToXML(filename);
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + format);
        return false;
    }
}

bool MonitoringManager::exportPerformanceMetrics(const std::string& filename, const std::string& format) {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "json") {
        // Export performance metrics as JSON
        nlohmann::json jsonData;
        
        jsonData["cpu_usage"] = performanceMetrics_.cpuUsage;
        jsonData["memory_usage_mb"] = performanceMetrics_.memoryUsageMB;
        jsonData["memory_usage_percent"] = performanceMetrics_.memoryUsagePercent;
        jsonData["neuron_count"] = performanceMetrics_.neuronCount;
        jsonData["synapse_count"] = performanceMetrics_.synapseCount;
        jsonData["active_neurons"] = performanceMetrics_.activeNeurons;
        jsonData["firing_neurons"] = performanceMetrics_.firingNeurons;
        jsonData["total_spikes"] = performanceMetrics_.totalSpikes;
        jsonData["avg_firing_rate"] = performanceMetrics_.avgFiringRate;
        jsonData["e_i_ratio"] = performanceMetrics_.e_iRatio;
        jsonData["step_time_ms"] = performanceMetrics_.stepTimeMs;
        jsonData["real_time_factor"] = performanceMetrics_.realTimeFactor;
        jsonData["neurons_per_second"] = performanceMetrics_.neuronsPerSecond;
        jsonData["spikes_per_second"] = performanceMetrics_.spikesPerSecond;
        jsonData["current_step"] = performanceMetrics_.currentStep;
        jsonData["current_time"] = performanceMetrics_.currentTime;
        
        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << jsonData.dump(2);
        file.close();
        
        NLM_LOG_INFO("Performance metrics exported to " + filename);
        return true;
    }
    
    NLM_LOG_ERROR("Unsupported export format: " + format);
    return false;
}

void MonitoringManager::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->realTimeHistory.clear();
    NLM_LOG_INFO("Real-time metrics history cleared");
}

void MonitoringManager::clearOldData(const std::chrono::milliseconds& age) {
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoff = now - age;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    for (auto& pair : pImpl->realTimeHistory) {
        auto& history = pair.second;
        auto newEnd = std::remove_if(history.begin(), history.end(),
            [cutoff](const RealTimeMetric& metric) {
                return metric.timestamp < cutoff;
            });
        history.erase(newEnd, history.end());
    }
    
    NLM_LOG_INFO("Old metrics data cleared (older than " + std::to_string(age.count()) + " ms)");
}

void MonitoringManager::setUpdateInterval(const std::chrono::milliseconds& interval) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_.updateInterval = interval;
    NLM_LOG_INFO("Update interval set to " + std::to_string(interval.count()) + " ms");
}

void MonitoringManager::writeToFile(const std::string& filename, const std::string& data) const {
    if (!config_.enableFileOutput) {
        return;
    }
    
    std::ofstream file;
    if (config_.outputFile.empty()) {
        file.open(filename, std::ios::app);
    } else {
        file.open(config_.outputFile, std::ios::app);
    }
    
    if (file.is_open()) {
        auto timestamp = std::chrono::high_resolution_clock::now();
        auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch());
        file << "[" << timeSinceEpoch.count() << "ms] " << data << "\n";
        file.close();
    }
}

bool MonitoringManager::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Write header
    file << "Timestamp,Metric,Value\n";
    
    // Get current time for timestamp
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
    // Write real-time metrics
    for (const auto& metric : realTimeMetrics_) {
        file << timestamp.count() << "," << metric.name << "," << std::fixed << std::setprecision(6) << metric.value << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Metrics exported to CSV: " + filename);
    return true;
}

bool MonitoringManager::exportToJSON(const std::string& filename) const {
    nlohmann::json jsonData;
    
    // Add real-time metrics
    jsonData["real_time_metrics"] = nlohmann::json::array();
    for (const auto& metric : realTimeMetrics_) {
        nlohmann::json metricJson;
        metricJson["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(metric.timestamp.time_since_epoch()).count();
        metricJson["name"] = metric.name;
        metricJson["value"] = metric.value;
        metricJson["label"] = metric.label;
        jsonData["real_time_metrics"].push_back(metricJson);
    }
    
    // Add performance metrics
    nlohmann::json perfJson;
    perfJson["cpu_usage"] = performanceMetrics_.cpuUsage;
    perfJson["memory_usage_mb"] = performanceMetrics_.memoryUsageMB;
    perfJson["neuron_count"] = performanceMetrics_.neuronCount;
    perfJson["synapse_count"] = performanceMetrics_.synapseCount;
    perfJson["active_neurons"] = performanceMetrics_.activeNeurons;
    perfJson["total_spikes"] = performanceMetrics_.totalSpikes;
    perfJson["avg_firing_rate"] = performanceMetrics_.avgFiringRate;
    jsonData["performance_metrics"] = perfJson;
    
    // Write to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << jsonData.dump(2);
    file.close();
    
    NLM_LOG_INFO("Metrics exported to JSON: " + filename);
    return true;
}

bool MonitoringManager::exportToXML(const std::string& filename) const {
    // XML export implementation
    NLM_LOG_WARNING("XML export not yet implemented");
    return false;
}

void MonitoringManager::updateGlobal(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    getInstance().update(brain, currentTime, currentStep);
}

void MonitoringManager::recordGlobalMetric(const std::string& name, double value, const std::string& label) {
    getInstance().recordGlobalMetric(name, value, label);
}

} // namespace nlm
