// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

using namespace nlm;

// =============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// =============================================================================

/**
 * @brief Constructs a new neuron with specified ID.
 * 
 * Initializes neuron with internal neuron type and invalid region/population IDs.
 * Membrane potential and other state variables are initialized to default values.
 * 
 * @param id Unique identifier for the neuron
 */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

/**
 * @brief Default destructor.
 */
Neuron::~Neuron() = default;

/**
 * @brief Move constructor.
 * 
 * Transfers ownership of the implementation from another neuron.
 * 
 * @param other Neuron to transfer ownership from
 */
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator.
 * 
 * Transfers ownership of the implementation from another neuron and cleans up previous.
 * 
 * @param other Neuron to transfer ownership from
 * @return Reference to this neuron after assignment
 */
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// =============================================================================
// PROPERTY ACCESSORS AND MODIFIERS
// =============================================================================

/**
 * @brief Sets the neuron type (sensory, motor, or internal).
 * 
 * @param type Neuron type to set
 */
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

/**
 * @brief Sets the membrane potential.
 * 
 * @param potential New membrane potential value in mV
 */
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

/**
 * @brief Adds to the membrane potential.
 * 
 * @param delta Value to add to membrane potential in mV
 */
void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

/**
 * @brief Sets the firing threshold voltage.
 * 
 * @param threshold Firing threshold in mV
 */
void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

/**
 * @brief Sets the neuron's firing state.
 * 
 * @param state Firing state enum value
 */
void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

/**
 * @brief Decrements refractory period counter.
 * 
 * Advances refractory countdown and resets firing state when period ends.
 */
void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

/**
 * @brief Sets the neuron's firing rate.
 * 
 * @param rate Firing rate in Hz
 */
void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

/**
 * @brief Sets the membrane leak conductance.
 * 
 * @param conductance Leak conductance in µS
 */
void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

/**
 * @brief Gets the membrane leak conductance.
 * 
 * @return Leak conductance in µS
 */
MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

/**
 * @brief Sets the refractory period duration.
 * 
 * @param steps Number of simulation steps for refractory period
 */
void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

/**
 * @brief Gets the refractory period duration.
 * 
 * @return Refractory period in simulation steps
 */
uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

/**
 * @brief Sets the resting membrane potential.
 * 
 * @param potential Resting potential in mV
 */
void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

/**
 * @brief Gets the resting membrane potential.
 * 
 * @return Resting potential in mV
 */
MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

/**
 * @brief Sets the reset membrane potential.
 * 
 * @param potential Reset potential in mV
 */
void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

// =============================================================================
// SPIKE DETECTION AND RECORDING
// =============================================================================

/**
 * @brief Checks if neuron has reached firing threshold.
 * 
 * @return true if membrane potential >= threshold, false otherwise
 */
bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

/**
 * @brief Gets the time of the last spike.
 * 
 * @return Time of last spike in ms, or -1.0 if no spike occurred
 */
float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

// =============================================================================
// SYNAPTIC INPUT HANDLING
// =============================================================================

/**
 * @brief Receives excitatory synaptic input.
 * 
 * Excitatory inputs increase membrane potential via AMPA/NMDA-like currents.
 * The amplitude represents synaptic conductance * reversal potential contribution.
 * 
 * @param amplitude Excitatory current amplitude in mV
 */
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

/**
 * @brief Receives inhibitory synaptic input.
 * 
 * Inhibitory inputs decrease membrane potential via GABA-mediated currents.
 * The amplitude represents inhibitory synaptic conductance contribution.
 * 
 * @param amplitude Inhibitory current amplitude in mV
 */
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

/**
 * @brief Receives modulatory synaptic input.
 * 
 * Modulatory inputs affect plasticity rules but don't directly integrate.
 * Used for neuromodulation (e.g., dopamine, acetylcholine, serotonin).
 * 
 * @param amplitude Modulatory input amplitude in mV
 */
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

/**
 * @brief Injects direct current into neuron.
 * 
 * Direct current injection simulates external input (e.g., from sensory input)
 * or artificial stimulation. Added to synaptic input for LIF integration.
 * 
 * @param current Current to inject in pA
 */
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

/**
 * @brief Clears synaptic input accumulator.
 * 
 * Resets synaptic input to zero for the next integration step.
 */
void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

// =============================================================================
// SPIKE RECORDING AND HISTORY MANAGEMENT
// =============================================================================

/**
 * @brief Records a spike at the given timestamp.
 * 
 * Maintains spike history for plasticity and analysis. Old spikes are evicted
 * to maintain memory limits.
 * 
 * @param timestamp Time of spike in ms
 */
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

/**
 * @brief Clears spike history.
 * 
 * Removes all recorded spike times from memory.
 */
void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

// =============================================================================
// SYNAPSE CONNECTIVITY MANAGEMENT
// =============================================================================

/**
 * @brief Adds incoming synaptic connection.
 * 
 * Registers a synapse that provides input to this neuron.
 * 
 * @param handle Synapse handle for incoming connection
 */
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

/**
 * @brief Adds outgoing synaptic connection.
 * 
 * Registers a synapse that provides output from this neuron.
 * 
 * @param handle Synapse handle for outgoing connection
 */
void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

// =============================================================================
// STATE ACCESSORS
// =============================================================================

/**
 * @brief Gets reference to neuron state.
 * 
 * @return Reference to neuron state structure
 */
NeuronState& Neuron::getState() {
    return pImpl->state;
}

/**
 * @brief Gets reference to plasticity flags.
 * 
 * @return Reference to plasticity flags structure
 */
PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
 * @brief Enables specified plasticity mechanisms.
 * 
 * Configures which plasticity rules are active for this neuron.
 * 
 * @param hebbian Enable Hebbian plasticity if true
 * @param stdp Enable Spike-Timing-Dependent Plasticity if true
 * @param rewardModulated Enable reward-modulated plasticity if true
 */
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * @brief Sets the region ID for this neuron.
 * 
 * @param region Region identifier
 */
void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

/**
 * @brief Sets the population ID for this neuron.
 * 
 * @param population Population identifier
 */
void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

// =============================================================================
// LIF NEURAL DYNAMICS
// =============================================================================

/**
 * @brief Performs one step of LIF neural dynamics.
 * 
 * Implements the Leaky Integrate-and-Fire model with spike-frequency adaptation.
 * Updates membrane potential based on synaptic input, leak currents, and
 * adaptation mechanisms. Detects and handles spikes.
 * 
 * @param currentTime Current simulation time in ms
 * @param dt Time step duration in ms
 * @return true if neuron fired during this step, false otherwise
 */
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
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
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
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

/**
 * @brief Performs one simulation step using default LIF parameters.
 * 
 * Wrapper around stepLIF with standard 1ms timestep.
 * 
 * @param currentTime Current simulation time in ms
 */
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

// =============================================================================
// LIF NEURON INITIALIZATION
// =============================================================================

/**
 * @brief Resets neuron to initial state.
 * 
 * Restores membrane potential to resting state, clears spike history,
 * and resets all internal state variables.
 */
void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

/**
 * @brief Initializes neuron with random biological parameters.
 * 
 * Sets realistic default values for neuron properties with controlled
 * biological variation. Membrane potential starts near resting, threshold
 * is typically -55mV, and refractory period is 2-10ms.
 * 
 * @param rng Random number generator for parameter variation
 */
void Neuron::initializeRandom(RandomGenerator& rng) {
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