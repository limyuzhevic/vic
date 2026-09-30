#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Error handling constants
namespace ErrorMessages {
    const std::string NEURON_NULL_IMPL = "Neuron::ensureImplNotNull: Neuron implementation is null";
    const std::string INVALID_TIMESTEP = "Neuron::stepLIF: Invalid timestep {} (must be positive)";
    const std::string CURRENT_OUT_OF_RANGE = "Neuron::stepLIF: Current value {} out of valid range [{}, {}]";
    const std::string SPIKE_HISTORY_FULL = "Neuron::recordSpike: Spike history at capacity (max = {})";
    const std::string NULL_NEURON_POINTER = "Neuron::{}: Neuron pointer is null";
    const std::string NULL_INPUT_POINTER = "Neuron::{}: Input pointer is null";
}

// Neural network constants
namespace NLMConstants {
    // Physical constants
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_CURRENT = -1000.0f;
    static constexpr float MAX_CURRENT = 1000.0f;
    static constexpr float MIN_TIMESTEP = 0.001f;
    static constexpr float MAX_TIMESTEP = 100.0f;
    
    // Biological parameters
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    static constexpr float DEFAULT_THRESHOLD = -55.0f;
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;
    
    // Biological range limits for initialization
    static constexpr float MEMBRANE_POTENTIAL_RANGE_MIN = -3.0f;
    static constexpr float MEMBRANE_POTENTIAL_RANGE_MAX = 3.0f;
    static constexpr float THRESHOLD_RANGE_MIN = -2.0f;
    static constexpr float THRESHOLD_RANGE_MAX = 2.0f;
    static constexpr float RESTING_POTENTIAL_RANGE_MIN = -2.0f;
    static constexpr float RESTING_POTENTIAL_RANGE_MAX = 2.0f;
    static constexpr float RESET_POTENTIAL_RANGE_MIN = 0.0f;
    static constexpr float RESET_POTENTIAL_RANGE_MAX = 5.0f;
    static constexpr uint32_t REFRACTORY_PERIOD_MIN = 2;
    static constexpr uint32_t REFRACTORY_PERIOD_MAX = 10;
    
    // Algorithm parameters
    static constexpr float MIN_NOISE_AMPLITUDE = 0.0f;
    static constexpr float MAX_NOISE_AMPLITUDE = 100.0f;
    static constexpr uint32_t MIN_SPIKE_HISTORY_SIZE = 0;
    static constexpr uint32_t MAX_SPIKE_HISTORY_SIZE = 1000;
    
    // STDP parameters
    static constexpr float LTP_WEIGHT = 0.01f;
    static constexpr float LTD_WEIGHT = 0.012f;
}

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
    
    // Physics constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Algorithm constants
    static constexpr float LIF_TIMESTEP = 0.001f;  // 1ms
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    static constexpr float ADAPTATION_INCREMENT = 1.0f;
    static constexpr float ADAPTATION_THRESHOLD = 0.01f;
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;
    static constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::~Neuron() = default;

// Ensure the implementation is valid
void Neuron::ensureImplNotNull() const {
    if (!pImpl) {
        NLM_LOG_ERROR(ErrorMessages::NEURON_NULL_IMPL);
        throw std::runtime_error(ErrorMessages::NEURON_NULL_IMPL);
    }
}

Impl* Neuron::getImpl() const {
    ensureImplNotNull();
    return pImpl.get();
}

Impl* Neuron::getImpl() {
    ensureImplNotNull();
    return pImpl.get();
}

Neuron::Neuron(NeuronId id) : pImpl(std::make_unique<Impl>()) {
    pImpl->id = id;
}

NeuronId Neuron::getId() const {
    ensureImplNotNull();
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    ensureImplNotNull();
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    ensureImplNotNull();
    pImpl->type = type;
}

const NeuronState& Neuron::getState() const {
    ensureImplNotNull();
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    ensureImplNotNull();
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    ensureImplNotNull();
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    ensureImplNotNull();
    
    // Validate input
    if (potential < NLMConstants::MIN_MEMBRANE_POTENTIAL || 
        potential > NLMConstants::MAX_MEMBRANE_POTENTIAL) {
        NLM_LOG_ERROR("Neuron::setMembranePotential: Potential {} out of range [{}, {}]",
                     potential, NLMConstants::MIN_MEMBRANE_POTENTIAL, NLMConstants::MAX_MEMBRANE_POTENTIAL);
        return;
    }
    
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    ensureImplNotNull();
    pImpl->state.membranePotential += delta;
}

MembranePotential Neuron::getThreshold() const {
    ensureImplNotNull();
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    ensureImplNotNull();
    
    // Validate threshold is reasonable
    if (threshold < NLMConstants::MIN_MEMBRANE_POTENTIAL || 
        threshold > NLMConstants::MAX_MEMBRANE_POTENTIAL) {
        NLM_LOG_ERROR("Neuron::setThreshold: Threshold {} out of valid range", threshold);
        return;
    }
    
    pImpl->state.threshold = threshold;
}

bool Neuron::isFiring() const {
    ensureImplNotNull();
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    ensureImplNotNull();
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    ensureImplNotNull();
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    ensureImplNotNull();
    
    // Validate refractory period is reasonable
    if (steps > NLMConstants::MAX_SPIKE_HISTORY_SIZE) {
        NLM_LOG_ERROR("Neuron::setRefractoryPeriod: Period {} exceeds maximum allowed", steps);
        steps = NLMConstants::MAX_SPIKE_HISTORY_SIZE;
    }
    
    pImpl->state.refractoryPeriod = steps;
}

void Neuron::decrementRefractory() {
    ensureImplNotNull();
    auto* impl = getImpl();
    if (impl->state.refractoryRemaining > 0) {
        --impl->state.refractoryRemaining;
        if (impl->state.refractoryRemaining == 0) {
            impl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const {
    ensureImplNotNull();
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    ensureImplNotNull();
    
    // Validate firing rate
    if (rate < 0.0f || rate > 1000.0f) {
        NLM_LOG_ERROR("Neuron::setFiringRate: Rate {} out of valid range [0, 1000]", rate);
        return;
    }
    
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    ensureImplNotNull();
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    ensureImplNotNull();
    return pImpl->state.leakConductance;
}

uint32_t Neuron::getRefractoryPeriod() const {
    ensureImplNotNull();
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    ensureImplNotNull();
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    ensureImplNotNull();
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    ensureImplNotNull();
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    ensureImplNotNull();
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    ensureImplNotNull();
    const auto* impl = getImpl();
    return impl->state.membranePotential >= impl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    ensureImplNotNull();
    return getImpl()->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    ensureImplNotNull();
    
    // Validate input amplitude
    if (amplitude < NLMConstants::MIN_CURRENT || amplitude > NLMConstants::MAX_CURRENT) {
        NLM_LOG_ERROR("Neuron::receiveExcitatoryInput: Amplitude {} out of valid range", amplitude);
        return;
    }
    
    getImpl()->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    ensureImplNotNull();
    
    // Validate input amplitude
    if (amplitude < NLMConstants::MIN_CURRENT || amplitude > NLMConstants::MAX_CURRENT) {
        NLM_LOG_ERROR("Neuron::receiveInhibitoryInput: Amplitude {} out of valid range", amplitude);
        return;
    }
    
    getImpl()->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    ensureImplNotNull();
    
    // Validate input amplitude
    if (amplitude < NLMConstants::MIN_CURRENT || amplitude > NLMConstants::MAX_CURRENT) {
        NLM_LOG_ERROR("Neuron::receiveModulatoryInput: Amplitude {} out of valid range", amplitude);
        return;
    }
    
    getImpl()->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    ensureImplNotNull();
    
    // Validate current value
    if (current < NLMConstants::MIN_CURRENT || current > NLMConstants::MAX_CURRENT) {
        NLM_LOG_ERROR("Neuron::injectCurrent: Current {} out of valid range [{}, {}]",
                     current, NLMConstants::MIN_CURRENT, NLMConstants::MAX_CURRENT);
        return;
    }
    
    getImpl()->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    ensureImplNotNull();
    return getImpl()->synapticInput;
}

void Neuron::clearTotalCurrent() {
    ensureImplNotNull();
    getImpl()->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    ensureImplNotNull();
    auto* impl = getImpl();
    
    // Validate spike time
    if (timestamp < 0.0) {
        NLM_LOG_ERROR("Neuron::recordSpike: Invalid timestamp {} (must be >= 0)", timestamp);
        return;
    }
    
    impl->spikeHistory.push_back(timestamp);
    
    // Check spike history bounds
    if (impl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        NLM_LOG_WARNING(ErrorMessages::SPIKE_HISTORY_FULL, Impl::MAX_SPIKE_HISTORY);
        impl->spikeHistory.erase(impl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    ensureImplNotNull();
    return getImpl()->spikeHistory;
}

void Neuron::clearSpikeHistory() {
    ensureImplNotNull();
    getImpl()->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    ensureImplNotNull();
    getImpl()->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    ensureImplNotNull();
    getImpl()->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    ensureImplNotNull();
    return getImpl()->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    ensureImplNotNull();
    return getImpl()->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    ensureImplNotNull();
    return getImpl()->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    ensureImplNotNull();
    return getImpl()->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    ensureImplNotNull();
    getImpl()->plasticityFlags.hebbian = hebbian;
    getImpl()->plasticityFlags.stdp = stdp;
    getImpl()->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    ensureImplNotNull();
    getImpl()->regionId = region;
}

RegionId Neuron::getRegionId() const {
    ensureImplNotNull();
    return getImpl()->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    ensureImplNotNull();
    getImpl()->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    ensureImplNotNull();
    return getImpl()->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    ensureImplNotNull();
    bool fired = false;
    
    auto* impl = getImpl();
    
    // Input validation
    if (dt <= 0.0) {
        NLM_LOG_ERROR(ErrorMessages::INVALID_TIMESTEP, dt);
        return false;
    }
    
    if (dt < NLMConstants::MIN_TIMESTEP) {
        NLM_LOG_WARNING("Neuron::stepLIF: Timestep {} below minimum ({}). Using minimum.",
                       dt, NLMConstants::MIN_TIMESTEP);
        dt = NLMConstants::MIN_TIMESTEP;
    } else if (dt > NLMConstants::MAX_TIMESTEP) {
        NLM_LOG_WARNING("Neuron::stepLIF: Timestep {} exceeds maximum ({}). Using maximum.",
                       dt, NLMConstants::MAX_TIMESTEP);
        dt = NLMConstants::MAX_TIMESTEP;
    }
    
    // Handle refractory period
    if (impl->state.refractoryRemaining > 0) {
        --impl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        impl->synapticInput = 0.0f;
        if (impl->state.refractoryRemaining == 0) {
            impl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = impl->state.membranePotential;
    MembranePotential V_rest = impl->state.restingPotential;
    MembranePotential V_reset = impl->state.resetPotential;
    MembranePotential threshold = impl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = impl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (impl->state.adaptationVariable > 0.0f) {
        V -= impl->state.adaptationVariable * Impl::ADAPTATION_THRESHOLD;
        impl->state.adaptationVariable *= Impl::ADAPTATION_DECAY_RATE;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, NLMConstants::MIN_MEMBRANE_POTENTIAL, NLMConstants::MAX_MEMBRANE_POTENTIAL);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        impl->state.firingState = FiringState::Active;
        impl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        impl->state.refractoryRemaining = impl->state.refractoryPeriod;
        impl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        impl->state.adaptationVariable += Impl::ADAPTATION_INCREMENT;
    } else {
        impl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    impl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    ensureImplNotNull();
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = Impl::LIF_TIMESTEP;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    ensureImplNotNull();
    getImpl()->state = NeuronState();
    getImpl()->synapticInput = 0.0f;
    getImpl()->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    ensureImplNotNull();
    
// Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    getImpl()->state.membranePotential = getImpl()->state.restingPotential + 
                                         rng.uniformReal(NLMConstants::MEMBRANE_POTENTIAL_RANGE_MIN, NLMConstants::MEMBRANE_POTENTIAL_RANGE_MAX);
    
    // Threshold is typically -55mV with small variation
    getImpl()->state.threshold = NLMConstants::DEFAULT_THRESHOLD + 
                                 rng.uniformReal(NLMConstants::THRESHOLD_RANGE_MIN, NLMConstants::THRESHOLD_RANGE_MAX);
    
    // Resting potential typically -70mV
    getImpl()->state.restingPotential = NLMConstants::DEFAULT_RESTING_POTENTIAL + 
                                         rng.uniformReal(NLMConstants::RESTING_POTENTIAL_RANGE_MIN, NLMConstants::RESTING_POTENTIAL_RANGE_MAX);
    
    // Reset potential is usually close to resting
    getImpl()->state.resetPotential = getImpl()->state.restingPotential + 
                                       rng.uniformReal(NLMConstants::RESET_POTENTIAL_RANGE_MIN, NLMConstants::RESET_POTENTIAL_RANGE_MAX);
    
    // Refractory period: 2-10ms typical
    getImpl()->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(NLMConstants::REFRACTORY_PERIOD_MIN, NLMConstants::REFRACTORY_PERIOD_MAX));
    
    // Initial state
    getImpl()->state.firingState = FiringState::Resting;
    getImpl()->state.refractoryRemaining = 0;
    getImpl()->state.adaptationVariable = 0.0f;
    getImpl()->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    getImpl()->synapticInput = 0.0f;
    getImpl()->spikeHistory.clear();
}

} // namespace nlm