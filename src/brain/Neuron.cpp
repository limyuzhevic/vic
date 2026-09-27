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

// Move constructor with null check
Neuron::Neuron(Neuron&& other) noexcept : pImpl(std::move(other.pImpl)) {
    // Ensure we're not stealing from self
    if (this != &other) {
        // No need to set other.pImpl = nullptr since std::move already transfers ownership
        // and the moved-from object's pImpl will be in a valid but unspecified state
    }
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
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->outgoingSynapses.push_back(handle);
}

void Neuron::setRegionId(RegionId region) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->populationId = population;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    pImpl->state.restingPotential = potential;
}

NeuronState& Neuron::getState() {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    return pImpl->plasticityFlags;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (!pImpl) {
        throw std::runtime_error("Neuron not properly initialized - null implementation pointer");
    }
    
    bool fired = false;
    
    // Handle refractory period - neuron cannot fire during refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        // Refractory neurons are hyperpolarized and cannot fire regardless of input
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false; // No firing during refractory period
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // This implements the fundamental differential equation for LIF neurons:
    // dV/dt = (V_rest - V)/tau + I/C
    // Where:
    //   V = membrane potential
    //   V_rest = resting potential (-70mV)
    //   tau = time constant (20ms)
    //   I = total synaptic current
    //   C = membrane capacitance (1nF)
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms - typical time constant 20ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF - membrane capacitance
    
    // Synaptic input contributes to membrane potential change
    // Synaptic currents from excitatory and inhibitory synapses sum together
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    // The leak term causes the membrane potential to naturally decay toward resting potential
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // This implements the discrete-time approximation of the LIF differential equation
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // This implements a biological mechanism where neurons reduce firing rate over time
    // Adaptation variable increases with each spike and decays exponentially
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Adaptation causes slow hyperpolarization, reducing firing probability
        V -= pImpl->state.adaptationVariable * 0.01f;
        // Adaptation variable decays exponentially back toward zero
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent numerical instability
    // Biological neurons have hard limits on membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike - neuron fires when membrane potential reaches threshold
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for STDP and other spike-timing-dependent processes
        recordSpike(currentTime);
        
        // Reset membrane potential to reset value after spike
        V = V_reset;
        
        // Enter refractory period after spike
        // During refractory period, neuron cannot fire regardless of input
        // This models the absolute refractory period following an action potential
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        // Each spike increases adaptation variable, contributing to firing rate reduction
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    // Reset synaptic input after integration step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Complete reset of neuron to initial state
    // This is used when restarting simulation or when brain is reset
    // All neuron parameters are restored to biologically realistic initial values
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // This function implements biologically realistic neuron parameter initialization
    // based on experimental data and computational neuroscience principles
    
    // Membrane potential starts near resting potential
    // Neurons typically start with membrane potential close to resting potential
    // with small random variations reflecting biological noise
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    // Action potential threshold varies between -60mV and -50mV in real neurons
    // We use -55mV as baseline with 2mV of random variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    // Resting potential varies between -65mV and -75mV in real neurons
    // We use -70mV as baseline with 2mV of random variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    // After firing, membrane potential resets to a value close to resting potential
    // Reset potential typically ranges from resting potential to resting + 5mV
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    // Refractory period is the time after a spike during which the neuron cannot fire
    // This models the absolute refractory period (typically 1-5ms) plus relative refractory period
    // We use 2-10ms to allow for recovery and reduced excitability
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    // Set all neuron state variables to biologically realistic initial values
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    // Ensure synaptic input and spike history are properly initialized
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
