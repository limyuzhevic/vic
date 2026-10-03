#pragma once

#include <string>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationInterface;

class NLMIntegration {
public:
    NLMIntegration(VisualizationInterface* visualizationInterface);
    ~NLMIntegration();

    // Initialize integration with NLM components
    bool initialize();

    // Update integration with new NLM data
    void update(const VisualizationData& data);

    // Update brain state from visualization
    void updateBrain(const VisualizationData& data);

    // Synchronize visualization with brain
    void synchronizeWithBrain();

    // Get current NLM data
    VisualizationData getCurrentData() const;

    // Enable/disable integration
    void enable(bool enabled);
    bool isEnabled() const;

    // Set data binding options
    void setDataBinding(const std::string& type, bool enabled);
    bool isDataBindingEnabled(const std::string& type) const;

    // Enable/disable event forwarding
    void enableEventForwarding(bool enabled);
    bool isEventForwardingEnabled() const;

    // Set simulation speed
    void setSimulationSpeed(double speed);
    double getSimulationSpeed() const;

    // Get integration statistics
    std::map<std::string, double> getStatistics() const;

    // Export integration data
    bool exportData(const std::string& filepath, const std::string& format);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm