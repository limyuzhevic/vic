// Include all necessary headers for NLM operation
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <limits>
#include <cfloat>
#include <deque>

namespace nlm {

// Forward declaration of Impl for Neuron
struct Neuron::Impl {
    // Core identity
    NeuronId id;
    NeuronType type;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;
    
    // State management
    NeuronState state;
    
    // Synaptic and spike management
    std::deque<Timestamp> spikeHistory;
    float synapticInput;
    float adaptationVariable;
    
    // Plasticity
    PlasticityFlags plasticityFlags;
    
    // Synapse management
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    
    // Constants
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;
    static constexpr float TIME_CONSTANT = 20.0f;      // ms
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;   // nF
};

/**
     * @brief Create a new neuron with unique identifier
     * @details Creates a LIF neuron with biologically realistic parameters and
     *          initializes its internal state for neural computation.
     * @param id Unique identifier for the neuron
     */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->spikeHistory.reserve(Impl::MAX_SPIKE_HISTORY);
    pImpl->incomingSynapses.reserve(32);
    pImpl->outgoingSynapses.reserve(32);
}

/**
     * @brief Default destructor
     */
Neuron::~Neuron() = default;

/**
     * @brief Copy constructor (deleted)
     */
Neuron::Neuron(const Neuron&) = delete;

/**
     * @brief Copy assignment operator (deleted)
     */
Neuron& Neuron::operator=(const Neuron&) = delete;

/**
     * @brief Move assignment operator
     * @details Transfer ownership of implementation from another neuron
     * @param other Neuron to transfer from
     * @return Reference to this neuron
     */
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
     * @brief Set neuron type (excitatory, inhibitory, sensory, etc.)
     * @details Neurons perform different functions based on their type.
     *          Excitatory neurons increase activity, inhibitory neurons decrease it,
     *          sensory neurons detect external stimuli, motor neurons trigger actions.
     * @param type The neuron type
     */
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

/**
     * @brief Set membrane potential
     * @details Direct control of neuron's electrical state. Typical resting
     *          potential is around -70mV. Values above threshold trigger spiking.
     * @param potential Membrane potential in millivolts (mV)
     */
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

/**
     * @brief Add to membrane potential
     * @details Increment membrane potential by delta. Useful for applying
     *          synaptic inputs or external current injections.
     * @param delta Amount to add in millivolts
     */
void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

/**
     * @brief Set firing threshold
     * @details The voltage at which the neuron fires a spike. Typical threshold
     *          is around -55mV. Must be greater than resting potential for stability.
     * @param threshold Firing threshold in millivolts (mV)
     */
void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

/**
     * @brief Set firing state
     * @details Controls the current state of the neuron in its life cycle.
     *          Resting, Active, or Refractory states represent different processing phases.
     * @param state The firing state
     */
void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

/**
     * @brief Decrement refractory period counter
     * @details Reduces remaining refractory time. When refractory period reaches
     *          zero, neuron can fire again and returns to resting state.
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
     * @brief Set firing rate for rate-based computation
     * @details Sets the target firing rate in Hertz (spikes per second).
     *          Used in rate-coded neural networks and for statistics.
     * @param rate Firing rate in Hz
     */
void Neuron::setFiringRate(FiringRate rate) {
    pImpl::state.firingRate = rate;
}

/**
     * @brief Set membrane leak conductance
     * @details Controls how quickly the neuron returns to resting potential
     *          in absence of inputs. Higher conductance = faster decay.
     * @param conductance Leak conductance in nanoSiemens (nS)
     */
void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

/**
     * @brief Get membrane leak conductance
     * @return Current leak conductance in nS
     */
MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

/**
     * @brief Set refractory period duration
     * @details After firing, neuron cannot fire again for this many steps.
     *          Prevents uncontrolled spiking and models biological refractory periods.
     * @param steps Number of simulation steps to remain refractory
     */
void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

/**
     * @brief Get current refractory period
     * @return Refractory period in simulation steps
     */
uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

/**
     * @brief Set resting potential
     * @details The baseline membrane potential when neuron is not receiving inputs.
     *          Typical value is -70mV. Must be below threshold for stability.
     * @param potential Resting potential in millivolts (mV)
     */
void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

/**
     * @brief Get resting potential
     * @return Resting potential in mV
     */
MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

/**
     * @brief Set reset potential (post-spike reset value)
     * @details The value to which membrane potential is reset after firing.
     *          Typically slightly below resting potential to ensure stability.
     * @param potential Reset potential in millivolts (mV)
     */
void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

/**
     * @brief Check if neuron has reached firing threshold
     * @details Core function for spike detection. Compares current membrane
     *          potential with threshold to determine if neuron should fire.
     * @return true if neuron should fire, false otherwise
     */
bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

/**
     * @brief Get last spike time
     * @details Returns the timestamp of the most recent spike, or -1.0f if never spiked.
     *          Used for STDP and spike-timing dependent computations.
     * @return Time of last spike, or -1.0f if no spikes recorded
     */
float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

/**
     * @brief Receive excitatory synaptic input
     * @details Add excitatory current to neuron. Excitatory inputs increase
     *          membrane potential and can trigger spikes.
     * @param amplitude Synaptic current amplitude in millivolts
     */
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

/**
     * @brief Receive inhibitory synaptic input
     * @details Subtract inhibitory current from neuron. Inhibitory inputs decrease
     *          membrane potential and can prevent spiking.
     * @param amplitude Synaptic current amplitude in millivolts
     */
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

/**
     * @brief Receive modulatory input
     * @details Modulatory inputs affect plasticity and learning without directly
     *          changing membrane potential. Used for neuromodulation (dopamine, etc.).
     * @param amplitude Modulatory input amplitude
     */
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

/**
     * @brief Inject external current into neuron
     * @details Direct current injection from external sources like sensory inputs
     *          or neuromodulatory signals. Adds to synaptic input for LIF integration.
     * @param current Current in millivolts
     */
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

/**
     * @brief Clear accumulated synaptic input
     * @details Resets synaptic input accumulator for next simulation step.
     *          Called automatically after each step to prevent carryover.
     */
void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

/**
     * @brief Record spike occurrence
     * @details Stores spike timestamp for plasticity computations and statistics.
     *          Maintains spike history for STDP and other timing-dependent rules.
     * @param timestamp Time of spike
     */
void Neuron::recordSpike(Timestamp timestamp) {
    // Use circular buffer approach with deque for O(1) operations
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        // Remove oldest spike (FIFO behavior)
        pImpl->spikeHistory.pop_front();
    }
    pImpl->spikeHistory.push_back(timestamp);
}

/**
     * @brief Clear spike history
     * @details Removes all recorded spikes. Used for resetting learning state
     *          or when starting a new simulation episode.
     */
void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

/**
     * @brief Add incoming synapse connection
     * @details Register a synapse that delivers input to this neuron. Maintains
     *          connection count for efficient processing.
     * @param handle Synapse handle for incoming connection
     */
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

/**
     * @brief Add outgoing synapse connection
     * @details Register a synapse that delivers output from this neuron. Maintains
     *          connection count for efficient spike propagation.
     * @param handle Synapse handle for outgoing connection
     */
void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

/**
     * @brief Get reference to neuron state
     * @details Access the complete neuron state structure. Used by other systems
     *          that need to read or modify neuron properties.
     * @return Reference to NeuronState structure
     */
NeuronState& Neuron::getState() {
    return pImpl->state;
}

/**
     * @brief Get const reference to neuron state
     * @details Const version of getState for read-only access to neuron state.
     * @return Const reference to NeuronState structure
     */
const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

/**
     * @brief Get plasticity flags
     * @details Returns flags controlling which plasticity rules apply to this neuron.
     *          Enables selective learning mechanisms.
     * @return Reference to PlasticityFlags structure
     */
const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

/**
     * @brief Get plasticity flags (non-const version)
     * @details Non-const version of getPlasticityFlags for modifying plasticity settings.
     * @return Reference to PlasticityFlags structure
     */
PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
     * @brief Enable specific plasticity rules
     * @details Configure which learning mechanisms apply to this neuron.
     *          Common combinations include STDP for spike timing, Hebbian for co-activity,
     *          and reward-modulated learning.
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated learning
     */
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
     * @brief Set region identifier
     * @details Identifies which brain region this neuron belongs to. Regions
     *          organize functionally specialized areas of the brain.
     * @param region Region ID
     */
void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

/**
     * @brief Get region identifier
     * @return Region ID, or INVALID_REGION_ID if not assigned
     */
RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

/**
     * @brief Set population identifier
     * @details Neurons within a region are organized into populations based on
     *          function (e.g., sensory, motor, association).
     * @param population Population ID
     */
void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

/**
     * @brief Get population identifier
     * @return Population ID, or INVALID_POPULATION_ID if not assigned
     */
PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

/**
     * @brief Execute one step of LIF dynamics
     * @details Core neural computation implementing Leaky Integrate-and-Fire dynamics.
     *          Updates membrane potential based on synaptic input and leak, detects
     *          spikes, handles refractory period, and applies adaptation.
     * @param currentTime Current simulation time
     * @param dt Simulation timestep in milliseconds
     * @return true if neuron fired this step, false otherwise
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
     * @brief Default LIF step with standard timestep
     * @details Convenience function for one millisecond timesteps. Calls
     *          stepLIF with dt = 0.001 ms.
     * @param currentTime Current simulation time
     */
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

/**
     * @brief Reset neuron to initial state
     * @details Clears all internal state and resets membrane potential to
     *          resting value. Essential for running multiple simulation episodes.
     */
void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

/**
     * @brief Initialize neuron with biologically realistic random parameters
     * @details Sets up neuron with random but biologically plausible values for
     *          membrane potentials, thresholds, refractory periods, etc.
     * @param rng Random number generator for reproducible initialization
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
