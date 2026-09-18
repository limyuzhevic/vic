#include "Neuron.hpp"
#include "../core/ErrorHandling.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Validation and Error Handling for Neuron
void Neuron::validate() const {
    const auto& state = pImpl->state;
    
    // Validate membrane potential range
    if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
        throw RangeError("membranePotential", state.membranePotential, -100.0f, 50.0f);
    }
    
    // Validate threshold range
    if (state.threshold < -100.0f || state.threshold > 50.0f) {
        throw RangeError("threshold", state.threshold, -100.0f, 50.0f);
    }
    
    // Validate refractory period
    if (state.refractoryPeriod > 1000) {
        throw RangeError("refractoryPeriod", state.refractoryPeriod, 0.0f, 1000.0f);
    }
    
    // Validate firing rate
    if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
        throw RangeError("firingRate", state.firingRate, 0.0f, 1000.0f);
    }
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
    
    // Biophysical parameters for realistic LIF dynamics
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr float MEMBRANE_RESISTANCE = 10.0f;  // MOhm
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize with biologically plausible default values
        state.restingPotential = -70.0f;
        state.resetPotential = -65.0f;
        state.threshold = -55.0f;
        state.firingRate = 0.0f;
        state.leakConductance = 0.001f;
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
        state.firingState = FiringState::Resting;
        state.refractoryPeriod = 5;
        state.refractoryRemaining = 0;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Validate input parameter
    if (id == INVALID_NEURON_ID) {
        throw ValidationError("NeuronId cannot be INVALID_NEURON_ID");
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
    validate();
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    validate();
    pImpl->state.membranePotential += delta;
    // Check for numerical stability
    if (std::isnan(pImpl->state.membranePotential)) {
        throw ValidationError("Numerical instability: NaN membrane potential");
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    validate();
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
    validate();
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps > 1000) {  // Reasonable upper bound
        throw RangeError("refractoryPeriod", steps, 0.0f, 1000.0f);
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    validate();
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    validate();
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
    pImpl->synapticInput += amplitude;
    // Check for numerical stability
    if (std::isnan(pImpl->synapticInput)) {
        throw ValidationError("Numerical instability: NaN synaptic input");
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    pImpl->synapticInput -= amplitude;
    if (std::isnan(pImpl->synapticInput)) {
        throw ValidationError("Numerical instability: NaN synaptic input");
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    if (std::isnan(pImpl->state.adaptationVariable)) {
        throw ValidationError("Numerical instability: NaN adaptation variable");
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection from sensory input or external source
    pImpl->synapticInput += current;
    if (std::isnan(pImpl->synapticInput)) {
        throw ValidationError("Numerical instability: NaN synaptic input");
    }
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Validate spike time
    if (timestamp < 0.0f) {
        throw ValidationError("Spike timestamp cannot be negative");
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Validate handle
    if (handle == INVALID_SYNAPSE_HANDLE) {
        throw ValidationError("Synapse handle cannot be INVALID_SYNAPSE_HANDLE");
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Validate handle
    if (handle == INVALID_SYNAPSE_HANDLE) {
        throw ValidationError("Synapse handle cannot be INVALID_SYNAPSE_HANDLE");
    }
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    validate();
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
    
    // Enhanced LIF dynamics: Biophysical integrate-and-fire model
    // dV/dt = (V_rest - V)/tau + I/C
    // Using exponential Euler integration for stability
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Apply synaptic currents (I = sum of synaptic conductances * driving force)
    // Use membrane capacitance for current-to-voltage conversion (I = V/C)
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution (restoring force toward resting potential)
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential with adaptive dynamics
    // Incorporate spike-frequency adaptation for biological realism
    float dV = static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation from previous spikes
    if (pImpl->state.adaptationVariable > 0.0f) {
        dV -= pImpl->state.adaptationVariable * 0.005f;
        pImpl->state.adaptationVariable *= 0.98f;  // Slower decay
    }
    
    // Update membrane potential
    V += dV;
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for numerical stability
    if (std::isnan(V) || std::isinf(V)) {
        throw ValidationError("Numerical instability in membrane potential update");
    }
    
    // Implement adaptive threshold dynamics (voltage-dependent threshold)
    if (pImpl->state.adaptationVariable > 0.0f) {
        float adaptiveThreshold = threshold - pImpl->state.adaptationVariable * 0.5f;
        adaptiveThreshold = std::clamp(adaptiveThreshold, -50.0f, -20.0f);
        threshold = adaptiveThreshold;
    }
    
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
    
    // Safety check for negative adaptation variable
    if (pImpl->state.adaptationVariable < 0.0f) {
        pImpl->state.adaptationVariable = 0.0f;
    }
    
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
