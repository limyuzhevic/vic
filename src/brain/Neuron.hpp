#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    /**
     * @brief Get the neuron's functional type (excitatory/inhibitory/internal)
     * 
     * @return NeuronType Type classification affecting synaptic properties
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's functional type
     * 
     * @param type New neuron type affecting synaptic transmission and plasticity
     */
    void setType(NeuronType type);
    
    /**
     * @brief Access to complete neuron state for advanced operations
     * 
     * @return const NeuronState& Read-only access to internal state (thread-safe for reading)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Access to complete neuron state for modification
     * 
     * @return NeuronState& Write access to internal state (caller must ensure thread safety)
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * 
     * @note Generally prefer using injectCurrent() for biologically realistic inputs
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add current-based input to membrane potential
     * 
     * Simulates synaptic or external current injection affecting membrane potential
     * @param delta Change in membrane potential (in mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Perform one LIF integration step and check for spike
     * 
     * Implements the Leaky Integrate-and-Fire dynamics model for neural computation.
     * Updates membrane potential based on inputs and determines if neuron fires.
     * 
     * @param currentTime Current simulation time (for spike timing)
     * @param dt Simulation timestep in milliseconds
     * @return bool True if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Models post-synaptic excitation from presynaptic neurons
     * @param amplitude Synaptic conductance * reversal potential contribution (mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Models post-synaptic inhibition from presynaptic neurons
     * @param amplitude Synaptic conductance * reversal potential contribution (mV)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Neuromodulators affect plasticity and adaptation, not directly integrated
     * into membrane potential (used for dopamine, acetylcholine, etc.)
     * @param amplitude Modulatory input strength
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject direct current from external sources
     * 
     * Simulates sensory input or external stimulation driving the neuron
     * @param current Current injection in nA (converted to mV effect)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total current affecting this neuron this timestep
     * 
     * Includes both synaptic and injected currents
     * @return MembranePotential Total current contribution (mV)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic current for next timestep
     * 
     * Resets synaptic input integration to prevent carry-over between steps
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike timestamp for plasticity and STDP
     * 
     * Maintains spike history used by STDP and other plasticity rules
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history
     * 
     * @return const std::vector<Timestamp>& Vector of recent spike timestamps (circular buffer)
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Used during reset or to prepare for new recording window
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming connection to this neuron
     * 
     * Used by Brain during synapse formation for maintaining connectivity lists
     * @param handle Handle to incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing connection from this neuron
     * 
     * Used by Brain during synapse formation for maintaining connectivity lists
     * @param handle Handle to outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get list of incoming synapses (read-only)
     * 
     * @return const std::vector<SynapseHandle>& Handles of synapses terminating on this neuron
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get list of outgoing synapses (read-only)
     * 
     * @return const std::vector<SynapseHandle>& Handles of synapses originating from this neuron
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Access plasticity configuration flags
     * 
     * Determines which plasticity rules affect this neuron
     * @return const PlasticityFlags& Read-only access to plasticity settings
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Access plasticity configuration flags for modification
     * 
     * @return PlasticityFlags& Write access to plasticity settings (thread safety required)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure plasticity rules for this neuron
     * 
     * Enables/disables Hebbian, STDP, and reward-modulated plasticity
     * @param hebbian Enable Hebbian (correlation-based) learning
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the brain region this neuron belongs to
     * 
     * Regions group neurons for spatial and functional organization
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the brain region this neuron belongs to
     * 
     * @return RegionId Region identifier, or INVALID_REGION_ID if not assigned
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population within the region
     * 
     * Populations allow fine-grained organization within regions
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population within the region
     * 
     * @return PopulationId Population identifier, or INVALID_POPULATION_ID if not assigned
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * Default implementation uses standard 1ms timestep for stepLIF
     * @param currentTime Current simulation timestamp
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Restores default parameters and clears spike history. Useful for starting
     * new simulations or testing.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with biologically realistic random parameters
     * 
     * Sets membrane potential, threshold, resting potential, refractory period,
     * and adaptation variables to values drawn from biological distributions
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief PImpl pattern implementation details
     * 
     * All actual implementation hidden in this private struct to reduce compilation time
     * and allow forward declarations in the header.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
