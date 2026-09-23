#ifndef NLM_BRAIN_SYNAPSE_HPP
#define NLM_BRAIN_SYNAPSE_HPP

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

class Synapse {
public:
    static SynapseId generateId();
    
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    ~Synapse();
    
    // Non-copyable
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    // Movable
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;
    
    // Core synapse operations
    void step(Timestamp currentTime);
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    
    // State management
    SynapticWeight getWeight() const { return pImpl->weight; }
    void setWeight(SynapticWeight weight);
    
    NeuronId getSourceNeuron() const { return pImpl->sourceNeuron; }
    NeuronId getDestinationNeuron() const { return pImpl->destinationNeuron; }
    
    SynapseType getType() const { return pImpl->type; }
    void setType(SynapseType type) { pImpl->type = type; }
    
    Delay getDelay() const { return pImpl->delay; }
    void setDelay(Delay delay) { pImpl->delay = delay; }
    
    // Plasticity
    bool isPlastic() const;
    bool hasPlasticityFlag(const std::string& flag) const;
    void enablePlasticity(bool hebbian, bool stdp, bool structural, bool neuromodulated);
    void setPlasticityFlags(const PlasticityFlags& flags) { pImpl->plasticityFlags = flags; }
    
    const PlasticityFlags& getPlasticityFlags() const { return pImpl->plasticityFlags; }
    
    // Spike history for STDP
    const std::vector<Timestamp>& getPreSpikeHistory() const { return pImpl->preSpikeHistory; }
    const std::vector<Timestamp>& getPostSpikeHistory() const { return pImpl->postSpikeHistory; }
    
    // Short-term plasticity
    float getUtilization() const { return pImpl->shortTermDepression; }
    float getFacilitation() const { return pImpl->shortTermFacilitation; }
    void updateShortTermPlasticity(Timestamp currentTime);
    
    // Eligibility trace for reward-modulated learning
    float getEligibilityTrace() const { return pImpl->eligibilityTrace; }
    void setEligibilityTrace(float trace) { pImpl->eligibilityTrace = trace; }
    
    // Synaptic efficacy
    float getEfficacy() const { return pImpl->efficacy; }
    void setEfficacy(float efficacy) { pImpl->efficacy = efficacy; }
    
    // Weight bounds and constraints
    static constexpr SynapticWeight MIN_WEIGHT = -1.0f;
    static constexpr SynapticWeight MAX_WEIGHT = 1.0f;
    static constexpr float DEFAULT_WEIGHT = 0.1f;
    
    // Synapse properties
    bool isExcitatory() const { return pImpl->type == SynapseType::Excitatory; }
    bool isInhibitory() const { return pImpl->type == SynapseType::Inhibitory; }
    
    // Utility methods
    std::string toString() const;
    
    // Weight change utilities
    void incrementWeight(float delta);
    bool canChangeWeight(float delta) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_BRAIN_SYNAPSE_HPP
