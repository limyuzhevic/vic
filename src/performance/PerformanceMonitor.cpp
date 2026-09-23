#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace nlm {

struct PerformanceMonitor::Impl {
    PerformanceConfig config;
    bool active;
    std::chrono::system_clock::time_point startTime;
    std::thread resourceThread;
    
    // Current state
    SimulationProgress progress;
    NeuralMetrics neuralMetrics;
    ResourceUsage resourceUsage;
    
    // Historical data
    std::vector<std::pair<double, double>> progressHistory;
    std::vector<std::pair<double, double>> neuralHistory;
    std::vector<std::pair<double, double>> resourceHistory;
    std::unordered_map<std::string, std::vector<std::pair<double, double>>> customHistory;
    
    // Custom metrics
    std::unordered_map<std::string, double> currentCustomMetrics;
    
    // Alerts
    std::vector<PerformanceAlert> alerts;
    mutable std::mutex alertsMutex;
    
    // Statistics
    std::atomic<size_t> totalUpdates{0};
    
    Impl() : active(false) {}
    
    void stop() {
        active = false;
        if (resourceThread.joinable()) {
            resourceThread.join();
        }
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() {
    pImpl->stop();
}

void PerformanceMonitor::configure(const PerformanceConfig& config) {
    pImpl->config = config;
}

void PerformanceMonitor::start() {
    if (pImpl->active) return;
    
    pImpl->active = true;
    pImpl->startTime = std::chrono::system_clock::now();
    
    // Start resource monitoring thread if enabled
    if (pImpl->config.enableResourceMonitoring) {
        pImpl->resourceThread = std::thread([this]() {
            while (pImpl->active) {
                updateResource();
                std::this_thread::sleep_for(std::chrono::seconds(
                    static_cast<int>(pImpl->config.resourceCheckInterval)));
            }
        });
    }
}

void PerformanceMonitor::stop() {
    pImpl->stop();
}

bool PerformanceMonitor::isActive() const {
    return pImpl->active;
}

void PerformanceMonitor::setProgress(SimulationStep currentStep, 
                                     SimulationStep totalSteps,
                                     double simulationTime) {
    if (!pImpl->active) return;
    
    pImpl->progress.currentStep = currentStep;
    pImpl->progress.totalSteps = totalSteps;
    pImpl->progress.simulationTime = simulationTime;
    pImpl->progress.lastUpdateTime = std::chrono::system_clock::now();
    
    // Calculate progress percentage
    if (totalSteps > 0) {
        pImpl->progress.progressPercent = 
            static_cast<double>(currentStep) / totalSteps * 100.0;
    }
    
    // Estimate time remaining
    auto elapsed = std::chrono::duration<double>(pImpl->progress.lastUpdateTime - 
                                                 pImpl->progress.startTime).count();
    if (elapsed > 0.0 && currentStep > 0) {
        double timePerStep = elapsed / currentStep;
        double remainingSteps = totalSteps - currentStep;
        pImpl->progress.estimatedTimeRemaining = timePerStep * remainingSteps;
    }
    
    // Store historical data
    updateProgress();
    pImpl->totalUpdates++;
}

void PerformanceMonitor::updateNeuralMetrics(const NeuralMetrics& metrics) {
    if (!pImpl->active) return;
    
    pImpl->neuralMetrics = metrics;
    updateProgress();
    pImpl->totalUpdates++;
}

void PerformanceMonitor::updateResourceUsage(const ResourceUsage& usage) {
    if (!pImpl->active) return;
    
    pImpl->resourceUsage = usage;
    
    // Store historical data
    auto now = std::chrono::system_clock::now();
    double timestamp = std::chrono::duration<double>(now - pImpl->startTime).count();
    pImpl->resourceHistory.emplace_back(timestamp, usage.memoryUsageMB);
    
    // Trim history if too large
    if (pImpl->resourceHistory.size() > pImpl->config.maxHistoricalDataPoints) {
        pImpl->resourceHistory.erase(pImpl->resourceHistory.begin());
    }
    
    pImpl->totalUpdates++;
}

void PerformanceMonitor::addCustomMetric(const std::string& name, double value) {
    if (!pImpl->active) return;
    
    pImpl->currentCustomMetrics[name] = value;
    
    // Store historical data
    auto now = std::chrono::system_clock::now();
    double timestamp = std::chrono::duration<double>(now - pImpl->startTime).count();
    pImpl->customHistory[name].emplace_back(timestamp, value);
    
    // Trim history if too large
    if (pImpl->customHistory[name].size() > pImpl->config.maxHistoricalDataPoints) {
        pImpl->customHistory[name].erase(pImpl->customHistory[name].begin());
    }
    
    pImpl->totalUpdates++;
}

SimulationProgress PerformanceMonitor::getProgress() const {
    return pImpl->progress;
}

NeuralMetrics PerformanceMonitor::getNeuralMetrics() const {
    return pImpl->neuralMetrics;
}

ResourceUsage PerformanceMonitor::getResourceUsage() const {
    return pImpl->resourceUsage;
}

double PerformanceMonitor::getCustomMetric(const std::string& name) const {
    auto it = pImpl->currentCustomMetrics.find(name);
    return (it != pImpl->currentCustomMetrics.end()) ? it->second : 0.0;
}

std::unordered_map<std::string, double> PerformanceMonitor::getCustomMetrics() const {
    return pImpl->currentCustomMetrics;
}

std::vector<PerformanceAlert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    return pImpl->alerts;
}

std::vector<PerformanceAlert> PerformanceMonitor::checkForIssues() {
    std::vector<PerformanceAlert> newAlerts;
    
    if (!pImpl->config.enableAlerting) return newAlerts;
    
    // Check memory usage
    if (pImpl->resourceUsage.memoryUsageMB > pImpl->config.memoryWarningThresholdMB) {
        newAlerts.emplace_back(AlertLevel::Warning,
            "Memory usage is above warning threshold",
            "memory_usage_mb", static_cast<double>(pImpl->resourceUsage.memoryUsageMB),
            pImpl->config.memoryWarningThresholdMB);
    }
    
    // Check CPU usage (simulated)
    if (pImpl->resourceUsage.cpuUsagePercent > pImpl->config.cpuWarningThresholdPercent) {
        newAlerts.emplace_back(AlertLevel::Warning,
            "CPU usage is above warning threshold",
            "cpu_usage_percent", pImpl->resourceUsage.cpuUsagePercent,
            pImpl->config.cpuWarningThresholdPercent);
    }
    
    // Check simulation time
    double elapsedHours = pImpl->progress.simulationTime / 3600.0;
    if (elapsedHours > pImpl->config.simulationTimeWarningHrs) {
        newAlerts.emplace_back(AlertLevel::Critical,
            "Simulation time is approaching warning threshold",
            "simulation_time_hours", elapsedHours,
            pImpl->config.simulationTimeWarningHrs);
    }
    
    // Add new alerts
    if (!newAlerts.empty()) {
        std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
        pImpl->alerts.insert(pImpl->alerts.end(), newAlerts.begin(), newAlerts.end());
    }
    
    return newAlerts;
}

std::string PerformanceMonitor::getSummary() const {
    std::ostringstream oss;
    oss << "=== Performance Summary ===" << std::endl;
    oss << "Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    oss << "Total updates: " << pImpl->totalUpdates << std::endl;
    
    if (pImpl->progress.totalSteps > 0) {
        oss << std::endl;
        oss << "=== Simulation Progress ===" << std::endl;
        oss << "Step: " << pImpl->progress.currentStep 
            << " / " << pImpl->progress.totalSteps << std::endl;
        oss << "Progress: " << std::fixed << std::setprecision(2)
            << pImpl->progress.progressPercent << "%" << std::endl;
        oss << "Simulation time: " << formatTime(pImpl->progress.simulationTime) << std::endl;
        oss << "Estimated time remaining: " << formatTime(pImpl->progress.estimatedTimeRemaining) << std::endl;
    }
    
    if (pImpl->neuralMetrics.totalNeurons > 0) {
        oss << std::endl;
        oss << "=== Neural Activity ===" << std::endl;
        oss << "Total neurons: " << pImpl->neuralMetrics.totalNeurons << std::endl;
        oss << "Active neurons: " << pImpl->neuralMetrics.activeNeurons << std::endl;
        oss << "Firing neurons: " << pImpl->neuralMetrics.firingNeurons << std::endl;
        oss << "Average firing rate: " << std::fixed << std::setprecision(3)
            << pImpl->neuralMetrics.averageFiringRate << " Hz" << std::endl;
        oss << "Total spikes: " << pImpl->neuralMetrics.totalSpikes << std::endl;
        oss << "Plasticity events: " << pImpl->neuralMetrics.plasticityEvents << std::endl;
    }
    
    oss << std::endl;
    oss << "=== Resource Usage ===" << std::endl;
    oss << "Memory: " << pImpl->resourceUsage.memoryUsageMB << " MB" << std::endl;
    oss << "Virtual memory: " << pImpl->resourceUsage.virtualMemoryUsageMB << " MB" << std::endl;
    oss << "CPU usage: " << std::fixed << std::setprecision(1)
        << pImpl->resourceUsage.cpuUsagePercent << "%" << std::endl;
    
    auto alerts = getAlerts();
    if (!alerts.empty()) {
        oss << std::endl;
        oss << "=== Performance Alerts ===" << std::endl;
        for (const auto& alert : alerts) {
            oss << "[" << static_cast<int>(alert.level) << "] " << alert.message
                << " (metric: " << alert.metricName << ")" << std::endl;
        }
    }
    
    return oss.str();
}

std::vector<std::pair<double, double>> PerformanceMonitor::getHistoricalData(const std::string& metricName) const {
    if (metricName == "progress") {
        return pImpl->progressHistory;
    } else if (metricName == "neural") {
        return pImpl->neuralHistory;
    } else if (metricName == "resource") {
        return pImpl->resourceHistory;
    } else {
        auto it = pImpl->customHistory.find(metricName);
        return (it != pImpl->customHistory.end()) ? it->second : std::vector<std::pair<double, double>>();
    }
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Performance Metrics Export" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    file << std::endl;
    
    // Export progress data
    file << "Progress History:" << std::endl;
    for (const auto& [time, value] : pImpl->progressHistory) {
        file << time << "," << value << std::endl;
    }
    file << std::endl;
    
    // Export neural metrics
    file << "Neural Metrics:" << std::endl;
    file << "totalNeurons," << pImpl->neuralMetrics.totalNeurons << std::endl;
    file << "activeNeurons," << pImpl->neuralMetrics.activeNeurons << std::endl;
    file << "firingNeurons," << pImpl->neuralMetrics.firingNeurons << std::endl;
    file << "averageFiringRate," << pImpl->neuralMetrics.averageFiringRate << std::endl;
    file << std::endl;
    
    // Export resource data
    file << "Resource History:" << std::endl;
    for (const auto& [time, value] : pImpl->resourceHistory) {
        file << time << "," << value << std::endl;
    }
    file << std::endl;
    
    // Export custom metrics
    file << "Custom Metrics:" << std::endl;
    for (const auto& [name, value] : pImpl->currentCustomMetrics) {
        file << name << "," << value << std::endl;
    }
    
    file.close();
    return true;
}

void PerformanceMonitor::clearHistory() {
    pImpl->progressHistory.clear();
    pImpl->neuralHistory.clear();
    pImpl->resourceHistory.clear();
    pImpl->customHistory.clear();
}

PerformanceMonitor& PerformanceMonitor::getGlobal() {
    static std::shared_ptr<PerformanceMonitor> globalInstance;
    if (!globalInstance) {
        globalInstance = std::make_shared<PerformanceMonitor>();
    }
    return *globalInstance;
}

void PerformanceMonitor::setGlobal(std::shared_ptr<PerformanceMonitor> monitor) {
    // This is a simplified implementation - in production you'd want better handling
    static std::shared_ptr<PerformanceMonitor> globalInstance = monitor;
}

void PerformanceMonitor::updateProgress() {
    auto now = std::chrono::system_clock::now();
    double timestamp = std::chrono::duration<double>(now - pImpl->startTime).count();
    
    // Store neural metrics
    pImpl->neuralHistory.emplace_back(timestamp, pImpl->neuralMetrics.totalSpikes);
    
    // Trim history if too large
    if (pImpl->neuralHistory.size() > pImpl->config.maxHistoricalDataPoints) {
        pImpl->neuralHistory.erase(pImpl->neuralHistory.begin());
    }
}

void PerformanceMonitor::updateResource() {
    // In a real implementation, this would read actual system metrics
    // For now, we'll simulate with some basic values
    static int counter = 0;
    counter++;
    
    ResourceUsage usage;
    usage.cpuUsagePercent = 50.0 + std::sin(counter * 0.1) * 20.0;  // Oscillating between 30-70%
    usage.memoryUsageMB = 512 + counter * 10;  // Gradually increasing
    usage.virtualMemoryUsageMB = 1024 + counter * 20;
    usage.fileDescriptors = 100 + counter * 5;
    usage.timestamp = std::chrono::system_clock::now();
    
    updateResourceUsage(usage);
}

std::string PerformanceMonitor::formatTime(double seconds) const {
    std::ostringstream oss;
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    int secs = static_cast<int>(seconds) % 60;
    
    if (hours > 0) {
        oss << hours << "h " << minutes << "m " << secs << "s";
    } else if (minutes > 0) {
        oss << minutes << "m " << secs << "s";
    } else {
        oss << secs << "s";
    }
    
    return oss.str();
}

} // namespace nlm
