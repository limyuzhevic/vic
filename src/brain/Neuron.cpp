#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "nlm_utilities.hpp"

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
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize plasticity flags
        plasticityFlags = PlasticityFlags();
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Validate input parameters
    if (id.value == UINT64_MAX) {
        NLM_LOG_ERROR("Invalid neuron ID: " + std::to_string(id.value));
        throw std::invalid_argument("Neuron ID cannot be UINT64_MAX");
    }
    
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
        // Clean up existing implementation
        if (pImpl) {
            delete pImpl;
        }
        // Transfer ownership
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate potential range (reasonable biological limits)
    if (potential < -100.0f || potential > 100.0f) {
        NLM_LOG_WARNING("Unusual membrane potential: " + std::to_string(potential) + " mV");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate delta range
    if (std::abs(delta) > 1000.0f) {
        NLM_LOG_WARNING("Large membrane potential delta: " + std::to_string(delta) + " mV");
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold range
    if (threshold < -100.0f || threshold > 100.0f) {
        NLM_LOG_WARNING("Unusual firing threshold: " + std::to_string(threshold) + " mV");
    }
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
    // Validate firing rate
    if (rate < 0.0f || rate > 1000.0f) {
        NLM_LOG_WARNING("Unusual firing rate: " + std::to_string(rate) + " Hz");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Validate leak conductance
    if (conductance < 0.0f || conductance > 100.0f) {
        NLM_LOG_WARNING("Unusual leak conductance: " + std::to_string(conductance) + " nS");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Validate refractory period
    if (steps == 0) {
        NLM_LOG_WARNING("Zero refractory period");
    } else if (steps > 1000) {
        NLM_LOG_WARNING("Unusually long refractory period: " + std::to_string(steps) + " steps");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 100.0f) {
        NLM_LOG_WARNING("Unusual resting potential: " + std::to_string(potential) + " mV");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 100.0f) {
        NLM_LOG_WARNING("Unusual reset potential: " + std::to_string(potential) + " mV");
    }
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

void Neuron::setFiringRate(FiringRate rate) {
    // Validate firing rate
    if (rate < 0.0f || rate > 1000.0f) {
        NLM_LOG_WARNING("Unusual firing rate: " + std::to_string(rate) + " Hz");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Validate leak conductance
    if (conductance < 0.0f || conductance > 100.0f) {
        NLM_LOG_WARNING("Unusual leak conductance: " + std::to_string(conductance) + " nS");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Validate refractory period
    if (steps == 0) {
        NLM_LOG_WARNING("Zero refractory period");
    } else if (steps > 1000) {
        NLM_LOG_WARNING("Unusually long refractory period: " + std::to_string(steps) + " steps");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 100.0f) {
        NLM_LOG_WARNING("Unusual resting potential: " + std::to_string(potential) + " mV");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 100.0f) {
        NLM_LOG_WARNING("Unusual reset potential: " + std::to_string(potential) + " mV");
    }
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
    // Validate excitatory input
    if (amplitude < 0.0f) {
        NLM_LOG_WARNING("Negative excitatory input: " + std::to_string(amplitude) + " mV");
    }
    if (std::abs(amplitude) > 1000.0f) {
        NLM_LOG_WARNING("Large excitatory input: " + std::to_string(amplitude) + " mV");
    }
    
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Validate inhibitory input
    if (amplitude > 0.0f) {
        NLM_LOG_WARNING("Positive inhibitory input: " + std::to_string(amplitude) + " mV");
    }
    if (std::abs(amplitude) > 1000.0f) {
        NLM_LOG_WARNING("Large inhibitory input: " + std::to_string(amplitude) + " mV");
    }
    
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Validate modulatory input
    if (std::abs(amplitude) > 100.0f) {
        NLM_LOG_WARNING("Large modulatory input: " + std::to_string(amplitude) + " mV");
    }
    
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Validate injected current
    if (std::abs(current) > 1000.0f) {
        NLM_LOG_WARNING("Large injected current: " + std::to_string(current) + " mV");
    }
    
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Validate timestamp
    if (timestamp < 0.0) {
        NLM_LOG_WARNING("Negative spike timestamp: " + std::to_string(timestamp));
        return;
    }
    
    // Record the spike
    pImpl->spikeHistory.push_back(timestamp);
    
    // Manage spike history size using utility function
    MemoryUtils::manageSpikeHistory(pImpl->spikeHistory, Impl::MAX_SPIKE_HISTORY);
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
    pImpl->plasticityFlags.structural = true; // Enable structural plasticity by default
    pImpl->plasticityFlags.eligible = true;  // Mark as eligible for plasticity
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Validate time parameters
    if (currentTime < 0.0 || dt <= 0.0) {
        NLM_LOG_ERROR("Invalid time parameters: currentTime=" + std::to_string(currentTime) + 
                     ", dt=" + std::to_string(dt));
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
