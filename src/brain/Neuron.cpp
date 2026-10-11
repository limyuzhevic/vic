#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <system_error>

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
    
    // Validation bounds
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_THRESHOLD = -70.0f;
    static constexpr float MAX_THRESHOLD = -40.0f;
    static constexpr float MIN_RESTING_POTENTIAL = -80.0f;
    static constexpr float MAX_RESTING_POTENTIAL = -60.0f;
    static constexpr float MIN_LEAK_CONDUCTANCE = 1.0f;
    static constexpr float MAX_LEAK_CONDUCTANCE = 100.0f;
    static constexpr uint32_t MIN_REFECTORY_PERIOD = 0;
    static constexpr uint32_t MAX_REFECTORY_PERIOD = 1000;
    
    // Error recovery
    mutable bool healthValid = false;
    mutable std::vector<std::string> healthIssues;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
    
    // Validation helpers
    bool validateMembranePotential() const {
        return state.membranePotential >= MIN_MEMBRANE_POTENTIAL &&
               state.membranePotential <= MAX_MEMBRANE_POTENTIAL;
    }
    
    bool validateThreshold() const {
        return state.threshold >= MIN_THRESHOLD &&
               state.threshold <= MAX_THRESHOLD;
    }
    
    bool validateRestingPotential() const {
        return state.restingPotential >= MIN_RESTING_POTENTIAL &&
               state.restingPotential <= MAX_RESTING_POTENTIAL;
    }
    
    bool validateLeakConductance() const {
        return state.leakConductance >= MIN_LEAK_CONDUCTANCE &&
               state.leakConductance <= MAX_LEAK_CONDUCTANCE;
    }
    
    bool validateRefractoryPeriod() const {
        return state.refractoryPeriod >= MIN_REFECTORY_PERIOD &&
               state.refractoryPeriod <= MAX_REFECTORY_PERIOD;
    }
    
    bool validateRefractoryRemaining() const {
        return state.refractoryRemaining <= state.refractoryPeriod;
    }
    
    void updateHealth() const {
        healthValid = false;
        healthIssues.clear();
        
        if (!validateMembranePotential()) {
            healthIssues.push_back("Membrane potential out of bounds: " + 
                                std::to_string(state.membranePotential) + " mV");
        }
        
        if (!validateThreshold()) {
            healthIssues.push_back("Threshold out of bounds: " + 
                                std::to_string(state.threshold) + " mV");
        }
        
        if (!validateRestingPotential()) {
            healthIssues.push_back("Resting potential out of bounds: " + 
                                std::to_string(state.restingPotential) + " mV");
        }
        
        if (!validateLeakConductance()) {
            healthIssues.push_back("Leak conductance out of bounds: " + 
                                std::to_string(state.leakConductance) + " nS");
        }
        
        if (!validateRefractoryPeriod()) {
            healthIssues.push_back("Refractory period out of bounds: " + 
                                std::to_string(state.refractoryPeriod) + " steps");
        }
        
        if (!validateRefractoryRemaining()) {
            healthIssues.push_back("Refractory remaining exceeds refractory period");
        }
        
        if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
            healthIssues.push_back("Spike history exceeds maximum size: " + 
                                std::to_string(spikeHistory.size()) + " > " + 
                                std::to_string(MAX_SPIKE_HISTORY));
        }
        
        healthValid = healthIssues.empty();
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->healthValid = false;
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
    try {
        NLM_VALIDATE(static_cast<int>(type) >= 0 && static_cast<int>(type) <= static_cast<int>(NeuronType::Internal),
                   "Invalid neuron type: " + std::to_string(static_cast<int>(type)));
        pImpl->type = type;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting neuron type: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    try {
        NLM_VALIDATE(validateInput(potential),
                   "Membrane potential out of valid range: " + std::to_string(potential) + " mV");
        pImpl->state.membranePotential = potential;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting membrane potential: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    try {
        NLM_VALIDATE(validateInput(delta),
                   "Invalid membrane potential delta: " + std::to_string(delta));
        pImpl->state.membranePotential += delta;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error adjusting membrane potential: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    try {
        NLM_VALIDATE(validateThreshold(threshold),
                   "Threshold out of valid range: " + std::to_string(threshold) + " mV");
        pImpl->state.threshold = threshold;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting threshold: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::setFiringState(FiringState state) {
    try {
        NLM_VALIDATE(static_cast<int>(state) >= 0 && static_cast<int>(state) <= static_cast<int>(FiringState::Inhibited),
                   "Invalid firing state: " + std::to_string(static_cast<int>(state)));
        pImpl->state.firingState = state;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting firing state: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        pImpl->healthValid = false;
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    NLM_VALIDATE(validateInput(rate), "Invalid firing rate: " + std::to_string(rate));
    pImpl->state.firingRate = rate;
    pImpl->healthValid = false;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    try {
        NLM_VALIDATE(validateLeakConductance(conductance),
                   "Leak conductance out of valid range: " + std::to_string(conductance) + " nS");
        pImpl->state.leakConductance = conductance;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting leak conductance: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    try {
        NLM_VALIDATE(validateRefractoryPeriod(steps),
                   "Refractory period out of valid range: " + std::to_string(steps) + " steps");
        pImpl->state.refractoryPeriod = steps;
        // Ensure refractoryRemaining doesn't exceed refractoryPeriod
        if (pImpl->state.refractoryRemaining > pImpl->state.refractoryPeriod) {
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        }
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting refractory period: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    try {
        NLM_VALIDATE(validateRestingPotential(potential),
                   "Resting potential out of valid range: " + std::to_string(potential) + " mV");
        pImpl->state.restingPotential = potential;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error setting resting potential: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
    pImpl->healthValid = false;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    try {
        NLM_VALIDATE(validateInput(amplitude),
                   "Invalid excitatory input: " + std::to_string(amplitude));
        pImpl->synapticInput += amplitude;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error receiving excitatory input: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    try {
        NLM_VALIDATE(validateInput(amplitude),
                   "Invalid inhibitory input: " + std::to_string(amplitude));
        pImpl->synapticInput -= amplitude;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error receiving inhibitory input: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    pImpl->healthValid = false;
}

void Neuron::injectCurrent(MembranePotential current) {
    try {
        NLM_VALIDATE(validateInput(current),
                   "Invalid current injection: " + std::to_string(current));
        pImpl->synapticInput += current;
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error injecting current: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
    pImpl->healthValid = false;
}

void Neuron::recordSpike(Timestamp timestamp) {
    try {
        NLM_VALIDATE(validateInput(static_cast<float>(timestamp)),
                   "Invalid spike timestamp: " + std::to_string(timestamp));
        pImpl->spikeHistory.push_back(timestamp);
        pImpl->state.lastSpikeTime = static_cast<float>(timestamp);
        
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error recording spike: " + std::string(e.what()),
                        ErrorCategory::Validation, ErrorSeverity::Warning);
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->healthValid = false;
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
    pImpl->healthValid = false;
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
    pImpl->healthValid = false;
}

NeuronState& Neuron::getState() {
    pImpl->healthValid = false;
    return pImpl->state;
}

const NeuronState& Neuron::getState() const {
    pImpl->updateHealth();
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    pImpl->healthValid = false;
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
    pImpl->healthValid = false;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
    pImpl->healthValid = false;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
    pImpl->healthValid = false;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

// Validation methods implementation
bool Neuron::validateState() const {
    pImpl->updateHealth();
    return pImpl->healthValid;
}

bool Neuron::validateInput(MembranePotential input) const {
    // Check for NaN or infinite values
    if (std::isnan(input) || std::isinf(input)) {
        NLM_REPORT_ERROR("Invalid input: NaN or infinite value detected",
                        ErrorCategory::Validation, ErrorSeverity::Error);
        return false;
    }
    return true;
}

bool Neuron::validateThreshold() const {
    return pImpl->validateThreshold();
}

bool Neuron::validateMembranePotential() const {
    return pImpl->validateMembranePotential();
}

bool Neuron::validateRefractoryPeriod() const {
    return pImpl->validateRefractoryPeriod();
}

std::vector<std::string> Neuron::getValidationIssues() const {
    pImpl->updateHealth();
    return pImpl->healthIssues;
}

// Error recovery methods implementation
void Neuron::attemptRecovery() {
    pImpl->healthValid = false;
    pImpl->healthIssues.clear();
    
    // Reset to safe defaults
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Validate state after recovery
    pImpl->updateHealth();
    
    if (!pImpl->healthValid) {
        NLM_REPORT_ERROR("Neuron recovery completed but state still has issues",
                        ErrorCategory::Simulation, ErrorSeverity::Warning);
    }
}

bool Neuron::isRecoverable() const {
    // Neurons are generally recoverable if not in critical state
    return pImpl->state.firingState != FiringState::Inhibited;
}

// Health checking methods implementation
bool Neuron::isHealthy() const {
    pImpl->updateHealth();
    return pImpl->healthValid;
}

std::vector<std::string> Neuron::getHealthIssues() const {
    pImpl->updateHealth();
    return pImpl->healthIssues;
}

// Step method with error handling
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
    
    // Validate inputs
    if (!validateInput(dt) || !validateInput(currentTime)) {
        NLM_REPORT_ERROR("Invalid inputs for LIF step: dt=" + std::to_string(dt) + ", time=" + std::to_string(currentTime),
                        ErrorCategory::Simulation, ErrorSeverity::Error);
        attemptRecovery();
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
    V = std::clamp(V, Impl::MIN_MEMBRANE_POTENTIAL, Impl::MAX_MEMBRANE_POTENTIAL);
    
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
    try {
        stepLIF(currentTime, dt);
        pImpl->healthValid = false;
    } catch (const NLMException& e) {
        NLM_REPORT_ERROR("Error in neuron step: " + std::string(e.what()),
                        ErrorCategory::Simulation, ErrorSeverity::Error);
        attemptRecovery();
    }
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->healthValid = false;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    try {
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
        
        pImpl->healthValid = false;
    } catch (const std::exception& e) {
        NLM_REPORT_ERROR("Error during neuron random initialization: " + std::string(e.what()),
                        ErrorCategory::Random, ErrorSeverity::Error);
        attemptRecovery();
    }
}

} // namespace nlm
