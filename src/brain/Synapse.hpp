#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    /**
     * @brief Construct a new Synapse object
     * 
     * @param id Unique identifier for this synapse
     * @param source Neuron that provides the pre-synaptic signal
     * @param destination Neuron that receives the post-synaptic signal
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destroy the Synapse object and clean up resources
     */
    ~Synapse();
    
    // Disable copying, enable moving (RAII pattern)
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this synapse
     * 
     * @return SynapseId The synapse's identifier
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the source neuron identifier
     * 
     * @return NeuronId Identifier of the pre-synaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the destination neuron identifier
     * 
     * @return NeuronId Identifier of the post-synaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight (typically -1.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight
     * 
     * @param weight New synaptic weight (clamped to valid range)
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adjust the synaptic weight by adding a delta value
     * 
     * @param delta Change in synaptic weight (positive or negative)
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the synaptic transmission delay
     * 
     * @return Delay Delay in simulation steps (1ms timestep = 1 step)
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the synaptic transmission delay
     * 
     * @param delay New delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType Type of synapse (Excitatory, Inhibitory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type New synapse type
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if this is an excitatory synapse
     * 
     * @return true if synapse is excitatory, false otherwise
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if this is an inhibitory synapse
     * 
     * @return true if synapse is inhibitory, false otherwise
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a pre-synaptic spike for STDP calculations
     * 
     * @param timestamp Time when pre-synaptic neuron spiked
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a post-synaptic spike for STDP calculations
     * 
     * @param timestamp Time when post-synaptic neuron spiked
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of pre-synaptic spike times
     * 
     * @return const std::vector<Timestamp>& Vector of spike times for STDP
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the history of post-synaptic spike times
     * 
     * @return const std::vector<Timestamp>& Vector of spike times for STDP
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * 
     * @note Called at start of new simulation episode
     */
    void clearHistory();
    
    /**
     * @brief Get the current plasticity flags configuration
     * 
     * @return const PlasticityFlags& Current plasticity settings
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get non-const access to plasticity flags
     * 
     * @return PlasticityFlags& Reference for modifying plasticity settings
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity rules are enabled for this synapse
     * 
     * @param hebbian Enable Hebbian learning rule
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the current eligibility trace for reward-modulated learning
     * 
     * @return float Current eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace by a given decay rate
     * 
     * @param decayRate Fraction of trace to decay per timestep (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the current synaptic efficacy
     * 
     * @return float Synaptic efficacy (0.0 to 2.0, use-dependent modulation)
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy
     * 
     * @param efficacy New efficacy value (clamped to 0.0-2.0)
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Execute one step of synaptic dynamics
     * 
     * @param currentTime Current simulation timestamp
     * 
     * @note Updates short-term plasticity, eligibility traces, and efficacy
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * 
     * @note Used when starting a new simulation episode
     */
    void reset();
    
    /**
     * @brief Initialize synapse with biologically plausible random parameters
     * 
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
