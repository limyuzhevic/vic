#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

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
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
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

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

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

// Update neuron for one simulation step
// Implements the main integration step using LIF dynamics
// Handles refractory period, membrane potential integration, and spike detection
// with improved error checking and robustness
void Neuron::step(Timestamp currentTime) {
    // Validate input parameters
    if (currentTime < 0) {
        return; // Invalid time, skip processing
    }
    
    // Use the pre-computed LIF step with default timestep
    // This maintains backward compatibility while providing better error handling
    TimestepDuration dt = 0.001; // 1ms standard timestep
    bool fired = stepLIF(currentTime, dt);
    
    // Update spike count if neuron fired
    if (fired) {
        pImpl->state.spikeCount++;
    }
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Validate input RNG to prevent undefined behavior
    if (!rng.isValid()) {
        // Fallback to deterministic initialization
        pImpl->state.membranePotential = pImpl->state.restingPotential;
        pImpl->state.threshold = -55.0f;
        pImpl->state.restingPotential = -70.0f;
        pImpl->state.resetPotential = pImpl->state.restingPotential;
        pImpl->state.refractoryPeriod = 5;
        return;
    }
    
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential (biological range: -73mV to -67mV)
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation (biological range: -57mV to -53mV)
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV (biological range: -72mV to -68mV)
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting (0-5mV above resting)
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical (biological range: 1-20ms)
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Ensure reasonable refractory period
    pImpl->state.refractoryPeriod = std::max(static_cast<uint32_t>(1), 
                                          std::min(pImpl->state.refractoryPeriod, static_cast<uint32_t>(20)));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.spikeCount = 0;  // Reset spike counter
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Update resting potential to reflect the new threshold relationship
    // Ensure threshold is reachable from resting potential
    if (pImpl->state.threshold <= pImpl->state.restingPotential) {
        pImpl->state.threshold = pImpl->state.restingPotential + 15.0f;
    }
}

/**
 * Calculate approximate energy consumption for this step
 * 
 * Uses a simplified biophysical model to estimate energy used
 * based on membrane potential changes and spike activity.
 * 
 * @return Energy consumption in arbitrary units
 */
float Neuron::calculateEnergyConsumption() const {
    float energy = 0.0f;
    
    // Energy from membrane potential changes (proportional to squared voltage)
    float potentialChange = std::abs(pImpl->state.membranePotential - pImpl->state.restingPotential);
    energy += potentialChange * potentialChange * 0.01f;
    
    // Energy from synaptic activity
    energy += std::abs(pImpl->synapticInput) * 0.1f;
    
    // Significant energy cost for spikes
    if (pImpl->state.firingState == FiringState::Active) {
        energy += 1.0f;  // Base spike cost
        energy += pImpl->state.adaptationVariable * 0.5f;  // Adaptation cost
    }
    
    // Refractory period maintenance cost
    if (pImpl->state.refractoryRemaining > 0) {
        energy += 0.1f * static_cast<float>(pImpl->state.refractoryRemaining);
    }
    
    return std::clamp(energy, 0.0f, 100.0f);  // Clamp to reasonable range
}

namespace nlm {

/**
 * Compute comprehensive neuron statistics for monitoring and analysis
 * 
 * Calculates firing statistics, adaptation level, energy consumption,
 * and activity status based on current neuron state.
 * 
 * @return NeuronStats containing detailed neuron metrics
 */
NeuronStats Neuron::getStats() const {
    NeuronStats stats;
    
    // Calculate firing rate from current state
    stats.firingRate = pImpl->state.firingRate;
    
    // Adaptation level from adaptation variable
    stats.adaptationLevel = std::clamp(pImpl->state.adaptationVariable, 0.0f, 1.0f);
    
    // Current membrane potential
    stats.membranePotential = pImpl->state.membranePotential;
    
    // Distance to threshold (positive = below threshold)
    stats.thresholdDistance = pImpl->state.threshold - pImpl->state.membranePotential;
    
    // Get recent spike count from spike history
    stats.recentSpikes = static_cast<uint32_t>(std::min(pImpl->spikeHistory.size(), 
                                                          static_cast<size_t>(Impl::MAX_SPIKE_HISTORY)));
    
    // Total spikes from counter
    stats.spikeCount = pImpl->state.spikeCount;
    
    // Energy consumption calculation (simplified model)
    stats.energyConsumption = calculateEnergyConsumption();
    
    // Determine if neuron is active
    stats.isActive = (pImpl->state.firingState == FiringState::Active) && 
                     (pImpl->state.refractoryRemaining == 0);
    
    return stats;
}

/**
 * Check if neuron can accept new synaptic input
 * 
 * Neurons in refractory period cannot receive input to prevent
 * interference during recovery from a spike.
 * 
 * @return true if neuron can receive input, false otherwise
 */
bool Neuron::canReceiveInput() const {
    return pImpl->state.refractoryRemaining == 0 && 
           pImpl->state.firingState != FiringState::Refractory;
}
