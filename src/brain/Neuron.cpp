#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
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
    
    // LIF model parameters (biologically plausible values)
    // Membrane capacitance: ~1 nF for typical cortical neurons
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    
    // Membrane time constant: ~20 ms for typical neurons
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    
    // Maximum spike history size: stores recent spike times for STDP and statistics
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Typical neuron parameter ranges (for random initialization)
    static constexpr float TYPICAL_RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float TYPICAL_THRESHOLD = -55.0f;          // mV
    static constexpr float TYPICAL_RESET_POTENTIAL = -70.0f;    // mV
    static constexpr float TYPICAL_REFACTORY_PERIOD_MS = 5.0f;  // ms
    
    // Membrane potential clamping limits (biological constraints)
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;   // mV (hyperpolarized limit)
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;     // mV (depolarized limit)
    
    // Spike-frequency adaptation parameters
    static constexpr float ADAPTATION_INCREMENT = 1.0f;         // Adaptation increase per spike
    static constexpr float ADAPTATION_DECAY_FACTOR = 0.95f;    // Exponential decay per timestep
    static constexpr float ADAPTATION_EFFECT = 0.01f;          // Hyperpolarization per adaptation unit
    
    // Numerical integration constants
    static constexpr float DT_CONVERSION_FACTOR = 1000.0f;      // Convert ms to seconds for integration
    
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

    bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration timeStep) {
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
    // Mathematical formulation:
    // dV/dt = (V_rest - V)/τ + I/C
    // Where V is membrane potential, V_rest is resting potential, τ is time constant,
    // I is total synaptic current, C is membrane capacitance
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/τ + I/C)

    MembranePotential& membranePotential = pImpl->state.membranePotential;
    MembranePotential restingPotential = pImpl->state.restingPotential;
    MembranePotential resetPotential = pImpl->state.resetPotential;
    MembranePotential firingThreshold = pImpl->state.threshold;
    float timeConstant = Impl::TIME_CONSTANT;  // ms
    float membraneCapacitance = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contribution to membrane potential change
    float synapticInputContribution = pImpl->synapticInput / membraneCapacitance;
    
    // Leak current contribution to membrane potential change
    float leakCurrentContribution = (restingPotential - membranePotential) / timeConstant;
    
    // Update membrane potential using exponential Euler integration
    membranePotential = membranePotential + static_cast<float>(timeStep) * 1000.0f * (leakCurrentContribution + synapticInputContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // Adaptation variable accumulates with each spike and causes gradual hyperpolarization
    if (pImpl->state.adaptationVariable > 0.0f) {
        membranePotential -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Exponential decay of adaptation
    }
    
    // Clamp membrane potential to prevent numerical instability
    // Uses biologically plausible limits for neuronal membranes
    membranePotential = std::clamp(membranePotential, 
                                  Impl::MIN_MEMBRANE_POTENTIAL, 
                                  Impl::MAX_MEMBRANE_POTENTIAL);
    
    // Check for spike
    if (membranePotential >= firingThreshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike in history for STDP and statistics
        recordSpike(currentTime);
        
        // Reset membrane potential to prevent bursting
        membranePotential = resetPotential;
        
        // Enter refractory period to prevent immediate re-firing
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation variable for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Maintain active firing state if neuron is already active but not spiking
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next integration step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration timeStep = 0.001;  // 1ms default
    stepLIF(currentTime, timeStep);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Initialize neuron parameters with biological constraints
    // Membrane potential starts near resting potential (typical variation: ±3mV)
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation (typical: ±2mV)
    pImpl->state.threshold = Impl::TYPICAL_THRESHOLD + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV with biological variation (typical: ±2mV)
    pImpl->state.restingPotential = Impl::TYPICAL_RESTING_POTENTIAL + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential usually close to resting (allows for slight depolarization after spike)
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical for cortical neurons
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state setup
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state for clean initialization
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
