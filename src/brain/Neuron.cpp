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
    // This is the biological mechanism where neurotransmitters cause ion channels to open
    // Excitatory neurotransmitters like glutamate depolarize the neuron, bringing it closer to threshold
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron, making firing harder
    // This represents the action of inhibitory neurotransmitters like GABA
    // Inhibitory inputs increase the distance between membrane potential and firing threshold
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
    // This simulates external current sources that bypass synapses
    // In biological terms, this represents direct stimulation of the neuron membrane
    // by external factors like sensory input or artificial stimulation
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    // Reset synaptic input accumulator for next simulation step
    // This ensures that synaptic inputs are only integrated for one timestep
    // Preventing temporal summation across multiple steps for more accurate dynamics
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Record spike timestamp for plasticity calculations and history
    // STDP and other plasticity rules depend on precise spike timing
    // This is crucial for spike-timing dependent plasticity mechanisms
    // that learn correlations between pre- and post-synaptic neurons
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Maintain bounded spike history to prevent memory overflow
        // and ensure efficient access for plasticity calculations
        // The limit balances plasticity learning capacity with memory usage
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    // Clear all recorded spike times
    // Used when resetting neuron state or starting new experiment
    // This ensures plasticity calculations start from clean state
    // Important for maintaining reproducibility between experiments
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
    
    // LIF dynamics: Leaky Integrate-and-Fire neural model
    // This implements the fundamental biological neuron model used throughout NLM
    // Equation: dV/dt = (V_rest - V)/tau + I/C
    // Where: V = membrane potential, V_rest = resting potential, tau = time constant, I = input current, C = capacitance
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    // The factor of 1000.0f converts from seconds to milliseconds for consistency
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // Membrane time constant in milliseconds (20ms biological)
    float C = Impl::MEMBRANE_CAPACITANCE;  // Membrane capacitance in nanoFarads (1nF biological)
    
    // Calculate synaptic contribution to membrane potential change
    // I/C converts current to voltage change across the membrane capacitance
    float synapticContribution = pImpl->synapticInput / C;
    
    // Calculate leak contribution: the natural tendency of the membrane to return to resting potential
    // This represents the passive properties of the neuron membrane
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // This is a numerical method for solving the differential equation
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike (membrane potential exceeds threshold)
    if (V >= threshold) {
        // Neuron has fired: set firing state and record spike time
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for plasticity and history tracking
        recordSpike(currentTime);
        
        // Reset membrane potential to reset potential after spike
        // This simulates the refractory period dynamics
        V = V_reset;
        
        // Enter refractory period: prevent immediate re-firing
        // Refractory period is biologically realistic (typically 2-10ms)
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation variable for spike-frequency adaptation
        // This implements the biological phenomenon where neurons fire less frequently over time
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // No spike: maintain active state but no reset
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input accumulator for next simulation step
    // This ensures that synaptic inputs are only integrated for one timestep
    // preventing temporal summation across multiple steps
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    // This provides a simple interface for standard simulation with 1ms time steps
    TimestepDuration dt = 0.001;  // 1ms default timestep
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset neuron to initial state
    // This clears all internal state variables to prepare for new simulation
    // Used when restarting simulation or between trials
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // This follows established neurobiological principles for neuron parameter distributions
    // 
    // Membrane potential starts near resting potential with small biological noise
    // Neurons don't start at extreme potentials due to homeostasis and developmental processes
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold typically around -55mV with biological variation
    // This represents the voltage at which voltage-gated sodium channels open
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically around -70mV with biological variation
    // This represents the equilibrium potential of the neuron membrane
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential usually close to resting potential
    // This simulates the voltage after an action potential, set by potassium channel dynamics
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical for biological neurons
    // This represents the time needed for sodium channels to recover after firing
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state: neuron starts at rest with no recent activity
    pImpl->state.firingState = FiringState::Resting;           // Not currently firing
    pImpl->state.refractoryRemaining = 0;                       // Not in refractory period
    pImpl->state.adaptationVariable = 0.0f;                     // No adaptation buildup
    pImpl->state.lastSpikeTime = -1.0f;                         // No previous spike recorded
    
    // Clear any residual state to ensure clean initialization
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
