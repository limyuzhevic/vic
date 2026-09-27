#pragma once

#include "IPlugin.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {
namespace plugins {

// Enhanced plasticity plugin interface with additional functionality
class IPlasticityPlugin : public virtual IPlugin {
public:
    virtual ~IPlasticityPlugin() = default;
    
    // Plasticity rule identification
    virtual const char* getPlasticityType() const = 0;
    virtual const char* getPlasticityMechanism() const = 0;
    
    // Core plasticity operation
    virtual float computeWeightChange(
        const Synapse& synapse,
        float preTrace,
        float postTrace,
        float neuromodulatorLevel = 1.0f,
        float timeFactor = 1.0f) = 0;
    
    // Eligibility trace management
    virtual float getEligibilityTrace() const = 0;
    virtual void updateEligibilityTrace(float preActivity, float postActivity, TimestepDuration dt) = 0;
    virtual void resetEligibilityTrace() = 0;
    
    // Weight change application
    virtual void applyWeightChange(Synapse& synapse, float delta, bool constrain = true) = 0;
    virtual float constrainWeightChange(float currentWeight, float delta) const = 0;
    
    // Activity-dependent plasticity
    virtual float getPostSynapticLTDThreshold() const = 0;
    virtual float getPostSynapticLTPThreshold() const = 0;
    virtual void setPostSynapticThresholds(float ltd, float ltp) = 0;
    
    // Neuromodulation integration
    virtual float getNeuromodulationSensitivity() const = 0;
    virtual void setNeuromodulationSensitivity(float sensitivity) = 0;
    virtual std::map<std::string, float> getNeuromodulatorEffects() const = 0;
    
    // Spike timing dependent plasticity (STDP) specific
    virtual float getSTDPLearningRate() const = 0;
    virtual void setSTDPLearningRate(float rate) = 0;
    virtual float getSTDPTimeConstant() const = 0;
    virtual void setSTDPTimeConstant(float timeConstant) = 0;
    
    // Hebbian plasticity specific
    virtual float getHebbianLearningRate() const = 0;
    virtual void setHebbianLearningRate(float rate) = 0;
    virtual float getHebbianCoherenceThreshold() const = 0;
    virtual void setHebbianCoherenceThreshold(float threshold) = 0;
    
    // Structural plasticity
    virtual bool supportsStructuralChanges() const = 0;
    virtual float getStructuralPlasticityRate() const = 0;
    virtual void setStructuralPlasticityRate(float rate) = 0;
    
    // Plasticity validation
    virtual bool validatePlasticityParameters() const = 0;
    virtual std::vector<std::string> getPlasticityErrors() const = 0;
    
    // Simulation-specific operations
    virtual void prepareForSimulation() = 0;
    virtual void resetForNewEpisode() = 0;
    virtual bool isReady() const = 0;
};

} // namespace plugins
} // namespace nlm
