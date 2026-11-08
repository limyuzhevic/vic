#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

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
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF - Membrane capacitance constant
    static constexpr float TIME_CONSTANT = 20.0f;  // ms - Membrane time constant for leak
    static constexpr size_t MAX_SPIKE_HISTORY = 100;  // Maximum number of spike timestamps to store
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Initialize neuron with unique identifier
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;  // Default to internal neuron
    pImpl->regionId = INVALID_REGION_ID;  // Will be set when added to a region
    pImpl->populationId = INVALID_POPULATION_ID;  // Will be set when added to a population
    pImpl->totalCurrent = 0.0f;
    
    // Initialize LIF neuron parameters with biologically plausible defaults
    // Membrane potential starts at resting level
    pImpl->state.membranePotential = -70.0f;  // mV - typical resting potential
    pImpl->state.restingPotential = -70.0f;  // mV
    pImpl->state.resetPotential = -70.0f;   // mV - reset potential after spike
    pImpl->state.threshold = -55.0f;        // mV - firing threshold
    pImpl->state.leakConductance = 1.0f / TIME_CONSTANT;  // mS - conductance for leak current
    pImpl->state.refractoryPeriod = 2;      // ms - refractory period duration
    pImpl->state.refractoryRemaining = 0;   // ms - remaining refractory time
    pImpl->state.firingRate = 0.0f;         // Hz - firing rate metric
    pImpl->state.lastSpikeTime = -1.0f;    // ms - last spike timestamp (-1 indicates no spike yet)
    pImpl->state.firingState = FiringState::Resting;  // Current firing state
    pImpl->state.adaptationVariable = 0.0f; // Arbitrary units - for spike-frequency adaptation
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    // Move constructor for efficient neuron copying
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    // Move assignment operator for efficient neuron copying
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    // Set the neuron type (Sensory, Internal, Motor, Inhibitory)
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Set the current membrane potential (mV)
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Add to the current membrane potential (for synaptic input)
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Set the firing threshold (mV) - neuron fires when V >= threshold
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    // Set the firing state of the neuron
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    // Decrease refractory period by 1 ms (called each simulation step)
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    // Set the firing rate metric (Hz) - exponential moving average of recent spikes
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Set leak conductance (mS) - determines rate of return to resting potential
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    // Get leak conductance (mS)
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Set refractory period duration (in simulation steps)
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    // Get refractory period duration (in simulation steps)
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Set resting potential (mV) - equilibrium potential when no inputs
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    // Get resting potential (mV)
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Set reset potential (mV) - membrane potential after spike
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    // Check if neuron has reached firing threshold
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    // Get last spike time (ms) - returns -1 if no spike has occurred
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Process excitatory synaptic input
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Process inhibitory synaptic input
    // Inhibitory synaptic currents hyperpolarize the neuron (make more negative)
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Process modulatory input (e.g., from neuromodulators like dopamine)
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
    // Clear accumulated synaptic input for next time step
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Record a spike event timestamp
    pImpl->spikeHistory.push_back(timestamp);
    // Maintain maximum spike history to prevent memory overflow
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    // Clear all recorded spike history
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Add a handle to an incoming synapse (for plasticity calculations)
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Add a handle to an outgoing synapse (for spike propagation)
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    // Get reference to neuron state for modification (const correctness consideration)
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    // Get reference to plasticity flags for modification
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    // Enable specific plasticity mechanisms for this neuron
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    // Set the region ID for this neuron
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    // Set the population ID for this neuron
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period - neuron cannot fire during refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // Keep the firing-rate metric decaying even while refractory, so it
        // reflects recent activity instead of freezing at its spike value.
        pImpl->state.firingRate *= 0.9f;
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Where: V = membrane potential, tau = time constant, I = synaptic current, C = capacitance
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution - brings membrane potential toward resting potential
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // Note: dt is in seconds, tau is in ms, so convert
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent numerical instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update the firing-rate metric from counted spikes, in Hz.
    // This is an exponential moving average: rate = 0.9 * rate + contribution from spikes
    // dt is in seconds (0.001 in the demos), so a neuron firing every
    // step would approach 1000 Hz, and refractory periods cap it well
    // below that in practice.
    if (dt > 0.0) {
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
    }
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        if (dt > 0.0) {
            // Each spike contributes 1/dt Hz to the firing rate metric
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike timestamp for plasticity calculations
        recordSpike(currentTime);
        
        // Reset membrane potential to reset value after spike
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
    // Reset neuron state to initial conditions
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential with small variations
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV with small variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(uint32_t(2), uint32_t(10)));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

NeuronId Neuron::getId() const { return pImpl->id; }
NeuronType Neuron::getType() const { return pImpl->type; }
const NeuronState& Neuron::getState() const { return pImpl->state; }
MembranePotential Neuron::getMembranePotential() const { return pImpl->state.membranePotential; }
MembranePotential Neuron::getThreshold() const { return pImpl->state.threshold; }
bool Neuron::isFiring() const { return pImpl->state.firingState == FiringState::Active; }
bool Neuron::isRefractory() const { return pImpl->state.refractoryRemaining > 0; }
FiringRate Neuron::getFiringRate() const { return pImpl->state.firingRate; }
MembranePotential Neuron::getTotalCurrent() const { return pImpl->totalCurrent; }
const std::vector<Timestamp>& Neuron::getSpikeHistory() const { return pImpl->spikeHistory; }
const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const { return pImpl->incomingSynapses; }
const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const { return pImpl->outgoingSynapses; }
const PlasticityFlags& Neuron::getPlasticityFlags() const { return pImpl->plasticityFlags; }
RegionId Neuron::getRegionId() const { return pImpl->regionId; }
PopulationId Neuron::getPopulationId() const { return pImpl->populationId; }

} // namespace nlm
