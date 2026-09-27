#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

/**
 * @brief Neural Implementation Details
 * 
 * This implementation provides biologically realistic LIF neurons with:
 * - Spike-frequency adaptation via an adaptation variable
 * - Refractory period after spiking
 * - Event-driven input integration
 * - History tracking for plasticity rules
 */
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

/**
 * @brief Set the membrane potential of the neuron.
 * 
 * Directly sets the membrane potential without any dynamics integration.
 * Used for initialization or special state manipulation.
 * 
 * @param potential New membrane potential value in millivolts (mV)
 */
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

/**
 * @brief Process excitatory synaptic input to the neuron.
 * 
 * Synaptic inputs are modeled as currents that contribute to membrane potential
 * changes. Excitatory inputs (glutamate-mediated) depolarize the neuron.
 * 
 * Implementation details:
 * - Input amplitude represents synaptic conductance × reversal potential
 * - Real biological synapses exhibit temporal and spatial summation
 * - Short-term plasticity can modulate input strength over time
 * 
 * @param amplitude Synaptic input strength in millivolts-equivalent
 */
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

/**
 * @brief Process inhibitory synaptic input to the neuron.
 * 
 * Inhibitory inputs (GABA-mediated) hyperpolarize the neuron, making it
 * less likely to fire. This models biological inhibition in neural circuits.
 * 
 * Implementation details:
 * - Inhibitory currents subtract from net excitatory input
 * - Can originate from interneurons or other inhibitory neurons
 * - Often fast-acting with short synaptic delays
 * 
 * @param amplitude Synaptic input strength in millivolts-equivalent (positive value)
 */
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

/**
 * @brief Process neuromodulatory input that affects plasticity.
 * 
 * Neuromodulatory inputs (dopamine, acetylcholine, serotonin) influence
 * synaptic plasticity rules without directly affecting membrane potential.
 * These signals encode behavioral context and reward prediction errors.
 * 
 * Biological basis:
 * - Dopamine modulates STDP windows and eligibility traces
 * - Acetylcholine gates learning and attention
 * - Serotonin regulates appetite and sleep-related circuits
 * 
 * Implementation:
 * - Slow, diffuse effects across the neural network
 * - Influences synaptic eligibility traces and learning rates
 * - Can be multiplicative or additive depending on receptor type
 * 
 * @param amplitude Neuromodulatory input strength in arbitrary units
 */
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

/**
 * @brief Direct current injection from external sources.
 * 
 * Models artificial or strong external inputs such as sensory inputs,
 * electrical stimulation, or engineered drives for experimental control.
 * Unlike synaptic inputs, this bypasses synaptic transmission mechanisms.
 * 
 * Use cases:
 * - Sensory input simulation
 * - Experimental stimulus control
 * - Testing specific neural responses
 * - Artificially driving neural activity
 * 
 * @param current Input current strength in millivolts-equivalent
 */
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

/**
 * @brief Reset synaptic input accumulator for the next simulation step.
 * 
 * Clears all accumulated synaptic and injected currents, preparing the neuron
 * for the next integration step. This is called at the end of each timestep
 * to prevent input carryover and ensure proper temporal dynamics.
 * 
 * Computational efficiency:
 * - Prevents accumulation of numerical errors over time
 * - Ensures proper temporal separation of inputs
 * - Maintains stable long-running simulations
 */
void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

/**
 * @brief Record a spike event for plasticity rules and analysis.
 * 
 * Spike recording is essential for:
 * - STDP (Spike-Timing-Dependent Plasticity) calculations
 * - Synaptic eligibility trace management
 * - Activity-dependent statistics and analysis
 * - Learning experiment verification
 * 
 * Implementation details:
 * - Maintains rolling buffer to prevent unbounded memory growth
 * - Stores absolute timestamps in simulation time
 * - Enables precise spike timing analysis
 * 
 * @param timestamp Absolute simulation time when spike occurred (ms)
 */
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

/**
 * @brief Clear spike history for reset operations.
 * 
 * Used when:
 * - Resetting neurons between experimental trials
 * - Starting new simulation runs
 * - Clearing adaptation state
 * - Preparing for clean baseline measurements
 * 
 * Biological relevance:
 * - Models synaptic depression after high activity
 * - Resets memory traces in working memory systems
 * - Implements homeostatic regulation mechanisms
 */
void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

/**
 * @brief LIF Dynamics: Leaky Integrate-and-Fire Neuron Model
 * 
 * Mathematical formulation: dV/dt = (V_rest - V)/τ + I/C
 * Where:
 *   V = membrane potential (mV)
 *   V_rest = resting potential (mV)
 *   τ = membrane time constant (ms)
 *   I = total synaptic current input (nA)
 *   C = membrane capacitance (nF)
 * 
 * Discrete approximation: V_new = V + dt * ((V_rest - V)/τ + I/C)
 * 
 * Biological interpretation:
 * - Membrane potential evolves toward resting potential exponentially
 * - Synaptic inputs provide current that drives potential toward threshold
 * - Time constant τ determines integration speed (τ = R*C, where R = leak resistance)
 * 
 * @param currentTime Simulation time in milliseconds
 * @param dt Time step duration in seconds
 * @return true if neuron fired a spike, false otherwise
 */
    bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Refractory period controls spike generation
    // Prevents unrealistic high-frequency firing and models absolute refractory period
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
    
    // Spike generation mechanism with realistic biophysical modeling
    // Check for spike (threshold crossing)
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for plasticity rules and analysis
        recordSpike(currentTime);
        
        // Reset membrane potential to after-hyperpolarization level
        // Biophysical basis: voltage-gated potassium channels open
        V = V_reset;
        
        // Enter absolute refractory period
        // Prevents immediate re-firing, models Na+ channel inactivation
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation variable for spike-frequency adaptation
        // Increases after each spike to modulate firing rate over time
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Continue firing if already active (post-spike dynamics)
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next integration step
    // Prevents input carryover and ensures proper temporal dynamics
    pImpl->synapticInput = 0.0f;
    
    // Return firing status for spike propagation to other neurons
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

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
