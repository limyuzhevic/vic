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
    
    /**
     * @brief Get unique identifier of this synapse
     * @return SynapseId identifier for this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron ID
     * @return NeuronId ID of pre-synaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron ID
     * @return NeuronId ID of post-synaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get current synaptic weight
     * @return SynapticWeight synaptic weight (-1.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * @param weight New synaptic weight
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add delta to synaptic weight
     * @param delta Weight change amount
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get synaptic transmission delay
     * @return Delay delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic transmission delay
     * @param delay Delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type
     * @return SynapseType excitatory, inhibitory, or modulatory
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * @return bool true if excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return bool true if inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record pre-synaptic spike for plasticity
     * @param timestamp Time of pre-synaptic spike
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record post-synaptic spike for plasticity
     * @param timestamp Time of post-synaptic spike
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get history of pre-synaptic spikes
     * @return const std::vector<Timestamp>& spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get history of post-synaptic spikes
     * @return const std::vector<Timestamp>& spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags for this synapse
     * @return const PlasticityFlags& plasticity state flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return PlasticityFlags& plasticity state flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace for reward-modulated learning
     * @return float current eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace by given factor
     * @param decayRate Decay factor (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy (use-dependent modulation)
     * @return float synaptic efficacy multiplier
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New efficacy multiplier
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     */
    void reset();
    
    /**
     * @brief Initialize synapse parameters with random values
     * @param rng Random number generator
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
