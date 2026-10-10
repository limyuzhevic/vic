#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

class Synapse {
public:
    /**
     * @brief Create synapse with unique identifiers
     * @param id Unique synapse identifier
     * @param source Source neuron identifier
     * @param destination Destination neuron identifier
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);

    /**
     * @brief Destructor
     */
    ~Synapse();

    /**
     * @brief Disable copying - synapses are non-copyable
     */
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;

    /**
     * @brief Enable moving - synapses can be moved but not copied
     * @param other Other synapse to move from
     */
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;

    /**
     * @brief Get the synapse's unique identifier
     * @return Synapse identifier
     */
    SynapseId getId() const;

    /**
     * @brief Get the source neuron identifier
     * @return Source neuron identifier
     */
    NeuronId getSourceNeuron() const;

    /**
     * @brief Get the destination neuron identifier
     * @return Destination neuron identifier
     */
    NeuronId getDestinationNeuron() const;

    /**
     * @brief Get the current synaptic weight
     * @return Synaptic weight (-1.0 to 1.0, negative for inhibitory)
     */
    SynapticWeight getWeight() const;

    /**
     * @brief Set the synaptic weight
     * @param weight New synaptic weight (-1.0 to 1.0)
     */
    void setWeight(SynapticWeight weight);

    /**
     * @brief Add delta to synaptic weight with bounds checking
     * @param delta Weight change amount
     */
    void addToWeight(SynapticWeight delta);

    /**
     * @brief Get the synaptic transmission delay
     * @return Delay in simulation steps
     */
    Delay getDelay() const;

    /**
     * @brief Set the synaptic transmission delay
     * @param delay Delay in simulation steps
     */
    void setDelay(Delay delay);

    /**
     * @brief Get the synapse type (excitatory, inhibitory, etc.)
     * @return Synapse type
     */
    SynapseType getType() const;

    /**
     * @brief Set the synapse type
     * @param type New synapse type
     */
    void setType(SynapseType type);

    /**
     * @brief Check if synapse is excitatory
     * @return true if synapse type is excitatory
     */
    bool isExcitatory() const;

    /**
     * @brief Check if synapse is inhibitory
     * @return true if synapse type is inhibitory
     */
    bool isInhibitory() const;

    /**
     * @brief Record a pre-synaptic spike for plasticity computation
     * @param timestamp Spike timestamp in milliseconds
     */
    void recordPreSpike(Timestamp timestamp);

    /**
     * @brief Record a post-synaptic spike for plasticity computation
     * @param timestamp Spike timestamp in milliseconds
     */
    void recordPostSpike(Timestamp timestamp);

    /**
     * @brief Get the history of pre-synaptic spikes
     * @return Vector of pre-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    /**
     * @brief Get the history of post-synaptic spikes
     * @return Vector of post-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    /**
     * @brief Clear all spike history
     */
    void clearHistory();

    /**
     * @brief Get plasticity flags (Hebbian, STDP, reward-modulated)
     * @return Plasticity flags (const reference)
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * @brief Get plasticity flags for modification
     * @return Plasticity flags (non-const reference)
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * @brief Enable specific plasticity rules for this synapse
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    /**
     * @brief Get the eligibility trace value
     * @return Eligibility trace (0.0 to 1.0 range)
     */
    float getEligibilityTrace() const;

    /**
     * @brief Set the eligibility trace value
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);

    /**
     * @brief Decay the eligibility trace by a given rate
     * @param decayRate Decay rate (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);

    /**
     * @brief Get the synaptic efficacy (use-dependent modulation factor)
     * @return Efficacy (0.0 to 2.0 range)
     */
    float getEfficacy() const;

    /**
     * @brief Set the synaptic efficacy (use-dependent modulation factor)
     * @param efficacy New efficacy value (0.0 to 2.0)
     */
    void setEfficacy(float efficacy);

    /**
     * @brief Update synapse for one simulation step
     * @param currentTime Current simulation timestamp in milliseconds
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset synapse to initial state
     */
    void reset();

    /**
     * @brief Initialize synapse with random biologically plausible parameters
     * @param rng Random number generator for parameter initialization
     */
    void initializeRandom(class RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
