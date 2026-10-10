// Synapse plugin base class interface
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../PluginBase.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

class ISynapsePlugin : public IPlugin {
public:
    virtual ~ISynapsePlugin() = default;
    
    // Plugin category identification
    virtual PluginCategory getCategory() const override { return PluginCategory::SYNAPSE; }
    
    // Synapse creation and management
    virtual std::shared_ptr<Synapse> createSynapse(SynapseHandle handle) = 0;
    virtual bool configureSynapse(Synapse& synapse, const std::map<std::string, std::string>& parameters) = 0;
    virtual std::vector<std::string> getSynapseTypes() const = 0;
    
    // Synaptic dynamics customization
    virtual bool initializeSynapseDynamics(Synapse& synapse) = 0;
    virtual void updateSynapseState(Synapse& synapse, Timestamp currentTime, TimestepDuration dt) = 0;
    virtual bool onPreSynapticSpike(Synapse& synapse, Timestamp spikeTime) = 0;
    
    // Plasticity integration
    virtual bool configurePlasticity(Synapse& synapse, const std::string& plasticityType) = 0;
    virtual void applyPlasticity(Synapse& synapse, Timestamp currentTime, double weightChange) = 0;
    
    // Plugin-specific configuration
    virtual std::map<std::string, std::string> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getSupportedParameterTypes() const = 0;
    
    // Synaptic connection management
    virtual bool createSynapseConnection(SynapseHandle source, SynapseHandle target) = 0;
    virtual bool removeSynapseConnection(SynapseHandle source, SynapseHandle target) = 0;
};

} // namespace nlm