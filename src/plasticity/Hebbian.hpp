#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // STDP parameters
    void setSTDPParameters(float learningRate, float timeWindow, float tauPre, float tauPost);
    
    // Neuromodulation
    void setNeuromodulationStrength(float strength);
    float getNeuromodulationStrength() const;
    void setExplorationRate(float rate);
    float getExplorationRate() const;
    
    // Homeostatic plasticity
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    void setHomeostaticRate(float rate);
    float getHomeostaticRate() const;
    
    // Trace management (for advanced usage)
    float getPreTrace(const SynapseId& synapseId) const;
    float getPostTrace(const SynapseId& synapseId) const;
    void resetTraces();
};

} // namespace nlm
