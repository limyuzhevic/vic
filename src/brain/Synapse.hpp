#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Synapse: Connects neurons with synaptic transmission
// Includes real synaptic dynamics: STDP, short-term plasticity, and reward-modulated learning
class Synapse {
public:
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Basic synapse properties
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    SynapseType getType() const;
    void setType(SynapseType type);
    
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Spike recording and history
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity control
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Reward-modulated learning
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Simulation updates
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm