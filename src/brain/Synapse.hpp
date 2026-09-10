#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // ========== PHASE 2: ADVANCED SYNAPTIC DYNAMICS ==========
    
    // Process spike transmission with delay
    void processSpikeTransmission();
    
    // Update synaptic efficacy based on recent activity
    void updateEfficacy();
    
    // Apply Hebbian learning rule
    void applyHebbianLearning();
    
    // Apply STDP rule based on spike timing
    void applySTDP();
    
    // Apply reward-modulated learning if enabled
    void applyRewardModulatedLearning(float reward, float predictionError);
    
    // Get total synaptic efficacy (short-term + long-term)
    float getTotalEfficacy() const;
    
    // Get neurotransmitter release probability
    float getReleaseProbability() const;
    void setReleaseProbability(float prob);
    
    // Get synaptic delay in simulation steps
    Delay getSynapticDelay() const;
    
    // Check if synapse has received recent input
    bool hasRecentInput(Timestamp currentTime, Timestamp maxAge) const;
    
    // Get spike history for plasticity
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    // Get short-term depression state
    float getShortTermDepression() const;
    
    // Get short-term facilitation state
    float getShortTermFacilitation() const;
    
    // Get last spike times for both sides
    Timestamp getLastPreSpikeTime() const;
    Timestamp getLastPostSpikeTime() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
