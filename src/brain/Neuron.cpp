#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include "constants.h"

namespace nlm {

// Neuron implementation

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    float synapticInput;  // Accumulated synaptic input this step (now in mV)
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             synapticInput(0.0f), spikeHistory(), incomingSynapses(), outgoingSynapses(),
             plasticityFlags() {
        // Initialize state with reasonable defaults
        state.membranePotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL;
        state.threshold = NeuronConstants::DEFAULT_THRESHOLD;
        state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL;
        state.resetPotential = NeuronConstants::DEFAULT_RESET_POTENTIAL;
        state.firingRate = 0.0f;
        state.leakConductance = NeuronConstants::DEFAULT_LEAK_CONDUCTANCE;
        state.refractoryRemaining = 0;
        state.refractoryPeriod = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
        state.firingState = FiringState::Resting;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (!id.isValid()) {
        throw std::invalid_argument("Neuron ID must be valid");
    }
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->synapticInput = 0.0f;
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
    // Validate membrane potential bounds for stability
    if (potential < NeuronConstants::MIN_MEMBRANE_POTENTIAL || 
        potential > NeuronConstants::MAX_MEMBRANE_POTENTIAL) {
        NLM_LOG_WARN("Membrane potential " + std::to_string(potential) + 
                    " outside stable bounds [" + 
                    std::to_string(NeuronConstants::MIN_MEMBRANE_POTENTIAL) + ", " +
                    std::to_string(NeuronConstants::MAX_MEMBRANE_POTENTIAL) + "]");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
    
    // Apply bounds checking after modification
    pImpl->state.membranePotential = std::clamp(
        pImpl->state.membranePotential,
        NeuronConstants::MIN_MEMBRANE_POTENTIAL,
        NeuronConstants::MAX_MEMBRANE_POTENTIAL
    );
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (threshold >= pImpl->state.restingPotential) {
        NLM_LOG_WARN("Neuron threshold " + std::to_string(threshold) + " not above resting potential " +
                    std::to_string(pImpl->state.restingPotential));
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
    if (rate < 0.0f) {
        NLM_LOG_WARN("Firing rate cannot be negative: " + std::to_string(rate));
        rate = 0.0f;
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f) {
        NLM_LOG_WARN("Leak conductance cannot be negative: " + std::to_string(conductance));
        conductance = 0.0f;
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps == 0) {
        NLM_LOG_WARN("Refractory period cannot be zero");
        steps = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
    } else if (steps > 100) {
        NLM_LOG_WARN("Refractory period " + std::to_string(steps) + " is unusually long");
    }
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
    // Excitatory synaptic input adds to membrane potential
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Inhibitory synaptic input subtracts from membrane potential
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * NeuronConstants::DEFAULT_ADAPTATION_STRENGTH;
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
    if (timestamp < 0.0f) {
        NLM_LOG_WARN("Recording spike with negative timestamp: " + std::to_string(timestamp));
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > NeuronConstants::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (handle == nullptr) {
        NLM_LOG_WARN("Attempted to add null synapse handle");
        return;
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (handle == nullptr) {
        NLM_LOG_WARN("Attempted to add null synapse handle");
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
    bool fired = false;
    
    // Validate timestep
    if (dt <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep in stepLIF: " + std::to_string(dt));
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
    float tau = NeuronConstants::TIME_CONSTANT;  // ms
    float C = NeuronConstants::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    // Current is converted to equivalent potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * NeuronConstants::DEFAULT_ADAPTATION_STRENGTH;
        pImpl->state.adaptationVariable *= NeuronConstants::DEFAULT_ADAPTATION_DECAY_RATE;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, NeuronConstants::MIN_MEMBRANE_POTENTIAL, NeuronConstants::MAX_MEMBRANE_POTENTIAL);
    
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
        
        // Prevent runaway adaptation
        if (pImpl->state.adaptationVariable > NeuronConstants::MAX_ADAPTATION_VARIABLE) {
            pImpl->state.adaptationVariable = NeuronConstants::MAX_ADAPTATION_VARIABLE;
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
    // Reset to initialized state
    pImpl->state.membranePotential = pImpl->state.restingPotential;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.refractoryPeriod = NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!rng.isSeeded()) {
        NLM_LOG_WARN("Random generator not properly seeded in Neuron::initializeRandom");
    }
    
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential with small variation
    pImpl->state.membranePotential = pImpl->state.restingPotential + 
        rng.uniformReal(-NeuronConstants::RESTING_POTENTIAL_VARIATION, 
                       NeuronConstants::RESTING_POTENTIAL_VARIATION);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = NeuronConstants::DEFAULT_THRESHOLD + 
        rng.uniformReal(-NeuronConstants::THRESHOLD_VARIATION, 
                       NeuronConstants::THRESHOLD_VARIATION);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL + 
        rng.uniformReal(-NeuronConstants::RESTING_POTENTIAL_VARIATION, 
                       NeuronConstants::RESTING_POTENTIAL_VARIATION);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
        rng.uniformReal(0.0f, NeuronConstants::RESET_POTENTIAL_VARIATION);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(
        rng.uniformInt(NeuronConstants::DEFAULT_MIN_REFRACTORY_PERIOD, 
                      NeuronConstants::DEFAULT_MAX_REFRACTORY_PERIOD)
    );
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Set default leak conductance
    pImpl->state.leakConductance = NeuronConstants::DEFAULT_LEAK_CONDUCTANCE;
}

} // namespace nlm