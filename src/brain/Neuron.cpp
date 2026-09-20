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
    
    // LIF dynamics: Leaky Integrate-and-Fire with conductance-based synapses
    // dV/dt = (V_rest - V + I_syn * g_syn)/tau_m + I_noise + I_adaptive
    // Where g_syn = excitatory_conductance - inhibitory_conductance
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    
    // Apply neuromodulation effects
    // Dopamine lowers threshold for excitability
    if (pImpl->state.adaptiveExcitability) {
        float dopamineEffect = pImpl->state.neuromodulationLevel * 0.5f;  // Up to 50% threshold reduction
        threshold -= dopamineEffect;
    }
    
    // Acetylcholine increases gain (noise amplitude) and boosts excitatory conductance
    float acetylcholineEffect = pImpl->state.gainFactor * 1.5f;  // Boost gain
    pImpl->state.noiseAmplitude = 0.1f * acetylcholineEffect;  // Higher noise for exploration
    
    // Add Gaussian noise for stochasticity (biologically realistic)
    float noise = pImpl->state.noiseAmplitude * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
    noise *= 2.0f;  // Scale noise appropriately
    
    // Calculate effective total conductance
    float totalConductance = pImpl->state.excitatoryConductance - pImpl->state.inhibitoryConductance;
    float effectiveConductance = totalConductance * acetylcholineEffect;  // ACh boosts all synaptic efficacy
    
    // Convert conductance to current: I_syn = g_syn * (V - E_syn)
    // For simplicity, use reversal potentials:
    // Excitatory (AMPA): ~0mV, Inhibitory (GABA): ~-70mV
    float excitatoryCurrent = pImpl->state.excitatoryConductance * (0.0f - V);
    float inhibitoryCurrent = pImpl->state.inhibitoryConductance * (-70.0f - V);
    float synapticCurrent = excitatoryCurrent + inhibitoryCurrent;
    
    // Add external input (current injection)
    synapticCurrent += pImpl->synapticInput;
    
    // Apply spike-frequency adaptation (slow hyperpolarization)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Apply metaplasticity: prediction error adjusts future learning rates
    if (pImpl->state.predictionError != 0.0f) {
        // Positive prediction error (better than expected) increases plasticity threshold
        if (pImpl->state.predictionError > 0.0f) {
            pImpl->state.metaplasticThreshold += pImpl->state.predictionError * 0.001f;
        } else {
            // Negative prediction error (worse than expected) decreases plasticity
            pImpl->state.metaplasticThreshold += pImpl->state.predictionError * 0.002f;
        }
        // Clamp metaplastic threshold to reasonable range
        pImpl->state.metaplasticThreshold = std::clamp(pImpl->state.metaplasticThreshold, 0.1f, 1.0f);
        
        // Clear prediction error after applying metaplastic changes
        pImpl->state.predictionError = 0.0f;
    }
    
    // Update adaptive membrane time constant based on recent activity
    if (pImpl->state.lastSpikeTime > 0.0f && currentTime - pImpl->state.lastSpikeTime < 100.0f) {
        // Recent high activity reduces time constant (faster response)
        pImpl->state.timeConstant = std::max(5.0f, pImpl->state.timeConstant * 0.98f);
    } else {
        // Baseline time constant recovers
        pImpl->state.timeConstant = std::min(50.0f, pImpl->state.timeConstant * 1.02f);
    }
    
    // Clamp membrane potential to prevent instability (input saturation)
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with noise-modulated threshold
    float thresholdMod = threshold;
    if (pImpl->state.noiseAmplitude > 0.0f) {
        // Noise can cause premature firing or delay spike
        thresholdMod += noise * 0.5f;
    }
    
    if (V >= thresholdMod) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastLastSpikeTime = pImpl->state.lastSpikeTime;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f + pImpl->state.predictionError * 0.1f;
        
        // Update adaptation variable 2 for slower dynamics
        pImpl->state.adaptationVariable2 += 0.5f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Calculate derivatives for membrane potential update
    float leakTerm = (V_rest - V) / pImpl->state.timeConstant;
    float synapticTerm = synapticCurrent / Impl::MEMBRANE_CAPACITANCE;
    
    // Apply noise to membrane dynamics
    float noiseTerm = noise / Impl::MEMBRANE_CAPACITANCE;
    
    // Update membrane potential using exponential Euler integration
    V += static_cast<float>(dt) * 1000.0f * (leakTerm + synapticTerm + noiseTerm);
    
    // Apply adaptation effects to membrane potential
    if (pImpl->state.adaptationVariable2 > 0.0f) {
        V -= pImpl->state.adaptationVariable2 * 0.001f;  // Slower adaptation
        pImpl->state.adaptationVariable2 *= 0.99f;
    }
    
    // Clamp again after integration
    V = std::clamp(V, -100.0f, 50.0f);
    
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
    
    // Excitation/inhibition conductance balance (typically 3:1 ratio)
    float balance = rng.uniformReal(0.8f, 1.2f);
    pImpl->state.excitatoryConductance = 3.0f * balance;  // More excitation
    pImpl->state.inhibitoryConductance = 1.0f * balance;
    
    // Neuromodulation levels (initially low)
    pImpl->state.neuromodulationLevel = rng.uniformReal(0.0f, 0.2f);  // Low initial neuromodulation
    pImpl->state.gainFactor = rng.uniformReal(1.0f, 1.5f);  // Slightly increased gain
    
    // Noise amplitude for stochastic dynamics
    pImpl->state.noiseAmplitude = rng.uniformReal(0.01f, 0.05f);  // Low noise initially
    
    // Adaptive membrane time constant
    pImpl->state.timeConstant = rng.uniformReal(15.0f, 25.0f);  // Typical 20ms
    
    // Prediction error for metaplasticity (initialized near 0)
    pImpl->state.predictionError = rng.uniformReal(-0.1f, 0.1f);
    
    // Metaplastic threshold
    pImpl->state.metaplasticThreshold = rng.uniformReal(0.3f, 0.7f);
    
    // Input clamping settings
    pImpl->state.totalInput = 0.0f;  // Will be updated dynamically
    
    // Adaptive excitability flag
    pImpl->state.adaptiveExcitability = false;  // Will be set by neuromodulators
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.adaptationVariable2 = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.lastLastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
