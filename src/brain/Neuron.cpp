#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <numeric>
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
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Error handling
    ErrorHandlingResult lastError;
    std::vector<ErrorInfo> allErrors;
    std::atomic<size_t> validationEnabled{1};  // 1 = enabled, 0 = disabled
    std::mutex errorMutex;
    
    // Stability tracking
    float voltageVelocity;           // dV/dt for adaptive timestep
    float voltageAcceleration;        // d2V/dt2 for stability metrics
    float minStableVoltage;           // Minimum observed stable voltage
    float maxStableVoltage;           // Maximum observed stable voltage
    uint32_t stableFrames;           // Frames with low voltage change
    bool hasNanInput;                // Flag for NaN input detected
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Adaptive timestep parameters
    static constexpr float MIN_TIMESTEP = 1e-6f;      // 1 microsecond minimum
    static constexpr float MAX_TIMESTEP = 1.0f;        // 1 second maximum
    static constexpr float ADAPTIVE_TIMESTEP_FACTOR = 0.1f;  // 10% change threshold
    static constexpr float STABLE_VELOCITY_THRESHOLD = 0.1f;  // mV/ms
    static constexpr float VOLTAGE_CHANGE_THRESHOLD = 1.0f;  // mV threshold for adaptive step
    static constexpr float BIOLOGICAL_CLAMP_MIN = -100.0f;  // mV
    static constexpr float BIOLOGICAL_CLAMP_MAX = 100.0f;   // mV
    static constexpr float INPUT_CLAMP_MIN = -1000.0f;      // nA
    static constexpr float INPUT_CLAMP_MAX = 1000.0f;       // nA
    static constexpr float MIN_CURRENT = 1e-6f;             // nA minimum
    static constexpr float MAX_CURRENT = 100.0f;            // nA maximum
    
    // Spike detection hysteresis
    static constexpr float SPIKE_HYSTERESIS = 2.0f;         // mV for hysteresis
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             voltageVelocity(0.0f), voltageAcceleration(0.0f),
             minStableVoltage(0.0f), maxStableVoltage(0.0f),
             stableFrames(0), hasNanInput(false), validationEnabled(1) {};
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

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
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

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
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

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateSynapticInput(amplitude)) {
        // Try recovery - clamp to valid range
        MembranePotential clampedAmplitude = std::clamp(amplitude, 
                                                       Impl::INPUT_CLAMP_MIN, 
                                                       Impl::INPUT_CLAMP_MAX);
        pImpl->lastError.addError(ErrorType::SynapticInputValidation, 
            "Synaptic input amplitude invalid, using clamped value", __FILE__, __LINE__, 
            "receiveExcitatoryInput()", true);
        pImpl->synapticInput += clampedAmplitude;
        return;
    }
    
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
    
    // Clamp to ensure it stays within bounds
    pImpl->synapticInput = std::clamp(pImpl->synapticInput, 
                                     Impl::INPUT_CLAMP_MIN, 
                                     Impl::INPUT_CLAMP_MAX);
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateSynapticInput(amplitude)) {
        // Try recovery - clamp to valid range
        MembranePotential clampedAmplitude = std::clamp(amplitude, 
                                                       Impl::INPUT_CLAMP_MIN, 
                                                       Impl::INPUT_CLAMP_MAX);
        pImpl->lastError.addError(ErrorType::SynapticInputValidation, 
            "Synaptic input amplitude invalid, using clamped value", __FILE__, __LINE__, 
            "receiveInhibitoryInput()", true);
        pImpl->synapticInput -= clampedAmplitude;
        return;
    }
    
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
    
    // Clamp to ensure it stays within bounds
    pImpl->synapticInput = std::clamp(pImpl->synapticInput, 
                                     Impl::INPUT_CLAMP_MIN, 
                                     Impl::INPUT_CLAMP_MAX);
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateSynapticInput(amplitude)) {
        // Try recovery - clamp to valid range
        MembranePotential clampedAmplitude = std::clamp(amplitude, 
                                                       Impl::INPUT_CLAMP_MIN, 
                                                       Impl::INPUT_CLAMP_MAX);
        pImpl->lastError.addError(ErrorType::SynapticInputValidation, 
            "Modulatory input amplitude invalid, using clamped value", __FILE__, __LINE__, 
            "receiveModulatoryInput()", true);
        pImpl->state.adaptationVariable += clampedAmplitude * 0.1f;
        return;
    }
    
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateCurrentInjection(current)) {
        // Try recovery - clamp to valid range
        MembranePotential clampedCurrent = std::clamp(current, 
                                                      Impl::MIN_CURRENT, 
                                                      Impl::MAX_CURRENT);
        pImpl->lastError.addError(ErrorType::CurrentInjectionValidation, 
            "Current injection value invalid, using clamped value", __FILE__, __LINE__, 
            "injectCurrent()", true);
        pImpl->synapticInput += clampedCurrent;
        return;
    }
    
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
    
    // Clamp to ensure it stays within bounds
    pImpl->synapticInput = std::clamp(pImpl->synapticInput, 
                                     Impl::INPUT_CLAMP_MIN, 
                                     Impl::INPUT_CLAMP_MAX);
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) {
        return;
    }
    
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateSpikeRecord(timestamp)) {
        // Try recovery - use current time as fallback
        Timestamp safeTimestamp = std::max(0.0, timestamp);
        pImpl->lastError.addError(ErrorType::SpikeRecordValidation, 
            "Spike timestamp invalid, using safe value", __FILE__, __LINE__, 
            "recordSpike()", true);
        safeTimestamp = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        pImpl->spikeHistory.push_back(safeTimestamp);
        return;
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (!pImpl) {
        return;
    }
    
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    
    // Validate handle before processing
    if (!validateSynapseOperations(handle)) {
        pImpl->lastError.addError(ErrorType::SynapseOperationValidation, 
            "Cannot add invalid synapse handle", __FILE__, __LINE__, 
            "addIncomingSynapse()");
        return;
    }
    
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    
    // Validate handle before processing
    if (!validateSynapseOperations(handle)) {
        pImpl->lastError.addError(ErrorType::SynapseOperationValidation, 
            "Cannot add invalid synapse handle", __FILE__, __LINE__, 
            "addOutgoingSynapse()");
        return;
    }
    
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) {
        return;
    }
    
    // Validate input before processing
    if (!validateSpikeRecord(timestamp)) {
        // Try recovery - use current time as fallback
        Timestamp safeTimestamp = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        pImpl->lastError.addError(ErrorType::SpikeRecordValidation, 
            "Spike timestamp invalid, using safe value", __FILE__, __LINE__, 
            "recordSpike()", true);
        pImpl->spikeHistory.push_back(safeTimestamp);
        return;
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    
    // Validate handle before processing
    if (!validateSynapseOperations(handle)) {
        pImpl->lastError.addError(ErrorType::SynapseOperationValidation, 
            "Cannot add invalid synapse handle", __FILE__, __LINE__, 
            "addIncomingSynapse()");
        return;
    }
    
    pImpl->incomingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    
    // Validate handle before processing
    if (!validateSynapseOperations(handle)) {
        pImpl->lastError.addError(ErrorType::SynapseOperationValidation, 
            "Cannot add invalid synapse handle", __FILE__, __LINE__, 
            "addOutgoingSynapse()");
        return;
    }
    
    pImpl->outgoingSynapses.push_back(handle);
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        return;
    }
    
    // Validate parameters before enabling plasticity
    if (!hebbian && !stdp && !rewardModulated) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "At least one plasticity mechanism must be enabled", __FILE__, __LINE__, 
            "enablePlasticity()");
    }
    
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    if (!pImpl) {
        return;
    }
    
    // Validate region ID
    if (region == INVALID_REGION_ID) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Invalid region ID provided", __FILE__, __LINE__, 
            "setRegionId()");
    }
    
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    if (!pImpl) {
        return INVALID_REGION_ID;
    }
    
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) {
        return;
    }
    
    // Validate population ID
    if (population == INVALID_POPULATION_ID) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Invalid population ID provided", __FILE__, __LINE__, 
            "setPopulationId()");
    }
    
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    if (!pImpl) {
        return INVALID_POPULATION_ID;
    }
    
    return pImpl->populationId;
}

MembranePotential Neuron::getTotalCurrent() const {
    if (!pImpl) {
        return 0.0f;
    }
    
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) {
        return;
    }
    
    pImpl->totalCurrent = 0.0f;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    if (!pImpl) {
        // Critical error: no implementation
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Validate parameters with error handling
    validateParameters();
    validateState();
    validateBoundaryConditions();
    
    // Log parameter validation results
    if (pImpl->lastError.errors.size() > 0 || pImpl->lastError.warnings.size() > 0) {
        NLM_LOG_WARNING("Neuron validation issues before stepLIF: " + std::to_string(pImpl->lastError.errors.size()) + 
                       " errors, " + std::to_string(pImpl->lastError.warnings.size()) + " warnings");
    }
    
    // Check if we're safe to operate
    if (!isSafeToOperate()) {
        pImpl->lastError.addError(ErrorType::NumericalStability, 
            "Neuron not safe to operate - skipping step", __FILE__, __LINE__, "stepLIF()");
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Attempting recovery by resetting to safe state", __FILE__, __LINE__, "stepLIF()");
        resetToSafeState();
        return false;
    }
    
    // Parameter validation
    if (!validateParameters() || std::isnan(dt) || std::isinf(dt)) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Invalid step parameters", __FILE__, __LINE__, "stepLIF()");
        // Try recovery - reset to safe state and continue with safe timestep
        resetToSafeState();
        dt = Impl::MIN_TIMESTEP;
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Recovered by resetting to safe state and using minimum timestep", __FILE__, __LINE__, "stepLIF()");
    }
    
    // Validate and clamp timestep
    TimestepDuration adaptiveDt = dt;
    if (dt < Impl::MIN_TIMESTEP) {
        adaptiveDt = Impl::MIN_TIMESTEP;
        pImpl->lastError.addError(ErrorType::BoundaryCondition, 
            "Timestep too small, clamped to minimum", __FILE__, __LINE__, "stepLIF()", true);
    } else if (dt > Impl::MAX_TIMESTEP) {
        adaptiveDt = Impl::MAX_TIMESTEP;
        pImpl->lastError.addError(ErrorType::BoundaryCondition, 
            "Timestep too large, clamped to maximum", __FILE__, __LINE__, "stepLIF()", true);
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
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Validate and clamp synaptic input
    pImpl->synapticInput = std::clamp(pImpl->synapticInput, 
                                     Impl::INPUT_CLAMP_MIN, 
                                     Impl::INPUT_CLAMP_MAX);
    
    // Check for NaN/inf values in state
    if (std::isnan(V) || std::isinf(V) || std::isnan(V_rest) || std::isinf(V_rest) ||
        std::isnan(threshold) || std::isinf(threshold)) {
        pImpl->hasNanInput = true;
        // Reset to safe values
        V = V_rest;
        pImpl->lastError.addError(ErrorType::NumericalStability, 
            "NaN/inf detected in state variables, reset to resting potential", __FILE__, __LINE__, "stepLIF()");
        return false;
    }
    
    // Calculate voltage derivative for adaptive timestep
    float synapticContribution = pImpl->synapticInput / C;
    float leakContribution = (V_rest - V) / tau;
    pImpl->voltageVelocity = static_cast<float>(adaptiveDt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Update membrane potential using exponential Euler integration
    V = V + pImpl->voltageVelocity;
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Update stability metrics
    pImpl->voltageAcceleration = pImpl->voltageVelocity - pImpl->voltageVelocity;  // Simplified acceleration
    pImpl->minStableVoltage = std::min(pImpl->minStableVoltage, V);
    pImpl->maxStableVoltage = std::max(pImpl->maxStableVoltage, V);
    
    if (std::abs(pImpl->voltageVelocity) < Impl::STABLE_VELOCITY_THRESHOLD) {
        pImpl->stableFrames++;
    } else {
        pImpl->stableFrames = 0;
    }
    
    // Apply biologically realistic membrane potential clamping
    V = std::clamp(V, Impl::BIOLOGICAL_CLAMP_MIN, Impl::BIOLOGICAL_CLAMP_MAX);
    
    // Improved spike detection with hysteresis
    bool shouldFire = false;
    
    // Check threshold with hysteresis to prevent multiple spikes
    float effectiveThreshold = threshold + (pImpl->state.firingState == FiringState::Refractory ? 
                                           Impl::SPIKE_HYSTERESIS : 0.0f);
    
    // Better threshold crossing detection
    if (V >= effectiveThreshold && !isRefractory()) {
        // Check if crossing threshold in the right direction (not already above)
        static float lastV = V;
        static bool wasAboveThreshold = false;
        
        if (V >= effectiveThreshold && !wasAboveThreshold) {
            shouldFire = true;
        }
        wasAboveThreshold = true;
        lastV = V;
    } else {
        static bool wasAboveThreshold = false;
        wasAboveThreshold = false;
    }
    
    if (shouldFire) {
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
    if (!pImpl) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    
    // Validate timestep
    if (!validateParameters()) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Invalid parameters for step, using safe timestep", __FILE__, __LINE__, "step()", true);
        dt = Impl::MIN_TIMESTEP;
    }
    
    stepLIF(currentTime, dt);
    
    // Log successful step if there were warnings
    if (pImpl->lastError.warnings.size() > 0) {
        NLM_LOG_INFO("Neuron step completed with " + std::to_string(pImpl->lastError.warnings.size()) + 
                    " warnings");
    }
}

// New public method: Get recommended timestep for optimal simulation
TimestepDuration Neuron::getRecommendedTimestep() const {
    if (!pImpl || pImpl->hasNanInput) {
        return Impl::MIN_TIMESTEP;
    }
    
    // Base timestep based on refractory period
    TimestepDuration recommendedDt = static_cast<TimestepDuration>(pImpl->state.refractoryPeriod) * 0.001f;  // 1ms per refractory step
    
    // Adjust based on voltage velocity (adaptive timestep)
    float voltageChange = std::abs(pImpl->voltageVelocity / 1000.0f);  // Convert from mV/ms to mV/s
    if (voltageChange > Impl::VOLTAGE_CHANGE_THRESHOLD) {
        recommendedDt *= Impl::ADAPTIVE_TIMESTEP_FACTOR;
    } else if (pImpl->stableFrames > 10) {
        recommendedDt *= 2.0f;  // Use larger timestep during stable periods
    }
    
    // Ensure within bounds
    if (recommendedDt < Impl::MIN_TIMESTEP) {
        recommendedDt = Impl::MIN_TIMESTEP;
    } else if (recommendedDt > Impl::MAX_TIMESTEP) {
        recommendedDt = Impl::MAX_TIMESTEP;
    }
    
    return recommendedDt;
}

// New public method: Get recommended timestep for optimal simulation
TimestepDuration Neuron::getRecommendedTimestep() const {
    if (!pImpl || pImpl->hasNanInput) {
        return Impl::MIN_TIMESTEP;
    }
    
    // Base timestep based on refractory period
    TimestepDuration recommendedDt = static_cast<TimestepDuration>(pImpl->state.refractoryPeriod) * 0.001f;  // 1ms per refractory step
    
    // Adjust based on voltage velocity (adaptive timestep)
    float voltageChange = std::abs(pImpl->voltageVelocity / 1000.0f);  // Convert from mV/ms to mV/s
    if (voltageChange > Impl::VOLTAGE_CHANGE_THRESHOLD) {
        recommendedDt *= Impl::ADAPTIVE_TIMESTEP_FACTOR;
    } else if (pImpl->stableFrames > 10) {
        recommendedDt *= 2.0f;  // Use larger timestep during stable periods
    }
    
    // Ensure within bounds
    if (recommendedDt < Impl::MIN_TIMESTEP) {
        recommendedDt = Impl::MIN_TIMESTEP;
    } else if (recommendedDt > Impl::MAX_TIMESTEP) {
        recommendedDt = Impl::MAX_TIMESTEP;
    }
    
    return recommendedDt;
}

// New public method: Validate all neuron parameters
void Neuron::validateParameters() const {
    if (!pImpl) {
        pImpl->lastError.addError(ErrorType::ParameterValidation, "Invalid neuron implementation (nullptr)", __FILE__, __LINE__, "validateParameters()");
        return;
    }
    
    bool isValid = true;
    
    // Check for NaN/inf in state variables
    if (std::isnan(pImpl->state.membranePotential) || std::isinf(pImpl->state.membranePotential) ||
        std::isnan(pImpl->state.restingPotential) || std::isinf(pImpl->state.restingPotential) ||
        std::isnan(pImpl->state.threshold) || std::isinf(pImpl->state.threshold) ||
        std::isnan(pImpl->state.resetPotential) || std::isinf(pImpl->state.resetPotential) ||
        std::isnan(pImpl->state.leakConductance) || std::isinf(pImpl->state.leakConductance)) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "NaN or infinity detected in neuron state variables", __FILE__, __LINE__, "validateParameters()");
    }
    
    // Check for biologically plausible ranges
    if (pImpl->state.restingPotential < Impl::BIOLOGICAL_CLAMP_MIN ||
        pImpl->state.restingPotential > Impl::BIOLOGICAL_CLAMP_MAX) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Resting potential out of biological range", __FILE__, __LINE__, "validateParameters()");
    }
    
    if (pImpl->state.threshold < Impl::BIOLOGICAL_CLAMP_MIN ||
        pImpl->state.threshold > Impl::BIOLOGICAL_CLAMP_MAX) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Threshold out of biological range", __FILE__, __LINE__, "validateParameters()");
    }
    
    if (pImpl->state.resetPotential < Impl::BIOLOGICAL_CLAMP_MIN ||
        pImpl->state.resetPotential > Impl::BIOLOGICAL_CLAMP_MAX) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Reset potential out of biological range", __FILE__, __LINE__, "validateParameters()");
    }
    
    // Check synaptic input ranges
    if (std::isnan(pImpl->synapticInput) || std::isinf(pImpl->synapticInput)) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "NaN or infinity in synaptic input", __FILE__, __LINE__, "validateParameters()");
    }
    
    if (std::abs(pImpl->synapticInput) > Impl::INPUT_CLAMP_MAX) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::ParameterValidation, 
            "Synaptic input exceeds maximum allowable value", __FILE__, __LINE__, "validateParameters()");
    }
    
    if (isValid && pImpl->validationEnabled.load()) {
        NLM_LOG_DEBUG("Neuron parameters validation passed");
    }
}

// New public method: Validate neuron state
void Neuron::validateState() const {
    if (!pImpl) {
        pImpl->lastError.addError(ErrorType::StateValidation, "Invalid neuron implementation (nullptr)", __FILE__, __LINE__, "validateState()");
        return;
    }
    
    bool isValid = true;
    
    // Validate state consistency
    if (pImpl->state.firingState == FiringState::Active && 
        pImpl->state.membranePotential < pImpl->state.threshold) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::StateValidation, 
            "Neuron marked as firing but membrane potential below threshold", __FILE__, __LINE__, "validateState()");
    }
    
    if (pImpl->state.firingState == FiringState::Refractory && 
        pImpl->state.refractoryRemaining <= 0) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::StateValidation, 
            "Neuron marked as refractory but refractory counter is zero", __FILE__, __LINE__, "validateState()");
    }
    
    if (pImpl->state.firingState == FiringState::Resting && 
        pImpl->state.refractoryRemaining > 0) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::StateValidation, 
            "Neuron marked as resting but still in refractory period", __FILE__, __LINE__, "validateState()");
    }
    
    // Validate state relationships
    if (pImpl->state.refractoryPeriod > 1000) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::StateValidation, 
            "Refractory period unreasonably large", __FILE__, __LINE__, "validateState()");
    }
    
    if (pImpl->state.adaptationVariable < 0.0f) {
        isValid = false;
        pImpl->lastError.addError(ErrorType::StateValidation, 
            "Adaptation variable cannot be negative", __FILE__, __LINE__, "validateState()");
    }
    
    if (isValid && pImpl->validationEnabled.load()) {
        NLM_LOG_DEBUG("Neuron state validation passed");
    }
}

// New public method: Validate synaptic input
bool Neuron::validateSynapticInput(MembranePotential amplitude) const {
    if (!pImpl) {
        return false;
    }
    
    if (!pImpl->validationEnabled.load()) {
        return true;  // Skip validation if disabled
    }
    
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        pImpl->lastError.addError(ErrorType::SynapticInputValidation, 
            "Synaptic input amplitude is NaN or infinity", __FILE__, __LINE__, "validateSynapticInput()");
        return false;
    }
    
    if (amplitude > Impl::INPUT_CLAMP_MAX || amplitude < Impl::INPUT_CLAMP_MIN) {
        pImpl->lastError.addError(ErrorType::SynapticInputValidation, 
            "Synaptic input amplitude out of acceptable range", __FILE__, __LINE__, "validateSynapticInput()");
        return false;
    }
    
    return true;
}

// New public method: Validate current injection
bool Neuron::validateCurrentInjection(MembranePotential current) const {
    if (!pImpl) {
        return false;
    }
    
    if (!pImpl->validationEnabled.load()) {
        return true;  // Skip validation if disabled
    }
    
    if (std::isnan(current) || std::isinf(current)) {
        pImpl->lastError.addError(ErrorType::CurrentInjectionValidation, 
            "Current injection value is NaN or infinity", __FILE__, __LINE__, "validateCurrentInjection()");
        return false;
    }
    
    if (current > Impl::MAX_CURRENT || current < Impl::MIN_CURRENT) {
        pImpl->lastError.addError(ErrorType::CurrentInjectionValidation, 
            "Current injection out of acceptable range", __FILE__, __LINE__, "validateCurrentInjection()");
        return false;
    }
    
    return true;
}

// New public method: Validate spike record
bool Neuron::validateSpikeRecord(Timestamp timestamp) const {
    if (!pImpl) {
        return false;
    }
    
    if (!pImpl->validationEnabled.load()) {
        return true;  // Skip validation if disabled
    }
    
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        pImpl->lastError.addError(ErrorType::SpikeRecordValidation, 
            "Spike record timestamp is NaN or infinity", __FILE__, __LINE__, "validateSpikeRecord()");
        return false;
    }
    
    if (timestamp < 0.0) {
        pImpl->lastError.addError(ErrorType::SpikeRecordValidation, 
            "Spike record timestamp cannot be negative", __FILE__, __LINE__, "validateSpikeRecord()");
        return false;
    }
    
    return true;
}

// New public method: Validate synapse operations
bool Neuron::validateSynapseOperations(SynapseHandle handle) const {
    if (!pImpl) {
        return false;
    }
    
    if (!pImpl->validationEnabled.load()) {
        return true;  // Skip validation if disabled
    }
    
    if (handle == INVALID_SYNAPSE_ID) {
        pImpl->lastError.addError(ErrorType::SynapseOperationValidation, 
            "Invalid synapse handle provided", __FILE__, __LINE__, "validateSynapseOperations()");
        return false;
    }
    
    return true;
}

// New public method: Get last error
ErrorHandlingResult Neuron::getLastError() const {
    if (!pImpl) {
        ErrorHandlingResult error(ErrorHandlingResult(false, "Invalid neuron implementation (nullptr)"));
        error.addError(ErrorType::MemoryAllocation, "Cannot access error from nullptr implementation", __FILE__, __LINE__, "getLastError()");
        return error;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    return pImpl->lastError;
}

// New public method: Clear errors
void Neuron::clearErrors() {
    if (!pImpl) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    pImpl->lastError.clear();
}

// New public method: Get error count
size_t Neuron::getErrorCount(ErrorType type) const {
    if (!pImpl) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    return pImpl->lastError.getErrorCount(type);
}

// New public method: Enable/disable validation
void Neuron::enableValidation(bool enabled) {
    if (!pImpl) {
        return;
    }
    
    pImpl->validationEnabled.store(enabled ? 1 : 0);
}

// New public method: Check if validation is enabled
bool Neuron::isValidationEnabled() const {
    if (!pImpl) {
        return false;
    }
    
    return pImpl->validationEnabled.load() != 0;
}

// Static public method: Get all errors
const std::vector<ErrorInfo>& Neuron::getAllErrors() {
    static std::vector<ErrorInfo> allErrors;
    return allErrors;
}

// New public method: Reset to safe state
void Neuron::resetToSafeState() {
    if (!pImpl) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Reset to biologically plausible default values
    pImpl->state.membranePotential = pImpl->state.restingPotential;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->synapticInput = 0.0f;
    pImpl->hasNanInput = false;
    pImpl->clearErrors();
    
    NLM_LOG_INFO("Neuron state reset to safe values");
}

// New public method: Recover from error
bool Neuron::recoverFromError(ErrorType errorType) {
    if (!pImpl) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    switch (errorType) {
        case ErrorType::ParameterValidation:
        case ErrorType::StateValidation:
            // For parameter and state errors, reset to safe state
            resetToSafeState();
            pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
                "Recovery from parameter/state validation error", __FILE__, __LINE__, "recoverFromError()");
            return true;
            
        case ErrorType::SynapticInputValidation:
            // For synaptic input errors, clamp and continue
            pImpl->synapticInput = std::clamp(pImpl->synapticInput, Impl::INPUT_CLAMP_MIN, Impl::INPUT_CLAMP_MAX);
            return true;
            
        case ErrorType::RefractoryPeriod:
            // For refractory period errors, ensure valid range
            if (pImpl->state.refractoryPeriod > 100) {
                pImpl->state.refractoryPeriod = 100;
            }
            return true;
            
        default:
            // For other errors, attempt reset
            resetToSafeState();
            return true;
    }
}

// New public method: Safe initialization
bool Neuron::initializeSafe() {
    if (!pImpl) {
        pImpl->lastError.addError(ErrorType::MemoryAllocation, 
            "Cannot initialize safe state with nullptr implementation", __FILE__, __LINE__, "initializeSafe()");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Validate before initializing
    validateParameters();
    validateState();
    
    if (pImpl->lastError.errors.size() > 0) {
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Cannot safely initialize with existing errors", __FILE__, __LINE__, "initializeSafe()");
        return false;
    }
    
    // Initialize to safe biological defaults
    pImpl->state.membranePotential = -70.0f;
    pImpl->state.restingPotential = -70.0f;
    pImpl->state.threshold = -55.0f;
    pImpl->state.resetPotential = -70.0f;
    pImpl->state.leakConductance = 10.0f;
    pImpl->state.synapseConductance = 0.0f;
    pImpl->state.firingRate = 0.0f;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.refractoryPeriod = 5;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->hasNanInput = false;
    clearErrors();
    
    NLM_LOG_INFO("Neuron safely initialized");
    return true;
}

// New public method: Safe cleanup
void Neuron::cleanupSafe() {
    if (!pImpl) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Clear all state
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->incomingSynapses.clear();
    pImpl->outgoingSynapses.clear();
    pImpl->clearErrors();
    
    NLM_LOG_INFO("Neuron safely cleaned up");
}

// New public method: Check if safe to operate
bool Neuron::isSafeToOperate() const {
    if (!pImpl) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    if (pImpl->hasNanInput) {
        return false;
    }
    
    if (std::isnan(pImpl->state.membranePotential) || std::isnan(pImpl->state.restingPotential) ||
        std::isnan(pImpl->state.threshold) || std::isnan(pImpl->state.resetPotential)) {
        return false;
    }
    
    if (pImpl->lastError.errors.size() > 0) {
        return false;
    }
    
    return true;
}

// New public method: Validate state transition
bool Neuron::validateStateTransition(const NeuronState& newState) const {
    if (!pImpl) {
        return false;
    }
    
    bool isValid = true;
    
    // Check if transition is biologically plausible
    if (newState.threshold <= newState.resetPotential) {
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Invalid threshold-reset potential relationship", __FILE__, __LINE__, "validateStateTransition()");
        isValid = false;
    }
    
    if (newState.refractoryPeriod > 1000) {
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Refractory period exceeds maximum allowable value", __FILE__, __LINE__, "validateStateTransition()");
        isValid = false;
    }
    
    if (newState.adaptationVariable < 0.0f) {
        pImpl->lastError.addError(ErrorType::StateTransitionValidation, 
            "Adaptation variable cannot be negative", __FILE__, __LINE__, "validateStateTransition()");
        isValid = false;
    }
    
    return isValid;
}

// New public method: Validate refractory period
bool Neuron::validateRefractoryPeriod(uint32_t steps) const {
    if (!pImpl) {
        return false;
    }
    
    if (steps > 100) {
        pImpl->lastError.addError(ErrorType::RefractoryPeriod, 
            "Refractory period exceeds maximum allowable value", __FILE__, __LINE__, "validateRefractoryPeriod()");
        return false;
    }
    
    return true;
}

// New public method: Validate boundary conditions
bool Neuron::validateBoundaryConditions() const {
    if (!pImpl) {
        return false;
    }
    
    bool isValid = true;
    
    if (pImpl->state.membranePotential < Impl::BIOLOGICAL_CLAMP_MIN ||
        pImpl->state.membranePotential > Impl::BIOLOGICAL_CLAMP_MAX) {
        pImpl->lastError.addError(ErrorType::BoundaryCondition, 
            "Membrane potential outside biological boundaries", __FILE__, __LINE__, "validateBoundaryConditions()");
        isValid = false;
    }
    
    if (pImpl->synapticInput > Impl::INPUT_CLAMP_MAX || pImpl->synapticInput < Impl::INPUT_CLAMP_MIN) {
        pImpl->lastError.addError(ErrorType::BoundaryCondition, 
            "Synaptic input outside boundary conditions", __FILE__, __LINE__, "validateBoundaryConditions()");
        isValid = false;
    }
    
    return isValid;
}

// New public method: Get error rate
float Neuron::getErrorRate() const {
    if (!pImpl) {
        return 0.0f;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Return a simple error rate (errors / total error/warning count)
    size_t total = pImpl->lastError.errors.size() + pImpl->lastError.warnings.size();
    if (total == 0) return 0.0f;
    
    return static_cast<float>(pImpl->lastError.errors.size()) / total;
}

// New public method: Get warning rate
float Neuron::getWarningRate() const {
    if (!pImpl) {
        return 0.0f;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Return a simple warning rate (warnings / total error/warning count)
    size_t total = pImpl->lastError.errors.size() + pImpl->lastError.warnings.size();
    if (total == 0) return 0.0f;
    
    return static_cast<float>(pImpl->lastError.warnings.size()) / total;
}

// New public method: Get numerical stability score
float Neuron::getNumericalStabilityScore() const {
    if (!pImpl) {
        return 0.0f;
    }
    
    float score = 1.0f;
    
    if (pImpl->hasNanInput) {
        score *= 0.0f;
    }
    
    if (std::isnan(pImpl->state.membranePotential) || std::isinf(pImpl->state.membranePotential)) {
        score *= 0.5f;
    }
    
    if (std::abs(pImpl->voltageVelocity) > 1000.0f) {
        score *= 0.8f;
    }
    
    return score;
}

// New public method: Get stability metrics for monitoring
const NeuronStabilityMetrics& Neuron::getStabilityMetrics() const {
    static NeuronStabilityMetrics metrics;
    
    if (!pImpl) {
        metrics.hasNumericalIssues = true;
        return metrics;
    }
    
    // Calculate stability metrics
    metrics.totalSteps = 1;  // Simplified - in real implementation would track over time
    metrics.avgVoltageVelocity = pImpl->voltageVelocity;
    metrics.maxVoltageVelocity = std::abs(pImpl->voltageVelocity);
    metrics.avgVoltageAcceleration = pImpl->voltageAcceleration;
    metrics.maxVoltageAcceleration = std::abs(pImpl->voltageAcceleration);
    metrics.minStableVoltage = pImpl->minStableVoltage;
    metrics.maxStableVoltage = pImpl->maxStableVoltage;
    metrics.stableFramesCount = pImpl->stableFrames;
    
    // Calculate stability score (0.0 to 1.0)
    if (metrics.totalSteps == 0) {
        metrics.stabilityScore = 1.0f;
    } else {
        float stabilityFactor = 1.0f - (static_cast<float>(metrics.stableFramesCount) / metrics.totalSteps);
        metrics.stabilityScore = 1.0f - stabilityFactor;
    }
    
    // Check for numerical stability issues
    metrics.numericalStability = 1.0f;
    if (pImpl->hasNanInput) {
        metrics.numericalStability = 0.0f;
    }
    if (std::isnan(pImpl->state.membranePotential) || std::isinf(pImpl->state.membranePotential)) {
        metrics.numericalStability = 0.0f;
    }
    if (std::abs(pImpl->voltageVelocity) > 1000.0f) {  // Extreme voltage changes
        metrics.numericalStability *= 0.5f;
    }
    
    metrics.hasNumericalIssues = (metrics.numericalStability < 0.5f);
    
    return metrics;
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

namespace nlm {

// Static member definitions
BatchProcessor Neuron::batchProcessor;
BatchProcessingConfig Neuron::batchConfig;
std::mutex Neuron::batchMutex;

BatchProcessingMetrics Neuron::getBatchMetrics() {
    std::lock_guard<std::mutex> lock(batchMutex);
    return batchProcessor.getAccumulatedMetrics();
}

void Neuron::batchLIFStep(std::vector<Neuron*>& neurons, Timestamp currentTime, TimestepDuration dt) {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t processedCount = 0;
    uint64_t stepCount = 0;
    uint64_t errors = 0;
    
    const BatchProcessingConfig& config = batchConfig;
    
    try {
        if (config.enableParallel && neurons.size() > 1) {
            // Parallel processing version
            std::vector<std::future<bool>> futures;
            futures.reserve(neurons.size());
            
            for (Neuron* neuron : neurons) {
                futures.push_back(std::async(std::launch::async, [neuron, currentTime, dt]() {
                    return neuron->stepLIF(currentTime, dt);
                }));
            }
            
            for (auto& future : futures) {
                try {
                    if (future.get()) {
                        stepCount++;
                        processedCount++;
                    }
                } catch (...) {
                    errors++;
                }
            }
            batchProcessor.getAccumulatedMetrics().parallelProcessed += processedCount;
        } else {
            // Synchronous processing version
            for (Neuron* neuron : neurons) {
                try {
                    if (neuron->stepLIF(currentTime, dt)) {
                        stepCount++;
                    }
                    processedCount++;
                } catch (...) {
                    errors++;
                }
            }
        }
    } catch (...) {
        errors++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double stepTime = (processedCount > 0) ? (duration / static_cast<double>(processedCount)) : 0.0;
    
    BatchProcessingMetrics metrics;
    metrics.neuronsProcessed = processedCount;
    metrics.totalSteps = stepCount;
    metrics.errorCount = errors;
    
    batchProcessor.recordOperation(metrics);
}

void Neuron::batchInjectCurrent(std::vector<Neuron*>& neurons, MembranePotential current) {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t processedCount = 0;
    uint64_t errors = 0;
    
    const BatchProcessingConfig& config = batchConfig;
    
    try {
        if (config.enableParallel && neurons.size() > 1) {
            std::vector<std::future<void>> futures;
            futures.reserve(neurons.size());
            
            for (Neuron* neuron : neurons) {
                futures.push_back(std::async(std::launch::async, [neuron, current]() {
                    neuron->injectCurrent(current);
                }));
            }
            
            for (auto& future : futures) {
                try {
                    future.wait();
                    processedCount++;
                } catch (...) {
                    errors++;
                }
            }
            batchProcessor.getAccumulatedMetrics().parallelProcessed += processedCount;
        } else {
            for (Neuron* neuron : neurons) {
                try {
                    neuron->injectCurrent(current);
                    processedCount++;
                } catch (...) {
                    errors++;
                }
            }
        }
    } catch (...) {
        errors++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double stepTime = (processedCount > 0) ? (duration / static_cast<double>(processedCount)) : 0.0;
    
    BatchProcessingMetrics metrics;
    metrics.neuronsProcessed = processedCount;
    metrics.errorCount = errors;
    
    batchProcessor.recordOperation(metrics);
}

void Neuron::batchReset(std::vector<Neuron*>& neurons) {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t processedCount = 0;
    uint64_t errors = 0;
    
    const BatchProcessingConfig& config = batchConfig;
    
    try {
        if (config.enableParallel && neurons.size() > 1) {
            std::vector<std::future<void>> futures;
            futures.reserve(neurons.size());
            
            for (Neuron* neuron : neurons) {
                futures.push_back(std::async(std::launch::async, [neuron]() {
                    neuron->reset();
                }));
            }
            
            for (auto& future : futures) {
                try {
                    future.wait();
                    processedCount++;
                } catch (...) {
                    errors++;
                }
            }
            batchProcessor.getAccumulatedMetrics().parallelProcessed += processedCount;
        } else {
            for (Neuron* neuron : neurons) {
                try {
                    neuron->reset();
                    processedCount++;
                } catch (...) {
                    errors++;
                }
            }
        }
    } catch (...) {
        errors++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double stepTime = (processedCount > 0) ? (duration / static_cast<double>(processedCount)) : 0.0;
    
    BatchProcessingMetrics metrics;
    metrics.neuronsProcessed = processedCount;
    metrics.errorCount = errors;
    
    batchProcessor.recordOperation(metrics);
}

// Update adaptation for spike-frequency adaptation
            pImpl->state.adaptationVariable += 1.0f;
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
    
        // Clear synaptic input for next step
        pImpl->synapticInput = 0.0f;
    
        return fired;
    }

    void Neuron::initializeBiologically(Neuron* neuron, NeuronType type, RandomGenerator& rng) {
        // Type-specific biological initialization
        // Each neuron type has distinct biophysical properties
        neuron->setType(type);
        
        // Initialize membrane potential based on type
        switch (type) {
            case NeuronType::Excitatory:
                // Excitatory neurons typically have higher firing rates
                neuron->setRestingPotential(-65.0f + rng.uniformReal(-5.0f, 5.0f));
                neuron->setThreshold(-50.0f + rng.uniformReal(-3.0f, 3.0f));
                neuron->setResetPotential(-65.0f + rng.uniformReal(0.0f, 5.0f));
                neuron->setLeakConductance(15.0f + rng.uniformReal(5.0f, 20.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(3, 15)));
                break;
                
            case NeuronType::Inhibitory:
                // Inhibitory neurons typically have more stable voltages
                neuron->setRestingPotential(-70.0f + rng.uniformReal(-3.0f, 3.0f));
                neuron->setThreshold(-55.0f + rng.uniformReal(-2.0f, 2.0f));
                neuron->setResetPotential(-70.0f + rng.uniformReal(0.0f, 3.0f));
                neuron->setLeakConductance(10.0f + rng.uniformReal(5.0f, 15.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 10)));
                break;
                
            case NeuronType::Sensory:
                // Sensory neurons often have higher sensitivity
                neuron->setRestingPotential(-60.0f + rng.uniformReal(-8.0f, 8.0f));
                neuron->setThreshold(-45.0f + rng.uniformReal(-4.0f, 4.0f));
                neuron->setResetPotential(-60.0f + rng.uniformReal(0.0f, 8.0f));
                neuron->setLeakConductance(20.0f + rng.uniformReal(10.0f, 30.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(5, 20)));
                break;
                
            case NeuronType::Motor:
                // Motor neurons typically have reliable output
                neuron->setRestingPotential(-75.0f + rng.uniformReal(-5.0f, 5.0f));
                neuron->setThreshold(-58.0f + rng.uniformReal(-3.0f, 3.0f));
                neuron->setResetPotential(-75.0f + rng.uniformReal(0.0f, 5.0f));
                neuron->setLeakConductance(12.0f + rng.uniformReal(5.0f, 18.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(4, 12)));
                break;
                
            case NeuronType::Modulatory:
                // Modulatory neurons often have slower dynamics
                neuron->setRestingPotential(-68.0f + rng.uniformReal(-6.0f, 6.0f));
                neuron->setThreshold(-52.0f + rng.uniformReal(-3.0f, 3.0f));
                neuron->setResetPotential(-68.0f + rng.uniformReal(0.0f, 6.0f));
                neuron->setLeakConductance(8.0f + rng.uniformReal(3.0f, 12.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(6, 18)));
                break;
                
            default: // Internal
                // Default internal neuron properties
                neuron->setRestingPotential(-70.0f + rng.uniformReal(-3.0f, 3.0f));
                neuron->setThreshold(-55.0f + rng.uniformReal(-2.0f, 2.0f));
                neuron->setResetPotential(-70.0f + rng.uniformReal(0.0f, 5.0f));
                neuron->setLeakConductance(10.0f + rng.uniformReal(5.0f, 15.0f));  // nS
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 10)));
                break;
        }
        
        // Initialize membrane potential based on resting potential with noise
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-3.0f, 3.0f));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().adaptationVariable = 0.0f;
        neuron->getState().lastSpikeTime = -1.0f;
        neuron->getState().firingRate = 0.0f;
        
        // Enable plasticity based on type
        bool hebbian = (type == NeuronType::Excitatory || type == NeuronType::Internal);
        bool stdp = (type == NeuronType::Excitatory || type == NeuronType::Inhibitory);
        bool rewardModulated = (type == NeuronType::Modulatory);
        neuron->enablePlasticity(hebbian, stdp, rewardModulated);
    }

    void Neuron::initializeByDevelopmentStage(Neuron* neuron, DevelopmentalStage stage, RandomGenerator& rng) {
        // Development stage-dependent initialization
        // Early developmental stages have different properties than mature neurons
        neuron->setType(NeuronType::Internal);  // Default to internal for developmental stages
        
        float scaleFactor = 1.0f;
        float variationFactor = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                // Very immature neurons with high variability
                scaleFactor = 0.5f;
                variationFactor = 2.0f;
                neuron->setRestingPotential(-65.0f * scaleFactor);
                neuron->setThreshold(-45.0f * scaleFactor);
                neuron->setResetPotential(-65.0f * scaleFactor);
                neuron->setLeakConductance(8.0f * scaleFactor);
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(1, 5) * scaleFactor));
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                // Plastic and highly excitable
                scaleFactor = 0.7f;
                variationFactor = 1.5f;
                neuron->setRestingPotential(-60.0f * scaleFactor);
                neuron->setThreshold(-40.0f * scaleFactor);
                neuron->setResetPotential(-60.0f * scaleFactor);
                neuron->setLeakConductance(12.0f * scaleFactor);
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 8) * scaleFactor));
                break;
                
            case DevelopmentalStage::Maturation:
                // Approaching adult properties
                scaleFactor = 0.85f;
                variationFactor = 1.2f;
                neuron->setRestingPotential(-68.0f * scaleFactor);
                neuron->setThreshold(-53.0f * scaleFactor);
                neuron->setResetPotential(-68.0f * scaleFactor);
                neuron->setLeakConductance(11.0f * scaleFactor);
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(3, 10) * scaleFactor));
                break;
                
            case DevelopmentalStage::Adult:
                // Mature adult neuron properties
                scaleFactor = 1.0f;
                variationFactor = 1.0f;
                neuron->setRestingPotential(-70.0f + rng.uniformReal(-3.0f * variationFactor, 3.0f * variationFactor));
                neuron->setThreshold(-55.0f + rng.uniformReal(-2.0f * variationFactor, 2.0f * variationFactor));
                neuron->setResetPotential(-70.0f + rng.uniformReal(0.0f * variationFactor, 5.0f * variationFactor));
                neuron->setLeakConductance(10.0f + rng.uniformReal(5.0f * variationFactor, 15.0f * variationFactor));
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 10) * scaleFactor));
                break;
                
            case DevelopmentalStage::Aging:
                // Age-related changes (more variability, slower)
                scaleFactor = 1.2f;  // Often larger in aging
                variationFactor = 1.5f;
                neuron->setRestingPotential(-68.0f * scaleFactor);  // Less negative
                neuron->setThreshold(-52.0f * scaleFactor);         // Higher threshold
                neuron->setResetPotential(-68.0f * scaleFactor);
                neuron->setLeakConductance(8.0f * scaleFactor);     // Lower conductance
                neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(5, 15) * scaleFactor));
                break;
        }
        
        // Initialize membrane potential
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-3.0f * variationFactor, 3.0f * variationFactor));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().adaptationVariable = 0.0f;
        neuron->getState().lastSpikeTime = -1.0f;
        neuron->getState().firingRate = 0.0f;
        
        // Plasticity changes with development
        bool hebbian = (stage >= DevelopmentalStage::Maturation);
        bool stdp = (stage >= DevelopmentalStage::CriticalPeriod);
        bool rewardModulated = (stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging);
        neuron->enablePlasticity(hebbian, stdp, rewardModulated);
    }

    void Neuron::initializeForPopulation(Neuron* neuron, PopulationId population, RandomGenerator& rng) {
        // Population-specific initialization using population statistics
        // This uses pre-computed population statistics to maintain consistency
        
        neuron->setType(NeuronType::Internal);  // Default population type
        neuron->setPopulationId(population);
        
        // Get or create population statistics
        PopulationStats& stats = getPopulationStats(population);
        
        // Apply population-specific distributions
        // Use log-normal distribution for more biologically realistic variability
        auto logNormal = [&rng](float mean, float stddev) {
            float sigma = stddev / mean;
            float u = rng.uniformReal(0.0, 1.0);
            float v = rng.uniformReal(0.0, 1.0);
            float s = sqrt(2.0 * log(u)) * cos(2.0 * M_PI * v);
            float scale = sigma * s;
            return mean * exp(scale);
        };
        
        // Initialize with population-specific parameters
        neuron->setRestingPotential(logNormal(stats.avgRestingPotential, stats.stdDevRestingPotential));
        neuron->setThreshold(logNormal(stats.avgThreshold, stats.stdDevThreshold));
        neuron->setResetPotential(logNormal(stats.avgResetPotential, stats.stdDevResetPotential));
        
        // Leak conductance with biological constraints
        float leakConductance = logNormal(stats.synapticDensity, 0.5f * stats.synapticDensity);
        neuron->setLeakConductance(std::clamp(leakConductance, MIN_LEAK_CONDUCTANCE, MAX_LEAK_CONDUCTANCE));
        
        // Refractory period based on firing rate (inverse relationship)
        float baseRefractory = 1000.0f / std::max(stats.firingRate, 1.0f);  // Convert Hz to ms
        neuron->setRefractoryPeriod(static_cast<uint32_t>(std::clamp(baseRefractory, MIN_REFECTORY_PERIOD, MAX_REFECTORY_PERIOD)));
        
        // Initialize membrane potential
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-2.0f, 2.0f));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().adaptationVariable = 0.0f;
        neuron->getState().lastSpikeTime = -1.0f;
        neuron->getState().firingRate = stats.firingRate;
        
        // Set plasticity based on population characteristics
        bool hebbian = (stats.synapticDensity > 1.0f);
        bool stdp = (stats.firingRate > 0.0f);
        bool rewardModulated = (stats.energyConsumption > 0.0f);
        neuron->enablePlasticity(hebbian, stdp, rewardModulated);
    }

    void Neuron::initializeWithSynapticContext(Neuron* neuron, int synapticConnections, RandomGenerator& rng) {
        // Synaptic context initialization based on connection count
        // More connected neurons have different initialization properties
        
        // Determine type based on connection count
        if (synapticConnections > 50) {
            neuron->setType(NeuronType::Excitatory);
        } else if (synapticConnections > 20) {
            neuron->setType(NeuronType::Internal);
        } else if (synapticConnections > 5) {
            neuron->setType(NeuronType::Inhibitory);
        } else {
            neuron->setType(NeuronType::Sensory);  // Weakly connected, likely sensory
        }
        
        // Connection count affects membrane properties
        float connectionFactor = 1.0f;
        if (synapticConnections > 100) {
            connectionFactor = 1.5f;  // Highly connected neurons are more stable
        } else if (synapticConnections > 50) {
            connectionFactor = 1.2f;
        } else if (synapticConnections < 10) {
            connectionFactor = 0.7f;  // Poorly connected neurons are more variable
        }
        
        // Initialize based on connection count with biological distributions
        neuron->setRestingPotential(-70.0f + rng.uniformReal(-3.0f * connectionFactor, 3.0f * connectionFactor));
        neuron->setThreshold(-55.0f + rng.uniformReal(-2.0f * connectionFactor, 2.0f * connectionFactor));
        neuron->setResetPotential(-70.0f + rng.uniformReal(0.0f * connectionFactor, 5.0f * connectionFactor));
        neuron->setLeakConductance(10.0f * connectionFactor + rng.uniformReal(0.0f, 10.0f * connectionFactor));
        
        // Refractory period scales with connectivity
        neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 10) * connectionFactor));
        
        // Highly connected neurons have better initial stability
        float stabilityBoost = std::min(1.0f, synapticConnections / 50.0f);
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-3.0f * (1.0f - stabilityBoost), 3.0f * (1.0f - stabilityBoost)));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().adaptationVariable = 0.0f;
        neuron->getState().lastSpikeTime = -1.0f;
        neuron->getState().firingRate = std::min(50.0f, synapticConnections * 0.5f);  // Hz
        
        // Plasticity depends on connectivity
        bool hebbian = (synapticConnections > 10);
        bool stdp = (synapticConnections > 20);
        bool rewardModulated = (synapticConnections > 100);
        neuron->enablePlasticity(hebbian, stdp, rewardModulated);
    }

    bool Neuron::validateInitializedParameters(const Neuron* neuron) {
        // Validate initialized parameters are within biological ranges
        if (!neuron || !neuron->pImpl) {
            return false;
        }
        
        const auto& state = neuron->pImpl->state;
        
        // Check basic parameter ranges
        if (state.restingPotential < MIN_RESTING_POTENTIAL || state.restingPotential > MAX_RESTING_POTENTIAL) {
            return false;
        }
        
        if (state.threshold < MIN_THRESHOLD || state.threshold > MAX_THRESHOLD) {
            return false;
        }
        
        if (state.resetPotential < MIN_RESET_POTENTIAL || state.resetPotential > MAX_RESET_POTENTIAL) {
            return false;
        }
        
        if (state.leakConductance < MIN_LEAK_CONDUCTANCE || state.leakConductance > MAX_LEAK_CONDUCTANCE) {
            return false;
        }
        
        if (state.refractoryPeriod < MIN_REFECTORY_PERIOD || state.refractoryPeriod > MAX_REFECTORY_PERIOD) {
            return false;
        }
        
        // Check for biological plausibility relationships
        // Reset potential should be close to resting potential (within reasonable range)
        if (std::abs(state.resetPotential - state.restingPotential) > 15.0f) {
            return false;
        }
        
        // Threshold should be more positive than reset potential (typical for LIF neurons)
        if (state.threshold <= state.resetPotential) {
            return false;
        }
        
        // Leak conductance should have reasonable relationship to membrane time constant
        float membraneTimeConstant = MEMBRANE_CAPACITANCE / state.leakConductance;
        if (membraneTimeConstant < 1.0f || membraneTimeConstant > 100.0f) {
            return false;
        }
        
        // Firing rate should be positive and reasonable
        if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
            return false;
        }
        
        // Adaptation variable should be non-negative initially
        if (state.adaptationVariable < 0.0f) {
            return false;
        }
        
        return true;
    }

    void Neuron::initializeWithEnergyBudget(Neuron* neuron, float maxEnergy, RandomGenerator& rng) {
        // Energy/budget-constrained initialization
        // Neurons are initialized with energy budgets that constrain their parameters
        
        // Determine neuron type based on available energy
        if (maxEnergy > 15.0f) {
            neuron->setType(NeuronType::Excitatory);  // High-energy neurons can be more active
        } else if (maxEnergy > 10.0f) {
            neuron->setType(NeuronType::Internal);
        } else if (maxEnergy > 5.0f) {
            neuron->setType(NeuronType::Inhibitory);  // Inhibitory neurons are more energy-efficient
        } else {
            neuron->setType(NeuronType::Sensory);  // Energy-constrained sensory input
        }
        
        // Energy budget affects initialization parameters
        float energyRatio = maxEnergy / 15.0f;  // Normalize to [0, 1]
        
        // Scale biophysical parameters based on energy availability
        neuron->setRestingPotential(-70.0f + rng.uniformReal(-3.0f * energyRatio, 3.0f * energyRatio));
        neuron->setThreshold(-55.0f + rng.uniformReal(-2.0f * energyRatio, 2.0f * energyRatio));
        neuron->setResetPotential(-70.0f + rng.uniformReal(0.0f * energyRatio, 5.0f * energyRatio));
        
        // Energy efficiency: low-energy neurons have lower conductance
        float energyEfficiency = 1.0f - energyRatio;
        neuron->setLeakConductance(10.0f * energyEfficiency + rng.uniformReal(0.0f, 5.0f * energyEfficiency));
        
        // Refractory period: high-energy neurons can afford longer refractory periods
        neuron->setRefractoryPeriod(static_cast<uint32_t>(rng.uniformInt(2, 10) * energyRatio));
        
        // Energy-dependent adaptation
        neuron->getState().adaptationVariable = rng.uniformReal(0.0f, 2.0f * energyRatio);
        
        // Set initial energy level
        neuron->getState().firingRate = maxEnergy * 10.0f;  // Hz per unit energy
        
        // Initialize membrane potential
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-3.0f, 3.0f));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().lastSpikeTime = -1.0f;
        
        // Plasticity based on energy availability
        bool hebbian = (maxEnergy > 10.0f);
        bool stdp = (maxEnergy > 8.0f);
        bool rewardModulated = (maxEnergy > 12.0f);
        neuron->enablePlasticity(hebbian, stdp, rewardModulated);
    }

    void Neuron::initializeFromTemplate(Neuron* neuron, const NeuronTemplate& template, RandomGenerator& rng) {
        // Template-based initialization for consistency
        // Neurons are initialized based on a template with controlled randomness
        
        neuron->setType(template.type);
        
        // Apply template parameters with controlled variation
        neuron->setRestingPotential(template.restingPotential + rng.uniformReal(-1.0f, 1.0f));
        neuron->setThreshold(template.threshold + rng.uniformReal(-0.5f, 0.5f));
        neuron->setResetPotential(template.resetPotential + rng.uniformReal(-1.0f, 1.0f));
        neuron->setLeakConductance(template.leakConductance + rng.uniformReal(-1.0f, 1.0f));
        neuron->setRefractoryPeriod(static_cast<uint32_t>(template.refractoryPeriod * rng.uniformReal(0.8f, 1.2f)));
        
        // Adaptation rate from template
        neuron->getState().adaptationVariable = template.adaptationRate;
        
        // Initialize membrane potential near resting potential
        neuron->setMembranePotential(neuron->getRestingPotential() + rng.uniformReal(-2.0f, 2.0f));
        
        // Initialize other state variables
        neuron->getState().firingState = FiringState::Resting;
        neuron->getState().refractoryRemaining = 0;
        neuron->getState().lastSpikeTime = -1.0f;
        neuron->getState().firingRate = 0.0f;
        
        // Set plasticity based on template
        neuron->enablePlasticity(template.hasPlasticity, template.hasPlasticity, template.hasPlasticity);
    }

    void Neuron::batchInitializeAdvanced(std::vector<Neuron*>& neurons, InitializationStrategy strategy, RandomGenerator& rng) {
        // Batch advanced initialization with multiple strategies
        // Thread-safe batch processing with error handling
        
        auto startTime = std::chrono::high_resolution_clock::now();
        uint64_t processedCount = 0;
        uint64_t errors = 0;
        
        const BatchProcessingConfig& config = batchConfig;
        
        try {
            if (config.enableParallel && neurons.size() > 1) {
                // Parallel processing version
                std::vector<std::future<void>> futures;
                futures.reserve(neurons.size());
                
                for (Neuron* neuron : neurons) {
                    futures.push_back(std::async(std::launch::async, [neuron, &rng, strategy]() {
                        switch (strategy) {
                            case InitializationStrategy::Random:
                                neuron->initializeRandom(rng);
                                break;
                            case InitializationStrategy::TypeSpecific:
                                neuron->initializeBiologically(neuron, static_cast<NeuronType>(rng.uniformInt(0, 4)), rng);
                                break;
                            case InitializationStrategy::DevelopmentStage:
                                neuron->initializeByDevelopmentStage(neuron, static_cast<DevelopmentalStage>(rng.uniformInt(0, 4)), rng);
                                break;
                            case InitializationStrategy::PopulationSpecific:
                                neuron->initializeForPopulation(neuron, PopulationId(rng.uniformInt(0, 100)), rng);
                                break;
                            case InitializationStrategy::SynapticContext:
                                neuron->initializeWithSynapticContext(neuron, rng.uniformInt(0, 200), rng);
                                break;
                            case InitializationStrategy::EnergyBudget:
                                neuron->initializeWithEnergyBudget(neuron, rng.uniformReal(1.0f, 20.0f), rng);
                                break;
                            case InitializationStrategy::TemplateBased:
                            { // Use a default template
                                NeuronTemplate template;
                                neuron->initializeFromTemplate(neuron, template, rng);
                            }
                                break;
                            case InitializationStrategy::ActivityDependent:
                                neuron->initializeRandom(rng);
                                break;
                            case InitializationStrategy::AgeDependent:
                                neuron->initializeByDevelopmentStage(neuron, static_cast<DevelopmentalStage>(rng.uniformInt(0, 4)), rng);
                                break;
                        }
                        
                        // Validate after initialization
                        if (!neuron->validateInitializedParameters(neuron)) {
                            throw std::runtime_error("Parameter validation failed during initialization");
                        }
                    }));
                }
                
                for (auto& future : futures) {
                    try {
                        future.wait();
                        processedCount++;
                    } catch (...) {
                        errors++;
                    }
                }
                batchProcessor.getAccumulatedMetrics().parallelProcessed += processedCount;
            } else {
                // Synchronous processing version
                for (Neuron* neuron : neurons) {
                    try {
                        switch (strategy) {
                            case InitializationStrategy::Random:
                                neuron->initializeRandom(rng);
                                break;
                            case InitializationStrategy::TypeSpecific:
                                neuron->initializeBiologically(neuron, static_cast<NeuronType>(rng.uniformInt(0, 4)), rng);
                                break;
                            case InitializationStrategy::DevelopmentStage:
                                neuron->initializeByDevelopmentStage(neuron, static_cast<DevelopmentalStage>(rng.uniformInt(0, 4)), rng);
                                break;
                            case InitializationStrategy::PopulationSpecific:
                                neuron->initializeForPopulation(neuron, PopulationId(rng.uniformInt(0, 100)), rng);
                                break;
                            case InitializationStrategy::SynapticContext:
                                neuron->initializeWithSynapticContext(neuron, rng.uniformInt(0, 200), rng);
                                break;
                            case InitializationStrategy::EnergyBudget:
                                neuron->initializeWithEnergyBudget(neuron, rng.uniformReal(1.0f, 20.0f), rng);
                                break;
                            case InitializationStrategy::TemplateBased:
                            { // Use a default template
                                NeuronTemplate template;
                                neuron->initializeFromTemplate(neuron, template, rng);
                            }
                                break;
                            case InitializationStrategy::ActivityDependent:
                                neuron->initializeRandom(rng);
                                break;
                            case InitializationStrategy::AgeDependent:
                                neuron->initializeByDevelopmentStage(neuron, static_cast<DevelopmentalStage>(rng.uniformInt(0, 4)), rng);
                                break;
                        }
                        
                        // Validate after initialization
                        if (!neuron->validateInitializedParameters(neuron)) {
                            throw std::runtime_error("Parameter validation failed during initialization");
                        }
                        
                        processedCount++;
                    } catch (...) {
                        errors++;
                    }
                }
            }
        } catch (...) {
            errors++;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        double stepTime = (processedCount > 0) ? (duration / static_cast<double>(processedCount)) : 0.0;
        
        BatchProcessingMetrics metrics;
        metrics.neuronsProcessed = processedCount;
        metrics.totalSteps = processedCount;
        metrics.errorCount = errors;
        
        batchProcessor.recordOperation(metrics);
    }

    // Static member definitions
    std::unordered_map<PopulationId, PopulationStats> Neuron::populationStats = std::unordered_map<PopulationId, PopulationStats>();
    std::mutex Neuron::populationStatsMutex;

    PopulationStats Neuron::getPopulationStats(PopulationId population) {
        std::lock_guard<std::mutex> lock(populationStatsMutex);
        if (populationStats.find(population) == populationStats.end()) {
            // Initialize with default statistics
            PopulationStats stats;
            stats.avgRestingPotential = -70.0f;
            stats.avgThreshold = -55.0f;
            stats.avgResetPotential = -70.0f;
            stats.stdDevRestingPotential = 2.0f;
            stats.stdDevThreshold = 2.0f;
            stats.stdDevResetPotential = 2.0f;
            stats.firingRate = 10.0f;
            stats.energyConsumption = 1.0f;
            stats.synapticDensity = 1.0f;
            return stats;
        }
        return populationStats[population];
    }

    void Neuron::setPopulationStats(PopulationId population, const PopulationStats& stats) {
        std::lock_guard<std::mutex> lock(populationStatsMutex);
        populationStats[population] = stats;
    }

} // namespace nlm