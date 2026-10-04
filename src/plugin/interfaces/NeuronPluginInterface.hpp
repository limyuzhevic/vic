// NeuronPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <vector>
#include <memory>
#include "../../brain/Neuron.hpp"

namespace nlm {

class NeuronPluginInterface : public virtual PluginBase {
public:
    virtual ~NeuronPluginInterface() = default;

    // Neuron model creation
    virtual std::unique_ptr<Neuron> createNeuron(const Config& config) = 0;
    virtual std::unique_ptr<Neuron> createNeuron(NeuronType type, const Config& config) = 0;

    // Neuron configuration
    virtual bool configureNeuron(Neuron* neuron, const Config& config) = 0;
    virtual bool validateNeuronConfiguration(const Config& config) const = 0;

    // Neuron properties
    virtual NeuronType getDefaultNeuronType() const = 0;
    virtual std::string getNeuronTypeName(NeuronType type) const = 0;
    virtual bool isNeuronTypeSupported(NeuronType type) const = 0;

    // Neuron population management
    virtual size_t getRecommendedPopulationSize() const = 0;
    virtual size_t getMinPopulationSize() const = 0;
    virtual size_t getMaxPopulationSize() const = 0;

    // Neuron dynamics
    virtual bool supportsCustomDynamics() const = 0;
    virtual std::string getDynamicModelName() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;
};

} // namespace nlm