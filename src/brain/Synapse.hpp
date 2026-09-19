#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// NMDA receptor state for synaptic transmission
struct NMDAState {
    float activation;        // NMDA receptor activation (0-1)
    float magnesiumBlock;     // Mg2+ block factor (0-1)
    float desensitization;   // Desensitization factor (0-1)
    float openProbability;   // Probability of being open
    
    NMDAState() : activation(0.0f), magnesiumBlock(1.0f), desensitization(1.0f), openProbability(0.0f) {}
};

// AMPA receptor state for synaptic transmission
struct AMPAState {
    float activation;        // AMPA receptor activation (0-1)
    float desensitization;   // Desensitization factor (0-1)
    float openProbability;   // Probability of being open
    
    AMPAState() : activation(0.0f), desensitization(1.0f), openProbability(0.0f) {}
};

// Short-term plasticity state
struct STPState {
    float utilization;         // Current utilization (0-1)
    float recovery;           // Current recovery (0-1)
    float facilitation;       // Current facilitation (0-1)
    
    STPState() : utilization(0.0f), recovery(1.0f), facilitation(0.0f) {}
};

// Synaptic conductance dynamics
struct SynapticConductance {
    float excitatory;        // Total excitatory conductance (nS)
    float inhibitory;        // Total inhibitory conductance (nS)
    float nmda;              // NMDA conductance (nS)
    float ampa;              // AMPA conductance (nS)
    float reversalExcitatory; // Reversal potential for excitatory (mV)
    float reversalInhibitory; // Reversal potential for inhibitory (mV)
    float reversalNMDA;       // NMDA reversal potential (mV)
    float reversalAMPA;       // AMPA reversal potential (mV)
    
    SynapticConductance()
        : excitatory(0.0f), inhibitory(0.0f), nmda(0.0f), ampa(0.0f)
        , reversalExcitatory(0.0f), reversalInhibitory(-70.0f)
        , reversalNMDA(0.0f), reversalAMPA(0.0f) {}
};

// Synapse representing a connection between neurons
// Implements real synaptic transmission with NMDA/AMPA receptor dynamics,
// short-term plasticity, and spike-dependent plasticity
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
    
    // NMDA/AMPA receptor state access
    const NMDAState& getNMDAState() const;
    NMDAState& getNMDAState();
    const AMPAState& getAMPAState() const;
    AMPAState& getAMPAState();
    
    // Short-term plasticity
    const STPState& getSTPState() const;
    STPState& getSTPState();
    
    // Synaptic conductance
    const SynapticConductance& getConductance() const;
    SynapticConductance& getConductance();
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // NMDA/AMPA receptor dynamics
    void updateNMDA(float dt, float V_post, float calciumConcentration);
    void updateAMPA(float dt, float V_post);
    void updateSTP(float dt, float preSpike, float postSpike);
    float computeNMDAConductance(float V_post);
    float computeAMPAGeometry(float weight);
    
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
    
    // Update synapse for one simulation step
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm