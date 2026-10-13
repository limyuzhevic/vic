#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

class Synapse {
public:
    /**
     * @brief Create synapse with connection between neurons
     * @param id Unique identifier for the synapse
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor deleted (synapses are not copyable)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Copy assignment operator deleted (synapses are not copyable)
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Synapse to move from
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Synapse to move from
     * @return Reference to this synapse
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get synapse identifier
     * @return Synapse ID
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron identifier
     * @return Source neuron ID
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get destination neuron identifier
     * @return Destination neuron ID
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get synaptic weight
     * @return Current synaptic weight (-1.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * @param weight New synaptic weight (-1.0 to 1.0)
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to synaptic weight
     * @param delta Change in synaptic weight
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get synaptic delay
     * @return Delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Set synaptic delay
     * @param delay Delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type
     * @return Synapse type (Excitatory, Inhibitory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * @return true if synapse is excitatory, false otherwise
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return true if synapse is inhibitory, false otherwise
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike
     * @param timestamp Time of presynaptic spike
     * @details Stores spike time for Spike-Timing-Dependent Plasticity (STDP)
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike
     * @param timestamp Time of postsynaptic spike
     * @details Stores spike time for STDP and Hebbian plasticity
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return Vector of presynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return Vector of postsynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @details Removes all recorded spike events for plasticity
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity flags
     * @return Reference to plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return Reference to plasticity configuration for modification
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity rules
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace
     * @return Current eligibility trace value for reward-modulated learning
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace
     * @param decayRate Decay rate per timestep
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy
     * @return Current synaptic efficacy (use-dependent modulation)
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New synaptic efficacy (0.0 to 2.0)
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update synapse for one simulation step with explicit timestep
     * @param currentTime Current simulation time
     * @param dt Timestep duration in milliseconds
     * @details Implements synaptic dynamics including:
     *          - Short-term plasticity (facilitation and depression)
     *          - Eligibility trace decay for reward-modulated learning
     *          - Synaptic weight clamping to prevent instability
     */
    void step(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Reset synapse to initial state
     * @details Restores synaptic parameters to initialized values:
     *          - Weight to 0.0
     *          - Spike histories to empty
     *          - Eligibility trace to 0.0
     *          - Efficacy to 1.0
     *          - Short-term plasticity to resting state
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biological parameters
     * @param rng Random number generator for biological variability
     * @details Sets realistic synapse parameters based on type:
     *          - Excitatory: positive weights (0.1-0.4), efficacy (0.8-1.0)
     *          - Inhibitory: negative weights (-0.1 to -0.4), efficacy (0.8-1.0)
     *          - Other: small random weights (-0.1 to 0.1), efficacy (0.9-1.0)
     *          - Random delay (1-5 steps)
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Private implementation structure for Synapse
     * @details Internal implementation details to reduce header size and improve compilation times
     */
    struct Impl;
    Impl* pImpl;
};
