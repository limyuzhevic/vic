#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    
    // LIF parameters - named constants instead of magic numbers
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr float MAX_POTENTIAL_CLAMP = 50.0f;  // mV - positive limit
    static constexpr float MIN_POTENTIAL_CLAMP = -100.0f;  // mV - negative limit
    static constexpr size_t SPIKE_HISTORY_SIZE = 100;  // Fixed-size circular buffer capacity
    
    // Circular buffer for efficient spike history management
    std::array<Timestamp, SPIKE_HISTORY_SIZE> spikeHistoryArray;
    size_t writeIndex = 0;
    size_t spikeCount = 0;
    
    // Synapse management with bounds checking
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    
    PlasticityFlags plasticityFlags;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f), 
             spikeCount(0), writeIndex(0)
    {
        // Initialize spike history array with sentinel values
        std::fill(spikeHistoryArray.begin(), spikeHistoryArray.end(), -1.0f);
    }
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

// Property accessors
NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Apply clamp to prevent numerical instability
    pImpl->state.membranePotential = std::clamp(potential, MIN_POTENTIAL_CLAMP, MAX_POTENTIAL_CLAMP);
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Apply clamp to prevent numerical instability
    pImpl->state.membranePotential = std::clamp(
        pImpl->state.membranePotential + delta, MIN_POTENTIAL_CLAMP, MAX_POTENTIAL_CLAMP);
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold is within reasonable bounds
    if (threshold < MIN_POTENTIAL_CLAMP) threshold = MIN_POTENTIAL_CLAMP + 1.0f;
    if (threshold > MAX_POTENTIAL_CLAMP) threshold = MAX_POTENTIAL_CLAMP - 1.0f;
    pImpl->state.threshold = threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Validate refractory period (reasonable biological limits)
    pImpl->state.refractoryPeriod = std::clamp(steps, uint32_t(1), uint32_t(1000));
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    // Clamp firing rate to biologically plausible range
    pImpl->state.firingRate = std::clamp(rate, 0.0f, 1000.0f);  // Hz limit
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Validate conductance is positive
    pImpl->state.leakConductance = std::max(conductance, 0.0f);
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = std::clamp(potential, MIN_POTENTIAL_CLAMP, MAX_POTENTIAL_CLAMP);
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = std::clamp(potential, MIN_POTENTIAL_CLAMP, MAX_POTENTIAL_CLAMP);
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Validate input amplitude
    if (amplitude < 0.0f) amplitude = 0.0f;
    pImpl->synapticInput += amplitude;
    // Clamp to prevent overflow
    pImpl->synapticInput = std::min(pImpl->synapticInput, 100.0f);
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Validate input amplitude
    if (amplitude < 0.0f) amplitude = 0.0f;
    pImpl->synapticInput -= amplitude;
    // Clamp to prevent underflow
    pImpl->synapticInput = std::max(pImpl->synapticInput, -100.0f);
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    // Decay adaptation variable gradually
    pImpl->state.adaptationVariable *= 0.99f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    if (current < 0.0f) current = 0.0f;
    pImpl->synapticInput += current;
    // Clamp to prevent numerical instability
    pImpl->synapticInput = std::min(pImpl->synapticInput, 1000.0f);
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

// Spike history management with circular buffer
void Neuron::recordSpike(Timestamp timestamp) {
    // Implement circular buffer for efficient spike history management
    pImpl->spikeHistoryArray[pImpl->writeIndex] = timestamp;
    pImpl->writeIndex = (pImpl->writeIndex + 1) % SPIKE_HISTORY_SIZE;
    // If buffer was full before this write, oldest entry is overwritten
    if (pImpl->spikeCount < SPIKE_HISTORY_SIZE) {
        pImpl->spikeCount++;
    }
}

Timestamp Neuron::getSpikeAt(size_t index) const {
    // Access spike at given index, handling circular buffer
    if (index >= pImpl->spikeCount) return -1.0f;
    size_t actualIndex = (pImpl->writeIndex + SPIKE_HISTORY_SIZE - 1 - index) % SPIKE_HISTORY_SIZE;
    return pImpl->spikeHistoryArray[actualIndex];
}

size_t Neuron::getSpikeHistorySize() const {
    // Return number of spikes currently stored (up to SPIKE_HISTORY_SIZE)
    return pImpl->spikeCount;
}

const std::array<Timestamp, SPIKE_HISTORY_SIZE>& Neuron::getAllSpikeHistory() const {
    // Return all stored spikes in chronological order (oldest to newest)
    return pImpl->spikeHistoryArray;
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    // Return spike history as vector for backward compatibility
    // Convert circular buffer to vector (costly but preserves API compatibility)
    static std::vector<Timestamp> history;
    history.clear();
    history.reserve(pImpl->spikeCount);
    for (size_t i = 0; i < pImpl->spikeCount; ++i) {
        history.push_back(getSpikeAt(i));
    }
    return history;
}

void Neuron::clearSpikeHistory() {
    // Clear spike history and reset counters
    std::fill(std::begin(pImpl->spikeHistoryArray), std::end(pImpl->spikeHistoryArray), -1.0f);
    pImpl->writeIndex = 0;
    pImpl->spikeCount = 0;
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Add incoming synapse with bounds checking
    pImpl->incomingSynapses.push_back(handle);
    if (pImpl->incomingSynapses.size() > 1000) {  // Safety limit
        pImpl->incomingSynapses.erase(pImpl->incomingSynapses.begin());
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Add outgoing synapse with bounds checking
    pImpl->outgoingSynapses.push_back(handle);
    if (pImpl->outgoingSynapses.size() > 1000) {  // Safety limit
        pImpl->outgoingSynapses.erase(pImpl->outgoingSynapses.begin());
    }
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
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

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

// LIF step function - returns true if neuron fired
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
    
    // Clamp membrane potential to prevent instability (using named constants)
    V = std::clamp(V, MIN_POTENTIAL_CLAMP, MAX_POTENTIAL_CLAMP);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike in circular buffer
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
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    clearSpikeHistory();
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
    clearSpikeHistory();
}

} // namespace nlm
