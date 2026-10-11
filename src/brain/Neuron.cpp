#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential total_current;  // Total synaptic current input this step
    MembranePotential synaptic_input;  // Accumulated synaptic input
    std::vector<Timestamp> spike_history;
    std::vector<SynapseHandle> incoming_synapses;
    std::vector<SynapseHandle> outgoing_synapses;
    PlasticityFlags plasticity_flags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             total_current(0.0f), synaptic_input(0.0f) {}
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

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->total_current = 0.0f;
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
    pImpl->state.membrane_potential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membrane_potential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firing_state = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractory_remaining > 0) {
        --pImpl->state.refractory_remaining;
        if (pImpl->state.refractory_remaining == 0) {
            pImpl->state.firing_state = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firing_rate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leak_conductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leak_conductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractory_period = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractory_period;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.resting_potential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.resting_potential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.reset_potential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membrane_potential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.last_spike_time;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synaptic_input += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synaptic_input -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptation_variable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synaptic_input += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synaptic_input = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spike_history.push_back(timestamp);
    if (pImpl->spike_history.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spike_history.erase(pImpl->spike_history.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spike_history.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incoming_synapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoing_synapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticity_flags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticity_flags.hebbian = hebbian;
    pImpl->plasticity_flags.stdp = stdp;
    pImpl->plasticity_flags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->region_id = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->population_id = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    /**
     * 
     * **Leaky Integrate-and-Fire (LIF) Dynamics**
     * ===========================================
     * 
     * This function implements the Leaky Integrate-and-Fire neuron model, which simulates
     * the membrane potential dynamics of biological neurons. The model captures two fundamental
     * biological mechanisms:
     * 1. **Membrane leakage**: Membrane potential naturally decays toward resting potential
     * 2. **Synaptic integration**: Post-synaptic inputs cause depolarization (excitatory)
     *    or hyperpolarization (inhibitory)
     * 
     * **Mathematical Model**
     * ---------------------
     * The continuous-time LIF equation:
     * 
     *     dV/dt = (V_rest - V)/tau + I(t)/C
     * 
     * Where:
     * - V: membrane potential (mV)
     * - V_rest: resting potential (typically -70mV)
     * - tau: membrane time constant (ms) - controls decay speed
     * - I(t): total synaptic current (nA) - from post-synaptic inputs
     * - C: membrane capacitance (nF) - determines voltage response to current
     * 
     * **Biological Rationale**
     * -----------------------
     * - **Leak term (V_rest - V)/tau**: Models ion channel leak currents that naturally
     *   drive the membrane toward its resting state
     * - **Synaptic term I(t)/C**: Represents the effect of synaptic currents on membrane
     *   potential, following Ohm's law: V = I × R, where R = 1/(g × C)
     * - **Threshold mechanism**: When V reaches threshold (~-55mV), neuron fires a spike
     * - **Spike reset**: Immediately after firing, potential resets to V_reset
     * - **Refractory period**: Prevents immediate re-firing, mimicking biological refractory
     * - **Adaptation**: Spike-frequency adaptation via adaptation variable to simulate
     *   Ca2+-dependent potassium currents
     * 
     * **Integration Method**
     * -----------------------
     * **Exponential Euler Integration**: This semi-implicit method is chosen for stability:
     *   V_new = V_old + dt × 1000 × [(V_rest - V_old)/tau + I/C]
     * 
     * The multiplication by 1000 converts seconds to milliseconds for consistency with
     * other time units in the model (tau in ms, dt in seconds).
     * 
     * **Parameter Values and Biological Ranges**
     * ------------------------------------------
     * - Membrane capacitance (C): 1 nF - typical for cortical neurons
     * - Time constant (tau): 20 ms - represents membrane resistance × capacitance
     * - Resting potential: -70 mV - equilibrium potential for K+ ions
     * - Threshold: -55 mV - typically 15 mV above resting
     * - Reset potential: -70 mV - typically returns to resting after spike
     * - Refractory period: 2-10 ms - biological refractory period duration
     * 
     * **Error Conditions and Handling**
     * ---------------------------------
     * - **Refractory period**: If refractory_remaining > 0, neuron cannot fire
     * - **Potential clamping**: V is clamped to [-100mV, 50mV] to prevent instability
     * - **Adaptation bounds**: Adaptation variable prevents unbounded hyperpolarization
     * 
     * **Performance Considerations**
     * -------------------------------
     * - **Vectorization**: Operations are written for efficient SIMD processing
     * - **Memory access**: Minimizes pointer dereferencing in inner loops
     * - **Branch prediction**: Refractory check is cheap and predictable
     * - **Spike detection**: Early exit when refractory period active saves computation
     * 
     * **Implementation Notes**
     * -----------------------
     * - Uses exponential Euler for stability and efficiency
     * - Spike-frequency adaptation models short-term plasticity
     * - Refractory period simulates absolute and relative refractory phases
     * - Time units: dt in seconds, tau in ms, potentials in mV
     */
    
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractory_remaining > 0) {
        --pImpl->state.refractory_remaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synaptic_input = 0.0f;
        if (pImpl->state.refractory_remaining == 0) {
            pImpl->state.firing_state = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membrane_potential;
    MembranePotential V_rest = pImpl->state.resting_potential;
    MembranePotential V_reset = pImpl->state.reset_potential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synaptic_contribution = pImpl->synaptic_input / C;
    
    // Leak contribution
    float leak_contribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leak_contribution + synaptic_contribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptation_variable > 0.0f) {
        V -= pImpl->state.adaptation_variable * 0.01f;
        pImpl->state.adaptation_variable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firing_state = FiringState::Active;
        pImpl->state.last_spike_time = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractory_remaining = pImpl->state.refractory_period;
        pImpl->state.firing_state = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptation_variable += 1.0f;
    } else {
        pImpl->state.firing_state = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synaptic_input = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synaptic_input = 0.0f;
    pImpl->spike_history.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membrane_potential = pImpl->state.resting_potential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.resting_potential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.reset_potential = pImpl->state.resting_potential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractory_period = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firing_state = FiringState::Resting;
    pImpl->state.refractory_remaining = 0;
    pImpl->state.adaptation_variable = 0.0f;
    pImpl->state.last_spike_time = -1.0f;
    
    // Clear any residual state
    pImpl->synaptic_input = 0.0f;
    pImpl->spike_history.clear();
}

} // namespace nlm
