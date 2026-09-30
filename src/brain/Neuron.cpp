#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Neuron constants class for neuron parameters
class NeuronConstants {
public:
    // Membrane dynamics parameters
    static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;       // ms
    static constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100;
    
    // LIF neuron parameters
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float DEFAULT_THRESHOLD = -55.0f;         // mV
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;    // mV
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;    // nS
    static constexpr float DEFAULT_MAX_MEMBRANE_POTENTIAL = 50.0f;   // mV
    static constexpr float DEFAULT_MIN_MEMBRANE_POTENTIAL = -100.0f; // mV
    
    // Spike-frequency adaptation
    static constexpr float DEFAULT_ADAPTATION_DECAY_RATE = 0.95f;
    static constexpr float DEFAULT_ADAPTATION_SPIKE_INCREMENT = 1.0f;
    static constexpr float DEFAULT_ADAPTATION_COUPLING = 0.01f;
    
    // Refractory period
    static constexpr uint32_t DEFAULT_MIN_REFRACTORY_PERIOD = 2;
    static constexpr uint32_t DEFAULT_MAX_REFRACTORY_PERIOD = 10;
    
    // Input scaling
    static constexpr float DEFAULT_EXCITATORY_SCALE = 1.0f;
    static constexpr float DEFAULT_INHIBITORY_SCALE = 1.0f;
    static constexpr float DEFAULT_MODULATORY_SCALE = 0.1f;
    static constexpr float DEFAULT_CURRENT_SCALE = 1.0f;
    
    // Randomization ranges for initialization
    static constexpr float DEFAULT_MEMBRANE_POTENTIAL_RANGE = 6.0f;
    static constexpr float DEFAULT_THRESHOLD_RANGE = 4.0f;
    static constexpr float DEFAULT_RESTING_POTENTIAL_RANGE = 4.0f;
    static constexpr float DEFAULT_RESET_POTENTIAL_RANGE = 5.0f;
};

// Neuron configuration structure for fine-tuneable parameters
struct NeuronConfig {
    float membraneCapacitance = NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE;
    float timeConstant = NeuronConstants::DEFAULT_TIME_CONSTANT;
    size_t maxSpikeHistory = NeuronConstants::DEFAULT_MAX_SPIKE_HISTORY;
    
    float restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL;
    float threshold = NeuronConstants::DEFAULT_THRESHOLD;
    float resetPotential = NeuronConstants::DEFAULT_RESET_POTENTIAL;
    float leakConductance = NeuronConstants::DEFAULT_LEAK_CONDUCTANCE;
    float maxMembranePotential = NeuronConstants::DEFAULT_MAX_MEMBRANE_POTENTIAL;
    float minMembranePotential = NeuronConstants::DEFAULT_MIN_MEMBRANE_POTENTIAL;
    
    float adaptationDecayRate = NeuronConstants::DEFAULT_ADAPTATION_DECAY_RATE;
    float adaptationSpikeIncrement = NeuronConstants::DEFAULT_ADAPTATION_SPIKE_INCREMENT;
    float adaptationCoupling = NeuronConstants::DEFAULT_ADAPTATION_COUPLING;
    
    uint32_t minRefractoryPeriod = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
    uint32_t maxRefractoryPeriod = NeuronConstants::DEFAULT_MAX_REFRACTORY_PERIOD;
};

// Helper function to clamp membrane potential to safe bounds
MembranePotential clampMembranePotential(MembranePotential v) {
    return std::clamp(v, NeuronConstants::DEFAULT_MIN_MEMBRANE_POTENTIAL, 
                      NeuronConstants::DEFAULT_MAX_MEMBRANE_POTENTIAL);
}

// Helper function to initialize random parameters for a neuron
void initializeNeuronRandomParams(NeuronState& state, RandomGenerator& rng) {
    // Membrane potential starts near resting potential
    state.membranePotential = state.restingPotential + 
        rng.uniformReal(-NeuronConstants::DEFAULT_MEMBRANE_POTENTIAL_RANGE, 
                        NeuronConstants::DEFAULT_MEMBRANE_POTENTIAL_RANGE);
    
    // Threshold is typically -55mV with small variation
    state.threshold = NeuronConstants::DEFAULT_THRESHOLD + 
        rng.uniformReal(-NeuronConstants::DEFAULT_THRESHOLD_RANGE, 
                        NeuronConstants::DEFAULT_THRESHOLD_RANGE);
    
    // Resting potential typically -70mV
    state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL + 
        rng.uniformReal(-NeuronConstants::DEFAULT_RESTING_POTENTIAL_RANGE, 
                        NeuronConstants::DEFAULT_RESTING_POTENTIAL_RANGE);
    
    // Reset potential is usually close to resting
    state.resetPotential = state.restingPotential + 
        rng.uniformReal(-NeuronConstants::DEFAULT_RESET_POTENTIAL_RANGE, 
                        NeuronConstants::DEFAULT_RESET_POTENTIAL_RANGE);
    
    // Refractory period: 2-10ms typical
    state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(
        NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD, 
        NeuronConstants::DEFAULT_MAX_REFRACTORY_PERIOD
    ));
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
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold: must be above resting potential for stability
    if (threshold < -150.0f) {
        NLM_LOG_WARN("Neuron threshold too low ({}), clamping to -150.0", threshold);
        pImpl->state.threshold = -150.0f;
    } else if (threshold > 100.0f) {
        NLM_LOG_WARN("Neuron threshold too high ({}), clamping to 100.0", threshold);
        pImpl->state.threshold = 100.0f;
    } else {
        pImpl->state.threshold = threshold;
    }
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
    // Validate refractory period: must be within reasonable bounds
    if (steps < NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD) {
        NLM_LOG_WARN("Refractory period too short ({}), clamping to minimum", steps);
        pImpl->state.refractoryPeriod = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
    } else if (steps > 1000) {  // Arbitrary upper limit for sanity
        NLM_LOG_WARN("Refractory period too long ({}), clamping to 1000", steps);
        pImpl->state.refractoryPeriod = 1000;
    } else {
        pImpl->state.refractoryPeriod = steps;
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

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    if (rate < 0.0f) {
        NLM_LOG_WARN("Firing rate negative ({}), setting to 0.0", rate);
        pImpl->state.firingRate = 0.0f;
    } else {
        pImpl->state.firingRate = rate;
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f) {
        NLM_LOG_WARN("Leak conductance negative ({}), setting to 0.0", conductance);
        pImpl->state.leakConductance = 0.0f;
    } else {
        pImpl->state.leakConductance = conductance;
    }
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

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    if (amplitude < -100.0f) {
        NLM_LOG_WARN("Excitatory input too low ({}), clamping to -100.0", amplitude);
        pImpl->synapticInput += -100.0f;
    } else if (amplitude > 100.0f) {
        NLM_LOG_WARN("Excitatory input too high ({}), clamping to 100.0", amplitude);
        pImpl->synapticInput += 100.0f;
    } else {
        pImpl->synapticInput += amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    if (amplitude < -100.0f) {
        NLM_LOG_WARN("Inhibitory input too low ({}), clamping to -100.0", amplitude);
        pImpl->synapticInput -= -100.0f;
    } else if (amplitude > 100.0f) {
        NLM_LOG_WARN("Inhibitory input too high ({}), clamping to 100.0", amplitude);
        pImpl->synapticInput -= 100.0f;
    } else {
        pImpl->synapticInput -= amplitude;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * NeuronConstants::DEFAULT_MODULATORY_SCALE;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current * NeuronConstants::DEFAULT_CURRENT_SCALE;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > NeuronConstants::DEFAULT_MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
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
    float tau = NeuronConstants::DEFAULT_TIME_CONSTANT;  // ms
    float C = NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * NeuronConstants::DEFAULT_ADAPTATION_COUPLING;
        pImpl->state.adaptationVariable *= NeuronConstants::DEFAULT_ADAPTATION_DECAY_RATE;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = clampMembranePotential(V);
    
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
        pImpl->state.adaptationVariable += NeuronConstants::DEFAULT_ADAPTATION_SPIKE_INCREMENT;
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
    // Better abstraction: Use configuration structure for reset
    // Reset to resting state with default values
    pImpl->state.membranePotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL;
    pImpl->state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL;
    pImpl->state.threshold = NeuronConstants::DEFAULT_THRESHOLD;
    pImpl->state.resetPotential = NeuronConstants::DEFAULT_RESET_POTENTIAL;
    pImpl->state.leakConductance = NeuronConstants::DEFAULT_LEAK_CONDUCTANCE;
    pImpl->state.firingRate = 0.0f;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.refractoryPeriod = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Better abstraction: Use configuration structure and helper functions
    // Initialize neuron with biologically realistic parameters
    initializeNeuronRandomParams(pImpl->state, rng);
    
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