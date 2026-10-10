#include "DebugTools.hpp"

namespace nlm {

struct RealTimeMonitor::Impl {
    VisualizationInterface* visualizationInterface;
    const Brain* monitoringBrain;
    double updateInterval;
    bool monitoring;
    std::thread monitorThread;
    std::mutex dataMutex;
    std::condition_variable dataCondition;
    std::unordered_map<std::string, std::deque<std::pair<double, double>>> monitoringData;
    std::unordered_map<std::string, double> alertThresholds;
    std::unordered_map<std::string, int> alertLevels;
    std::function<void(const std::string&, double)> alertCallback;
    std::function<void(const std::unordered_map<std::string, double>&)> updateCallback;
    double lastUpdateTime;
    
    Impl() : visualizationInterface(nullptr), monitoringBrain(nullptr), updateInterval(0.1),
             monitoring(false), lastUpdateTime(0.0) {}
};

RealTimeMonitor::RealTimeMonitor() : pImpl(std::make_unique<Impl>()) {}

RealTimeMonitor::~RealTimeMonitor() {
    if (pImpl->monitoring) {
        stopMonitoring();
    }
}

RealTimeMonitor::RealTimeMonitor(RealTimeMonitor&&) noexcept = default;
RealTimeMonitor& RealTimeMonitor::operator=(RealTimeMonitor&&) noexcept = default;

bool RealTimeMonitor::initialize(VisualizationInterface& visualizationInterface) {
    pImpl->visualizationInterface = &visualizationInterface;
    return true;
}

bool RealTimeMonitor::startMonitoring(const Brain& brain, double updateInterval) {
    if (pImpl->monitoring) {
        return false;
    }
    
    pImpl->monitoringBrain = &brain;
    pImpl->updateInterval = updateInterval;
    pImpl->monitoring = true;
    
    pImpl->monitorThread = std::thread([this]() {
        while (pImpl->monitoring && pImpl->monitoringBrain) {
            auto startTime = std::chrono::steady_clock::now();
            
            // Collect monitoring data
            auto monitoringData = getMonitoringStatus();
            
            // Check for alerts
            for (const auto& [metricName, value] : monitoringData) {
                auto thresholdIt = pImpl->alertThresholds.find(metricName);
                if (thresholdIt != pImpl->alertThresholds.end() && value > thresholdIt->second) {
                    if (pImpl->alertCallback) {
                        pImpl->alertCallback(metricName, value);
                    }
                }
            }
            
            if (pImpl->updateCallback) {
                pImpl->updateCallback(monitoringData);
            }
            
            // Wait for next update
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            double sleepTime = std::max(0.0, pImpl->updateInterval - duration);
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    });
    
    return true;
}

bool RealTimeMonitor::stopMonitoring() {
    if (!pImpl->monitoring) {
        return false;
    }
    
    pImpl->monitoring = false;
    pImpl->dataCondition.notify_all();
    
    if (pImpl->monitorThread.joinable()) {
        pImpl->monitorThread.join();
    }
    
    pImpl->monitoringBrain = nullptr;
    return true;
}

void RealTimeMonitor::addMonitoringTarget(const std::string& monitorType, const std::string& target) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->monitoringData[monitorType + "_" + target];
}

void RealTimeMonitor::removeMonitoringTarget(const std::string& monitorType, const std::string& target) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->monitoringData.erase(monitorType + "_" + target);
}

void RealTimeMonitor::setAlertThreshold(const std::string& metricName, double threshold, int level) {
    pImpl->alertThresholds[metricName] = threshold;
    pImpl->alertLevels[metricName] = level;
}

std::unordered_map<std::string, double> RealTimeMonitor::getMonitoringStatus() const {
    std::unordered_map<std::string, double> status;
    
    if (!pImpl->monitoringBrain) {
        return status;
    }
    
    // Get basic brain metrics
    status["total_neurons"] = static_cast<double>(pImpl->monitoringBrain->getTotalNeuronCount());
    status["total_spikes"] = static_cast<double>(pImpl->monitoringBrain->getTotalSpikeCount());
    status["total_synapses"] = static_cast<double>(pImpl->monitoringBrain->getTotalSynapseCount());
    status["firing_rate"] = static_cast<double>(pImpl->monitoringBrain->getFiringNeuronCount()) /
                          std::max(1.0, static_cast<double>(pImpl->monitoringBrain->getTotalNeuronCount()));
    
    return status;
}

bool RealTimeMonitor::isMonitoring() const {
    return pImpl->monitoring;
}

std::vector<std::pair<double, double>> RealTimeMonitor::getRecentData(const std::string& monitorType, size_t count) const {
    std::vector<std::pair<double, double>> recentData;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    auto it = pImpl->monitoringData.find(monitorType);
    if (it != pImpl->monitoringData.end() && !it->second.empty()) {
        auto dataPoints = it->second;
        recentData.assign(dataPoints.end() - std::min(count, dataPoints.size()), dataPoints.end());
    }
    
    return recentData;
}

bool RealTimeMonitor::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export monitoring data in specified format
    return false;
}

void RealTimeMonitor::setAlertCallback(std::function<void(const std::string&, double)> callback) {
    pImpl->alertCallback = callback;
}

void RealTimeMonitor::setUpdateCallback(std::function<void(const std::unordered_map<std::string, double>&)> callback) {
    pImpl->updateCallback = callback;
}

} // namespace nlm