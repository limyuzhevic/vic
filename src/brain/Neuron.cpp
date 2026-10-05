#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <sstream>

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

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate input range
    if (potential < -100.0f || potential > 50.0f) {
        throw std::out_of_range("Membrane potential out of valid range: " + std::to_string(potential));
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    setMembranePotential(newPotential);
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold against resting potential
    if (threshold <= pImpl->state.restingPotential) {
        throw std::invalid_argument("Threshold must be greater than resting potential");
    }
    if (threshold < -100.0f || threshold > 50.0f) {
        throw std::out_of_range("Threshold out of valid range: " + std::to_string(threshold));
    }
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps > 1000) { // Reasonable upper limit
        throw std::out_of_range("Refractory period too large: " + std::to_string(steps));
    }
    pImpl->state.refractoryPeriod = steps;
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
    if (rate < 0.0f || rate > 1000.0f) { // Reasonable range for firing rate
        throw std::out_of_range("Firing rate out of valid range: " + std::to_string(rate));
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f || conductance > 100.0f) {
        throw std::out_of_range("Leak conductance out of valid range: " + std::to_string(conductance));
    }
    pImpl->state.leakConductance = conductance;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 50.0f) {
        throw std::out_of_range("Resting potential out of valid range: " + std::to_string(potential));
    }
    pImpl->state.restingPotential = potential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > 50.0f) {
        throw std::out_of_range("Reset potential out of valid range: " + std::to_string(potential));
    }
    pImpl->state.resetPotential = potential;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Excitatory input amplitude must be positive");
    }
    pImpl->state.synapseConductance += amplitude;
    updateStabilityWarnings();
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Inhibitory input amplitude must be positive");
    }
    pImpl->state.synapseConductance -= amplitude;
    updateStabilityWarnings();
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->totalCurrent += current;
    pImpl->state.synapseConductance += current;
    updateStabilityWarnings();
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->state.spikeCount++;
    pImpl->state.lastSpikeTime = static_cast<float>(timestamp);
    
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    pImpl->state.adaptationVariable += 1.0f; // Increase adaptation for spike-frequency adaptation
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

void Neuron::validateState() const {
    if (!pImpl->state.isValid()) {
        throw std::runtime_error("Invalid neuron state detected");
    }
    
    // Check membrane potential bounds
    if (pImpl->state.membranePotential < -100.0f || pImpl->state.membranePotential > 50.0f) {
        throw std::runtime_error("Membrane potential out of bounds: " + std::to_string(pImpl->state.membranePotential));
    }
    
    // Check spike count reasonable
    if (pImpl->state.spikeCount > 1000000) { // Arbitrary large number
        throw std::runtime_error("Spike count exceeds reasonable limit: " + std::to_string(pImpl->state.spikeCount));
    }
}

void Neuron::throwIfError(NeuronError error, const std::string& message) const {
    if (error != NeuronError::INVALID_STATE) {
        throw std::runtime_error("Neuron error: " + message);
    }
}

Neuron::PerformanceMetrics Neuron::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    metrics.spikeCount = pImpl->state.spikeCount;
    metrics.totalSteps = 0; // This would need to be tracked separately
    metrics.refractoryExits = 0; // This would need to be tracked separately
    metrics.avgFiringRate = pImpl->state.firingRate;
    metrics.maxMembranePotential = pImpl->state.membranePotential;
    metrics.minMembranePotential = pImpl->state.membranePotential;
    metrics.instabilityEvents = pImpl->state.instabilityCounter;
    return metrics;
}

void Neuron::updateStabilityWarnings() {
    // Check for numerical stability issues
    if (pImpl->state.membranePotential > 40.0f) {
        pImpl->state.stabilityWarning = true;
        pImpl->state.instabilityCounter++;
    }
    
    if (pImpl->state.adaptationVariable > 10.0f) {
        pImpl->state.stabilityWarning = true;
        pImpl->state.instabilityCounter++;
    }
    
    // Clamp membrane potential to prevent instability
    clampMembranePotential();
}

void Neuron::clampMembranePotential() {
    pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential, -100.0f, 50.0f);
}

float Neuron::calculateMembranePotentialChange(
    const NeuronState& state, 
    MembranePotential synapticInput, 
    TimestepDuration dt) {
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * 1000.0f * ((V_rest - V)/tau + I/C)
    
    MembranePotential V_rest = state.restingPotential;
    MembranePotential V = state.membranePotential;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Calculate change using exponential Euler integration
    float change = dt * 1000.0f * (leakContribution + synapticContribution);
    
    return change;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    validateState(); // Validate before stepping
    
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
    float potentialChange = calculateMembranePotentialChange(pImpl->state, pImpl->synapticInput, dt);
    
    pImpl->state.membranePotential += potentialChange;
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        pImpl->state.membranePotential -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential, -100.0f, 50.0f);
    
    // Check for spike
    if (pImpl->state.membranePotential >= pImpl->state.threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        
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
    validateState(); // Validate before reset
    
    // Reset to initial state
    pImpl->state = NeuronState(); // Use default constructor
    pImpl->state.id = pImpl->id; // Preserve ID
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    clearSpikeHistory();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
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
    pImpl->state.spikeCount = 0;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    clearSpikeHistory();
}

} // namespace nlm
