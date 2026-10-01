/**
 * @file Neuron.cpp
 * @brief Implementation of the Leaky Integrate-and-Fire (LIF) neuron model
 * @author NLM Research Team
 * @date 2026
 * @version 1.0
 *
 * This file implements the LIF neuron model, which is the fundamental computational
 * unit in the NLM (熙然) artificial brain system. The LIF model provides a biologically
 * plausible approximation of neuronal dynamics while remaining computationally efficient.
 *
 * Key features:
 * - Leaky Integrate-and-Fire dynamics with exponential Euler integration
 * - Spike-frequency adaptation via adaptation variable
 * - Refractory period management
 * - Synaptic input integration
 * - Memory of recent spike history for STDP learning
 * - Random initialization with biological constraints
 *
 * Biological inspiration:
 * - Based on Hodgkin-Huxley model but simplified to 2 variables
 * - Exponential decay of membrane potential (leak)
 * - Threshold-based spike generation
 * - Refractory period after spike
 * - Adaptation mechanisms for spike-frequency control
 *
 * @note This implementation follows the classic LIF model with additional
 *       adaptations for realistic neural dynamics and learning capabilities.
 */

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @brief Implementation details for Neuron class
 * 
 * This struct contains the internal implementation details for Neuron,
 * implementing the PIMPL (Pointer-to-Implementation) pattern to reduce
 * compile-time dependencies and improve encapsulation.
 */

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters - biologically inspired constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF, typical cortical neuron
    static constexpr float TIME_CONSTANT = 20.0f;  // ms, membrane time constant
    static constexpr size_t MAX_SPIKE_HISTORY = 100;  // Number of spikes to remember
    
    /**
     * @brief Default constructor with biologically plausible initial values
     * 
     * Initializes neuron state variables to typical values found in cortical neurons.
     * Note: id, regionId, and populationId are intentionally left default-initialized
     * as they should be set explicitly by the brain system during construction.
     */
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

/**
 * @brief Neuron constructor
 * 
 * @param id Unique identifier for this neuron
 * 
 * Creates a new LIF neuron with the specified ID. The neuron starts in a
 * resting state with default parameters. The actual values for membrane potential,
 * threshold, and other parameters will be set during initialization via
 * the initializeRandom() method.
 */
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

/**
 * @brief Move constructor with proper resource management
 * 
 * Efficiently transfers ownership of the implementation pointer from another neuron.
 * This is essential for performance when working with large neural populations.
 */
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator with proper resource management
 * 
 * Safely transfers ownership of the implementation pointer from another neuron.
 * Ensures proper cleanup of the previously owned implementation.
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
 * @brief Retrieve the neuron's unique identifier
 * 
 * @return NeuronId The unique identifier assigned to this neuron
 */
NeuronId Neuron::getId() const {
    if (!pImpl) return INVALID_NEURON_ID;
    return pImpl->id;
}

/**
 * @brief Retrieve the neuron's type
 * 
 * @return NeuronType The type classification of this neuron (Internal, Sensory, Motor)
 */
NeuronType Neuron::getType() const {
    if (!pImpl) return NeuronType::Internal;
    return pImpl->type;
}

/**
 * @brief Set the neuron's type
 * 
 * @param type New neuron type
 */
void Neuron::setType(NeuronType type) {
    if (!pImpl) return;
    pImpl->type = type;
}

/**
 * @brief Set the membrane potential
 * 
 * @param potential New membrane potential in mV
 */
void Neuron::setMembranePotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.membranePotential = potential;
}

/**
 * @brief Add to the membrane potential
 * 
 * @param delta Change in membrane potential in mV
 */
void Neuron::addToMembranePotential(MembranePotential delta) {
    if (!pImpl) return;
    pImpl->state.membranePotential += delta;
}

/**
 * @brief Retrieve the current membrane potential
 * 
 * @return MembranePotential Current membrane potential in mV
 */
MembranePotential Neuron::getMembranePotential() const {
    if (!pImpl) return -70.0f;  // Default resting potential
    return pImpl->state.membranePotential;
}

/**
 * @brief Set the firing threshold
 * 
 * @param threshold New firing threshold in mV (typically -55mV)
 */
void Neuron::setThreshold(MembranePotential threshold) {
    if (!pImpl) return;
    pImpl->state.threshold = threshold;
}

/**
 * @brief Retrieve the firing threshold
 * 
 * @return MembranePotential Firing threshold in mV
 */
MembranePotential Neuron::getThreshold() const {
    if (!pImpl) return -55.0f;
    return pImpl->state.threshold;
}

/**
 * @brief Check if neuron is currently firing
 * 
 * @return bool true if neuron is in Active or Refractory state
 */
bool Neuron::isFiring() const {
    if (!pImpl) return false;
    return pImpl->state.firingState == FiringState::Active ||
           pImpl->state.firingState == FiringState::Refractory;
}

/**
 * @brief Check if neuron is in refractory period
 * 
 * @return bool true if neuron cannot fire due to refractory period
 */
bool Neuron::isRefractory() const {
    if (!pImpl) return false;
    return pImpl->state.refractoryRemaining > 0;
}

/**
 * @brief Set the firing state
 * 
 * @param state New firing state
 */
void Neuron::setFiringState(FiringState state) {
    if (!pImpl) return;
    pImpl->state.firingState = state;
}

/**
 * @brief Set the refractory period duration
 * 
 * @param steps Number of simulation steps the neuron should be refractory
 */
void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!pImpl) return;
    pImpl->state.refractoryPeriod = steps;
}

/**
 * @brief Decrement refractory period counter
 * 
 * Called each simulation step to track remaining refractory time.
 * Automatically transitions neuron back to resting state when refractory period ends.
 */
void Neuron::decrementRefractory() {
    if (!pImpl) return;
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

/**
 * @brief Retrieve the neuron's firing rate
 * 
 * @return FiringRate Current firing rate in Hz
 */
FiringRate Neuron::getFiringRate() const {
    if (!pImpl) return 0.0f;
    return pImpl->state.firingRate;
}

/**
 * @brief Set the neuron's firing rate
 * 
 * @param rate New firing rate in Hz
 */
void Neuron::setFiringRate(FiringRate rate) {
    if (!pImpl) return;
    pImpl->state.firingRate = rate;
}

/**
 * @brief Set the leak conductance
 * 
 * @param conductance Leak conductance in nS
 */
void Neuron::setLeakConductance(MembranePotential conductance) {
    if (!pImpl) return;
    pImpl->state.leakConductance = conductance;
}

/**
 * @brief Retrieve the leak conductance
 * 
 * @return MembranePotential Leak conductance in nS
 */
MembranePotential Neuron::getLeakConductance() const {
    if (!pImpl) return 10.0f;
    return pImpl->state.leakConductance;
}

/**
 * @brief Retrieve the refractory period duration
 * 
 * @return uint32_t Refractory period in simulation steps
 */
uint32_t Neuron::getRefractoryPeriod() const {
    if (!pImpl) return 5;
    return pImpl->state.refractoryPeriod;
}

/**
 * @brief Set the resting potential
 * 
 * @param potential New resting potential in mV (typically -70mV)
 */
void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.restingPotential = potential;
}

/**
 * @brief Retrieve the resting potential
 * 
 * @return MembranePotential Resting potential in mV
 */
MembranePotential Neuron::getRestingPotential() const {
    if (!pImpl) return -70.0f;
    return pImpl->state.restingPotential;
}

/**
 * @brief Set the reset potential
 * 
 * @param potential Post-spike reset potential in mV
 */
void Neuron::setResetPotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.resetPotential = potential;
}

/**
 * @brief Retrieve the reset potential
 * 
 * @return MembranePotential Reset potential in mV
 */
MembranePotential Neuron::getResetPotential() const {
    if (!pImpl) return -70.0f;
    return pImpl->state.resetPotential;
}

/**
 * @brief Check if neuron has reached firing threshold
 * 
 * @return bool true if membrane potential >= threshold
 */
bool Neuron::checkThreshold() const {
    if (!pImpl) {
        return false;
    }
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

/**
 * @brief Get time since last spike
 * 
 * @return float Time since last spike in ms (-1 if no spikes yet)
 */
float Neuron::getLastSpikeTime() const {
    if (!pImpl) {
        return -1.0f;
    }
    return pImpl->state.lastSpikeTime;
}

/**
 * @brief Receive excitatory synaptic input
 * 
 * @param amplitude Synaptic conductance contribution (mV)
 * 
 * Excitatory inputs depolarize the neuron, bringing it closer to threshold.
 * This represents the effect of AMPA or NMDA receptor activation.
 */
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

/**
 * @brief Receive inhibitory synaptic input
 * 
 * @param amplitude Synaptic conductance contribution (mV)
 * 
 * Inhibitory inputs hyperpolarize the neuron, pushing it away from threshold.
 * This represents the effect of GABA receptor activation.
 */
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

/**
 * @brief Receive modulatory synaptic input
 * 
 * @param amplitude Modulatory conductance contribution (mV)
 * 
 * Modulatory inputs affect plasticity but not directly integrated into
 * membrane potential. Used for neuromodulation (e.g., dopamine, acetylcholine).
 */
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

/**
 * @brief Inject external current
 * 
 * @param current Current injection in nA
 * 
 * Used for external stimulation or sensory input. Unlike synaptic input,
 * this current is not filtered by synaptic dynamics but directly affects
 * the neuron's membrane potential.
 */
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    if (!pImpl) return;
    pImpl->synapticInput += current;
}

/**
 * @brief Get total injected current
 * 
 * @return MembranePotential Total current in nA
 */
MembranePotential Neuron::getTotalCurrent() const {
    if (!pImpl) return 0.0f;
    return pImpl->totalCurrent;
}

/**
 * @brief Clear synaptic input buffer
 * 
 * Called at the beginning of each simulation step to reset accumulated
 * synaptic input for the new timestep.
 */
void Neuron::clearTotalCurrent() {
    if (!pImpl) return;
    pImpl->synapticInput = 0.0f;
}

/**
 * @brief Record a spike event
 * 
 * @param timestamp Time of spike occurrence
 * 
 * Records spike timing for spike-timing dependent plasticity (STDP) learning
 * and for maintaining spike history for prediction mechanisms.
 */
void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) return;
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

/**
 * @brief Get recent spike history
 * 
 * @return const std::vector<Timestamp>& Vector of spike timestamps
 * 
 * Provides access to recent spike timing for learning and prediction tasks.
 * Only the most recent spikes are retained due to memory constraints.
 */
const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    if (!pImpl) {
        static const std::vector<Timestamp> empty;
        return empty;
    }
    return pImpl->spikeHistory;
}

/**
 * @brief Clear spike history
 * 
 * Removes all recorded spike timing information. Used when neuron is reset
 * or when spike history is no longer needed.
 */
void Neuron::clearSpikeHistory() {
    if (!pImpl) return;
    pImpl->spikeHistory.clear();
}

/**
 * @brief Add incoming synapse connection
 * 
 * @param handle Handle to the incoming synapse
 * 
 * Records a synapse that provides input to this neuron. This is used
 * for synaptic management and potential STDP learning.
 */
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) return;
    pImpl->incomingSynapses.push_back(handle);
}

/**
 * @brief Add outgoing synapse connection
 * 
 * @param handle Handle to the outgoing synapse
 * 
 * Records a synapse that this neuron influences. This is used
 * for synaptic management and potential STDP learning.
 */
void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) return;
    pImpl->outgoingSynapses.push_back(handle);
}

/**
 * @brief Get list of incoming synapses
 * 
 * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
 */
const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    if (!pImpl) {
        static const std::vector<SynapseHandle> empty;
        return empty;
    }
    return pImpl->incomingSynapses;
}

/**
 * @brief Get list of outgoing synapses
 * 
 * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
 */
const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    if (!pImpl) {
        static const std::vector<SynapseHandle> empty;
        return empty;
    }
    return pImpl->outgoingSynapses;
}

/**
 * @brief Get plasticity flags
 * 
 * @return const PlasticityFlags& Current plasticity configuration
 * 
 * Provides access to plasticity settings that determine how this neuron
 * responds to learning signals.
 */
const PlasticityFlags& Neuron::getPlasticityFlags() const {
    if (!pImpl) {
        static const PlasticityFlags dummy;
        return dummy;
    }
    return pImpl->plasticityFlags;
}

/**
 * @brief Get mutable plasticity flags
 * 
 * @return PlasticityFlags& Mutable plasticity configuration
 */nPlasticityFlags& Neuron::getPlasticityFlags() {
    if (!pImpl) {
        static PlasticityFlags dummy;
        return dummy;
    }
    return pImpl->plasticityFlags;
}

/**
 * @brief Configure plasticity mechanisms
 * 
 * @param hebbian Enable Hebbian plasticity
 * @param stdp Enable spike-timing dependent plasticity
 * @param rewardModulated Enable reward-modulated plasticity
 * 
 * Sets the plasticity flags that determine how synaptic weights change
 * in response to neural activity and learning signals.
 */
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) return;
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * @brief Set region membership
 * 
 * @param region Region ID this neuron belongs to
 */
void Neuron::setRegionId(RegionId region) {
    if (!pImpl) return;
    pImpl->regionId = region;
}

/**
 * @brief Get region membership
 * 
 * @return RegionId Region ID this neuron belongs to
 */
RegionId Neuron::getRegionId() const {
    if (!pImpl) return INVALID_REGION_ID;
    return pImpl->regionId;
}

/**
 * @brief Set population membership
 * 
 * @param population Population ID this neuron belongs to
 */
void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) return;
    pImpl->populationId = population;
}

/**
 * @brief Get population membership
 * 
 * @return PopulationId Population ID this neuron belongs to
 */
PopulationId Neuron::getPopulationId() const {
    if (!pImpl) return INVALID_POPULATION_ID;
    return pImpl->populationId;
}

/**
 * @brief Perform one LIF integration step
 * 
 * @param currentTime Current simulation time in ms
 * @param dt Timestep duration in ms
 * @return bool true if neuron fired during this step
 * 
 * Implements the core Leaky Integrate-and-Fire dynamics using exponential
 * Euler integration. This is the main computational step that simulates
 * how neurons integrate synaptic inputs and generate spikes.
 * 
 * The LIF equation: dV/dt = (V_rest - V)/τ + I/C
 * where V is membrane potential, V_rest is resting potential,
 * τ is time constant, I is synaptic input, and C is capacitance.
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
    // Biologically plausible bounds: -100mV to +50mV
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
 * @brief Perform simulation step with default timestep
 * 
 * @param currentTime Current simulation time in ms
 * 
 * Convenience method for the common case of 1ms timesteps.
 * Calls stepLIF with dt = 0.001 (1ms).
 */
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

/**
 * @brief Reset neuron to initial state
 * 
 * Clears all internal state variables and prepares neuron for new simulation
 * or after learning procedures.
 */
void Neuron::reset() {
    if (!pImpl) return;
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

/**
 * @brief Initialize neuron with random biological parameters
 * 
 * @param rng Random number generator for stochastic initialization
 * 
 * Sets biologically plausible initial values for neuron parameters.
 * This ensures neurons start with realistic characteristics that support
 * stable neural dynamics and effective learning.
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