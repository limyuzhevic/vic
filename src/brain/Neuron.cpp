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

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period - early exit for optimization
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Optimized LIF dynamics with reduced memory access
    auto& state = pImpl->state;
    auto synapticInput = pImpl->synapticInput;
    
    // Pre-calculate constants and combine contributions
    const float V = state.membranePotential;
    const float V_rest = state.restingPotential;
    const float threshold = state.threshold;
    const float V_reset = state.resetPotential;
    const float tau = Impl::TIME_CONSTANT;
    const float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Combined integration: V_new = V + dt * 1000 * ((V_rest - V)/tau + synapticInput/C)
    // Using Horner's method for better optimization
    const float leakCoefficient = 1000.0f / tau;
    const float inputCoefficient = 1000.0f / C;
    const float totalCoefficient = dt * (leakCoefficient + inputCoefficient);
    
    float newV = V + totalCoefficient * (leakCoefficient * (V_rest - V) + inputCoefficient * synapticInput);
    
    // Apply spike-frequency adaptation in a branchless way
    float adaptationFactor = state.adaptationVariable;
    if (adaptationFactor > 0.0f) {
        newV -= adaptationFactor * 0.01f;
        state.adaptationVariable = adaptationFactor * 0.95f;
    }
    
    // Clamp membrane potential
    newV = std::clamp(newV, -100.0f, 50.0f);
    state.membranePotential = newV;
    
    // Spike detection and handling with optimized state updates
    if (newV >= threshold) {
        fired = true;
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpike(currentTime);
        
        // Reset with optimized state updates
        state.membranePotential = V_reset;
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        state.adaptationVariable += 1.0f;
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Optimized reset with direct struct assignment
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Optimized random initialization with cached values
    auto& state = pImpl->state;
    
    // Generate random values once to reduce RNG calls
    const float restingVariation = rng.uniformReal(-2.0f, 2.0f);
    const float thresholdVariation = rng.uniformReal(-2.0f, 2.0f);
    const float resetVariation = rng.uniformReal(0.0f, 5.0f);
    const float membraneVariation = rng.uniformReal(-3.0f, 3.0f);
    
    // Initialize with biologically plausible defaults
    state.restingPotential = -70.0f + restingVariation;
    state.threshold = -55.0f + thresholdVariation;
    state.resetPotential = state.restingPotential + resetVariation;
    state.membranePotential = state.restingPotential + membraneVariation;
    
    // Other parameters
    state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initialize state with optimized assignments
    state.firingState = FiringState::Resting;
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    state.lastSpikeTime = -1.0f;
    state.leakConductance = 10.0f;  // Default value
    state.synapseConductance = 0.0f;
    state.firingRate = 0.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
