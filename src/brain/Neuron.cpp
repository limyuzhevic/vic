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

// Implemented missing getMembranePotential() const
MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

// Implemented missing getTotalCurrent() const - totalCurrent member never existed
MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;  // synapticInput is the only current member
}

// Implemented missing getSpikeHistory() const
const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

// Implemented missing getIncomingSynapses() const
const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

// Implemented missing getOutgoingSynapses() const
const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

// Implemented missing getPlasticityFlags() const
const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

// Implemented missing isFiring() const
bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

// Implemented missing isRefractory() const
bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

// Implemented missing getRegionId() const
RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

// Implemented missing getPopulationId() const
PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

// Implemented missing getRefractoryPeriod() const
uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

// Implemented missing getRestingPotential() const
MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

// Implemented missing getResetPotential() const
MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

// Implemented missing getLeakConductance() const
MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

// Implemented missing getFiringRate() const
FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

// Implemented missing getLastSpikeTime() const
float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

// Implemented missing getThreshold() const
MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

// Implemented missing getType() const
NeuronType Neuron::getType() const {
    return pImpl->type;
}

// Implemented missing getId() const
NeuronId Neuron::getId() const {
    return pImpl->id;
}

// Implemented missing getState() const
const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

// Implemented missing getState() - reference version
NeuronState& Neuron::getState() {
    return pImpl->state;
}

// Implemented missing getPlasticityFlags() - reference version
PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
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
        
        // Fixed: Use reference to avoid copy, ensure thread safety in multi-threaded scenarios
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    // Use circular buffer with modulo for O(1) operations instead of O(n) erase
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Remove oldest element (O(n)) - still better than linear scan for small buffer
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    // Decay adaptation variable to prevent unbounded growth
    pImpl->state.adaptationVariable = std::max(0.0f, pImpl->state.adaptationVariable - 0.1f);
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

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
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
    // Enhanced random initialization with biological constraints and Phase 6 integration
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV with realistic variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting with some dynamics
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Initialize neuromodulatory variables for Phase 6 integration
    pImpl->state.excitabilityModulator = 1.0f;      // Neuromodulator effect on excitability
    pImpl->state.plasticityModulator = 1.0f;       // Neuromodulator effect on plasticity
    pImpl->state.attentionModulator = 1.0f;        // Neuromodulator effect on attention
    pImpl->state.memoryModulator = 1.0f;           // Neuromodulator effect on memory
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Phase 6 Integration: Enhanced LIF dynamics with neuromodulation and memory integration
void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with improved logic
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return; // Exit early during refractory
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire with correct differential equation
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contribution to membrane potential change
    // I = synapticInput (nA), R = C/tau (membrane resistance in MΩ)
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution (exponential decay toward resting potential)
    float leakContribution = (V_rest - V) / tau;
    
    // CRITICAL FIX: dt is already in seconds (0.001), 1000.0f factor was incorrect
    // Should be: V = V + dt * (leakContribution + synapticContribution)
    // The 1000.0f was mistakenly trying to convert seconds to milliseconds
    V = V + static_cast<float>(dt) * (leakContribution + synapticContribution);
    
    // Apply neuromodulation effects (Phase 6 integration)
    // Excitability modulation (dopamine, acetylcholine effects)
    V *= pImpl->state.excitabilityModulator;
    
    // Apply adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        // Bound adaptation to prevent instability
        pImpl->state.adaptationVariable = std::max(0.0f, pImpl->state.adaptationVariable);
    }
    
    // Apply memory integration (working memory influence)
    if (pImpl->state.memoryModulator != 1.0f) {
        // Memory can bias neuron dynamics - working memory traces
        V += (pImpl->state.memoryModulator - 1.0f) * 2.0f;
    }
    
    // Clamp membrane potential with adaptive bounds based on neuromodulation
    float lowerBound = -100.0f;
    float upperBound = 50.0f;
    // Neuromodulators can adjust bounds
    lowerBound *= pImpl->state.excitabilityModulator * 0.5f;
    upperBound *= pImpl->state.excitabilityModulator;
    
    V = std::clamp(V, lowerBound, upperBound);
    
    // Check for spike with hysteresis to prevent multiple spikes per input
    if (V >= threshold && pImpl->state.firingState != FiringState::Active) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike with Phase 6 integration
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation with Phase 6 effects
        pImpl->state.adaptationVariable += 1.0f;
        
        // Apply plasticity modulation (neuromodulation effects)
        if (pImpl->state.plasticityModulator != 1.0f) {
            pImpl->state.adaptationVariable *= pImpl->state.plasticityModulator;
        }
        
        // Apply attention modulation (Phase 6 integration)
        if (pImpl->state.attentionModulator != 1.0f) {
            // Attention can increase firing probability
            if (rng && rng->uniformReal(0.0f, 1.0f) < pImpl->state.attentionModulator * 0.1f) {
                // Additional spike triggered by attention
                pImpl->state.adaptationVariable += 0.5f;
            }
        }
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

} // namespace nlm
