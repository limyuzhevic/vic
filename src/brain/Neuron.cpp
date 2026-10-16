#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>

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
    // Set the neuron type (Excitatory, Inhibitory, Sensory, etc.)
    // This determines how the neuron processes and responds to input
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Set the absolute membrane potential of the neuron
    // Directly sets V_m without integration
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Increment membrane potential by the given amount
    // Simulates the effect of synaptic input integration
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Set the firing threshold potential (V_thresh)
    // Neuron fires when V_m reaches this value
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    // Set the firing state of the neuron (Resting, Active, Refractory)
    // Used to track the neuron's current state in the simulation
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    // Decrease the refractory period counter by 1
    // Called each timestep while neuron is in refractory period
    // Automatically transitions back to Resting state when complete
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    // Set the neuron's firing rate in Hz
    // Used for statistics and analysis
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Set the leak conductance parameter (g_L)
    // Controls the strength of the membrane potential decay toward resting potential
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    // Return the leak conductance parameter
    // Higher values mean faster decay toward resting potential
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Set the total refractory period duration in simulation steps
    // Neuron cannot fire during this entire period after a spike
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    // Return the refractory period duration in steps
    // Typical values: 2-10 ms worth of simulation steps
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Set the resting membrane potential (V_rest)
    // The equilibrium potential toward which V_m decays
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    // Return the resting membrane potential
    // Typical biological values: -70mV
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Set the reset potential after a spike (V_reset)
    // Membrane potential is immediately reset to this value after firing
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    // Check if the neuron has reached firing threshold
    // Returns true if V_m >= V_thresh, indicating an imminent spike
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    // Return the timestamp of the last spike
    // Returns -1.0f if the neuron has never spiked
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Add excitatory synaptic input to the neuron
    // Excitatory input increases membrane potential
    // Amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Add inhibitory synaptic input to the neuron
    // Inhibitory input decreases membrane potential (hyperpolarizes)
    // Amplitude represents inhibitory conductance * reversal potential contribution
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Add modulatory input that affects plasticity but not direct integration
    // Used for neuromodulation (dopamine, acetylcholine, etc.)
    // Modulators can enable/disable specific plasticity mechanisms
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Adds current to the neuron's input for LIF integration
    // Different from synaptic input: bypasses synaptic dynamics
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    // Clear all synaptic input accumulated this timestep
    // Called at the end of each timestep after integration
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Record a spike occurrence at the given timestamp
    // Maintains spike history for plasticity and analysis
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > pImpl->state.maxSpikeHistory) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::recordSpikes(const std::vector<Timestamp>& timestamps) {
    for (Timestamp ts : timestamps) {
        recordSpike(ts);
    }
}

bool Neuron::shouldLimitSpikeHistory() const {
    return pImpl->spikeHistory.size() >= pImpl->state.maxSpikeHistory;
}

void Neuron::trimSpikeHistory() {
    if (shouldLimitSpikeHistory()) {
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
    
    calculateLIFDynamics(dt);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    MembranePotential& V = pImpl->state.membranePotential;
    applySpikeFrequencyAdaptation(V);
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (handleSpikeDetection(currentTime)) {
        // Reset membrane potential and prepare for next step
        pImpl->state.membranePotential = V_reset;
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
        fired = true;
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
    
    // Set spike history from config (would be passed or accessed differently)
    // pImpl->state.maxSpikeHistory = config.getOr("neuron.max_spike_history", 100);
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    validateNeuronParameters();
    
    setInitialMembranePotential(rng);
    setInitialThreshold(rng);
    setInitialRestingPotential(rng);
    setInitialResetPotential(rng);
    setInitialRefractoryPeriod(rng);
    setInitialAdaptationState();
    clearResidualState();
}

// Helper method for spike history management
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > pImpl->state.maxSpikeHistory) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::recordSpikes(const std::vector<Timestamp>& timestamps) {
    for (Timestamp ts : timestamps) {
        recordSpike(ts);
    }
}

bool Neuron::shouldLimitSpikeHistory() const {
    return pImpl->spikeHistory.size() >= pImpl->state.maxSpikeHistory;
}

void Neuron::trimSpikeHistory() {
    if (shouldLimitSpikeHistory()) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::calculateLIFDynamics(TimestepDuration dt) {
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    
    // Read configuration values with defaults for backward compatibility
    static const float membraneCapacitance = 1.0f;  // nF - default
    static const float timeConstant = 20.0f;  // ms - default
    
    float C = membraneCapacitance;  // nF
    float tau = timeConstant;  // ms
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
}

bool Neuron::handleSpikeDetection(Timestamp currentTime) {
    MembranePotential V = pImpl->state.membranePotential;
    MembranePotential threshold = pImpl->state.threshold;
    MembranePotential V_reset = pImpl->state.resetPotential;
    
    // Check for spike
    if (V >= threshold) {
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->state.membranePotential = V_reset;
        
        return true;
    } else {
        pImpl->state.firingState = FiringState::Active;
        return false;
    }
}

void Neuron::applySpikeFrequencyAdaptation(MembranePotential& V) {
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
}

void Neuron::validateNeuronParameters() {
    // Validate that neuron parameters are within biological bounds
    // Resting potential should be between -90mV and -50mV
    if (pImpl->state.restingPotential < -90.0f || pImpl->state.restingPotential > -50.0f) {
        pImpl->state.restingPotential = -70.0f;  // Reset to typical value
    }
    
    // Threshold should be above resting potential
    if (pImpl->state.threshold <= pImpl->state.restingPotential) {
        pImpl->state.threshold = pImpl->state.restingPotential + 10.0f;
    }
    
    // Reset potential should be within reasonable range
    if (pImpl->state.resetPotential < -80.0f || pImpl->state.resetPotential > -60.0f) {
        pImpl->state.resetPotential = pImpl->state.restingPotential;
    }
    
    // Refractory period should be positive
    if (pImpl->state.refractoryPeriod == 0) {
        pImpl->state.refractoryPeriod = 5;  // Default 5ms
    }
    
    // Adaptation variable should be non-negative
    if (pImpl->state.adaptationVariable < 0.0f) {
        pImpl->state.adaptationVariable = 0.0f;
    }
    
    // Last spike time should be negative if no spike
    if (pImpl->state.lastSpikeTime >= 0.0f) {
        pImpl->state.lastSpikeTime = -1.0f;
    }
}

void Neuron::setInitialMembranePotential(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
}

void Neuron::setInitialThreshold(RandomGenerator& rng) {
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
}

void Neuron::setInitialRestingPotential(RandomGenerator& rng) {
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
}

void Neuron::setInitialResetPotential(RandomGenerator& rng) {
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
}

void Neuron::setInitialRefractoryPeriod(RandomGenerator& rng) {
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
}

void Neuron::setInitialAdaptationState() {
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
}

void Neuron::clearResidualState() {
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
