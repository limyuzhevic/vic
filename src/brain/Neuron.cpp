#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Neuron::Impl {
    /// Neuron identity and metadata
    NeuronId id;                           ///< Unique identifier for this neuron
    NeuronType type;                       ///< Neuron classification (excitatory, inhibitory, etc.)
    NeuronState state;                     ///< Complete neuron state variables
    RegionId regionId;                     ///< ID of the brain region containing this neuron
    PopulationId populationId;             ///< ID of the neuron population this neuron belongs to
    MembranePotential totalCurrent;        ///< Total synaptic current input this step (nA)
    MembranePotential synapticInput;       ///< Accumulated synaptic input since last integration (mV)
    std::vector<Timestamp> spikeHistory;   ///< History of recent spike times (ms)
    std::vector<SynapseHandle> incomingSynapses;  ///< Handles of synapses that provide input to this neuron
    std::vector<SynapseHandle> outgoingSynapses;  ///< Handles of synapses that this neuron sends output to
    PlasticityFlags plasticityFlags;        ///< Plasticity mechanism flags and parameters
    
    // LIF model parameters
    /// Membrane capacitance determines how much voltage change is produced by a given current
    /// Typical biological values: 0.1-1.0 nF for cortical neurons
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    /// Membrane time constant (τ) controls the rate of membrane potential decay
    /// Biological range: 10-100ms for most neurons, with 20ms as typical cortical value
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;  ///< Maximum number of spikes to retain in history
    
    /// Constructor initializes neuron with default values
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

/// @brief Construct a new Neuron object with the specified ID
/// @param id Unique identifier for this neuron
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

/// @brief Move constructor
/// @note Efficiently transfers ownership of implementation pointer, other neuron becomes null
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/// @brief Move assignment operator
/// @param other Neuron to move from
/// @return Reference to this neuron after assignment
/// @note Efficiently transfers ownership, original becomes null
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/// @brief Set the neuron type (excitatory, inhibitory, modulatory, etc.)
/// @param type New neuron type to assign
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

/// @brief Set the membrane potential directly
/// @param potential New membrane potential value (mV)
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

/// @brief Increment the membrane potential by a delta value
/// @param delta Change in membrane potential to add (mV)
void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

/// @brief Set the firing threshold potential
/// @param threshold Membrane potential at which the neuron will fire (mV)
void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

/// @brief Set the firing state of the neuron
/// @param state New firing state (Resting, Active, Refractory, etc.)
void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

/// @brief Decrement refractory period counter and transition to resting state when complete
/// @note Handles automatic state transitions when refractory period ends
void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

/// @brief Set the firing rate (useful for rate-based computation models)
/// @param rate Firing rate in Hz
void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

/// @brief Set the leak conductance (inverse of membrane resistance)
/// @param conductance Leak conductance in nS
void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

/// @brief Get the leak conductance value
/// @return Leak conductance in nS
MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

/// @brief Set the refractory period duration
/// @param steps Number of simulation steps the neuron remains refractory
void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

/// @brief Get the refractory period duration
/// @return Refractory period in simulation steps
uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

/// @brief Set the resting (equilibrium) potential
/// @param potential Resting potential (mV), typically between -80mV and -60mV
void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

/// @brief Get the resting potential value
/// @return Resting potential in mV
MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

/// @brief Set the reset potential (membrane potential after a spike)
/// @param potential Reset potential (mV), typically close to resting potential
void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

/// @brief Check if the membrane potential has exceeded the firing threshold
/// @return true if membrane potential >= threshold, false otherwise
bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

/// @brief Get the timestamp of the last spike
/// @return Last spike time in ms, or -1 if neuron has never spiked
float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

/// @brief Receive excitatory synaptic input (post-synaptic potential)
/// @param amplitude Amplitude of excitatory input (mV)
/// @note Excitatory input increases membrane potential, bringing neuron closer to threshold
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

/// @brief Receive inhibitory synaptic input (hyperpolarizing current)
/// @param amplitude Amplitude of inhibitory input (mV)
/// @note Inhibitory input decreases membrane potential, moving neuron away from threshold
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

/// @brief Receive modulatory input (neuromodulator influence)
/// @param amplitude Amplitude of modulatory input
/// @note Modulatory input affects plasticity mechanisms rather than directly integrating
///       Examples: dopamine, acetylcholine, norepinephrine affecting synaptic strength
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

/// @brief Inject external current directly into the neuron
/// @param current External current input (nA)
/// @note Used for sensory input or external stimulation; integrates like synaptic input
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

/// @brief Clear the accumulated synaptic input for the next integration step
/// @note Should be called after integration to prepare neuron for next timestep
void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

/// @brief Record a spike event with timestamp
/// @param timestamp Time at which the spike occurred (ms)
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

/// @brief Clear the spike history
/// @note Used during neuron reset to prepare for new spike recording
void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

/// @brief Get the neuron's spike history
/// @return Span to the spike history
std::span<const Timestamp> Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

/// @brief Add an incoming synapse connection
/// @param handle Handle to the synapse that provides input to this neuron
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

/// @brief Add an outgoing synapse connection
/// @param handle Handle to the synapse that this neuron sends output to
void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

/// @brief Get a reference to the neuron's state
/// @return Reference to the neuron state structure
NeuronState& Neuron::getState() {
    return pImpl->state;
}

/// @brief Get a reference to the neuron's plasticity flags
/// @return Reference to plasticity flags structure
PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/// @brief Enable specific plasticity mechanisms
/// @param hebbian Enable Hebbian plasticity ( Spike-Timing Dependent Plasticity )
/// @param stdp Enable STDP
/// @param rewardModulated Enable reward-modulated plasticity
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/// @brief Set the brain region ID for this neuron
/// @param region Region ID where this neuron resides
void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

/// @brief Set the population ID for this neuron
/// @param population Population ID this neuron belongs to
void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

/// @brief Update membrane potential using Leaky Integrate-and-Fire dynamics
/// @param currentTime Current simulation timestamp (ms)
/// @param dt Simulation timestep (s)
/// @return true if neuron fired (threshold crossed), false otherwise
/// @note Uses exponential Euler integration with refractory period
/// @details The LIF equation is: dV/dt = (V_rest - V)/τ + I/C
///          where τ = 20ms (membrane time constant), C = 1nF
///          V resets to -65mV after spike with 2-10ms refractory period
/// @note Algorithm implements:
/// 1. Refractory period handling (prevents excessive firing)
/// 2. LIF potential update using Euler integration
/// 3. Spike detection and reset
/// 4. Adaptation mechanism for spike-frequency adaptation
/// 5. Refractory period initialization
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
    
    // Biological explanation:
    // - The leak term (V_rest - V)/τ represents the natural tendency of the membrane
    //   to return to its resting potential, modeling ion channel leakage
    // - The synaptic term I/C represents the change in membrane potential caused by
    //   synaptic currents (I) flowing through the membrane capacitance (C)
    // - This models how inputs from other neurons cause voltage changes
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms - membrane time constant
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF - membrane capacitance
    
    // Synaptic input contributes to membrane potential change
    // I/C: Synaptic current divided by capacitance gives dV/dt contribution
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    // (V_rest - V)/τ: Rate at which membrane potential decays toward resting potential
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // The factor 1000.0f converts from ms to s for dt in seconds
    // This implements: V_new = V + dt * (leakContribution + synapticContribution)
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // AdaptationVariable builds up with each spike and causes gradual hyperpolarization
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Each spike-frequency adaptation unit subtracts 10mV from membrane potential
        // This implements post-spike afterhyperpolarization (AHP)
        V -= pImpl->state.adaptationVariable * 0.01f;
        // Adaptation decays exponentially with time constant, reducing its effect over time
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    // Neurons have biophysical limits: -100mV (hyperpolarization limit) to +50mV (depolarization limit)
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike (threshold crossing)
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for STDP and monitoring
        recordSpike(currentTime);
        
        // Reset membrane potential to afterhyperpolarization value
        // This simulates the ion channel dynamics that occur after an action potential
        V = V_reset;
        
        // Enter refractory period: neuron cannot fire again for refractoryPeriod steps
        // This models the absolute refractory period where sodium channels are inactivated
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        // Each spike adds 1.0 unit to adaptation, implementing spike-triggered adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Neuron has not reached threshold but is in active state (excitable)
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    // This ensures that inputs from the current timestep don't accumulate
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

/// @brief Update neuron state for one simulation step
/// @param currentTime Current simulation timestamp (ms)
/// @note Uses default timestep of 1ms (0.001 seconds)
/// @warning This is a convenience wrapper around stepLIF with fixed timestep
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

/// @brief Reset neuron to initial default state
/// @note Resets membrane potential, firing state, spike history, and input accumulations
void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

/// @brief Initialize neuron parameters with biologically realistic random values
/// @param rng Random number generator for stochastic initialization
/// @note Implements biological constraints and typical parameter ranges
/// @details
/// - Membrane potential: near resting (-70mV ± 3mV)
/// - Threshold: -55mV ± 2mV (typical for cortical neurons)
/// - Resting potential: -70mV ± 2mV
/// - Reset potential: close to resting (-70mV to -35mV)
/// - Refractory period: 2-10ms (typical biological range)
/// - All state variables initialized to appropriate defaults
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
