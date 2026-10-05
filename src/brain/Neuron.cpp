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
    // Create neuron with unique identifier
    // id: unique neuron ID for tracking and debugging
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    // Move constructor for efficient neuron transfer
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    // Move assignment operator
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    // Set neuron type (sensory, motor, excitatory, inhibitory, etc.)
    // type: neuron type from NeuronType enum
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Set absolute membrane potential (in mV)
    // potential: membrane potential value in millivolts
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Add delta to membrane potential (in mV)
    // delta: change in membrane potential to apply
    pImpl->state.membranePotential += delta;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
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

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
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

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
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

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
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

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;
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
    // Optimized spike recording using vector with capacity reservation
    // O(1) amortized push_back, only bounds check needed
    pImpl->spikeHistory.push_back(timestamp);
    
    // Use swap+clear instead of erase for better performance when size > limit
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Reserve additional space and swap to avoid expensive erase
        std::vector<Timestamp> temp;
        temp.reserve(Impl::MAX_SPIKE_HISTORY);
        temp.insert(temp.end(), pImpl->spikeHistory.begin() + 1, pImpl->spikeHistory.end());
        pImpl->spikeHistory.swap(temp);
    }
}

void Neuron::clearSpikeHistory() {
    // Use clear with reserve for better performance on repeated clears
    pImpl->spikeHistory.clear();
    pImpl->spikeHistory.shrink_to_fit();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Pre-allocate synapse vectors to avoid reallocations
    if (pImpl->incomingSynapses.size() == pImpl->incomingSynapses.capacity()) {
        pImpl->incomingSynapses.reserve(pImpl->incomingSynapses.size() * 1.5);
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Pre-allocate synapse vectors to avoid reallocations
    if (pImpl->outgoingSynapses.size() == pImpl->outgoingSynapses.capacity()) {
        pImpl->outgoingSynapses.reserve(pImpl->outgoingSynapses.size() * 1.5);
    }
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
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

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period - check once, exit early if in refractory
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
    // Optimized for performance with pre-calculated constants and local variables
    
    MembranePotential& V = pImpl->state.membranePotential;
    const MembranePotential& V_rest = pImpl->state.restingPotential;
    const MembranePotential& V_reset = pImpl->state.resetPotential;
    const MembranePotential& threshold = pImpl->state.threshold;
    const float& adaptationVariable = pImpl->state.adaptationVariable;
    
    // Pre-calculated constants for better performance
    constexpr float TIME_CONSTANT_MS = 20.0f;  // ms
    constexpr float MEMBRANE_CAPACITANCE_NF = 1.0f;  // nF
    constexpr float DT_SCALE_FACTOR = 1000.0f;  // Convert ms to s
    constexpr float ADAPTATION_COEFF = 0.01f;
    constexpr float ADAPTATION_DECAY = 0.95f;
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / MEMBRANE_CAPACITANCE_NF;
    
    // Leak contribution - vectorized and pre-calculated
    float leakContribution = (V_rest - V) / TIME_CONSTANT_MS;
    
    // Update membrane potential using exponential Euler integration
    // Fused multiplication and addition for better pipelining
    V += static_cast<float>(dt) * DT_SCALE_FACTOR * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // Only process if adaptation is active
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * ADAPTATION_COEFF;
        pImpl->state.adaptationVariable = adaptationVariable * ADAPTATION_DECAY;
    }
    
    // Clamp membrane potential to prevent instability (using local variables)
    if (V < -100.0f) V = -100.0f;
    if (V > 50.0f) V = 50.0f;
    
    // Check for spike - early exit optimization
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
        // Simplified firing state assignment
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Optimized reset with direct state initialization
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    clearSpikeHistory();  // Use optimized version
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Optimized random initialization with pre-calculated bounds
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
    clearSpikeHistory();  // Use optimized version
}

} // namespace nlm
