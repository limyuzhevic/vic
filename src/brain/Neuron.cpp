/**
 * @file Neuron.cpp
 * @brief Implementation of Leaky Integrate-and-Fire (LIF) neuron dynamics
 * 
 * This file implements the complete LIF neuron model with biological accuracy
 * including membrane potential integration, spike generation, refractory periods,
 * adaptation, and synaptic input handling.
 * 
 * **Mathematical Implementation**:
 * 
 * 1. **Membrane Potential Integration**:
 * ```
 * τ = C / g_L  (membrane time constant)
 * α = exp(-dt / τ)  (decay factor)
 * V_new = V * α + (V_rest + I/C) * (1 - α)  (membrane update)
 * ```
 * 
 * 2. **Spike Detection and Reset**:
 * ```
 * if V_new ≥ V_th:
 *     spike = true
 *     V_new = V_reset
 *     refractory_remaining = refractory_period
 *     adaptation += adaptation_increment
 * else:
 *     spike = false
 *     adaptation_decay = exp(-dt / τ_adaptation)
 *     V_new = V_new - adaptation * α_adaptation
 * ```
 * 
 * 3. **Refractory Period Dynamics**:
 * ```
 * if refractory_remaining > 0:
 *     refractory_remaining--
 *     adaptation *= (1 - recovery_rate)
 *     g_L = g_L_rest * (1 - recovery_factor)
 * ```
 * 
 * 4. **Adaptation Variables**:
 * ```
 * dA/dt = -A/τ_A + k·spike(t)  (three time constants)
 * ```
 * 
 * **Biological Parameters**:
 * - Membrane capacitance: C = 1.0 nF
 * - Time constant: τ = 20.0 ms
 * - Adaptation time constants: τ_A = 100ms, 1s, 10s
 * - Refractory recovery rate: 10% per timestep
 * - Numerical stability: Clamping to [-100, 60] mV range
 * 
 * @see Neuron.hpp - interface documentation
 * @see ../plasticity/STDP.hpp - spike timing dependent plasticity
 * @see ../brain/Brain.cpp:339 - integration in brain simulation loop
 */

namespace nlm {

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

NeuronId Neuron::getId() const { return pImpl->id; }
NeuronType Neuron::getType() const { return pImpl->type; }
void Neuron::setType(NeuronType type) { pImpl->type = type; }

const NeuronState& Neuron::getState() const { return pImpl->state; }
NeuronState& Neuron::getState() { return pImpl->state; }

MembranePotential Neuron::getMembranePotential() const { return pImpl->state.membranePotential; }
void Neuron::setMembranePotential(MembranePotential potential) { pImpl->state.membranePotential = potential; }
void Neuron::addToMembranePotential(MembranePotential delta) { pImpl->state.membranePotential += delta; }

MembranePotential Neuron::getThreshold() const { return pImpl->state.threshold; }
void Neuron::setThreshold(MembranePotential threshold) { pImpl->state.threshold = threshold; }

bool Neuron::isFiring() const { return pImpl->state.firingState == FiringState::Active; }
bool Neuron::isRefractory() const { return pImpl->state.refractoryRemaining > 0; }
void Neuron::setFiringState(FiringState state) { pImpl->state.firingState = state; }
void Neuron::setRefractoryPeriod(uint32_t steps) { pImpl->state.refractoryPeriod = steps; }
void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const { return pImpl->state.firingRate; }
void Neuron::setFiringRate(FiringRate rate) { pImpl->state.firingRate = rate; }

void Neuron::setLeakConductance(MembranePotential conductance) { pImpl->state.leakConductance = conductance; }
MembranePotential Neuron::getLeakConductance() const { return pImpl->state.leakConductance; }
uint32_t Neuron::getRefractoryPeriod() const { return pImpl->state.refractoryPeriod; }
void Neuron::setRestingPotential(MembranePotential potential) { pImpl->state.restingPotential = potential; }
MembranePotential Neuron::getRestingPotential() const { return pImpl->state.restingPotential; }
void Neuron::setResetPotential(MembranePotential potential) { pImpl->state.resetPotential = potential; }

bool Neuron::checkThreshold() const { return pImpl->state.membranePotential >= pImpl->state.threshold; }
float Neuron::getLastSpikeTime() const { return pImpl->state.lastSpikeTime; }

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) { pImpl->synapticInput += amplitude; }
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) { pImpl->synapticInput -= amplitude; }
void Neuron::receiveModulatoryInput(MembranePotential amplitude) { pImpl->state.adaptationVariable += amplitude * 0.1f; }

void Neuron::injectCurrent(MembranePotential current) { pImpl->synapticInput += current; }
MembranePotential Neuron::getTotalCurrent() const { return pImpl->synapticInput; }
void Neuron::clearTotalCurrent() { pImpl->synapticInput = 0.0f; }

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}
void Neuron::clearSpikeHistory() { pImpl->spikeHistory.clear(); }

void Neuron::addIncomingSynapse(SynapseHandle handle) { pImpl->incomingSynapses.push_back(handle); }
void Neuron::addOutgoingSynapse(SynapseHandle handle) { pImpl->outgoingSynapses.push_back(handle); }
const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const { return pImpl->incomingSynapses; }
const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const { return pImpl->outgoingSynapses; }

const PlasticityFlags& Neuron::getPlasticityFlags() const { return pImpl->plasticityFlags; }
PlasticityFlags& Neuron::getPlasticityFlags() { return pImpl->plasticityFlags; }
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) { pImpl->regionId = region; }
RegionId Neuron::getRegionId() const { return pImpl->regionId; }
void Neuron::setPopulationId(PopulationId population) { pImpl->populationId = population; }
PopulationId Neuron::getPopulationId() const { return pImpl->populationId; }

/**
 * @brief LIF neuron dynamics step function
 * 
 * @details Implements the complete Leaky Integrate-and-Fire neuron model
 * with refractory period handling, adaptation, and type-specific dynamics.
 * This is the core computational method for each neuron simulation step.
 * 
 * **Complete LIF Algorithm**:
 * ```
 * if refractory_remaining > 0:
 *     // Refractory period handling
 *     refractory_remaining--
 *     adaptation *= (1 - recovery_rate)
 *     g_L = g_L_rest * (1 - recovery_factor)
 *     V = clamp(V, V_rest - 30, V_reset)
 *     if refractory_remaining == 0:
 *         firing_state = Resting
 *         g_L = g_L_rest
 *     return false
 * 
 *     // Membrane potential integration
 *     tau_adaptive = tau * (0.8 + 0.2 * (V - V_rest) / threshold)
 *     alpha_leak = exp(-dt * 1000 / tau_adaptive)
 *     V = V * alpha_leak + (V_rest + synapticConductance * C) * (1 - alpha_leak)
 *     
 *     // Adaptation effects
 *     if adaptation > 0:
 *         V -= adaptation * alpha_adaptation
 *         adaptation *= (1 - beta_adaptation)
 *     if adaptation > 0:
 *         V -= adaptation * gamma_adaptation
 *     
 *     // Type-specific clamping
 *     clamp_min = type-specific range
 *     clamp_max = type-specific range
 *     V = clamp(V, clamp_min, clamp_max)
 *     
 *     // Spike detection
 *     if V ≥ threshold:
 *         // Spike generation
 *         firing_state = Active
 *         lastSpikeTime = currentTime
 *         recordSpike(currentTime)
 *         V = V_reset
 *         refractory_remaining = type-specific refractory duration
 *         firing_state = Refractory
 *         adaptation += type-specific_increment
 *         g_L = 20.0
 *     else:
 *         firing_state = Active
 * ```
 * 
 * **Numerical Stability**:
 * - Time step: dt = 0.001s (1ms)
 * - Adaptive time constant prevents numerical instability
 * - Clamping prevents biological unrealistic values
 * - Exponential decay ensures stability
 * 
 * @param currentTime Current simulation time (ms)
 * @param dt Time step duration (seconds)
 * @return true if neuron fired (spike emitted), false otherwise
 * @post Updates membrane potential, firing state, and spike history
 * @see ../brain/Neuron.cpp:225 - wrapper method with fixed dt
 * @see step() - convenience method for standard timestep
 */
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Refractory period handling with biological recovery dynamics
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->state.adaptationVariable *= (1.0f - Impl::REFACTORY_RECOVERY_RATE);
        
        // Gradual leak conductance recovery during refractory period
        float recoveryFactor = static_cast<float>(pImpl->state.refractoryRemaining) / 
                             static_cast<float>(pImpl->state.refractoryPeriod);
        pImpl->state.leakConductance = 10.0f * (1.0f - recoveryFactor * 0.5f);
        
        // Membrane potential clamping during refractory period
        MembranePotential& V = pImpl->state.membranePotential;
        V = std::clamp(V, pImpl->state.restingPotential - 30.0f, pImpl->state.resetPotential);
        
        // Recovery to resting state when refractory period ends
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
            pImpl->state.leakConductance = 10.0f;
        }
        return false;
    }
    
    // Membrane potential integration with adaptive time constant
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Convert synaptic input to conductance and integrate membrane potential
    float synapticConductance = pImpl->synapticInput / C;
    
    // Adaptive time constant based on membrane potential relative to threshold
    float tau_adaptive = tau * (0.8f + 0.2f * (V - V_rest) / threshold);
    float alpha_leak = static_cast<float>(std::exp(-static_cast<double>(dt) * 1000.0 / tau_adaptive));
    
    // Leaky integration with synaptic input
    V = (V * alpha_leak) + ((V_rest + synapticConductance * C) * (1.0f - alpha_leak));
    
    // Spike-frequency adaptation with three time constants
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * Impl::ADAPTATION_ALPHA;
        pImpl->state.adaptationVariable *= (1.0f - Impl::ADAPTATION_BETA);
    }
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * Impl::ADAPTATION_GAMMA;
    }
    
    // Type-specific membrane potential clamping
    float clampMin = Impl::MEMBRANE_CLAMP_MIN;
    float clampMax = Impl::MEMBRANE_CLAMP_MAX;
    
    switch (pImpl->type) {
        case NeuronType::Excitatory: clampMin = -85.0f; clampMax = 55.0f; break;
        case NeuronType::Inhibitory: clampMin = -75.0f; clampMax = 50.0f; break;
        case NeuronType::Modulatory: clampMin = -80.0f; clampMax = 52.0f; break;
        case NeuronType::Internal: clampMin = Impl::MEMBRANE_CLAMP_MIN; clampMax = Impl::MEMBRANE_CLAMP_MAX; break;
    }
    
    V = std::clamp(V, clampMin, clampMax);
    
    // Spike detection and generation
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpike(currentTime);
        
        // Reset membrane potential to reset value
        V = V_reset;
        
        // Set refractory period based on neuron type
        if (pImpl->type == NeuronType::Inhibitory) {
            pImpl->state.refractoryRemaining = static_cast<uint32_t>(pImpl->state.refractoryPeriod * 0.7f);
        } else if (pImpl->type == NeuronType::Modulatory) {
            pImpl->state.refractoryRemaining = static_cast<uint32_t>(pImpl->state.refractoryPeriod * 1.5f);
        } else {
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        }
        pImpl->state.firingState = FiringState::Refractory;
        
        // Adaptation variable increment based on neuron type
        float adaptationIncrement = 1.0f;
        switch (pImpl->type) {
            case NeuronType::Excitatory: adaptationIncrement = 0.8f; break;
            case NeuronType::Inhibitory: adaptationIncrement = 1.2f; break;
            case NeuronType::Modulatory: adaptationIncrement = 1.5f; break;
            default: adaptationIncrement = 1.0f;
        }
        pImpl->state.adaptationVariable += adaptationIncrement;
        
        // Increase leak conductance after spike (post-spike dynamics)
        pImpl->state.leakConductance = 20.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input buffer for next step
    pImpl->synapticInput = 0.0f;
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    if (pImpl->type == NeuronType::Excitatory) {
        pImpl->state.leakConductance = 8.0f + rng.uniformReal(-1.0f, 3.0f);
    } else if (pImpl->type == NeuronType::Inhibitory) {
        pImpl->state.leakConductance = 15.0f + rng.uniformReal(-2.0f, 4.0f);
    } else {
        pImpl->state.leakConductance = 10.0f + rng.uniformReal(-1.5f, 2.5f);
    }
    
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
