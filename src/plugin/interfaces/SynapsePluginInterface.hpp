// SynapsePluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../brain/Synapse.hpp"

namespace nlm {

class SynapsePluginInterface : public virtual PluginBase {
public:
    virtual ~SynapsePluginInterface() = default;

    // Synapse model creation
    virtual std::unique_ptr<Synapse> createSynapse(const Config& config) = 0;
    virtual std::unique_ptr<Synapse> createSynapse(NeuronId source, NeuronId target, const Config& config) = 0;

    // Synapse configuration
    virtual bool configureSynapse(Synapse* synapse, const Config& config) = 0;
    virtual bool validateSynapseConfiguration(const Config& config) const = 0;

    // Synapse properties
    virtual SynapticWeight getDefaultWeight() const = 0;
    virtual Delay getDefaultDelay() const = 0;
    virtual float getWeightRangeMin() const = 0;
    virtual float getWeightRangeMax() const = 0;
    virtual Delay getDelayRangeMin() const = 0;
    virtual Delay getDelayRangeMax() const = 0;

    // Synapse dynamics
    virtual bool supportsCustomDynamics() const = 0;
    virtual std::string getDynamicModelName() const = 0;
    virtual float getTimeConstant() const = 0;

    // Plasticity integration
    virtual bool supportsCustomPlasticity() const = 0;
    virtual std::string getPlasticityModelName() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;
};

} // namespace nlm