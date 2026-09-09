#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>

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
    
    // Tsodyks-Markram Short-Term Plasticity (STP) variables
    float u;           // Utilization (probability of vesicle release)
    float x;           // Fraction of available vesicles
    float R;           // Recovery variable (fraction of resources not used)
    float lastPreSpikeTime;  // Time of last presynaptic spike (ms)
    float lastPostSpikeTime;  // Time of last postsynaptic spike (ms)
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Reversal potentials (in mV)
    float reversalPotential;
    
    // Synaptic conductance state
    float conductance;      // Total synaptic conductance
    float AMPAConductance;   // AMPA receptor conductance
    float NMDAConductance;  // NMDA receptor conductance
    
    // Synaptic weight dynamics
    float calciumConcentration;  // Intracellular calcium level
    float weightChangeRate;       // Rate of weight change
    
    // Synaptic noise
    float noiseLevel;
    
    // Synaptic activity history
    float burstHistory;  // Number of spikes in recent burst
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Biological reversal potentials (in mV)
    static constexpr float DEFAULT_E_EXC = 0.0f;    // Excitatory (AMPA/NMDA)
    static constexpr float DEFAULT_E_INH = -75.0f;  // Inhibitory (GABA)
    static constexpr float DEFAULT_E_MOD = -50.0f;  // Modulatory (acetylcholine)
    
    // Synaptic time constants
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;     // ms
    static constexpr float STP_U_MAX = 1.0f;                 // Max utilization
    static constexpr float STP_X_MAX = 1.0f;                 // Max resources
    static constexpr float SYNAPTIC_RISE_TAU = 1.0f;         // ms
    static constexpr float SYNAPTIC_FALL_TAU = 5.0f;         // ms
    static constexpr float NMDA_TAU = 50.0f;                 // ms (NMDA slow decay)
    static constexpr float CALCIUM_TAU = 100.0f;             // ms
    static constexpr float CALCIUM_DECAY = 0.1f;             // Calcium decay rate
    
    // Plasticity parameters
    static constexpr float LTP_THRESHOLD = 0.5f;    // Calcium threshold for LTP
    static constexpr float LTD_THRESHOLD = 0.2f;    // Calcium threshold for LTD
    static constexpr float STDP_WIDTH = 20.0f;       // STDP window width (ms)
    static constexpr float WEIGHT_CHANGE_MAX = 0.01f;  // Maximum weight change per event
    
    // Noise parameters
    static constexpr float NOISE_LEVEL_DEFAULT = 0.05f;  // Default noise level
    
    // Bursting parameters
    static constexpr int BURST_WINDOW = 20;        // ms window for burst detection
    static constexpr int BURST_THRESHOLD = 3;      // Minimum spikes for burst
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->u = 0.0f;           // Utilization (0-1)
    pImpl->x = 1.0f;           // Available vesicles (0-1)
    pImpl->R = 1.0f;           // Recovery variable (0-1)
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->reversalPotential = Impl::DEFAULT_E_EXC;
    pImpl->conductance = 0.0f;
    pImpl->AMPAConductance = 0.0f;
    pImpl->NMDAConductance = 0.0f;
    pImpl->calciumConcentration = 0.0f;
    pImpl->weightChangeRate = 0.0f;
    pImpl->noiseLevel = Impl::NOISE_LEVEL_DEFAULT;
    pImpl->burstHistory = 0.0f;
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

SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
    // Update reversal potential based on type
    if (type == SynapseType::Excitatory) {
        pImpl->reversalPotential = Impl::DEFAULT_E_EXC;
    } else if (type == SynapseType::Inhibitory) {
        pImpl->reversalPotential = Impl::DEFAULT_E_INH;
    } else {
        pImpl->reversalPotential = Impl::DEFAULT_E_MOD;
    }
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    onPreSpike(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    onPostSpike(timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->R = 1.0f;
    pImpl->calciumConcentration = 0.0f;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

float Synapse::getUtilization() const {
    return pImpl->u;
}

float Synapse::getRecovery() const {
    return pImpl->R;
}

float Synapse::getResources() const {
    return pImpl->x;
}

float Synapse::getReversalPotential() const {
    return pImpl->reversalPotential;
}

void Synapse::setReversalPotential(float potential) {
    pImpl->reversalPotential = potential;
}

float Synapse::getConductance() const {
    return pImpl->conductance;
}

void Synapse::setConductance(float conductance) {
    pImpl->conductance = conductance;
}

float Synapse::getAMPAConductance() const {
    return pImpl->AMPAConductance;
}

float Synapse::getNMDAConductance() const {
    return pImpl->NMDAConductance;
}

float Synapse::getCalciumConcentration() const {
    return pImpl->calciumConcentration;
}

void Synapse::setCalciumConcentration(float calcium) {
    pImpl->calciumConcentration = calcium;
}

float Synapse::getWeightChangeRate() const {
    return pImpl->weightChangeRate;
}

float Synapse::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void Synapse::setNoiseLevel(float noise) {
    pImpl->noiseLevel = std::max(0.0f, noise);
}

int Synapse::getRecentSpikeCount(int timeWindow, Timestamp currentTime) const {
    int count = 0;
    for (auto& timestamp : pImpl->preSpikeHistory) {
        if (timestamp > 0 && (currentTime - timestamp) < timeWindow) {
            count++;
        }
    }
    return count;
}

void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Update Tsodyks-Markram STP variables
    float u = pImpl->u;
    float x = pImpl->x;
    float R = pImpl->R;
    
    // Decay resources and recovery variables
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        R = std::max(0.0f, R - timeSincePre / Impl::STP_DEPRESSION_TAU);
    }
    
    // Decay resources based on overall activity
    float timeSinceActivity = std::max(
        (pImpl->lastPreSpikeTime >= 0.0f) ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f,
        (pImpl->lastPostSpikeTime >= 0.0f) ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f
    );
    x = std::max(0.0f, x - timeSinceActivity / Impl::STP_FACILITATION_TAU);
    
    // Update utilization based on recent activity
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        u = std::min(Impl::STP_U_MAX, u + 0.1f);  // Facilitation
    } else {
        u = std::max(0.0f, u - 0.01f / Impl::STP_DEPRESSION_TAU * dt);  // Depression
    }
    
    // Apply use-dependent modulation
    if (u > 0.0f && x > 0.0f) {
        pImpl->efficacy = std::min(2.0f, pImpl->efficacy * (1.0f + u * 0.5f));
    }
    
    // Update STP variables
    pImpl->u = u;
    pImpl->x = x;
    pImpl->R = R;
    
    // Decay eligibility trace
    decayEligibilityTrace(0.001f);
    
    // Decay calcium concentration
    pImpl->calciumConcentration *= std::exp(-dt / Impl::CALCIUM_TAU);
    
    // Apply weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::onPreSpike(Timestamp time) {
    // Tsodyks-Markram STP update on presynaptic spike
    float u = pImpl->u;
    float x = pImpl->x;
    float R = pImpl->R;
    
    // Update utilization (release probability)
    u = std::min(Impl::STP_U_MAX, u + 0.5f);
    
    // Update resources (decrease available vesicles)
    x = x * (1.0f - u);
    
    // Update recovery variable (resources not used)
    R = R - u;
    
    // Store state
    pImpl->u = u;
    pImpl->x = x;
    pImpl->R = R;
    pImpl->lastPreSpikeTime = time;
    
    // Update calcium concentration (presynaptic calcium)
    pImpl->calciumConcentration += 0.1f;
    
    // Record spike for STDP
    recordPreSpike(time);
    
    // Apply plasticity
    applyPlasticity(time);
}

void Synapse::onPostSpike(Timestamp time) {
    // Postsynaptic spike handling
    pImpl->lastPostSpikeTime = time;
    
    // Record spike
    recordPostSpike(time);
    
    // Update calcium dynamics
    pImpl->calciumConcentration += 0.3f;
    
    // Apply plasticity
    applyPlasticity(time);
}

float Synapse::calculateCurrent(float vPost, Timestamp currentTime) const {
    // Calculate synaptic current using conductance-based dynamics
    float totalConductance = pImpl->conductance;
    
    if (totalConductance <= 0.0f) {
        return 0.0f;
    }
    
    // Apply synaptic weight scaling
    float effectiveWeight = pImpl->weight * pImpl->efficacy;
    
    // Calculate driving force
    float reversalPotential = pImpl->reversalPotential;
    float drivingForce = vPost - reversalPotential;
    
    // Apply NMDA voltage dependence (simplified)
    float NMDA_factor = 1.0f;
    if (pImpl->type == SynapseType::Excitatory) {
        NMDA_factor = 1.0f / (1.0f + std::exp(-0.056f * (vPost + 35.0f)));
    }
    
    // Calculate current with noise
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::normal_distribution<float> noise(0.0f, pImpl->noiseLevel);
    
    float current = effectiveWeight * totalConductance * drivingForce * NMDA_factor + noise(rng);
    
    return current;
}

void Synapse::applyPlasticity(Timestamp currentTime) {
    // Calculate calcium-dependent plasticity
    float calcium = pImpl->calciumConcentration;
    
    // STDP component
    if (pImpl->plasticityFlags.stdp) {
        // Calculate synaptic eligibility from spike timing
        float eligibility = 0.0f;
        
        // Simple STDP-like update based on calcium
        if (calcium > Impl::LTP_THRESHOLD) {
            // Long-term potentiation
            pImpl->weightChangeRate += Impl::WEIGHT_CHANGE_MAX * (calcium - Impl::LTP_THRESHOLD) / 100.0f;
        } else if (calcium < Impl::LTD_THRESHOLD) {
            // Long-term depression
            pImpl->weightChangeRate -= Impl::WEIGHT_CHANGE_MAX * (Impl::LTD_THRESHOLD - calcium) / 100.0f;
        }
        
        // Clip weight change rate
        pImpl->weightChangeRate = std::clamp(pImpl->weightChangeRate, -Impl::WEIGHT_CHANGE_MAX, Impl::WEIGHT_CHANGE_MAX);
    }
    
    // Apply Hebbian component
    if (pImpl->plasticityFlags.hebbian) {
        // Hebbian-like potentiation based on co-activation
        if (calcium > 0.3f) {
            pImpl->weightChangeRate += 0.005f * calcium;
        }
    }
    
    // Apply reward-modulated component
    if (pImpl->plasticityFlags.reward_modulated) {
        // Integrate with eligibility trace
        pImpl->weightChangeRate += pImpl->eligibilityTrace * 0.01f;
    }
    
    // Apply weight change
    if (std::abs(pImpl->weightChangeRate) > 0.0f) {
        addToWeight(pImpl->weightChangeRate * 0.001f);  // dt factor
        
        // Decay weight change rate
        pImpl->weightChangeRate *= 0.9f;
    }
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->R = 1.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->reversalPotential = Impl::DEFAULT_E_EXC;
    pImpl->conductance = 0.0f;
    pImpl->AMPAConductance = 0.0f;
    pImpl->NMDAConductance = 0.0f;
    pImpl->calciumConcentration = 0.0f;
    pImpl->weightChangeRate = 0.0f;
    pImpl->noiseLevel = Impl::NOISE_LEVEL_DEFAULT;
    pImpl->burstHistory = 0.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        pImpl->reversalPotential = Impl::DEFAULT_E_EXC;
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        pImpl->reversalPotential = Impl::DEFAULT_E_INH;
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
        pImpl->reversalPotential = Impl::DEFAULT_E_MOD;
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize STP state
    pImpl->u = 0.0f;
    pImpl->x = 1.0f;
    pImpl->R = 1.0f;
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Initialize calcium and noise
    pImpl->calciumConcentration = 0.0f;
    pImpl->noiseLevel = rng.uniformReal(0.01f, 0.1f);
}

} // namespace nlm
