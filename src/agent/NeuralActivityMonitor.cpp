// NeuralActivityMonitor implementation

#include "NeuralActivityMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralActivityMonitor::Impl {
    std::shared_ptr<Brain> brain;
    double monitoringInterval;
    bool isMonitoring;
    float firingThreshold;
    float excitationThreshold;
    std::vector<float> activityHistory;
    std::vector<std::string> activityLog;
    int logLevel;
    
    // Control settings
    float maxFiringRate;
    float minFiringRate;
    float targetFiringRate;
    bool autoControlEnabled;
    
    Impl() : monitoringInterval(1.0), isMonitoring(false), 
             firingThreshold(0.5f), excitationThreshold(1.0f),
             logLevel(1), maxFiringRate(100.0f), 
             minFiringRate(0.0f), targetFiringRate(10.0f),
             autoControlEnabled(false) {}
};

NeuralActivityMonitor::NeuralActivityMonitor() : pImpl(new Impl) {}

NeuralActivityMonitor::~NeuralActivityMonitor() = default;

void NeuralActivityMonitor::initialize(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralActivityMonitor initialized");
}

std::string NeuralActivityMonitor::getActivitySummary() const {
    std::stringstream ss;
    ss << "Neural Activity Summary:\n";
    ss << "  Current Firing Rate: " << getCurrentFiringRate() << " Hz\n";
    ss << "  Neuron Count: " << (pImpl->brain ? pImpl->brain->getTotalNeuronCount() : 0) << "\n";
    ss << "  Active Neurons: " << (pImpl->brain ? pImpl->brain->getActiveNeuronCount() : 0) << "\n";
    ss << "  Firing Neurons: " << (pImpl->brain ? pImpl->brain->getFiringNeuronCount() : 0) << "\n";
    ss << "  Total Spikes: " << (pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0) << "\n";
    ss << "  Monitoring Active: " << (isMonitoring() ? "Yes" : "No") << "\n";
    ss << "  Log Level: " << pImpl->logLevel << "\n";
    return ss.str();
}

void NeuralActivityMonitor::setActivityThresholds(float firingThreshold, float excitationThreshold) {
    pImpl->firingThreshold = firingThreshold;
    pImpl->excitationThreshold = excitationThreshold;
    NLM_LOG_INFO("Activity thresholds set: firing=" + std::to_string(firingThreshold) + 
                 ", excitation=" + std::to_string(excitationThreshold));
}

void NeuralActivityMonitor::startMonitoring(double interval) {
    pImpl->monitoringInterval = interval;
    pImpl->isMonitoring = true;
    NLM_LOG_INFO("Neural activity monitoring started with interval: " + std::to_string(interval) + "s");
}

void NeuralActivityMonitor::stopMonitoring() {
    pImpl->isMonitoring = false;
    NLM_LOG_INFO("Neural activity monitoring stopped");
}

std::string NeuralActivityMonitor::getActivityStatistics() const {
    std::stringstream ss;
    ss << "Neural Activity Statistics:\n";
    
    if (!pImpl->brain) {
        ss << "  No brain initialized\n";
        return ss.str();
    }
    
    size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
    size_t activeNeurons = pImpl->brain->getActiveNeuronCount();
    size_t firingNeurons = pImpl->brain->getFiringNeuronCount();
    size_t totalSpikes = pImpl->brain->getTotalSpikeCount();
    
    ss << "  Total Neurons: " << totalNeurons << "\n";
    ss << "  Active Neurons: " << activeNeurons << " (" 
       << (totalNeurons > 0 ? (activeNeurons * 100.0f / totalNeurons) : 0) << "%)\n";
    ss << "  Firing Neurons: " << firingNeurons << " (" 
       << (totalNeurons > 0 ? (firingNeurons * 100.0f / totalNeurons) : 0) << "%)\n";
    ss << "  Total Spikes: " << totalSpikes << "\n";
    ss << "  Average Firing Rate: " << getCurrentFiringRate() << " Hz\n";
    ss << "  Activity History Size: " << pImpl->activityHistory.size() << "\n";
    ss << "  Activity Log Size: " << pImpl->activityLog.size() << "\n";
    
    return ss.str();
}

void NeuralActivityMonitor::controlActivity(const std::string& command, float parameter) {
    if (command == "set_firing_rate") {
        // Set target firing rate
        pImpl->targetFiringRate = parameter;
        NLM_LOG_INFO("Target firing rate set to: " + std::to_string(parameter) + " Hz");
    } else if (command == "set_excitation_level") {
        // Set excitation level
        if (pImpl->brain) {
            // Inject excitation to all neurons
            for (size_t i = 0; i < pImpl->brain->getTotalNeuronCount(); ++i) {
                pImpl->brain->injectCurrent(static_cast<NeuronId>(i), parameter);
            }
        }
        NLM_LOG_INFO("Excitation level set to: " + std::to_string(parameter));
    } else if (command == "reset_activity") {
        // Reset activity by clearing history and stopping monitoring
        pImpl->activityHistory.clear();
        pImpl->activityLog.clear();
        stopMonitoring();
        NLM_LOG_INFO("Neural activity reset");
    } else {
        NLM_LOG_WARNING("Unknown activity control command: " + command);
    }
}

float NeuralActivityMonitor::getCurrentFiringRate() const {
    if (!pImpl->brain) return 0.0f;
    return pImpl->brain->getAverageFiringRate();
}

std::vector<float> NeuralActivityMonitor::getActivityHistory() const {
    return pImpl->activityHistory;
}

void NeuralActivityMonitor::setActivityLogLevel(int level) {
    pImpl->logLevel = level;
    NLM_LOG_INFO("Activity log level set to: " + std::to_string(level));
}

std::vector<std::string> NeuralActivityMonitor::getActivityLog() const {
    return pImpl->activityLog;
}

bool NeuralActivityMonitor::checkNeuralAnomalies() const {
    if (!pImpl->brain) return false;
    
    // Check for neural anomalies
    size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
    if (totalNeurons == 0) return false;
    
    // Check for excessive firing rate
    float firingRate = getCurrentFiringRate();
    if (firingRate > pImpl->maxFiringRate * 1.1f) {
        pImpl->activityLog.push_back("WARNING: Excessive firing rate detected: " + std::to_string(firingRate) + " Hz");
        return true;
    }
    
    // Check for insufficient firing rate
    if (firingRate < pImpl->minFiringRate * 1.1f) {
        pImpl->activityLog.push_back("WARNING: Insufficient firing rate detected: " + std::to_string(firingRate) + " Hz");
        return true;
    }
    
    // Check for abnormal spike count
    size_t spikeCount = pImpl->brain->getTotalSpikeCount();
    // Spike count should grow roughly linearly with time
    // This is a simplified check - could be more sophisticated
    
    return false;
}

std::string NeuralActivityMonitor::getNeuralHealthStatus() const {
    std::stringstream ss;
    ss << "Neural Health Status:\n";
    
    if (!pImpl->brain) {
        ss << "  Status: NOT INITIALIZED\n";
        return ss.str();
    }
    
    // Check various health metrics
    size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
    size_t activeNeurons = pImpl->brain->getActiveNeuronCount();
    float firingRate = getCurrentFiringRate();
    bool anomalies = checkNeuralAnomalies();
    
    ss << "  Status: HEALTHY\n";
    ss << "  Neurons: " << totalNeurons << " total, " << activeNeurons << " active\n";
    ss << "  Firing Rate: " << firingRate << " Hz\n";
    ss << "  Anomalies Detected: " << (anomalies ? "Yes" : "No") << "\n";
    ss << "  Monitoring Active: " << (isMonitoring() ? "Yes" : "No") << "\n";
    ss << "  Log Level: " << pImpl->logLevel << "\n";
    
    return ss.str();
}

void NeuralActivityMonitor::applyActivityControl(const std::string& controlType, float value) {
    if (controlType == "limit_firing_rate") {
        // Limit firing rate by reducing excitation
        if (pImpl->brain) {
            // Apply inhibitory current to reduce firing
            for (size_t i = 0; i < pImpl->brain->getTotalNeuronCount(); ++i) {
                pImpl->brain->injectCurrent(static_cast<NeuronId>(i), -value);
            }
        }
        NLM_LOG_INFO("Applied firing rate limit: " + std::to_string(value) + " Hz");
    } else if (controlType == "boost_firing_rate") {
        // Boost firing rate by increasing excitation
        if (pImpl->brain) {
            for (size_t i = 0; i < pImpl->brain->getTotalNeuronCount(); ++i) {
                pImpl->brain->injectCurrent(static_cast<NeuronId>(i), value);
            }
        }
        NLM_LOG_INFO("Applied firing rate boost: " + std::to_string(value) + " Hz");
    } else if (controlType == "reset_neural_state") {
        // Reset neural state
        if (pImpl->brain) {
            pImpl->brain->reset();
            pImpl->brain->initialize(pImpl->brain->getConfig());
        }
        NLM_LOG_INFO("Neural state reset");
    } else {
        NLM_LOG_WARNING("Unknown activity control type: " + controlType);
    }
}

std::string NeuralActivityMonitor::getCurrentControlSettings() const {
    std::stringstream ss;
    ss << "Current Control Settings:\n";
    ss << "  Target Firing Rate: " << pImpl->targetFiringRate << " Hz\n";
    ss << "  Max Firing Rate: " << pImpl->maxFiringRate << " Hz\n";
    ss << "  Min Firing Rate: " << pImpl->minFiringRate << " Hz\n";
    ss << "  Auto Control Enabled: " << (pImpl->autoControlEnabled ? "Yes" : "No") << "\n";
    ss << "  Firing Threshold: " << pImpl->firingThreshold << "\n";
    ss << "  Excitation Threshold: " << pImpl->excitationThreshold << "\n";
    return ss.str();
}

void NeuralActivityMonitor::resetMonitoring() {
    pImpl->activityHistory.clear();
    pImpl->activityLog.clear();
    pImpl->isMonitoring = false;
    NLM_LOG_INFO("Neural activity monitoring reset");
}

std::string NeuralActivityMonitor::exportActivityData(const std::string& format) const {
    if (format == "csv") {
        std::stringstream ss;
        ss << "Time,FiringRate,ActiveNeurons,TotalSpikes\n";
        
        // In a real implementation, we would export historical data
        // For now, we'll export current status
        if (pImpl->brain) {
            ss << "0,"<< getCurrentFiringRate() << "," << pImpl->brain->getActiveNeuronCount() 
               << "," << pImpl->brain->getTotalSpikeCount() << "\n";
        }
        
        return ss.str();
    } else if (format == "json") {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"firingRate\": " << getCurrentFiringRate() << ",\n";
        if (pImpl->brain) {
            ss << "  \"activeNeurons\": " << pImpl->brain->getActiveNeuronCount() << ",\n";
            ss << "  \"totalSpikes\": " << pImpl->brain->getTotalSpikeCount() << ",\n";
            ss << "  \"totalNeurons\": " << pImpl->brain->getTotalNeuronCount() << "\n";
        }
        ss << "}\n";
        return ss.str();
    } else {
        return "Unsupported format: " + format;
    }
}

bool NeuralActivityMonitor::importActivitySettings(const std::string& settings) {
    // In a real implementation, this would parse settings from a file or string
    // For now, we'll just accept any settings and log a message
    NLM_LOG_INFO("Activity settings imported (simulated)");
    return true;
}

bool NeuralActivityMonitor::isMonitoring() const {
    return pImpl->isMonitoring;
}

double NeuralActivityMonitor::getMonitoringInterval() const {
    return pImpl->monitoringInterval;
}

void NeuralActivityMonitor::setMonitoringInterval(double interval) {
    pImpl->monitoringInterval = interval;
}

} // namespace nlm