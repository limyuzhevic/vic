#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

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
    validateNeuronId(id);
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

void Neuron::validateNeuronId(NeuronId id) {
    if (id == INVALID_NEURON_ID) {
        throw std::invalid_argument("Neuron ID cannot be INVALID_NEURON_ID");
    }
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
    validateSynapticInput(amplitude);
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    validateSynapticInput(amplitude);
    pImpl->synapticInput -= amplitude;
}

void Neuron::validateSynapticInput(MembranePotential amplitude) {
    // Validate synaptic input ranges for biological plausibility
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        throw std::invalid_argument("Invalid synaptic input: NaN or infinite value");
    }
    
    // Biological constraints for synaptic inputs
    const float maxInput = 100.0f;  // Maximum physiologically plausible input
    if (amplitude < -maxInput || amplitude > maxInput) {
        throw std::invalid_argument("Synaptic input out of biological range: " + std::to_string(amplitude));
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    validateModulatoryInput(amplitude);
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::validateModulatoryInput(MembranePotential amplitude) {
    // Validate modulatory input ranges
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        throw std::invalid_argument("Invalid modulatory input: NaN or infinite value");
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    validateDirectCurrent(current);
    pImpl->synapticInput += current;
}

void Neuron::validateDirectCurrent(MembranePotential current) {
    // Validate direct current injection
    if (std::isnan(current) || std::isinf(current)) {
        throw std::invalid_argument("Invalid direct current: NaN or infinite value");
    }
    
    // Biologically plausible current ranges
    const float maxCurrent = 50.0f;
    if (current < -maxCurrent || current > maxCurrent) {
        throw std::invalid_argument("Direct current out of biological range: " + std::to_string(current));
    }
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    validateTimestamp(timestamp);
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::validateTimestamp(Timestamp timestamp) {
    if (std::isnan(timestamp) || std::isinf(timestamp) || timestamp < 0.0f) {
        throw std::invalid_argument("Invalid timestamp: " + std::to_string(timestamp));
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!handle) {
        NLM_LOG_WARNING("Neuron::addIncomingSynapse: Null synapse handle");
        return;
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!handle) {
        NLM_LOG_WARNING("Neuron::addOutgoingSynapse: Null synapse handle");
        return;
    }
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
    validateLIFParameters(currentTime, dt);
    
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
    
    // Validate neuron state before computation
    if (!validateNeuronState(V, V_rest, V_reset, threshold)) {
        NLM_LOG_ERROR("Neuron::stepLIF: Invalid neuron state");
        return false;
    }
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = validateAndClampSynapticInput(pImpl->synapticInput, C);
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Validate contributions
    if (!validateContributions(synapticContribution, leakContribution)) {
        NLM_LOG_ERROR("Neuron::stepLIF: Invalid contributions");
        return false;
    }
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with validation
    if (checkForSpike(V, threshold, currentTime, fired, V_reset)) {
        // Handle spike event
        handleSpikeEvent(fired, V, currentTime);
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::validateLIFParameters(Timestamp currentTime, TimestepDuration dt) {
    if (std::isnan(currentTime) || std::isinf(currentTime) || currentTime < 0.0f) {
        throw std::invalid_argument("Invalid current time in LIF step");
    }
    
    if (dt <= 0.0 || dt > 1.0) {  // Reasonable timestep range
        throw std::invalid_argument("Invalid timestep in LIF step: " + std::to_string(dt));
    }
}

bool Neuron::validateNeuronState(MembranePotential V, MembranePotential V_rest, 
                                 MembranePotential V_reset, MembranePotential threshold) {
    if (std::isnan(V) || std::isinf(V) || 
        std::isnan(V_rest) || std::isinf(V_rest) ||
        std::isnan(V_reset) || std::isinf(V_reset) ||
        std::isnan(threshold) || std::isinf(threshold)) {
        return false;
    }
    
    // Biological plausibility checks
    if (V_reset >= V_rest) {
        NLM_LOG_WARNING("Neuron::validateNeuronState: Reset potential should be lower than resting potential");
    }
    
    return true;
}

float Neuron::validateAndClampSynapticInput(MembranePotential synapticInput, float C) {
    if (std::isnan(synapticInput) || std::isinf(synapticInput)) {
        throw std::invalid_argument("Invalid synaptic input in validation");
    }
    
    // Clamp synaptic input to prevent instability
    const float maxSynapticInput = C * 100.0f;  // Max input based on capacitance
    float clampedInput = std::clamp(synapticInput, -maxSynapticInput, maxSynapticInput);
    
    if (clampedInput != synapticInput) {
        NLM_LOG_WARNING("Neuron::validateAndClampSynapticInput: Synaptic input clamped from " + 
                       std::to_string(synapticInput) + " to " + std::to_string(clampedInput));
    }
    
    return clampedInput / C;
}

bool Neuron::validateContributions(float synapticContribution, float leakContribution) {
    if (std::isnan(synapticContribution) || std::isinf(synapticContribution) ||
        std::isnan(leakContribution) || std::isinf(leakContribution)) {
        return false;
    }
    
    // Check for extreme values that might cause numerical instability
    const float maxContribution = 1000.0f;
    if (std::abs(synapticContribution) > maxContribution || std::abs(leakContribution) > maxContribution) {
        NLM_LOG_WARNING("Neuron::validateContributions: Contribution values may cause instability");
    }
    
    return true;
}

bool Neuron::checkForSpike(MembranePotential V, MembranePotential threshold, 
                          Timestamp currentTime, bool& fired, MembranePotential& V_reset) {
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = currentTime;
        return true;
    }
    return false;
}

void Neuron::handleSpikeEvent(bool& fired, MembranePotential& V, MembranePotential V_reset) {
    // Record spike
    recordSpike(pImpl->state.lastSpikeTime);
    
    // Reset membrane potential
    V = V_reset;
    
    // Enter refractory period
    pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    pImpl->state.firingState = FiringState::Refractory;
    
    // Update adaptation for spike-frequency adaptation
    pImpl->state.adaptationVariable += 1.0f;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    validateBasicStepParameters(currentTime);
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::validateBasicStepParameters(Timestamp currentTime) {
    if (std::isnan(currentTime) || std::isinf(currentTime) || currentTime < 0.0f) {
        throw std::invalid_argument("Invalid current time in basic step");
    }
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
    
    // Validate membrane potential
    validateMembranePotential(pImpl->state.membranePotential);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Validate threshold
    validateThreshold(pImpl->state.threshold);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Validate resting potential
    validateRestingPotential(pImpl->state.restingPotential);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Validate reset potential
    validateResetPotential(pImpl->state.resetPotential);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Validate refractory period
    validateRefractoryPeriod(pImpl->state.refractoryPeriod);
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::validateMembranePotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid membrane potential: NaN or infinite");
    }
    
    // Biologically plausible range
    if (potential < -100.0f || potential > 50.0f) {
        NLM_LOG_WARNING("Neuron::validateMembranePotential: Membrane potential outside typical range: " + 
                       std::to_string(potential));
    }
}

void Neuron::validateThreshold(MembranePotential threshold) {
    if (std::isnan(threshold) || std::isinf(threshold)) {
        throw std::invalid_argument("Invalid threshold: NaN or infinite");
    }
    
    // Threshold should be more depolarized than resting potential typically
    if (threshold <= pImpl->state.restingPotential) {
        NLM_LOG_WARNING("Neuron::validateThreshold: Threshold should be more positive than resting potential");
    }
}

void Neuron::validateRestingPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid resting potential: NaN or infinite");
    }
    
    // Typical resting potential range
    if (potential < -85.0f || potential > -55.0f) {
        NLM_LOG_WARNING("Neuron::validateRestingPotential: Resting potential outside typical range: " + 
                       std::to_string(potential));
    }
}

void Neuron::validateResetPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid reset potential: NaN or infinite");
    }
    
    // Reset potential should be near resting potential
    float diff = std::abs(potential - pImpl->state.restingPotential);
    if (diff > 10.0f) {
        NLM_LOG_WARNING("Neuron::validateResetPotential: Reset potential differs significantly from resting potential: " + 
                       std::to_string(potential) + " vs " + std::to_string(pImpl->state.restingPotential));
    }
}

void Neuron::validateRefractoryPeriod(uint32_t period) {
    if (period > 100) {  // Max reasonable refractory period
        NLM_LOG_WARNING("Neuron::validateRefractoryPeriod: Refractory period unusually high: " + 
                       std::to_string(period) + "ms");
    }
}

} // namespace nlm

