#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @file Synapse.cpp
 * @brief Implementation of Tsodyks-Markram short-term plasticity (STP) model
 * 
 * This file implements the complete synaptic dynamics model based on the
 * Tsodyks-Markram framework for short-term synaptic plasticity. The model
 * captures multiple temporal scales of synaptic modulation:
 * - Fast facilitation (200ms time constant)
 * - Slow depression (800ms time constant)
 * - Spike-frequency adaptation (eligibility traces)
 * - Synaptic efficacy modulation
 * 
 * **Complete STP Model Equations**:
 * 
 * 1. **Facilitation Dynamics**:
 * ```
 * du/dt = (U_max - u)/τ_u + U_max * x * R(t)
 * ```
 * 
 * 2. **Resources Available**:
 * ```
 * dx/dt = (xd - x * u * R(t)) / τ_x
 * ```
 * 
 * 3. **Depression Recovery**:
 * ```
 * dxd/dt = (xd_0 - x d) / τ_xd + x * u * R(t)
 * ```
 * 
 * 4. **Eligibility Trace for Reward Modulation**:
 * ```
 * dE/dt = -λ * E + u * x * R(t)
 * ```
 * 
 * 5. **Synaptic Efficacy Learning**:
 * ```
 * de/dt = k * (e_baseline - e) + u * x * R(t)
 * ```
 * 
 * **Biologically Realistic Parameters**:
 * - Facilitation time constant: τ_u = 200ms
 * - Depression time constant: τ_xd = 800ms  
 * - Maximum utilization: U_max = 0.95
 * - Eligibility decay rate: λ = 0.001
 * - Synaptic learning rate: k = 0.01
 * - Resource capacity: x ∈ [0.0, 1.0], xd ∈ [0.0, 2.0]
 * 
 * **STP State Variables**:
 * - u: utilization parameter (0-1, fraction used per spike)
 * - x: resources available for transmission
 * - xd: depression recovery resources
 * - facilitating_vars: facilitation variable
 * - eligibilityTrace: reward-modulated eligibility trace
 * - efficacy: use-dependent synaptic strength
 * 
 * @see Synapse.hpp - interface documentation
 * @see ../plasticity/STDP.hpp - long-term spike-timing dependent plasticity
 * @see ../neuromodulation/Neuromodulator.hpp - reward-modulated learning
 * @see ../../brain/Brain.cpp:476 - synapse step integration
 */

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Enhanced short-term plasticity state
    float u;  // Utilization parameter (0-1)
    float x;  // Resources available
    float U_max;  // Maximum utilization (varies by synapse)
    
    // Short-term depression recovery
    float xd;  // Depression resources
    float xd_0;  // Baseline depression resources
    
    // Short-term facilitation
    float facilitating_vars;  // Facilitation variable
    float facilitating_vars_0;  // Baseline facilitation variable
    
    // Last spike times for STP
    float lastPreSpikeTime;
    float lastPostSpikeTime;
    
    // Enhanced eligibility trace for reward-modulated learning
    float eligibilityTrace;
    float eligibilityDecayRate;
    
    // Synaptic efficacy with use-dependent modulation
    float efficacy;
    float efficacyBaseline;
    float efficacyMax;  // Maximum achievable efficacy
    
    // Weight bounds with type-specific constraints
    static constexpr float MIN_WEIGHT_EXCITATORY = -0.1f;
    static constexpr float MAX_WEIGHT_EXCITATORY = 1.0f;
    static constexpr float MIN_WEIGHT_INHIBitory = -1.0f;
    static constexpr float MAX_WEIGHT_INHIBitory = 0.1f;
    static constexpr float MIN_WEIGHT_MODULATORY = -0.5f;
    static constexpr float MAX_WEIGHT_MODULATORY = 0.5f;
    
    // STP time constants (more biologically realistic)
    static constexpr float STP_FACILITATION_TAU = 200.0f;  // ms - faster facilitation
    static constexpr float STP_DEPRESSION_TAU = 800.0f;   // ms - slower depression
    static constexpr float STP_U_MAX = 0.95f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->eligibilityDecayRate = 0.001f;
    pImpl->efficacy = 1.0f;
    pImpl->efficacyBaseline = 1.0f;
    pImpl->efficacyMax = 2.0f;
    
    // Initialize STP state using Tsodyks-Markram model
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->xd = 1.0f;
    pImpl->xd_0 = 1.0f;
    pImpl->facilitating_vars = 0.0f;
    pImpl->facilitating_vars_0 = 0.0f;
    pImpl->U_max = Impl::STP_U_MAX;
    
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

SynapseId Synapse::getId() const { return pImpl->id; }
NeuronId Synapse::getSourceNeuron() const { return pImpl->sourceNeuron; }
NeuronId Synapse::getDestinationNeuron() const { return pImpl->destinationNeuron; }

SynapticWeight Synapse::getWeight() const { return pImpl->weight; }
void Synapse::setWeight(SynapticWeight weight) {
    float minWeight = Impl::MIN_WEIGHT_EXCITATORY;
    float maxWeight = Impl::MAX_WEIGHT_EXCITATORY;
    
    switch (pImpl->type) {
        case SynapseType::Excitatory: minWeight = Impl::MIN_WEIGHT_EXCITATORY; maxWeight = Impl::MAX_WEIGHT_EXCITATORY; break;
        case SynapseType::Inhibitory: minWeight = Impl::MIN_WEIGHT_INHIBitory; maxWeight = Impl::MAX_WEIGHT_INHIBitory; break;
        case SynapseType::Modulatory: minWeight = Impl::MIN_WEIGHT_MODULATORY; maxWeight = Impl::MAX_WEIGHT_MODULATORY; break;
    }
    
    pImpl->weight = std::clamp(weight, minWeight, maxWeight);
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    
    float minWeight = Impl::MIN_WEIGHT_EXCITATORY;
    float maxWeight = Impl::MAX_WEIGHT_EXCITATORY;
    
    switch (pImpl->type) {
        case SynapseType::Excitatory: minWeight = Impl::MIN_WEIGHT_EXCITATORY; maxWeight = Impl::MAX_WEIGHT_EXCITATORY; break;
        case SynapseType::Inhibitory: minWeight = Impl::MIN_WEIGHT_INHIBitory; maxWeight = Impl::MAX_WEIGHT_INHIBitory; break;
        case SynapseType::Modulatory: minWeight = Impl::MIN_WEIGHT_MODULATORY; maxWeight = Impl::MAX_WEIGHT_MODULATORY; break;
    }
    
    pImpl->weight = std::clamp(pImpl->weight, minWeight, maxWeight);
}

Delay Synapse::getDelay() const { return pImpl->delay; }
void Synapse::setDelay(Delay delay) { pImpl->delay = delay; }

SynapseType Synapse::getType() const { return pImpl->type; }
void Synapse::setType(SynapseType type) { pImpl->type = type; }

bool Synapse::isExcitatory() const { return pImpl->type == SynapseType::Excitatory; }
bool Synapse::isInhibitory() const { return pImpl->type == SynapseType::Inhibitory; }

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const { return pImpl->preSpikeHistory; }
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const { return pImpl->postSpikeHistory; }
void Synapse::clearHistory() { pImpl->preSpikeHistory.clear(); pImpl->postSpikeHistory.clear(); }

const PlasticityFlags& Synapse::getPlasticityFlags() const { return pImpl->plasticityFlags; }
PlasticityFlags& Synapse::getPlasticityFlags() { return pImpl->plasticityFlags; }
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const { return pImpl->eligibilityTrace; }
void Synapse::setEligibilityTrace(float trace) { pImpl->eligibilityTrace = trace; }
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= std::exp(-decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const { return pImpl->efficacy; }
void Synapse::setEfficacy(float efficacy) { pImpl->efficacy = std::clamp(efficacy, 0.0f, pImpl->efficacyMax); }
float Synapse::getEfficacyBaseline() const { return pImpl->efficacyBaseline; }

float Synapse::getUtilization() const { return pImpl->u; }
float Synapse::getAvailableResources() const { return pImpl->x; }
float Synapse::getDepressionResources() const { return pImpl->xd; }
float Synapse::getFacilitationVariable() const { return pImpl->facilitating_vars; }

    /**
     * @brief Complete synapse simulation step
     * 
     * @details Updates all synaptic dynamic variables for one timestep:
     * 
     * 1. **Recovery**: Resources recover from depression
     * 2. **Facilitation**: Previous spikes decay
     * 3. **STP update**: New utilization computed
     * 4. **Eligibility trace**: Decays over time
     * 5. **Efficacy**: Modulates based on activity
     * 6. **Weight clamping**: Respects type constraints
     * 
     * **Complete STP Equations**:
     * ```
     * u(t) = U_max * (1 - facilitating_vars)
     * x(t) = xd(t) * (1 - u(t))
     * xd(t+dt) = xd(t) + (xd_0 - xd(t)) * (1 - exp(-dt/τ_xd))
     * facilitating_vars(t+dt) = facilitating_vars(t) * exp(-dt/τ_u)
     * E(t+dt) = E(t) * exp(-λ * dt)
     * efficacy(t+dt) = efficacy(t) + learning_rate * (baseline - efficacy(t))
     * ```
     * 
     * @param currentTime Current simulation time (ms)
     * @post All STP variables updated according to equations above
     * @see ../brain/Neuron.cpp:226 - synapse step integration
     */
    void Synapse::step(Timestamp currentTime) {
        TimestepDuration dt = 0.001;
        
        // Update STP state based on time since last pre-synaptic spike
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            // Time since last pre-synaptic spike
            float delta_t = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
            
            // Facilitation decays exponentially with time
            pImpl->facilitating_vars *= std::exp(-delta_t / Impl::STP_FACILITATION_TAU);
            
            // Depression recovers toward baseline
            pImpl->xd += (pImpl->xd_0 - pImpl->xd) * (1.0f - std::exp(-delta_t / Impl::STP_DEPRESSION_TAU));
            
            // Utilization decreases as facilitation recovers
            pImpl->u = pImpl->U_max * (1.0f - pImpl->facilitating_vars);
            
            // Available resources = depression resources * (1 - utilization)
            pImpl->x = pImpl->xd * (1.0f - pImpl->u);
        } else {
            // No spike history - apply continuous decay
            pImpl->facilitating_vars *= std::exp(-dt * 1000.0f / Impl::STP_FACILITATION_TAU);
            pImpl->xd += (pImpl->xd_0 - pImpl->xd) * (1.0f - std::exp(-dt * 1000.0f / Impl::STP_DEPRESSION_TAU));
            pImpl->u = pImpl->U_max * (1.0f - pImpl->facilitating_vars);
            pImpl->x = pImpl->xd * (1.0f - pImpl->u);
        }
        
        // Eligibility trace decays over time
        float eligibilityDecayFactor = std::exp(-Impl::STP_FACILITATION_TAU * dt * 1000.0f);
        pImpl->eligibilityTrace *= eligibilityDecayFactor;
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
        
        // Synaptic efficacy modulates toward baseline over time
        float efficacyChangeRate = 0.01f;
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            // Recent spike: efficacy increases toward maximum
            pImpl->efficacy += 0.1f * (1.0f - pImpl->efficacy / pImpl->efficacyMax);
        } else {
            // No recent spike: efficacy decays toward baseline
            pImpl->efficacy += (pImpl->efficacyBaseline - pImpl->efficacy) * efficacyChangeRate;
        }
        
        // Clamp efficacy to valid range
        pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, pImpl->efficacyMax);
        
        // Clamp synaptic weight to type-specific bounds
        float minWeight = Impl::MIN_WEIGHT_EXCITATORY;
        float maxWeight = Impl::MAX_WEIGHT_EXCITATORY;
        
        switch (pImpl->type) {
            case SynapseType::Excitatory: minWeight = Impl::MIN_WEIGHT_EXCITATORY; maxWeight = Impl::MAX_WEIGHT_EXCITATORY; break;
            case SynapseType::Inhibitory: minWeight = Impl::MIN_WEIGHT_INHIBitory; maxWeight = Impl::MAX_WEIGHT_INHIBitory; break;
            case SynapseType::Modulatory: minWeight = Impl::MIN_WEIGHT_MODULATORY; maxWeight = Impl::MAX_WEIGHT_MODULATORY; break;
        }
        
        pImpl->weight = std::clamp(pImpl->weight, minWeight, maxWeight);
        
        // Clamp STP state variables to biologically plausible ranges
        pImpl->x = std::clamp(pImpl->x, 0.0f, 1.0f);           // Resources available [0, 1]
        pImpl->xd = std::clamp(pImpl->xd, 0.0f, 2.0f);         // Depression resources [0, 2]
        pImpl->facilitating_vars = std::clamp(pImpl->facilitating_vars, 0.0f, 1.0f); // Facilitation [0, 1]
    }

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = pImpl->efficacyBaseline;
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->xd = 1.0f;
    pImpl->xd_0 = 1.0f;
    pImpl->facilitating_vars = 0.0f;
    pImpl->facilitating_vars_0 = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (pImpl->type == SynapseType::Excitatory) {
        pImpl->weight = rng.uniformReal(Impl::MIN_WEIGHT_EXCITATORY, Impl::MAX_WEIGHT_EXCITATORY);
        pImpl->efficacyBaseline = rng.uniformReal(0.9f, 1.2f);
        pImpl->efficacy = pImpl->efficacyBaseline;
        pImpl->U_max = rng.uniformReal(0.7f, 0.95f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        pImpl->weight = -rng.uniformReal(std::abs(Impl::MIN_WEIGHT_INHIBitory), std::abs(Impl::MAX_WEIGHT_INHIBitory));
        pImpl->efficacyBaseline = rng.uniformReal(0.8f, 1.1f);
        pImpl->efficacy = pImpl->efficacyBaseline;
        pImpl->U_max = rng.uniformReal(0.6f, 0.9f);
    } else {
        pImpl->weight = rng.uniformReal(Impl::MIN_WEIGHT_MODULATORY, Impl::MAX_WEIGHT_MODULATORY);
        pImpl->efficacyBaseline = rng.uniformReal(0.85f, 1.0f);
        pImpl->efficacy = pImpl->efficacyBaseline;
        pImpl->U_max = rng.uniformReal(0.5f, 0.85f);
    }
    
    if (pImpl->type == SynapseType::Excitatory) {
        pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 3));
    } else if (pImpl->type == SynapseType::Inhibitory) {
        pImpl->delay = static_cast<Delay>(rng.uniformInt(2, 5));
    } else {
        pImpl->delay = static_cast<Delay>(rng.uniformInt(3, 7));
    }
    
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->xd = 1.0f;
    pImpl->xd_0 = 1.0f;
    pImpl->facilitating_vars = 0.0f;
    pImpl->facilitating_vars_0 = 0.0f;
    
    pImpl->eligibilityTrace = 0.0f;
    
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm
