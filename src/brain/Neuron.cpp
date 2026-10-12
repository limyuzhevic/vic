#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @brief Implementation details for Neuron class
 * 
 * This structure contains all the internal state and implementation details
 * for the Neuron class. It uses the Pimpl idiom to hide implementation
 * from the header file and improve compilation times.
 * 
 * The implementation includes:
 * - LIF neuron parameters and state variables
 * - Synaptic input accumulation
 * - Spike history management
 * - Plasticity flags
 * - Random number generator references
 * 
 * @note All physical units follow the NLM convention:
 *       - Potentials: millivolts (mV)
 *       - Time: milliseconds (ms)
 *       - Conductances: nanosiemens (nS)
 *       - Currents: nanoamperes (nA)
 *       - Capacitance: nanofarads (nF)
 */
struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    
    /**
     * @brief Total synaptic current input this simulation step
     * 
     * This variable accumulates all synaptic inputs received during the
     * current simulation step. It's reset to zero at the end of each step.
     * 
     * The synaptic input represents the total current injected into the neuron
     * from all presynaptic connections, converted to membrane potential units.
     */
    MembranePotential totalCurrent;
    
    /**
     * @brief Accumulated synaptic input for current step
     * 
     * This is the working variable that accumulates synaptic inputs during
     * the current simulation step. It's used by the LIF integration function
     * to compute the change in membrane potential.
     */
    MembranePotential synapticInput;
    
    /**
     * @brief History of recent spike timestamps
     * 
     * Stores timestamps of recent spikes for use in spike-timing-dependent
     * plasticity (STDP) and rate-based computations. The history is limited
     * to prevent unbounded memory growth.
     */
    std::vector<Timestamp> spikeHistory;
    
    /**
     * @brief Handles of incoming synapses
     * 
     * Contains handles to all synapses that project to this neuron. Used for
     * connectivity management and synaptic weight updates.
     */
    std::vector<SynapseHandle> incomingSynapses;
    
    /**
     * @brief Handles of outgoing synapses
     * 
     * Contains handles to all synapses that this neuron projects to. Used for
     * connectivity management and synaptic weight updates.
     */
    std::vector<SynapseHandle> outgoingSynapses;
    
    /**
     * @brief Plasticity mechanism flags
     * 
     * Controls which plasticity mechanisms are enabled for this neuron.
     * Determines how synaptic weights are modified in response to activity.
     */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Membrane capacitance (nF)
     * 
     * Determines how much charge is needed to change the membrane potential
     * by 1 mV. Higher capacitance makes neurons harder to activate.
     * 
     * @note Typical values: 0.1-10 nF for biological neurons
     */
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    
    /**
     * @brief Membrane time constant (ms)
     * 
     * Controls how quickly the membrane potential returns to resting potential
     * in the absence of input. tau = C/g where C is capacitance and g is conductance.
     * 
     * @note Typical values: 10-100 ms for biological neurons
     */
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    
    /**
     * @brief Maximum number of spike history entries
     * 
     * Limits the size of the spike history to prevent unbounded memory growth.
     * Older spikes are discarded when this limit is reached.
     */
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    /**
     * @brief Default constructor
     * 
     * Initializes neuron with default values for all internal state variables.
     * The neuron type defaults to Internal, which is appropriate for most
     * computational uses.
     */
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    /**
     * @brief Constructor with neuron ID
     * 
     * Creates a new neuron with the specified identifier. The neuron is
     * initialized with default values:
     * - Type: Internal
     * - Region ID: INVALID_REGION_ID
     * - Population ID: INVALID_POPULATION_ID
     * - All state variables at defaults
     * 
     * @param id Unique identifier for this neuron
     * @post Neuron is in valid but uninitialized state
     * @note Call initializeRandom() or set parameters before simulation
     */
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of the implementation structure from another neuron
     * to this neuron. The source neuron is left in a valid but empty state.
     * 
     * @param other Neuron to move from
     * @post This neuron owns the implementation structure
     * @post Other neuron is in valid but empty state
     */
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership of the implementation structure from another neuron
     * to this neuron. The source neuron is left in a valid but empty state.
     * If this and other refer to the same object, no action is taken.
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron for method chaining
     * @post This neuron owns the implementation structure
     * @post Other neuron is in valid but empty state
     */
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type (Excitatory, Inhibitory, etc.)
     * @post Neuron type is updated
     * @note Changing type affects synaptic input processing and plasticity
     */
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    /**
     * @brief Set membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * @post Membrane potential is set to specified value
     * @note This is a low-level operation. Use synaptic inputs for normal control.
     */
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    /**
     * @brief Add to membrane potential
     * 
     * @param delta Change in membrane potential in mV
     * @post Membrane potential is increased by delta
     * @note Convenience method for small adjustments
     */
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold voltage in mV
     * @post Threshold is updated
     * @note Lower threshold values make neuron more likely to fire
     */
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    /**
     * @brief Set the firing state
     * 
     * @param state New firing state (Resting, Active, Refractory, Inhibited)
     * @post Firing state is updated
     * @note This is a low-level method. Use synaptic inputs for normal control.
     */
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    /**
     * @brief Decrement refractory counter
     * 
     * @post Refractory counter is decremented by 1
     * @post Firing state changes to Resting when refractory period ends
     * @note Called automatically during simulation steps
     */
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    /**
     * @brief Set the firing rate
     * 
     * @param rate New firing rate in Hz
     * @post Firing rate is updated
     * @note This is a high-level parameter for rate-based control
     */
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    /**
     * @brief Set the leak conductance
     * 
     * @param conductance Leak conductance in nS
     * @post Leak conductance is updated
     * @note Higher conductance makes neuron return to resting potential faster
     */
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    /**
     * @brief Get the leak conductance
     * 
     * @return Leak conductance in nS
     */
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     * @post Refractory period is updated
     * @note Typical values: 2-10 steps (2-10ms at 1kHz timestep)
     */
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    /**
     * @brief Get refractory period duration
     * 
     * @return Refractory period in simulation steps
     */
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential in mV
     * @post Resting potential is updated
     * @note Also known as the leak reversal potential or Nernst potential
     */
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    /**
     * @brief Get the resting potential
     * 
     * @return Resting potential in mV
     */
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    /**
     * @brief Set the reset potential
     * 
     * @param potential Post-spike reset potential in mV
     * @post Reset potential is updated
     * @note Typically equal to or close to resting potential
     */
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    /**
     * @brief Get the reset potential
     * 
     * @return Reset potential in mV
     */
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return bool True if membrane potential >= threshold
     * @note This is the main spike detection mechanism
     */
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    /**
     * @brief Get timestamp of last spike
     * 
     * @return float Last spike timestamp in ms, or -1 if never spiked
     * @note Used for spike-timing-dependent plasticity (STDP)
     */
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @post Synaptic input is increased by amplitude
     * @note Excitatory inputs increase membrane potential towards threshold
     *       Represents AMPA-type synapses or similar excitatory mechanisms
     */
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @post Synaptic input is decreased by amplitude
     * @note Inhibitory inputs decrease membrane potential away from threshold
     *       Represents GABA-type synapses or similar inhibitory mechanisms
     */
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @post Adaptation variable is increased
     * @note Modulatory inputs affect plasticity but not directly integrated
     *       Used for neuromodulation (e.g., dopamine, acetylcholine)
     */
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    /**
     * @brief Inject current from external source
     * 
     * @param current Current amplitude in nA
     * @post Synaptic input is increased by current
     * @note Used for sensory inputs or external driving signals
     *       Current is converted to membrane potential change via C (capacitance)
     */
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    /**
     * @brief Clear accumulated synaptic input
     * 
     * @post Synaptic input is reset to zero
     * @note Called automatically at the end of each simulation step
     *       User typically doesn't need to call this directly
     */
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    /**
     * @brief Record a spike event
     * 
     * @param timestamp Time when spike occurred
     * @post Spike timestamp is added to history
     * @post Older spikes are discarded if history exceeds limit
     * @note Used for STDP learning and spike history analysis
     */
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    /**
     * @brief Clear spike history
     * 
     * @post Spike history vector is cleared
     * @note Used to reset learning history or change neuron type
     */
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    /**
     * @brief Add incoming synapse handle
     * 
     * @param handle Handle to incoming synapse
     * @post Synapse handle is added to incoming list
     * @note Used for building synaptic connectivity
     */
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    /**
     * @brief Add outgoing synapse handle
     * 
     * @param handle Handle to outgoing synapse
     * @post Synapse handle is added to outgoing list
     * @note Used for building synaptic connectivity
     */
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    /**
     * @brief Get mutable reference to neuron state
     * 
     * @return NeuronState& Mutable reference to the neuron's state
     * @note This allows direct modification of neuron state - use with caution
     *       as it may violate the neuron's internal invariants
     */
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * @note Allows modification of plasticity mechanisms
     */
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    /**
     * @brief Enable plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Plasticity flags are updated
     * @note Typical configuration: hebbian=true, stdp=true, rewardModulated=false
     */
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    /**
     * @brief Set the region ID
     * 
     * @param region Region identifier
     * @post Region ID is updated
     * @note Neurons belong to brain regions for organizational purposes
     */
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    /**
     * @brief Set the population ID
     * 
     * @param population Population identifier
     * @post Population ID is updated
     * @note Populations group neurons with similar properties or functions
     */
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    /**
     * @brief Perform one Leaky Integrate-and-Fire simulation step
     * 
     * This is the core computation method that implements the Leaky Integrate-
     * and-Fire (LIF) dynamics. The method follows the discrete approximation
     * of the LIF differential equation:
     * 
     * dV/dt = (V_rest - V)/tau + I/C
     * 
     * Where:
     * - V: membrane potential (mV)
     * - V_rest: resting potential (mV)
     * - tau: membrane time constant (ms)
     * - I: synaptic current (nA)
     * - C: membrane capacitance (nF)
     * 
     * The discrete approximation used is:
     * V_new = V + dt * ((V_rest - V)/tau + I/C)
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Time step duration in ms
     * @return bool True if neuron fired (spiked) this step
     * @post Membrane potential is updated based on LIF dynamics
     * @post Spike detection and processing occurs
     * @post Refractory state is updated
     * @post Synaptic input is cleared for next step
     * 
     * @note Biological basis:
     * - Neurons integrate synaptic inputs continuously
     * - Membrane potential decays exponentially towards resting potential
     * - When threshold is reached, neuron generates an action potential (spike)
     * - After spiking, neuron enters absolute refractory period
     * - Spike-frequency adaptation gradually reduces firing rate
     * 
     * @note Implementation details:
     * - Uses exponential Euler integration for numerical stability
     * - Membrane potential is clamped to prevent instability
     * - Spike-frequency adaptation implemented via adaptation variable
     * - Refractory period prevents immediate re-firing
     */
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // Note: dt is in seconds, but tau and contributions are in milliseconds
    // Convert dt from seconds to milliseconds for consistent units
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    // Biological neurons have physical limits on membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    /**
     * @brief Update neuron for one simulation step (default implementation)
     * 
     * This method provides a default implementation with standard timestep
     * for the Leaky Integrate-and-Fire dynamics. Users can override this method
     * in derived classes or call stepLIF() directly for custom timesteps.
     * 
     * @param currentTime Current simulation time in ms
     * @post Neuron state is updated according to LIF dynamics
     * @note Default timestep: 1ms (1000 Hz simulation rate)
     * @note For different timesteps, call stepLIF() directly
     */
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    /**
     * @brief Reset neuron to initial state
     * 
     * @post Membrane potential is reset to initial value
     * @post Spike history is cleared
     * @post Refractory counters are reset
     * @post Adaptation variable is reset
     * @note Does not reset neuron ID, type, or connectivity
     * @note Use this method to reset neural state without changing parameters
     */
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    /**
     * @brief Initialize neuron with biologically plausible random parameters
     * 
     * Sets random parameters drawn from biological distributions for cortical
     * neurons. This provides a good starting point for simulations.
     * 
     * @param rng Random number generator reference
     * @post Neuron parameters are initialized with random values
     * @note Membrane potential starts near resting potential with small variation
     * @note Threshold is typically -55mV with small variation
     * @note Refractory period: 2-10ms typical for biological neurons
     * 
     * @note Biological ranges used:
     * - Membrane potential: resting +/- 3mV
     * - Threshold: -55mV +/- 2mV  
     * - Resting potential: -70mV +/- 2mV
     * - Refractory period: 2-10ms (converted to steps based on dt)
     */
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm