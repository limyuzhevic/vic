#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Real synaptic dynamics implementation
// Based on exponential decay model for synaptic conductance
class SynapticDynamics {
public:
    SynapticDynamics(float time_constant = 5.0f, float reversal_potential = 0.0f);
    ~SynapticDynamics();
    
    void update(Synapse* synapse, TimestepDuration dt);
    void applyInput(Synapse* synapse, SynapticWeight weight);
    void reset();
    
    // Parameters
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setReversalPotential(float reversal);
    float getReversalPotential() const;
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    // State
    float getConductance(SynapseId synapseId) const;
    void setConductance(SynapseId synapseId, float conductance);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Real plasticity implementation based on STDP
// Spike-Timing-Dependent Plasticity (STDP)
class STDPPlasticity : public PlasticityRule {
public:
    STDPPlasticity(float learning_rate = 0.001f, float tau_plus = 20.0f, float tau_minus = 20.0f);
    ~STDPPlasticity() override;
    
    void applyPlasticity(Neuron* pre, Neuron* post, float dt) override;
    void recordPreSynapseSpike(Neuron* neuron) override;
    void recordPostSynapseSpike(Neuron* neuron) override;
    void reset() override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setTauPlus(float tau);
    float getTauPlus() const;
    void setTauMinus(float tau);
    float getTauMinus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hebbian learning implementation
class HebbianPlasticity : public PlasticityRule {
public:
    HebbianPlasticity(float learning_rate = 0.001f, float threshold = 0.1f);
    ~HebbianPlasticity() override;
    
    void applyPlasticity(Neuron* pre, Neuron* post, float dt) override;
    void recordPreSynapseSpike(Neuron* neuron) override;
    void recordPostSynapseSpike(Neuron* neuron) override;
    void reset() override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setThreshold(float threshold);
    float getThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Structural plasticity for creating/removing synapses
class StructuralPlasticity : public PlasticityRule {
public:
    StructuralPlasticity(float creation_rate = 0.0001f, float removal_rate = 0.00005f,
                         float min_weight = 0.01f, float max_weight = 1.0f);
    ~StructuralPlasticity() override;
    
    void applyPlasticity(Neuron* pre, Neuron* post, float dt) override;
    void recordPreSynapseSpike(Neuron* neuron) override;
    void recordPostSynapseSpike(Neuron* neuron) override;
    void reset() override;
    
    // Structural operations
    bool createSynapse(Neuron* pre, Neuron* post);
    bool removeSynapse(SynapseId synapseId);
    bool createNeuron(RegionId regionId, NeuronType type = NeuronType::Excitatory);
    bool removeNeuron(NeuronId neuronId);
    
    // Parameters
    void setCreationRate(float rate);
    float getCreationRate() const;
    void setRemovalRate(float rate);
    float getRemovalRate() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm