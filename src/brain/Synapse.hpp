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
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Tsodyks-Markram STP variables (for short-term plasticity)
    float getUtilization() const;
    float getRecovery() const;
    float getResources() const;
    
    // Reversal potentials (in mV)
    float getReversalPotential() const;
    void setReversalPotential(float potential);
    
    // Synaptic conductance state
    float getConductance() const;
    void setConductance(float conductance);
    float getAMPAConductance() const;
    float getNMDAConductance() const;
    
    // Synaptic weight dynamics
    float getCalciumConcentration() const;
    void setCalciumConcentration(float calcium);
    float getWeightChangeRate() const;
    
    // Synaptic noise
    float getNoiseLevel() const;
    void setNoiseLevel(float noise);
    
    // Synaptic activity history for bursting
    int getRecentSpikeCount(int timeWindow, Timestamp currentTime) const;
    float getBurstingProbability() const;
    
    // Update synapse for one simulation step with realistic synaptic dynamics
    void step(Timestamp currentTime);
    
    // Handle presynaptic spike (update STP variables)
    void onPreSpike(Timestamp time);
    
    // Handle postsynaptic spike (for STP and calcium dynamics)
    void onPostSpike(Timestamp time);
    
    // Calculate synaptic current based on conductance dynamics
    float calculateCurrent(float vPost, Timestamp currentTime) const;
    
    // Apply plasticity rules based on spike timing and calcium
    void applyPlasticity(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
