#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
     * @struct NeuronState
     * @brief Immutable state container for a neuron at a specific time.
     * @details Contains all internal variables needed to compute neuron dynamics:
     * - Membrane potentials (current, resting, threshold, reset)
     * - Electrical properties (leak conductance, synaptic conductance)
     * - Firing properties (rate, state, refractory timing)
     * - History (last spike time, adaptation variable)
     * @note This is a POD (Plain Old Data) structure for efficient copying.
     */
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
        // Phase 6: 9 steps, not 5. One action presentation is 10
        // brain steps, so a 5-step refractory let a neuron fire
        // twice inside a single burst; 9 steps caps it at about
        // one spike per presentation, which is the population's
        // natural burst rhythm anyway. This lowers the reported
        // rate without changing any drive constant (drive
        // scaling was tried and reverted: -30% drive silenced
        // the network entirely, 0 Hz, because this is a
        // threshold system with no gentle middle ground).
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @class Neuron
 * @brief Represents a single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.
 * @details This class models a biological neuron with:
 * - Membrane potential dynamics governed by LIF equations
 * - Refractory period to prevent unrealistic firing rates
 * - Spike history tracking for synaptic plasticity
 * - Synaptic connectivity management (incoming/outgoing connections)
 * - Plasticity state for STDP, Hebbian, and reward-modulated learning
 * - Population and region membership for network organization
 * 
 * The neuron implements the fundamental equation: τₘ dV/dt = - (V - Vᵣ) + R(Iₛᵢₙₐₚₛₜᵢcₛ + Iₑₓₜᵣₐₙₐₗ)
 * where τₘ is membrane time constant, V is membrane potential, and I represents currents.
 * 
 * Key features:
 * - Threshold-based spike generation when V ≥ Vₜₕ
 * - Spike reset to Vᵣ after firing
 * - Refractory period (9 steps for Phase 6) preventing consecutive spikes
 * - Spike-frequency adaptation via adaptation variable
 * - Fast synaptic transmission with delays
 * 
 * @ingroup brain
 * @author NLM Development Team
 * @version 0.1.0
 */
class Neuron {
public:
    /**
     * @brief Constructor with neuron ID.
     * @param[in] id Unique identifier for this neuron.
     * @details Creates neuron with default state parameters and initializes implementation.
     */
    explicit Neuron(NeuronId id);
    
/**
     * @brief Virtual destructor.
     * @details Cleans up implementation and releases resources.
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Returns the neuron's unique identifier.
     * @return NeuronId The unique identifier.
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (excitatory, inhibitory, or internal).
     * @return NeuronType The neuron type classification.
     * @details Neurons are classified into:
     * - Motor neurons: control motor actions
     * - Sensory neurons: receive environmental input
 * - Internal neurons: hidden processing units
     */
    NeuronType getType() const;

    /**
     * @brief Set the neuron's type.
     * @param[in] type The neuron type to assign.
     * @details Changes the neuron's functional role in the network.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's current state (constant access).
     * @return const NeuronState& Reference to the current state.
     * @details The state contains all internal variables needed for computation.
     */
    const NeuronState& getState() const;

    /**
     * @brief Get the neuron's current state (mutable access).
     * @return NeuronState& Reference to the current state for modification.
     * @warning Modifying state directly bypasses validation. Use setters instead.
     */
    NeuronState& getState();
    
    /**
     * @brief Get the neuron's membrane potential (voltage).
     * @return MembranePotential The current membrane potential in mV.
     * @details This is the primary state variable that integrates incoming currents.
     * Typical values: -70mV (resting), -55mV (threshold), +40mV (peak).
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Set the membrane potential directly.
     * @param[in] potential The new membrane potential in mV.
     * @warning Use with caution - may bypass biophysics validation.
     */
    void setMembranePotential(MembranePotential potential);

    /**
     * @brief Inject additional current to change membrane potential.
     * @param[in] delta Change in membrane potential (mV).
     * @details This represents postsynaptic potentials or external input.
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the neuron's firing threshold.
     * @return MembranePotential The firing threshold in mV.
     * @details When membrane potential reaches or exceeds this value, the neuron spikes.
     * Typical value: -55mV.
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Set the neuron's firing threshold.
     * @param[in] threshold New firing threshold in mV.
     * @details Must be greater than resting potential for valid neuron behavior.
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
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
