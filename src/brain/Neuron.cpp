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
    
    // LIF parameters - now using constants from nlm::constants
    
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

// Validation helper functions
namespace {
    bool validateMembranePotential(MembranePotential potential) {
        // Validate membrane potential is within reasonable biological bounds
        // Typical range: -100mV to +50mV for most neurons
        const float MIN_POTENTIAL = -100.0f;
        const float MAX_POTENTIAL = 50.0f;
        return potential >= MIN_POTENTIAL && potential <= MAX_POTENTIAL;
    }
    
    bool validateThreshold(MembranePotential threshold) {
        // Threshold should be within membrane potential range
        return validateMembranePotential(threshold);
    }
    
    bool validateSpikeTimestamp(Timestamp timestamp) {
        // Spike timestamps should be non-negative
        return timestamp >= 0.0;
    }
    
    bool validateRefractoryPeriod(uint32_t steps) {
        // Refractory period should be reasonable (1-1000 steps)
        const uint32_t MIN_STEPS = 1;
        const uint32_t MAX_STEPS = 1000;
        return steps >= MIN_STEPS && steps <= MAX_STEPS;
    }
    
    bool validateSynapticInput(MembranePotential input) {
        // Synaptic input should be within reasonable bounds
        const float MIN_INPUT = -100.0f;
        const float MAX_INPUT = 100.0f;
        return input >= MIN_INPUT && input <= MAX_INPUT;
    }
    
    bool validateRegionId(RegionId id) {
        // Region IDs should not be invalid
        return id != INVALID_REGION_ID;
    }
    
    bool validatePopulationId(PopulationId id) {
        // Population IDs should not be invalid
        return id != INVALID_POPULATION_ID;
    }
} // namespace anonymous

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate input before setting
    if (validateMembranePotential(potential)) {
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate delta before modifying
    if (validateSynapticInput(delta)) {
        pImpl->state.membranePotential += delta;
        
        // Clamp to biological limits after modification
        pImpl->state.membranePotential = std::clamp(
            pImpl->state.membranePotential, 
            -100.0f, 
            50.0f
        );
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold before setting
    if (validateThreshold(threshold)) {
        pImpl->state.threshold = threshold;
    }
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
    // Firing rate should be non-negative
    pImpl->state.firingRate = std::max(0.0f, rate);
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Leak conductance should be positive
    pImpl->state.leakConductance = std::max(0.0f, conductance);
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Validate refractory period before setting
    if (validateRefractoryPeriod(steps)) {
        pImpl->state.refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Validate resting potential before setting
    if (validateMembranePotential(potential)) {
        pImpl->state.restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Validate reset potential before setting
    if (validateMembranePotential(potential)) {
        pImpl->state.resetPotential = potential;
    }
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Validate synaptic input before applying
    if (validateSynapticInput(amplitude)) {
        // Real synaptic input: excitatory currents add to total current
        // amplitude represents synaptic conductance * reversal potential contribution
        pImpl->synapticInput += amplitude;
        
        // Clamp to reasonable bounds
        pImpl->synapticInput = std::clamp(pImpl->synapticInput, -100.0f, 100.0f);
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Validate synaptic input before applying
    if (validateSynapticInput(amplitude)) {
        // Real inhibitory input: subtract from total current
        // Inhibitory synaptic currents hyperpolarize the neuron
        pImpl->synapticInput -= amplitude;
        
        // Clamp to reasonable bounds
        pImpl->synapticInput = std::clamp(pImpl->synapticInput, -100.0f, 100.0f);
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Validate synaptic input before applying
    if (validateSynapticInput(amplitude)) {
        // Modulatory input affects plasticity but not directly integrated
        // Used for neuromodulation (e.g., dopamine, acetylcholine)
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Validate current injection before applying
    if (validateSynapticInput(current)) {
        // Direct current injection (e.g., from sensory input or external source)
        // Add to synaptic input for LIF integration
        pImpl->synapticInput += current;
        
        // Clamp to reasonable bounds
        pImpl->synapticInput = std::clamp(pImpl->synapticInput, -100.0f, 100.0f);
    }
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Validate timestamp before recording
    if (validateSpikeTimestamp(timestamp)) {
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > nlm::constants::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Update last spike time
        pImpl->state.lastSpikeTime = static_cast<float>(timestamp);
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
    pImpl->state.lastSpikeTime = -1.0f;
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
    // Validate region ID before setting
    if (validateRegionId(region)) {
        pImpl->regionId = region;
    }
}

void Neuron::setPopulationId(PopulationId population) {
    // Validate population ID before setting
    if (validatePopulationId(population)) {
        pImpl->populationId = population;
    }
}

// Add validation to stepLIF function
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Validate inputs
    if (!validateSpikeTimestamp(currentTime) || dt < 0.0) {
        return false;
    }
    
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
    float tau = nlm::constants::TIME_CONSTANT;  // ms
    float C = nlm::constants::MEMBRANE_CAPACITANCE;  // nF
    
    // Validate state variables
    if (!validateMembranePotential(V) || !validateMembranePotential(V_rest) || 
        !validateMembranePotential(V_reset) || !validateMembranePotential(threshold)) {
        return false;
    }
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * nlm::constants::MS_TO_S_FACTOR * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability (using validateMembranePotential)
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
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = nlm::constants::DEFAULT_TIMESTEP;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->state.lastSpikeTime = -1.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Validate membrane potential
    if (!validateMembranePotential(pImpl->state.membranePotential)) {
        pImpl->state.membranePotential = pImpl->state.restingPotential;
    }
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = nlm::constants::DEFAULT_THRESHOLD + rng.uniformReal(-2.0f, 2.0f);
    
    // Validate threshold
    if (!validateThreshold(pImpl->state.threshold)) {
        pImpl->state.threshold = nlm::constants::DEFAULT_THRESHOLD;
    }
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = nlm::constants::DEFAULT_RESTING_POTENTIAL + rng.uniformReal(-2.0f, 2.0f);
    
    // Validate resting potential
    if (!validateMembranePotential(pImpl->state.restingPotential)) {
        pImpl->state.restingPotential = nlm::constants::DEFAULT_RESTING_POTENTIAL;
    }
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Validate reset potential
    if (!validateMembranePotential(pImpl->state.resetPotential)) {
        pImpl->state.resetPotential = pImpl->state.restingPotential;
    }
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Validate refractory period
    if (!validateRefractoryPeriod(pImpl->state.refractoryPeriod)) {
        pImpl->state.refractoryPeriod = nlm::constants::DEFAULT_REFACTORY_PERIOD;
    }
    
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
