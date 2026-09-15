#ifndef NLM_SYNAPSE_HPP
#define NLM_SYNAPSE_HPP

#include "../core/Types/Types.hpp"

namespace nlm {

class Synapse {
public:
    Synapse(SynapseId id, NeuronId source, NeuronId target, float weight, Delay delay);
    ~Synapse();
    
    // Disable copying
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    // Enable moving
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Synapse identification
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight management
    float getWeight() const;
    void setWeight(float weight);
    void addToWeight(float delta);
    
    // Properties
    Delay getDelay() const;
    SynapseType getType() const;
    bool getIsExcitatory() const;
    float getEfficacy() const;
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    bool hasPlasticity(PlasticityFlags& flags) const;
    
    // Spike history for plasticity
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void decaySpikeHistory(float decayFactor);
    
    // Eligibility trace for reward-modulated plasticity
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void addToEligibilityTrace(float delta);
    void decayEligibilityTrace(float factor);
    
    // State
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_SYNAPSE_HPP