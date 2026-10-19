#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synapse representing a connection between neurons
 * 
 * This class implements real synaptic transmission with delays and short-term plasticity.
 * Synapses are the fundamental communication channels in the neural network, allowing
 * neurons to influence each other's activity through electrical and chemical signals.
 * 
 * Key features:
 * - Direction-specific connections (source -> destination)
 * - Synaptic delays for realistic signal transmission timing
 * - Multiple synapse types (excitatory, inhibitory, modulatory)
 * - Spike-timing-dependent plasticity (STDP)
 * - Hebbian learning mechanisms
 * - Reward-modulated plasticity
 * - Eligibility traces for delayed reward signals
 * - Short-term synaptic plasticity (use-dependent efficacy)
 * - Integration with neural dynamics and neuron state
 */
class Synapse {
public:
    /**
     * @brief Constructor creates synapse with specified connection parameters
     * @param id Unique identifier for this synapse
     * @param source Neuron that sends signals (presynaptic)
     * @param destination Neuron that receives signals (postsynaptic)
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor cleans up synapse resources
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor is deleted (synapses are unique connections)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor enables efficient transfer of synapse ownership
     * @param other Synapse to move from (becomes invalid after move)
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator enables efficient transfer of synapse ownership
     * @param other Synapse to move from (becomes invalid after assignment)
     * @return Reference to this synapse after move operation
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Gets the unique identifier of this synapse
     * @return SynapseId of this synapse
     */
    SynapseId getId() const;
    
    /**
     * @brief Gets the source neuron (presynaptic neuron)
     * @return NeuronId of the neuron that sends signals
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Gets the destination neuron (postsynaptic neuron)
     * @return NeuronId of the neuron that receives signals
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Gets the current synaptic weight
     * @return Synaptic weight (typically 0.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Sets the synaptic weight directly
     * @param weight New synaptic weight value
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adds to the synaptic weight (for incremental updates)
     * @param delta Change in synaptic weight
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Gets the synaptic transmission delay
     * @return Delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Sets the synaptic transmission delay
     * @param delay New delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Gets the synapse type
     * @return Synapse type (Excitatory, Inhibitory, Modulatory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Sets the synapse type
     * @param type New synapse type to assign
     */
    void setType(SynapseType type);
    
    /**
     * @brief Checks if this is an excitatory synapse
     * @return true if synapse promotes neural excitation
     */
    bool isExcitatory() const;
    
    /**
     * @brief Checks if this is an inhibitory synapse
     * @return true if synapse suppresses neural activity
     */
    bool isInhibitory() const;
    
    /**
     * @brief Records a presynaptic spike for plasticity computations
     * @param timestamp Time when presynaptic neuron spiked
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Records a postsynaptic spike for plasticity computations
     * @param timestamp Time when postsynaptic neuron spiked
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the history of presynaptic spikes
     * @return Vector of presynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Gets the history of postsynaptic spikes
     * @return Vector of postsynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clears the spike history
     */
    void clearHistory();
    
    /**
     * @brief Gets the plasticity flags for this synapse
     * @return Const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets the plasticity flags for this synapse (non-const version)
     * @return Non-const reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity rules for this synapse
     * @param hebbian Enable Hebbian plasticity (co-activation based)
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Gets the eligibility trace (for reward-modulated learning)
     * @return Current eligibility trace value (typically 0.0 to 1.0)
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Sets the eligibility trace value
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decays the eligibility trace over time
     * @param decayRate Decay factor (typically 0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Gets the current synaptic efficacy (use-dependent modulation)
     * @return Synaptic efficacy factor (typically 0.0 to 1.0)
     */
    float getEfficacy() const;
    
    /**
     * @brief Sets the synaptic efficacy (use-dependent modulation)
     * @param efficacy New efficacy factor
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Updates synapse for one simulation step
     * 
     * This function implements the synaptic dynamics including signal transmission,
     * plasticity updates, and short-term modulation effects. The implementation
     * details are hidden in the Pimpl structure to maintain clean interface separation.
     * 
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets synapse to initial resting state
     */
    void reset();
    
    /**
     * @brief Initializes synapse with random biologically plausible parameters
     * @param rng Random number generator for stochastic initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation detail structure
     * 
     * This structure holds all the implementation details of the Synapse class,
     * including the actual synaptic transmission dynamics, plasticity computations,
     * delay handling, and short-term plasticity mechanisms. This separation allows
     * for better encapsulation and easier maintenance of the complex synaptic code.
     */
    struct Impl;
    Impl* pImpl;  // Pointer to implementation
};

/**
 * @brief Create a new Synapse with specified connection parameters
 * @param id Unique identifier for the synapse
 * @param source Neuron that sends signals (presynaptic)
 * @param destination Neuron that receives signals (postsynaptic)
 * @return Shared pointer to the newly created synapse
 */
inline std::shared_ptr<Synapse> createSynapse(SynapseId id, NeuronId source, NeuronId destination) {
    return std::make_shared<Synapse>(id, source, destination);
}

} // namespace nlm

/**
 * @brief Helper function to create a synapse between two neurons with random parameters
 * @param sourceNeuronId ID of the presynaptic neuron
 * @param destinationNeuronId ID of the postsynaptic neuron
 * @param rng Random number generator for parameter initialization
 * @return Shared pointer to the created synapse
 */
inline std::shared_ptr<Synapse> createRandomSynapse(NeuronId sourceNeuronId, NeuronId destinationNeuronId, RandomGenerator& rng) {
    auto synapse = createSynapse(INVALID_SYNAPSE_ID, sourceNeuronId, destinationNeuronId);
    synapse->initializeRandom(rng);
    return synapse;
}
