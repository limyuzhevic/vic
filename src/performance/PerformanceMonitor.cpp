// PerformanceMonitor.cpp
#include "PerformanceMonitor.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <random>

using json = nlohmann::json;

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : running_(false) {
    // Initialize with default thresholds
    setThreshold("step_time", 1000.0);      // 1000ms
    setThreshold("spikes", 10000);         // 10k spikes
    setThreshold("active_neurons", 100000); // 100k neurons
    setThreshold("memory", 1024 * 1024 * 100); // 100MB
    setThreshold("cpu", 90.0);              // 90%
    
    regressionThreshold_ = 0.2; // 20%
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::start() {
    if (running_) return;
    
    running_ = true;
    
    // Reset statistics
    currentStats_.averageStepTime = 0;
    currentStats_.maxStepTime = 0;
    currentStats_.minStepTime = std::numeric_limits<double>::max();
    currentStats_.stepTimeStandardDeviation = 0;
    currentStats_.totalSteps = 0;
    currentStats_.successfulSteps = 0;
    currentStats_.successRate = 0;
    currentStats_.peakMemoryUsage = 0;
    currentStats_.averageMemoryUsage = 0;
    currentStats_.averageCPUUsage = 0;
    currentStats_.peakCPUUsage = 0;
    currentStats_.averageSpikesPerStep = 0;
    currentStats_.averageActiveNeurons = 0;
    currentStats_.performanceTrend = 0;
    currentStats_.stabilityScore = 0;
    currentStats_.efficiencyScore = 0;
}

void PerformanceMonitor::stop() {
    running_ = false;
}

void PerformanceMonitor::recordStep(double stepTime, size_t spikes, size_t activeNeurons,
                                   size_t memoryUsage, double cpuTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    currentStats_.totalSteps++;
    
    if (stepTime < currentStats_.minStepTime) {
        currentStats_.minStepTime = stepTime;
    }
    currentStats_.maxStepTime = std::max(currentStats_.maxStepTime, stepTime);
    currentStats_.averageStepTime += stepTime;
    
    // Calculate running standard deviation
    double deviation = stepTime - currentStats_.averageStepTime / currentStats_.totalSteps;
    currentStats_.stepTimeStandardDeviation += deviation * deviation;
    
    currentStats_.averageSpikesPerStep += static_cast<double>(spikes);
    currentStats_.averageActiveNeurons += static_cast<double>(activeNeurons);
    currentStats_.peakMemoryUsage = std::max(currentStats_.peakMemoryUsage, memoryUsage);
    currentStats_.averageMemoryUsage += memoryUsage;
    currentStats_.averageCPUUsage += cpuTime;
    currentStats_.peakCPUUsage = std::max(currentStats_.peakCPUUsage, cpuTime);
    
    // Check step success (based on thresholds)
    bool success = true;
    for (const auto& pair : thresholds_) {
        if (pair.first == "step_time" && stepTime > pair.second) {
            success = false;
            addAlert(Alert("step_time_high_" + std::to_string(currentStats_.totalSteps), 
                          "threshold", "step_time", stepTime, pair.second, "warning",
                          "Step time exceeded threshold: " + std::to_string(stepTime) + "s > " + std::to_string(pair.second) + "s"));
        } else if (pair.first == "spikes" && spikes > static_cast<size_t>(pair.second)) {
            success = false;
            addAlert(Alert("spike_count_high_" + std::to_string(currentStats_.totalSteps), 
                          "threshold", "spikes", static_cast<double>(spikes), pair.second, "warning",
                          "Spike count exceeded threshold: " + std::to_string(spikes) + " > " + std::to_string(pair.second)));
        } else if (pair.first == "active_neurons" && activeNeurons > static_cast<size_t>(pair.second)) {
            success = false;
            addAlert(Alert("active_neurons_high_" + std::to_string(currentStats_.totalSteps), 
                          "threshold", "active_neurons", static_cast<double>(activeNeurons), pair.second, "warning",
                          "Active neuron count exceeded threshold: " + std::to_string(activeNeurons) + " > " + std::to_string(pair.second)));
        } else if (pair.first == "memory" && memoryUsage > static_cast<size_t>(pair.second)) {
            success = false;
            addAlert(Alert("memory_high_" + std::to_string(currentStats_.totalSteps), 
                          "threshold", "memory", static_cast<double>(memoryUsage), pair.second, "warning",
                          "Memory usage exceeded threshold: " + std::to_string(memoryUsage) + " > " + std::to_string(pair.second)));
        } else if (pair.first == "cpu" && cpuTime > pair.second) {
            success = false;
            addAlert(Alert("cpu_high_" + std::to_string(currentStats_.totalSteps), 
                          "threshold", "cpu", cpuTime, pair.second, "warning",
                          "CPU usage exceeded threshold: " + std::to_string(cpuTime) + "% > " + std::to_string(pair.second) + "%"));
        }
    }
    
    if (success) {
        currentStats_.successfulSteps++;
    }
    
    // Calculate success rate
    currentStats_.successRate = (currentStats_.successfulSteps / static_cast<double>(currentStats_.totalSteps)) * 100.0;
    
    // Calculate averages
    currentStats_.averageStepTime /= currentStats_.totalSteps;
    currentStats_.averageSpikesPerStep /= currentStats_.totalSteps;
    currentStats_.averageActiveNeurons /= currentStats_.totalSteps;
    currentStats_.averageMemoryUsage /= currentStats_.totalSteps;
    currentStats_.averageCPUUsage /= currentStats_.totalSteps;
    
    // Calculate standard deviation
    currentStats_.stepTimeStandardDeviation = std::sqrt(currentStats_.stepTimeStandardDeviation / currentStats_.totalSteps);
    
    // Calculate performance trend (simple moving average)
    if (currentStats_.totalSteps > 10) {
        currentStats_.performanceTrend = (currentStats_.averageStepTime - (currentStats_.averageStepTime / 10)) / (currentStats_.averageStepTime / 10);
    }
    
    // Check for performance regression
    checkForAlerts();
}

void PerformanceMonitor::addAlert(const Alert& alert) {
    alerts_.push_back(alert);
    
    // Remove old resolved alerts
    for (auto it = alerts_.begin(); it != alerts_.end(); ) {
        if (it->resolved) {
            it = alerts_.erase(it);
        } else {
            ++it;
        }
    }
}

void PerformanceMonitor::resolveAlert(const std::string& alertId) {
    for (auto& alert : alerts_) {
        if (alert.id == alertId) {
            alert.resolved = true;
            break;
        }
    }
}

void PerformanceMonitor::setThreshold(const std::string& metric, double threshold,
                                     const std::string& severity) {
    thresholds_[metric] = threshold;
}

void PerformanceMonitor::setRegressionThreshold(double threshold) {
    regressionThreshold_ = threshold;
}

std::string PerformanceMonitor::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "Performance Monitor Report\n";
    ss << "==========================\n\n";
    
    ss << "Performance Statistics:\n";
    ss << "  Total steps: " << currentStats_.totalSteps << "\n";
    ss << "  Success rate: " << currentStats_.successRate << "%\n";
    ss << "  Average step time: " << currentStats_.averageStepTime << "s\n";
    ss << "  Min step time: " << currentStats_.minStepTime << "s\n";
    ss << "  Max step time: " << currentStats_.maxStepTime << "s\n";
    ss << "  Step time standard deviation: " << currentStats_.stepTimeStandardDeviation << "s\n";
    ss << "  Average spikes per step: " << currentStats_.averageSpikesPerStep << "\n";
    ss << "  Average active neurons: " << currentStats_.averageActiveNeurons << "\n";
    ss << "  Peak memory usage: " << currentStats_.peakMemoryUsage / (1024 * 1024) << " MB\n";
    ss << "  Average memory usage: " << currentStats_.averageMemoryUsage / (1024 * 1024) << " MB\n";
    ss << "  Average CPU usage: " << currentStats_.averageCPUUsage << "%\n";
    ss << "  Peak CPU usage: " << currentStats_.peakCPUUsage << "%\n\n";
    
    // Calculate performance scores
    double stepTimeScore = std::max(0.0, 1.0 - (currentStats_.maxStepTime / 10.0)); // 10s max
    double memoryScore = std::max(0.0, 1.0 - (double(currentStats_.peakMemoryUsage) / (1024 * 1024 * 100)));
    double cpuScore = std::max(0.0, 1.0 - (currentStats_.peakCPUUsage / 100.0));
    
    currentStats_.efficiencyScore = (stepTimeScore + memoryScore + cpuScore) / 3.0 * 100.0;
    
    ss << "Performance Scores:\n";
    ss << "  Step time score: " << stepTimeScore * 100 << "%\n";
    ss << "  Memory score: " << memoryScore * 100 << "%\n";
    ss << "  CPU score: " << cpuScore * 100 << "%\n";
    ss << "  Overall efficiency score: " << currentStats_.efficiencyScore << "%\n\n";
    
    // Show recent alerts
    if (!alerts_.empty()) {
        ss << "Recent Alerts:\n";
        for (const auto& alert : alerts_) {
            if (!alert.resolved) {
                ss << "  [" << alert.severity << "] " << alert.message << " (" << alert.timestamp << "s)\n";
            }
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string PerformanceMonitor::generateDashboardData() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"performanceStats\": {\n";
    ss << "    \"totalSteps\": " << currentStats_.totalSteps << ",\n";
    ss << "    \"successRate\": " << currentStats_.successRate << ",\n";
    ss << "    \"averageStepTime\": " << currentStats_.averageStepTime << ",\n";
    ss << "    \"maxStepTime\": " << currentStats_.maxStepTime << ",\n";
    ss << "    \"averageSpikesPerStep\": " << currentStats_.averageSpikesPerStep << ",\n";
    ss << "    \"averageActiveNeurons\": " << currentStats_.averageActiveNeurons << ",\n";
    ss << "    \"peakMemoryUsage\": " << currentStats_.peakMemoryUsage << ",\n";
    ss << "    \"averageMemoryUsage\": " << currentStats_.averageMemoryUsage << ",\n";
    ss << "    \"averageCPUUsage\": " << currentStats_.averageCPUUsage << ",\n";
    ss << "    \"peakCPUUsage\": " << currentStats_.peakCPUUsage << ",\n";
    ss << "    \"efficiencyScore\": " << currentStats_.efficiencyScore << "\n";
    ss << "  },\n";
    ss << "  \"alerts\": [\n";
    
    for (size_t i = 0; i < alerts_.size(); ++i) {
        const auto& alert = alerts_[i];
        ss << "    {\n";
        ss << "      \"id\": \"" << alert.id << "\",\n";
        ss << "      \"type\": \"" << alert.type << "\",\n";
        ss << "      \"metric\": \"" << alert.metric << "\",\n";
        ss << "      \"value\": " << alert.value << ",\n";
        ss << "      \"threshold\": " << alert.threshold << ",\n";
        ss << "      \"severity\": \"" << alert.severity << "\",\n";
        ss << "      \"message\": \"" << alert.message << "\",\n";
        ss << "      \"timestamp\": " << alert.timestamp << ",\n";
        ss << "      \"resolved\": " << (alert.resolved ? "true" : "false") << "\n";
        ss << "    }";
        if (i < alerts_.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"thresholds\": {\n";
    
    for (size_t i = 0; i < thresholds_.size(); ++i) {
        const auto& pair = *std::next(thresholds_.begin(), i);
        ss << "    \"" << pair.first << "\": " << pair.second;
        if (i < thresholds_.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

bool PerformanceMonitor::saveAlerts(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["alerts"] = json::array();
    j["thresholds"] = thresholds_;
    j["regressionThreshold"] = regressionThreshold_;
    
    for (const auto& alert : alerts_) {
        json jAlert;
        jAlert["id"] = alert.id;
        jAlert["type"] = alert.type;
        jAlert["metric"] = alert.metric;
        jAlert["value"] = alert.value;
        jAlert["threshold"] = alert.threshold;
        jAlert["severity"] = alert.severity;
        jAlert["message"] = alert.message;
        jAlert["timestamp"] = alert.timestamp;
        jAlert["resolved"] = alert.resolved;
        j["alerts"].push_back(jAlert);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool PerformanceMonitor::loadAlerts(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearAlerts();
    
    // Load thresholds
    thresholds_ = j.value("thresholds", thresholds_);
    regressionThreshold_ = j.value("regressionThreshold", regressionThreshold_);
    
    // Load alerts
    for (const auto& jAlert : j["alerts"]) {
        Alert alert(
            jAlert["id"].get<std::string>(),
            jAlert["type"].get<std::string>(),
            jAlert["metric"].get<std::string>(),
            jAlert["value"].get<double>(),
            jAlert["threshold"].get<double>(),
            jAlert["severity"].get<std::string>(),
            jAlert["message"].get<std::string>()
        );
        alert.timestamp = jAlert.value("timestamp", 0.0);
        alert.resolved = jAlert.value("resolved", false);
        alerts_.push_back(alert);
    }
    
    file.close();
    return true;
}

void PerformanceMonitor::clearAlerts() {
    alerts_.clear();
}

void PerformanceMonitor::updateStats(double stepTime, size_t spikes, size_t activeNeurons,
                                    size_t memoryUsage, double cpuTime) {
    // Update statistics (simplified version)
    recordStep(stepTime, spikes, activeNeurons, memoryUsage, cpuTime);
}

void PerformanceMonitor::checkForAlerts() {
    // Check for performance regression
    if (currentStats_.totalSteps >= 10) {
        // Calculate regression from historical data (simplified)
        double historicalAverage = currentStats_.averageStepTime * 1.1; // Simulate worse performance
        double currentPerformance = currentStats_.averageStepTime;
        
        if (historicalAverage > 0) {
            double regression = (historicalAverage - currentPerformance) / historicalAverage;
            
            if (regression > regressionThreshold_) {
                addAlert(Alert("performance_regression_" + std::to_string(currentStats_.totalSteps),
                              "regression", "performance", currentPerformance, historicalAverage, "warning",
                              "Performance regression detected: " + std::to_string(regression * 100) + "% worse"));
            }
        }
    }
}

} // namespace nlm
