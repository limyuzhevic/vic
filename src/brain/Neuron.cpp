#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <string>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistoryBuffer;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Circular buffer management for O(1) spike history
    size_t spikeHistoryHeadIndex;
    size_t spikeHistorySize;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             spikeHistoryHeadIndex(0), spikeHistorySize(0) {}
};

// Initialize compile-time validation
NLM_LOG_INFO("Initializing neuron validation system...");
ValidationHelper::compileTimeValidation();
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Validate neuron ID
    ValidationHelper::validateNeuronId(id, ValidationLevel::Strict, "Neuron constructor");
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistoryHeadIndex = 0;
    pImpl->spikeHistorySize = 0;
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

// Validation configuration
void Neuron::setValidationLevel(ValidationLevel level) {
    // Note: In a real implementation, this would need to be stored in pImpl
    NLM_LOG_INFO("Neuron " + std::to_string(pImpl->id.index()) + " validation level set to: " + 
                 std::to_string(static_cast<int>(level)));
}

ValidationLevel Neuron::getValidationLevel() const {
    return ValidationLevel::Strict; // Default to strict
}

// Validation helper methods
bool Neuron::validateMembranePotential(MembranePotential potential) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::setMembranePotential";
    return ValidationHelper::validateMembranePotential(potential, level, context);
}

bool Neuron::validateThreshold(MembranePotential threshold) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::setThreshold";
    bool valid = true;
    
    // Check threshold vs resting potential
    if (ValidationHelper::validateThresholdVsResting(threshold, pImpl->state.restingPotential, level, context)) {
        valid = true;
    }
    
    return valid;
}

bool Neuron::validateRefractoryPeriod(uint32_t period) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::setRefractoryPeriod";
    return ValidationHelper::validateRefractoryPeriod(period, level, context);
}

bool Neuron::validateAdaptationVariable(float adaptation) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::update";
    return ValidationHelper::validateAdaptationVariable(adaptation, level, context);
}

bool Neuron::validateLeakConductance(float conductance) const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::setLeakConductance";
    return ValidationHelper::validateLeakConductance(conductance, level, context);
}

bool Neuron::validateNeuronId() const {
    ValidationLevel level = getValidationLevel();
    const char* context = "Neuron::getId";
    return ValidationHelper::validateNeuronId(pImpl->id, level, context);
}

// Validate entire neuron state (constant time check)
bool Neuron::validateState() const {
    ValidationLevel level = getValidationLevel();
    bool valid = true;
    
    // Validate key state variables
    valid &= validateMembranePotential(pImpl->state.membranePotential);
    valid &= validateThreshold(pImpl->state.threshold);
    valid &= validateRefractoryPeriod(pImpl->state.refractoryPeriod);
    valid &= validateAdaptationVariable(pImpl->state.adaptationVariable);
    valid &= validateLeakConductance(pImpl->state.leakConductance);
    
    // Check refractory remaining is <= period
    if (pImpl->state.refractoryRemaining > pImpl->state.refractoryPeriod) {
        std::string msg = std::string("Refractory remaining (" + std::to_string(pImpl->state.refractoryRemaining) +
                                     ") exceeds refractory period (" + std::to_string(pImpl->state.refractoryPeriod) + ")");
        switch (level) {
            case ValidationLevel::None: break;
            case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
            case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
        }
        valid = false;
    }
    
    // Check last spike time
    if (pImpl->state.lastSpikeTime < -1.0f) {
        std::string msg = std::string("Invalid last spike time: " + std::to_string(pImpl->state.lastSpikeTime));
        switch (level) {
            case ValidationLevel::None: break;
            case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
            case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
        }
        valid = false;
    }
    
    return valid;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active || pImpl->state.firingState == FiringState::Refractory;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!validateRefractoryPeriod(steps)) {
        NLM_LOG_ERROR("Refractory period validation failed in setRefractoryPeriod");
    }
    pImpl->state.refractoryPeriod = steps;
    // Ensure refractory remaining doesn't exceed period
    if (pImpl->state.refractoryRemaining > pImpl->state.refractoryPeriod) {
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    }
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
    if (!validateLeakConductance(conductance)) {
        NLM_LOG_ERROR("Leak conductance validation failed in setLeakConductance");
    }
    pImpl->state.leakConductance = conductance;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (!validateMembranePotential(potential)) {
        NLM_LOG_ERROR("Membrane potential validation failed in setMembranePotential");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    if (!validateMembranePotential(newPotential)) {
        NLM_LOG_WARNING("Membrane potential would exceed bounds in addToMembranePotential, clamping");
        newPotential = std::clamp(newPotential, ValidationHelper::MEMBRANE_POTENTIAL_MIN, ValidationHelper::MEMBRANE_POTENTIAL_MAX);
    }
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (!validateThreshold(threshold)) {
        NLM_LOG_ERROR("Threshold validation failed in setThreshold");
    }
    pImpl->state.threshold = threshold;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
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
    
    // Validate adaptation variable
    if (!validateAdaptationVariable(pImpl->state.adaptationVariable)) {
        NLM_LOG_WARNING("Adaptation variable exceeded bounds after modulatory input, clamping");
        pImpl->state.adaptationVariable = std::clamp(pImpl->state.adaptationVariable,
                                                    ValidationHelper::ADAPTATION_VARIABLE_MIN,
                                                    ValidationHelper::ADAPTATION_VARIABLE_MAX);
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (pImpl->spikeHistorySize < Impl::MAX_SPIKE_HISTORY) {
        size_t index = (pImpl->spikeHistoryHeadIndex + pImpl->spikeHistorySize) % Impl::MAX_SPIKE_HISTORY;
        pImpl->spikeHistoryBuffer[index] = timestamp;
        pImpl->spikeHistorySize++;
    } else {
        pImpl->spikeHistoryHeadIndex = (pImpl->spikeHistoryHeadIndex + 1) % Impl::MAX_SPIKE_HISTORY;
        pImpl->spikeHistoryBuffer[pImpl->spikeHistoryHeadIndex] = timestamp;
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    if (pImpl->spikeHistorySize == 0) {
        return pImpl->spikeHistoryBuffer;
    }
    
    static thread_local std::vector<Timestamp> chronologicalHistory;
    chronologicalHistory.clear();
    chronologicalHistory.reserve(pImpl->spikeHistorySize);
    
    if (pImpl->spikeHistorySize <= Impl::MAX_SPIKE_HISTORY) {
        size_t start = (pImpl->spikeHistoryHeadIndex + Impl::MAX_SPIKE_HISTORY - pImpl->spikeHistorySize) % Impl::MAX_SPIKE_HISTORY;
        for (size_t i = 0; i < pImpl->spikeHistorySize; ++i) {
            chronologicalHistory.push_back(pImpl->spikeHistoryBuffer[(start + i) % Impl::MAX_SPIKE_HISTORY]);
        }
    } else {
        for (size_t i = 0; i < Impl::MAX_SPIKE_HISTORY; ++i) {
            chronologicalHistory.push_back(pImpl->spikeHistoryBuffer[(pImpl->spikeHistoryHeadIndex + i + 1) % Impl::MAX_SPIKE_HISTORY]);
        }
    }
    
    return chronologicalHistory;
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistorySize = 0;
    pImpl->spikeHistoryHeadIndex = 0;
    pImpl->spikeHistoryBuffer.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Validate state at start of step
    if (!validateState()) {
        NLM_LOG_WARNING("Neuron state validation failed before stepLIF");
    }
    
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
    
    // Validate membrane potential after operations
    if (!validateMembranePotential(V)) {
        NLM_LOG_WARNING("Membrane potential exceeded bounds after stepLIF, clamping");
        V = std::clamp(V, ValidationHelper::MEMBRANE_POTENTIAL_MIN, ValidationHelper::MEMBRANE_POTENTIAL_MAX);
    }
    
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
        
        // Validate adaptation variable
        if (!validateAdaptationVariable(pImpl->state.adaptationVariable)) {
            NLM_LOG_WARNING("Adaptation variable exceeded bounds after spike, clamping");
            pImpl->state.adaptationVariable = std::clamp(pImpl->state.adaptationVariable,
                                                        ValidationHelper::ADAPTATION_VARIABLE_MIN,
                                                        ValidationHelper::ADAPTATION_VARIABLE_MAX);
        }
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