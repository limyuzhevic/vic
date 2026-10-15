#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "../core/Config/Config.hpp"

namespace nlm {

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
};

// Neuron model plugin interface
class INeuronModelPlugin : public IPlugin {
public:
    virtual ~INeuronModelPlugin() = default;
    virtual std::string getNeuronModelName() const = 0;
    virtual float computeThreshold(const class NeuronState& state) = 0;
    virtual MembranePotential updateDynamics(class NeuronState& state, double dt) = 0;
    virtual float computeAdaptation(class NeuronState& state, double dt) = 0;
    virtual void initializeNeuron(class Neuron& neuron, const Config& config) = 0;
};

// Plasticity rule plugin interface
class IPlasticityRulePlugin : public IPlugin {
public:
    virtual ~IPlasticityRulePlugin() = default;
    virtual std::string getPlasticityRuleName() const = 0;
    virtual float computeWeightChange(const class SynapseState& pre, const class SynapseState& post, double dt) = 0;
    virtual bool shouldApplyPlasticity(const class SynapseState& pre, const class SynapseState& post, const Config& config) = 0;
    virtual void onPreSynapseSpikes(class Synapse& synapse, Timestamp timestamp) = 0;
    virtual void onPostSynapseSpikes(class Synapse& synapse, Timestamp timestamp) = 0;
};

// Synaptic dynamics plugin interface
class ISynapticDynamicsPlugin : public IPlugin {
public:
    virtual ~ISynapticDynamicsPlugin() = default;
    virtual std::string getSynapticDynamicsName() const = 0;
    virtual float computeSynapticWeight(class SynapseState& state, double dt) = 0;
    virtual float computeSynapticConductance(class SynapseState& state, double dt) = 0;
    virtual float computeTransmissionProbability(class SynapseState& state, double dt) = 0;
    virtual void onActivityDependence(class SynapseState& state, float preActivity, float postActivity) = 0;
};

// Plugin metadata for discovery
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    bool isCorePlugin;
    PluginMetadata()
        : name(), version("1.0.0"), description(), author(), dependencies(), isCorePlugin(true) {}
};

} // namespace nlm
