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

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
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
    
    // Handle refractory period with adaptive refractory period length
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
            // Recover from refractory period by resetting adaptation
            pImpl->state.adaptationVariable = std::max(0.0f, pImpl->state.adaptationVariable - 0.5f);
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire with improved stability
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change with conductance-based model
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution with adaptive time constant (spike-frequency adaptation)
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // Convert dt to milliseconds for consistency with tau
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    V = V + dt_ms * (leakContribution + synapticContribution);
    
    // Enhanced spike-frequency adaptation with slower recovery
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Stronger adaptation effect during high firing rates
        float adaptationFactor = 0.01f * (1.0f + pImpl->state.adaptationVariable / 5.0f);
        V -= pImpl->state.adaptationVariable * adaptationFactor;
        // Faster decay of adaptation
        pImpl->state.adaptationVariable *= 0.9f;
    }
    
    // Improved membrane potential clamping with dynamic bounds
    float lowerBound = V_rest - 50.0f;  // Allow deeper hyperpolarization
    float upperBound = threshold + 20.0f;  // Allow overshoot
    V = std::clamp(V, lowerBound, upperBound);
    
    // Enhanced spike detection with hysteresis to prevent re-firing
    if (V >= threshold && pImpl->state.firingState != FiringState::Refractory) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for firing rate calculation
        recordSpike(currentTime);
        
        // Reset membrane potential to reset potential (not just threshold)
        V = V_reset;
        
        // Enter refractory period - extend refractory period with spike history
        // Higher firing rates lead to longer refractory periods
        float refractoryMultiplier = 1.0f + std::min(1.0f, pImpl->state.adaptationVariable / 3.0f);
        pImpl->state.refractoryRemaining = static_cast<uint32_t>(pImpl->state.refractoryPeriod * refractoryMultiplier);
        pImpl->state.firingState = FiringState::Refractory;
        
        // Enhanced adaptation for spike-frequency adaptation
        // Stronger adaptation with each spike
        pImpl->state.adaptationVariable += 1.0f + pImpl->state.adaptationVariable * 0.1f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Enhanced LIF step with adaptive timestep handling
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    // Reset adaptation variable to zero
    pImpl->state.adaptationVariable = 0.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Improved random initialization with biological constraints and adaptive parameters
    // Membrane potential starts near resting potential with Gaussian noise
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.gaussianReal(0.0f, 3.0f);
    
    // Adaptive threshold: typically -55mV with larger variation for dynamic threshold
    pImpl->state.threshold = -55.0f + rng.uniformReal(-3.0f, 3.0f);
    
    // Resting potential typically -70mV with larger biological variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-3.0f, 3.0f);
    
    // Reset potential is usually close to resting but with biological variation
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 8.0f);
    
    // Adaptive refractory period: 2-20ms with biological variation
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 20));
    
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
