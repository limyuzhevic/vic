#include "BasePimpl.hpp"
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
    pImpl->get()->id = id;
    pImpl->get()->type = NeuronType::Internal;
    pImpl->get()->regionId = INVALID_REGION_ID;
    pImpl->get()->populationId = INVALID_POPULATION_ID;
    pImpl->get()->totalCurrent = 0.0f;
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
    pImpl->get()->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->get()->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->get()->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->get()->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->get()->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->get()->state.refractoryRemaining > 0) {
        --pImpl->get()->state.refractoryRemaining;
        if (pImpl->get()->state.refractoryRemaining == 0) {
            pImpl->get()->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->get()->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->get()->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->get()->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->get()->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->get()->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->get()->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->get()->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->get()->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->get()->state.membranePotential >= pImpl->get()->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->get()->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->get()->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->get()->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->get()->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->get()->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->get()->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->get()->spikeHistory.push_back(timestamp);
    if (pImpl->get()->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->get()->spikeHistory.erase(pImpl->get()->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->get()->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->get()->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->get()->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->get()->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->get()->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->get()->plasticityFlags.hebbian = hebbian;
    pImpl->get()->plasticityFlags.stdp = stdp;
    pImpl->get()->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->get()->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->get()->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    uint32_t& refractoryRemaining = pImpl->get()->state.refractoryRemaining;
    if (refractoryRemaining > 0) {
        --refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->get()->synapticInput = 0.0f;
        if (refractoryRemaining == 0) {
            pImpl->get()->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    NeuronState& state = pImpl->get()->state;
    float& V = state.membranePotential;
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->get()->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    float& adaptationVar = state.adaptationVariable;
    if (adaptationVar > 0.0f) {
        V -= adaptationVar * 0.01f;
        adaptationVar *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        adaptationVar += 1.0f;
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->get()->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->get()->state = NeuronState();
    pImpl->get()->synapticInput = 0.0f;
    pImpl->get()->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->get()->state.membranePotential = pImpl->get()->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->get()->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->get()->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->get()->state.resetPotential = pImpl->get()->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->get()->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->get()->state.firingState = FiringState::Resting;
    pImpl->get()->state.refractoryRemaining = 0;
    pImpl->get()->state.adaptationVariable = 0.0f;
    pImpl->get()->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->get()->synapticInput = 0.0f;
    pImpl->get()->spikeHistory.clear();
}

} // namespace nlm
