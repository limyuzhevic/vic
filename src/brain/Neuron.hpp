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
// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
// Provides biologically inspired spiking behavior for neural simulation
// 
// The Neuron class models individual neurons as computational units that:
// - Integrate incoming synaptic inputs over time
// - Maintain membrane potential with leak dynamics
// - Generate action potentials when threshold is reached
// - Have refractory periods preventing excessive firing
// - Exhibit spike-frequency adaptation for realistic firing patterns
// 
// This implementation uses the Leaky Integrate-and-Fire model, a balance
// between biological accuracy and computational efficiency. While simplified
// compared to detailed neuron models (like Hodgkin-Huxley), LIF captures the
// essential dynamics of neural computation while remaining tractable for large-scale
// simulations.
class Neuron {
public:
    /**
     * @brief Constructor - Creates neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron within its brain/region.
     *           Must be valid and unique among neurons in the same region.
     * 
     * @note Neurons are created by brain regions during initialization.
     *       IDs are used for referencing neurons in synaptic connections and
     *       for debugging/logging purposes.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - Cleans up neuron resources
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor - Deleted (Neurons are not copyable)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator - Deleted (Neurons are not copyable)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - Enabled for efficient transfer
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - Enabled for efficient transfer
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get unique identifier for this neuron
     * 
     * @return NeuronId Unique identifier assigned during construction.
     * 
     * @note ID is immutable after creation and used for synaptic connections
     *       and debugging.
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type classification
     * 
     * @return NeuronType Type of neuron (Excitatory, Inhibitory, etc.).
     *         Determines synaptic output sign and functional role.
     * 
     * @details Neuron types influence:
     * - Synaptic output polarity (excitatory = positive, inhibitory = negative)
     * - Default firing characteristics
     * - Plasticity rule eligibility
     * - Integration into neural circuits
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type classification
     * 
     * @param type New neuron type classification.
     * 
     * @note Type changes may require re-initialization of some parameters
     *       and affects how the neuron participates in circuits.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Access complete neuron state structure
     * 
     * @return const NeuronState& Constant reference to neuron state.
     * 
     * @details State contains all dynamic variables for neuron computation:
     * - Membrane potential and dynamics parameters
     * - Firing history and adaptation variables
     * - Refractory state
     * - Synaptic inputs
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * 
     * @return NeuronState& Mutable reference for performance-critical code.
     * 
     * @warning Direct modification of state should be done with caution.
     *          Most operations should use higher-level methods.
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV).
     *         Values below -55mV indicate resting state, positive values indicate
     *         depolarization toward firing threshold.
     * 
     * @details Membrane potential represents the difference in electrical
     * potential across the neuronal membrane. It integrates:
     * - Ionic currents from leak channels
     * - Synaptic currents from incoming connections
     * - External injected current
     * - Current reset after spike generation
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * 
     * @param potential New membrane potential value (mV).
     * 
     * @warning Direct setting bypasses the integration dynamics. Use with caution
     *          as it may lead to unphysical states. Prefer injectCurrent() for
     *          adding to membrane potential.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add current to membrane potential
     * 
     * @param delta Change in membrane potential (mV).
     * 
     * @details This is the primary method for modifying membrane potential,
     * representing:
     * - Synaptic input currents
     * - External injected current
     * - Modulatory influences
     * 
     * @note Positive delta represents depolarization (towards firing),
     *       negative delta represents hyperpolarization (away from firing).
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold voltage
     * 
     * @return MembranePotential Threshold voltage (mV).
     *         When membrane potential reaches this value, neuron fires.
     * 
     * @details Threshold represents the voltage at which voltage-gated
     * sodium channels activate sufficiently to generate an action potential.
     * Typical biological neurons have thresholds around -55mV.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold voltage
     * 
     * @param threshold New threshold voltage (mV).
     * 
     * @note Changing threshold affects firing probability and is commonly used
     *       to implement gain control and attention modulation.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool True if neuron has generated a spike in current time step.
     * 
     * @details Firing status is reset after each simulation step.
     * Use getState().firingState for persistent firing state information.
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire (refractoryRemaining > 0).
     * 
     * @details Refractory period prevents excessive firing rates and models
     * the biological recovery time after spike generation.
     * Typical refractory periods are 1-5 simulation steps.
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state directly
     * 
     * @param state New firing state (Resting, Firing, Refractory, etc.).
     * 
     * @warning Direct state modification bypasses normal spike generation
     *          dynamics. Use for specialized control or initialization.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period.
     * 
     * @details Refractory period determines minimum interval between spikes.
     * Longer refractory periods reduce maximum firing rates but increase
     * temporal precision. Typical values range from 1-20 steps.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter
     * 
     * @details Called each simulation step to allow neuron to exit
     * refractory period when enough time has passed.
     */
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
