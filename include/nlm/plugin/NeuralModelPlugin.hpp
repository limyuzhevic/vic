#pragma once

#include "PluginBase.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <concepts>

namespace nlm {

// Neural model plugin capability concept
template<typename T>
concept NeuralModelPluginCapability = PluginCapability<T> && requires(T plugin) {
    { plugin.createNeuron() } -> std::same_as<std::unique_ptr<class Neuron>>;
    { plugin.getNeuronModelType() } -> std::same_as<std::string>;
};

// Neural Model Plugin Interface for custom neuron implementations
class NeuralModelPlugin : public virtual PluginBase {
public:
    virtual ~NeuralModelPlugin() = default;

    // Create a new neuron instance using this plugin's model
    virtual std::unique_ptr<class Neuron> createNeuron() = 0;

    // Get the type/model name of this neural model
    virtual std::string getNeuronModelType() const = 0;

    // Get neuron parameters for configuration
    virtual std::unordered_map<std::string, double> getDefaultParameters() const = 0;

    // Validate neuron parameters
    virtual bool validateParameters(const std::unordered_map<std::string, double>& params) const = 0;

    // Get supported neuron types
    virtual std::vector<std::string> getSupportedNeuronTypes() const = 0;
};

} // namespace nlm
