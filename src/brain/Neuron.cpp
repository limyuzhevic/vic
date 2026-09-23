#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>

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
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron constructor: implementation is null");
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() {
    NLM_LOG_INFO("Neuron " + std::to_string(pImpl->id.index()) + " destroyed");
}

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
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setType: implementation is null");
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setMembranePotential: implementation is null");
    
    // Validate potential
    if (!nlm::Validation::isFinite(static_cast<double>(potential), "Invalid membrane potential")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid membrane potential: " + std::to_string(potential),
                       "Neuron", "setMembranePotential");
        return;
    }
    
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::addToMembranePotential: implementation is null");
    
    // Validate delta
    if (!nlm::Validation::isFinite(static_cast<double>(delta), "Invalid membrane potential delta")) {
        NLM_LOG_WARNING("Invalid membrane potential delta: " + std::to_string(delta));
        return;
    }
    
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setThreshold: implementation is null");
    
    // Validate threshold
    if (!nlm::Validation::isFinite(static_cast<double>(threshold), "Invalid threshold")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid threshold: " + std::to_string(threshold),
                       "Neuron", "setThreshold");
        return;
    }
    
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setFiringState: implementation is null");
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::decrementRefractory: implementation is null");
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setFiringRate: implementation is null");
    
    // Validate rate
    if (!nlm::Validation::isFinite(static_cast<double>(rate), "Invalid firing rate")) {
        NLM_LOG_WARNING("Invalid firing rate: " + std::to_string(rate));
        return;
    }
    
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setLeakConductance: implementation is null");
    
    // Validate conductance
    if (!nlm::Validation::isFinite(static_cast<double>(conductance), "Invalid leak conductance")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid leak conductance: " + std::to_string(conductance),
                       "Neuron", "setLeakConductance");
        return;
    }
    
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getLeakConductance: implementation is null");
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setRefractoryPeriod: implementation is null");
    
    // Validate steps
    if (!nlm::Validation::isNonNegative(static_cast<long long>(steps), 
                                        "Refractory period cannot be negative")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid refractory period: " + std::to_string(steps),
                       "Neuron", "setRefractoryPeriod");
        return;
    }
    
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getRefractoryPeriod: implementation is null");
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setRestingPotential: implementation is null");
    
    // Validate potential
    if (!nlm::Validation::isFinite(static_cast<double>(potential), "Invalid resting potential")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid resting potential: " + std::to_string(potential),
                       "Neuron", "setRestingPotential");
        return;
    }
    
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getRestingPotential: implementation is null");
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setResetPotential: implementation is null");
    
    // Validate potential
    if (!nlm::Validation::isFinite(static_cast<double>(potential), "Invalid reset potential")) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Validation,
                       "Invalid reset potential: " + std::to_string(potential),
                       "Neuron", "setResetPotential");
        return;
    }
    
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getResetPotential: implementation is null");
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::checkThreshold: implementation is null");
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getLastSpikeTime: implementation is null");
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::receiveExcitatoryInput: implementation is null");
    
    // Validate amplitude
    if (!nlm::Validation::isFinite(static_cast<double>(amplitude), "Invalid excitatory input amplitude")) {
        NLM_LOG_WARNING("Invalid excitatory input amplitude: " + std::to_string(amplitude));
        return;
    }
    
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::receiveInhibitoryInput: implementation is null");
    
    // Validate amplitude
    if (!nlm::Validation::isFinite(static_cast<double>(amplitude), "Invalid inhibitory input amplitude")) {
        NLM_LOG_WARNING("Invalid inhibitory input amplitude: " + std::to_string(amplitude));
        return;
    }
    
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::receiveModulatoryInput: implementation is null");
    
    // Validate amplitude
    if (!nlm::Validation::isFinite(static_cast<double>(amplitude), "Invalid modulatory input amplitude")) {
        NLM_LOG_WARNING("Invalid modulatory input amplitude: " + std::to_string(amplitude));
        return;
    }
    
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::injectCurrent: implementation is null");
    
    // Validate current
    if (!nlm::Validation::isFinite(static_cast<double>(current), "Invalid injected current")) {
        NLM_LOG_WARNING("Invalid injected current: " + std::to_string(current));
        return;
    }
    
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::clearTotalCurrent: implementation is null");
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::recordSpike: implementation is null");
    
    // Validate timestamp
    if (!nlm::Validation::isFinite(static_cast<double>(timestamp), "Invalid spike timestamp")) {
        NLM_LOG_WARNING("Invalid spike timestamp: " + std::to_string(timestamp));
        return;
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::clearSpikeHistory: implementation is null");
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::addIncomingSynapse: implementation is null");
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::addOutgoingSynapse: implementation is null");
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getState: implementation is null");
    return pImpl->state;
}

const NeuronState& Neuron::getState() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getState: implementation is null");
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getPlasticityFlags: implementation is null");
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getPlasticityFlags: implementation is null");
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::enablePlasticity: implementation is null");
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setRegionId: implementation is null");
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getRegionId: implementation is null");
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::setPopulationId: implementation is null");
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::getPopulationId: implementation is null");
    return pImpl->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::stepLIF: implementation is null");
    
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
    
    // Validate membrane potential
    if (!nlm::Validation::isFinite(static_cast<double>(V), "Invalid membrane potential")) {
        NLM_LOG_WARNING("Invalid membrane potential calculated: " + std::to_string(V));
        V = pImpl->state.restingPotential;  // Reset to safe value
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

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::reset: implementation is null");
    
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Neuron::initializeRandom: implementation is null");
    NLM_CHECK_NULL_PTR(&rng, "rng", "Neuron::initializeRandom: random generator is null");
    
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
