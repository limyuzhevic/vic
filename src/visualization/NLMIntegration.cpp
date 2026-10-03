#include "NLMIntegration.hpp"
#include "VisualizationInterfaceAdvanced.hpp"

namespace nlm {

struct NLMIntegration::Impl {
    VisualizationInterface* visualizationInterface;
    bool enabled;
    std::map<std::string, bool> dataBinding;
    bool eventForwardingEnabled;
    double simulationSpeed;
    VisualizationData currentData;
    
    Impl(VisualizationInterface* viz) : visualizationInterface(viz), enabled(true),
                                         eventForwardingEnabled(true), simulationSpeed(1.0) {
        dataBinding["network"] = true;
        dataBinding["activity"] = true;
        dataBinding["spikes"] = true;
        dataBinding["weights"] = true;
        dataBinding["timeSeries"] = true;
        dataBinding["memory"] = true;
        dataBinding["prediction"] = true;
    }
};

NLMIntegration::NLMIntegration(VisualizationInterface* visualizationInterface) 
    : pImpl(std::make_unique<Impl>(visualizationInterface)) {}

NLMIntegration::~NLMIntegration() = default;

bool NLMIntegration::initialize() {
    // Initialize integration with NLM components
    return pImpl->visualizationInterface != nullptr;
}

void NLMIntegration::update(const VisualizationData& data) {
    if (!pImpl->enabled) return;
    
    pImpl->currentData = data;
    
    // Update visualization interface
    if (pImpl->visualizationInterface) {
        pImpl->visualizationInterface->update(data);
    }
    
    // Forward events if enabled
    if (pImpl->eventForwardingEnabled) {
        // Implementation would forward events to NLM
    }
}

void NLMIntegration::updateBrain(const VisualizationData& data) {
    // Implementation would update brain state from visualization
}

void NLMIntegration::synchronizeWithBrain() {
    // Implementation would synchronize visualization with brain
}

VisualizationData NLMIntegration::getCurrentData() const {
    return pImpl->currentData;
}

void NLMIntegration::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool NLMIntegration::isEnabled() const {
    return pImpl->enabled;
}

void NLMIntegration::setDataBinding(const std::string& type, bool enabled) {
    pImpl->dataBinding[type] = enabled;
}

bool NLMIntegration::isDataBindingEnabled(const std::string& type) const {
    auto it = pImpl->dataBinding.find(type);
    return it != pImpl->dataBinding.end() ? it->second : false;
}

void NLMIntegration::enableEventForwarding(bool enabled) {
    pImpl->eventForwardingEnabled = enabled;
}

bool NLMIntegration::isEventForwardingEnabled() const {
    return pImpl->eventForwardingEnabled;
}

void NLMIntegration::setSimulationSpeed(double speed) {
    pImpl->simulationSpeed = speed;
}

double NLMIntegration::getSimulationSpeed() const {
    return pImpl->simulationSpeed;
}

std::map<std::string, double> NLMIntegration::getStatistics() const {
    std::map<std::string, double> stats;
    stats["dataBindingCount"] = static_cast<double>(pImpl->dataBinding.size());
    stats["enabled"] = pImpl->enabled ? 1.0 : 0.0;
    stats["eventForwardingEnabled"] = pImpl->eventForwardingEnabled ? 1.0 : 0.0;
    stats["simulationSpeed"] = pImpl->simulationSpeed;
    return stats;
}

bool NLMIntegration::exportData(const std::string& filepath, const std::string& format) {
    // Implementation would export integration data
    return false;
}

} // namespace nlm