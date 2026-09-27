#pragma once

#include "PluginBase.hpp"
#include <string>
#include <memory>
#include <concepts>
#include <vector>

namespace nlm {

// Plasticity rule plugin capability concept
template<typename T>
concept PlasticityPluginCapability = PluginCapability<T> && requires(T plugin) {
    { plugin.calculatePlasticity() } -> std::same_as<double>;
    { plugin.getPlasticityType() } -> std::same_as<std::string>;
};

// Plasticity Rule Plugin Interface for custom learning algorithms
class PlasticityPlugin : public virtual PluginBase {
public:
    virtual ~PlasticityPlugin() = default;

    // Calculate synaptic plasticity based on pre- and post-synaptic activity
    virtual double calculatePlasticity(double preSpikeTime, double postSpikeTime,
                                        double previousWeight, const std::any& context) = 0;

    // Get the type of plasticity rule
    virtual std::string getPlasticityType() const = 0;

    // Get learning rate for this plasticity rule
    virtual double getLearningRate() const = 0;

    // Set learning rate
    virtual void setLearningRate(double rate) = 0;

    // Get maximum weight change
    virtual double getMaxWeightChange() const = 0;

    // Set maximum weight change
    virtual void setMaxWeightChange(double maxChange) = 0;

    // Initialize plasticity rule with specific parameters
    virtual void initializeRule(const std::unordered_map<std::string, double>& params) = 0;

    // Get supported plasticity rule types
    virtual std::vector<std::string> getSupportedRuleTypes() const = 0;

    // Get rule parameters
    virtual std::unordered_map<std::string, double> getRuleParameters() const = 0;
};

} // namespace nlm
