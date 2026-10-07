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
    
    // LIF parameters - make them constexpr for better optimization
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Constructor with initialization list for efficiency
    Impl() noexcept : id(), type(NeuronType::Internal), regionId(), populationId(),
                     totalCurrent(0.0f), synapticInput(0.0f), spikeHistory(),
                     incomingSynapses(), outgoingSynapses(), plasticityFlags() {
        // Initialize vectors with capacity to prevent reallocations
        spikeHistory.reserve(MAX_SPIKE_HISTORY);
        incomingSynapses.reserve(100);  // Typical synapse count
        outgoingSynapses.reserve(100);
    }
};

Neuron::Neuron(NeuronId id) : pImpl(std::make_unique<Impl>()) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(std::move(other.pImpl)) {
    // Ensure we don't have double-free scenarios
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
    if (pImpl) {
        pImpl->type = type;
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (pImpl) {
        pImpl->state.membranePotential += delta;
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (pImpl) {
        pImpl->state.threshold = threshold;
    }
}

void Neuron::setFiringState(FiringState state) {
    if (pImpl) {
        pImpl->state.firingState = state;
    }
}

void Neuron::decrementRefractory() {
    if (!pImpl) return;
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (pImpl) {
        pImpl->state.firingRate = rate;
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (pImpl) {
        pImpl->state.leakConductance = conductance;
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl ? pImpl->state.leakConductance : 0.0f;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (pImpl) {
        pImpl->state.refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl ? pImpl->state.refractoryPeriod : 0;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl ? pImpl->state.restingPotential : 0.0f;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.resetPotential = potential;
    }
}

bool Neuron::checkThreshold() const {
    return pImpl ? pImpl->state.membranePotential >= pImpl->state.threshold : false;
}

float Neuron::getLastSpikeTime() const {
    return pImpl ? pImpl->state.lastSpikeTime : -1.0f;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) return;
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    if (pImpl) {
        pImpl->synapticInput = 0.0f;
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) return;
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (pImpl) {
        pImpl->spikeHistory.clear();
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->incomingSynapses.push_back(handle);
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->outgoingSynapses.push_back(handle);
    }
}

NeuronState& Neuron::getState() {
    return pImpl ? pImpl->state : *static_cast<NeuronState*>(nullptr); // Should not happen
}

const NeuronState& Neuron::getState() const {
    static const NeuronState emptyState{}; // Empty state for null pImpl
    return pImpl ? pImpl->state : emptyState;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl ? pImpl->plasticityFlags : *static_cast<PlasticityFlags*>(nullptr);
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    static const PlasticityFlags emptyFlags{}; // Empty flags for null pImpl
    return pImpl ? pImpl->plasticityFlags : emptyFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    }
}

void Neuron::setRegionId(RegionId region) {
    if (pImpl) {
        pImpl->regionId = region;
    }
}

RegionId Neuron::getRegionId() const {
    return pImpl ? pImpl->regionId : INVALID_REGION_ID;
}

void Neuron::setPopulationId(PopulationId population) {
    if (pImpl) {
        pImpl->populationId = population;
    }
}

PopulationId Neuron::getPopulationId() const {
    return pImpl ? pImpl->populationId : INVALID_POPULATION_ID;
}

bool Neuron::isFiring() const {
    return pImpl ? pImpl->state.firingState == FiringState::Active : false;
}

bool Neuron::isRefractory() const {
    return pImpl ? pImpl->state.refractoryRemaining > 0 : false;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl ? pImpl->state.membranePotential : 0.0f;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl ? pImpl->state.threshold : 0.0f;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl ? pImpl->state.firingRate : 0.0f;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    if (!pImpl) return false;
    
    // Handle refractory period with early exit for performance
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Pre-compute constants for better performance
    const float V_rest = pImpl->state.restingPotential;
    const float V_reset = pImpl->state.resetPotential;
    const float threshold = pImpl->state.threshold;
    const float tau = Impl::TIME_CONSTANT;  // ms
    const float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    float& V = pImpl->state.membranePotential;
    
    // Optimized LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution (pre-compute factor for efficiency)
    const float leakFactor = 1.0f / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * ((V_rest - V) * leakFactor + synapticContribution);
    
    // Apply spike-frequency adaptation (optimized)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Use more efficient clamping (avoid std::clamp function call overhead)
    if (V > 50.0f) V = 50.0f;
    else if (V < -100.0f) V = -100.0f;
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike (optimized)
        pImpl->spikeHistory.push_back(currentTime);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
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
