#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

struct Synapse::Impl {
    // Basic identity and properties
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // === ADVANCED SYNAPTIC STATE ===
    
    // Calcium dynamics for plasticity induction
    float calciumConcentration;           // Intracellular calcium (nM)
    float calciumBaseline;                 // Resting calcium level
    float calciumTrapped;                  // Calcium bound to buffers
    float calciumUptakeRate;               // Calcium pump rate (1/ms)
    float calciumDecayRate;                // Calcium leak/diffusion rate (1/ms)
    
    // Synaptic resource pools
    float vesiclePool;                     // Available vesicles (0-1)
    float receptorPool;                    // Available receptors (0-1)
    float vesicleRecyclingRate;            // Vesicle recycling rate (1/ms)
    float receptorTurnoverRate;            // Receptor turnover rate (1/ms)
    
    // Voltage-dependent conductances
    float voltageDependentG;               // Voltage sensitivity factor
    float voltageIntegrationTime;          // Time constant for voltage integration
    float membranePotentialTrace;          // Low-pass filtered membrane potential
    
    // Metabolic energy constraints
    float metabolicEnergy;                 // Available metabolic energy (arbitrary units)
    float energyCostPerSpike;              // Energy cost per spike (units/ms)
    float energyHarvestPerSpike;           // Energy harvested per spike (units/ms)
    float energyEfficiency;                // Overall energy efficiency
    
    // === PLASTICITY MECHANISMS ===
    
    // Spike history for STDP (with filtering)
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Advanced spike history filtering (recent spikes for plasticity)
    std::vector<Timestamp> filteredPreSpikeHistory;
    std::vector<Timestamp> filteredPostSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // === ADVANCED STDP ===
    
    // STDP parameters (pre-post interactions)
    float stdpLearningRatePre;            // Learning rate for pre-post spike pairs
    float stdpLearningRatePost;           // Learning rate for post-pre spike pairs
    float stdpTauPre;                      // Time constant for pre-post dependencies (ms)
    float stdpTauPost;                     // Time constant for post-pre dependencies (ms)
    float stdpMaxWeightChange;             // Maximum weight change per STDP event
    
    // Triplet STDP parameters (three spike interactions)
    float tripletLearningRatePre;         // Learning rate for pre-post-post triplets
    float tripletLearningRatePost;        // Learning rate for post-pre-post triplets
    float tripletLearningRatePrePost;     // Learning rate for post-post-pre triplets
    float tripletTau;                      // Time constant for triplet interactions (ms)
    
    // === ENHANCED SHORT-TERM PLASTICITY ===
    
    // Short-term depression (vesicle depletion model)
    float shortTermDepression;            // Utilization factor (0-1)
    float depressionRecoveryRate;         // Recovery from depression (1/ms)
    float depressionTimeConstant;         // Time constant for depression (ms)
    float depressionMaxUtilization;       // Maximum utilization (u)
    
    // Short-term facilitation (residual calcium model)
    float shortTermFacilitation;          // Facilitation factor (0-1)
    float facilitationDecayRate;          // Decay of facilitation (1/ms)
    float facilitationTimeConstant;       // Time constant for facilitation (ms)
    float facilitationIncrement;          // Increment from each presynaptic spike
    
    // Use-dependent modulation
    float lastPreSpikeTime;               // For short-term dynamics
    float lastPostSpikeTime;
    float useHistory;                      // Accumulated usage for efficacy modulation
    
    // === METAPLASTICITY ===
    
    // Activity-dependent STP parameter changes
    float facilitationU;                  // Utilization for facilitation
    float depressionU;                     // Utilization for depression
    float facilitationTau;                // Time constant for facilitation
    float depressionTau;                   // Time constant for depression
    
    // === REWARD-MODULATED LEARNING ===
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    float rewardTraceDecayRate;            // Decay rate for eligibility traces (1/ms)
    float rewardIntegrationRate;          // Integration rate for reward signals
    float rewardModulationStrength;       // Strength of reward modulation
    
    // === SYNAPTIC EFFICACY ===
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    float efficacyTimeConstant;           // Time constant for efficacy changes (ms)
    float efficacyBaseline;                // Baseline efficacy
    
    // === BIOLICAL REALISM ===
    
    // Stochastic noise and variability
    float weightNoiseStdDev;               // Standard deviation for weight noise
    float noiseCorrelationTime;            // Correlation time for noise (ms)
    float noiseUpdateRate;                 // Rate at which noise is updated (1/ms)
    float currentNoiseValue;               // Current noise sample
    
    // Refractory period interactions
    float refractorySynergy;              // Interaction with postsynaptic refractory period
    float refractoryTimeConstant;          // Time constant for refractory interactions (ms)
    
    // Adaptation effects
    float homeostaticTarget;              // Target weight for homeostatic plasticity
    float homeostaticLearningRate;        // Rate of homeostatic adjustment
    float adaptationVariable;             // Internal adaptation state
    
    // Structural plasticity state
    float structuralPotential;            // Potential for structural changes
    float structuralThreshold;            // Threshold for structural changes
    float synaptogenesisRate;              // Rate of new synapse formation
    float pruningRate;                     // Rate of synapse elimination
    
    // === BOUNDARIES AND CONSTRAINTS ===
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Calcium concentration bounds
    static constexpr float MIN_CALCIUM = 0.0f;
    static constexpr float MAX_CALCIUM = 10000.0f;
    
    // Resource pool bounds
    static constexpr float MIN_RESOURCE_POOL = 0.0f;
    static constexpr float MAX_RESOURCE_POOL = 1.0f;
    
    // Metabolic energy bounds
    static constexpr float MIN_ENERGY = 0.0f;
    static constexpr float MAX_ENERGY = 1000.0f;
    
    // Spike history bounds
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    static constexpr size_t MAX_FILTERED_SPIKE_HISTORY = 50;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    static constexpr float STDV_STDP_TAU = 30.0f;    // STDP time constant (ms)
    static constexpr float STDV_STDP_MAX_DELTA = 0.05f;  // Max STDP weight change
    static constexpr float STDV_STDP_LR = 0.01f;      // STDP learning rate
    
    // Advanced STP parameters
    static constexpr float TRIPLET_TAU = 100.0f;        // Triplet time constant (ms)
    static constexpr float TRIPLET_LR = 0.005f;        // Triplet learning rate
    static constexpr float METAPLASTICITY_LR = 0.001f;  // Metaplasticity learning rate
    static constexpr float EFFICACY_TIME_CONSTANT = 500.0f;  // Efficacy time constant (ms)
    static constexpr float NOISE_UPDATE_RATE = 0.1f;   // Noise update rate (1/ms)
    static constexpr float NOISE_STDDEV = 0.01f;       // Weight noise stddev
    static constexpr float REF_SIM_SYNERGY = 0.5f;    // Refractory synergy factor
    
    // Calcium dynamics parameters
    static constexpr float CALCIUM_UPTAKE_RATE = 0.005f;    // Calcium pump rate (1/ms)
    static constexpr float CALCIUM_DECAY_RATE = 0.001f;     // Calcium leak rate (1/ms)
    static constexpr float CALCIUM_CONCENTRATION_PEAK = 2000.0f;  // Peak calcium concentration (nM)
    
    // Resource dynamics parameters
    static constexpr float VESICLE_RECYCLING_RATE = 0.002f;  // Vesicle recycling rate (1/ms)
    static constexpr float RECEPTOR_TURNOVER_RATE = 0.0005f; // Receptor turnover rate (1/ms)
    
    // Metabolic parameters
    static constexpr float ENERGY_COST_PER_SPIKE = 0.01f;   // Energy cost per spike (units/ms)
    static constexpr float ENERGY_HARVEST_PER_SPIKE = 0.008f; // Energy harvested per spike (units/ms)
    static constexpr float METABOLIC_EFFICIENCY = 0.8f;      // Metabolic efficiency
    
    // Reward modulation parameters
    static constexpr float REWARD_TRACE_DECAY = 0.99f;      // Eligibility trace decay per step
    static constexpr float REWARD_INTEGRATION_RATE = 0.1f;   // Reward integration rate
    static constexpr float REWARD_MODULATION_STRENGTH = 0.5f; // Reward modulation strength
    
    // Adaptation parameters
    static constexpr float HOMEOSTATIC_TARGET = 0.5f;       // Homeostatic target weight
    static constexpr float HOMEOSTATIC_LEARNING_RATE = 0.0001f; // Homeostatic learning rate
    static constexpr float ADAPTATION_TIME_CONSTANT = 1000.0f;   // Adaptation time constant (ms)
    
    // Structural plasticity parameters
    static constexpr float STRUCTURAL_POTENTIAL_THRESHOLD = 0.8f; // Threshold for structural changes
    static constexpr float SYNAPTONGENESIS_RATE = 0.0001f;        // Synapse formation rate (1/ms)
    static constexpr float PRUNING_RATE = 0.00005f;               // Synapse elimination rate (1/ms)
    static constexpr float SYNAPTIC_COMPETITION_FACTOR = 0.3f;    // Competition factor between synapses
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    
    // Initialize advanced synaptic state
    pImpl->calciumConcentration = Impl::MIN_CALCIUM;
    pImpl->calciumBaseline = 0.1f;
    pImpl->calciumTrapped = 0.0f;
    pImpl->calciumUptakeRate = Impl::CALCIUM_UPTAKE_RATE;
    pImpl->calciumDecayRate = Impl::CALCIUM_DECAY_RATE;
    
    pImpl->vesiclePool = Impl::MAX_RESOURCE_POOL;
    pImpl->receptorPool = Impl::MAX_RESOURCE_POOL;
    pImpl->vesicleRecyclingRate = Impl::VESICLE_RECYCLING_RATE;
    pImpl->receptorTurnoverRate = Impl::RECEPTOR_TURNOVER_RATE;
    
    pImpl->voltageDependentG = 0.0f;
    pImpl->voltageIntegrationTime = 10.0f;
    pImpl->membranePotentialTrace = 0.0f;
    
    pImpl->metabolicEnergy = Impl::MAX_ENERGY * 0.8f;
    pImpl->energyCostPerSpike = Impl::ENERGY_COST_PER_SPIKE;
    pImpl->energyHarvestPerSpike = Impl::ENERGY_HARVEST_PER_SPIKE;
    pImpl->energyEfficiency = Impl::METABOLIC_EFFICIENCY;
    
    // Advanced STDP parameters
    pImpl->stdpLearningRatePre = Impl::STDV_STDP_LR;
    pImpl->stdpLearningRatePost = Impl::STDV_STDP_LR;
    pImpl->stdpTauPre = Impl::STDV_STDP_TAU;
    pImpl->stdpTauPost = Impl::STDV_STDP_TAU;
    pImpl->stdpMaxWeightChange = Impl::STDV_STDP_MAX_DELTA;
    
    pImpl->tripletLearningRatePre = Impl::TRIPLET_LR;
    pImpl->tripletLearningRatePost = Impl::TRIPLET_LR;
    pImpl->tripletLearningRatePrePost = Impl::TRIPLET_LR;
    pImpl->tripletTau = Impl::TRIPLET_TAU;
    
    // Enhanced STP parameters
    pImpl->shortTermDepression = 1.0f;
    pImpl->depressionRecoveryRate = 1.0f / Impl::STP_DEPRESSION_TAU;
    pImpl->depressionTimeConstant = Impl::STP_DEPRESSION_TAU;
    pImpl->depressionMaxUtilization = Impl::STP_U_MAX;
    
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->facilitationDecayRate = 1.0f / Impl::STP_FACILITATION_TAU;
    pImpl->facilitationTimeConstant = Impl::STP_FACILITATION_TAU;
    pImpl->facilitationIncrement = 0.5f;
    
    // Metaplasticity parameters
    pImpl->facilitationU = 0.5f;
    pImpl->depressionU = 0.5f;
    pImpl->facilitationTau = Impl::STP_FACILITATION_TAU;
    pImpl->depressionTau = Impl::STP_DEPRESSION_TAU;
    
    // Reward-modulated learning
    pImpl->eligibilityTrace = 0.0f;
    pImpl->rewardTraceDecayRate = Impl::REWARD_TRACE_DECAY;
    pImpl->rewardIntegrationRate = Impl::REWARD_INTEGRATION_RATE;
    pImpl->rewardModulationStrength = Impl::REWARD_MODULATION_STRENGTH;
    
    // Synaptic efficacy
    pImpl->efficacy = 1.0f;
    pImpl->efficacyTimeConstant = Impl::EFFICACY_TIME_CONSTANT;
    pImpl->efficacyBaseline = 1.0f;
    
    // Biological realism
    pImpl->weightNoiseStdDev = Impl::NOISE_STDDEV;
    pImpl->noiseCorrelationTime = 10.0f;
    pImpl->noiseUpdateRate = Impl::NOISE_UPDATE_RATE;
    pImpl->currentNoiseValue = 0.0f;
    
    pImpl->refractorySynergy = Impl::REF_SIM_SYNERGY;
    pImpl->refractoryTimeConstant = 50.0f;
    
    pImpl->homeostaticTarget = Impl::HOMEOSTATIC_TARGET;
    pImpl->homeostaticLearningRate = Impl::HOMEOSTATIC_LEARNING_RATE;
    pImpl->adaptationVariable = 0.0f;
    
    // Structural plasticity
    pImpl->structuralPotential = 0.0f;
    pImpl->structuralThreshold = Impl::STRUCTURAL_POTENTIAL_THRESHOLD;
    pImpl->synaptogenesisRate = Impl::SYNAPTONGENESIS_RATE;
    pImpl->pruningRate = Impl::PRUNING_RATE;
    
    pImpl->useHistory = 0.0f;
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
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
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

// Helper methods for advanced synaptic dynamics

void Synapse::updateCalciumDynamics(TimestepDuration dt) {
    // Calcium dynamics based on spike activity and metabolic state
    float calciumChange = 0.0f;
    
    // Calcium influx from presynaptic spikes (simplified)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        calciumChange += pImpl->calciumConcentration * 0.1f * dt;
    }
    
    // Calcium uptake by buffers
    calciumChange -= pImpl->calciumTrapped * pImpl->calciumUptakeRate * dt;
    
    // Calcium decay/leak
    calciumChange -= pImpl->calciumConcentration * pImpl->calciumDecayRate * dt;
    
    // Calcium uptake by pumps (metabolism-dependent)
    if (pImpl->metabolicEnergy > 0.0f) {
        calciumChange -= std::min(pImpl->calciumConcentration, pImpl->metabolicEnergy * 0.01f) * 0.05f * dt;
    }
    
    pImpl->calciumConcentration += calciumChange;
    pImpl->calciumTrapped += std::max(0.0f, calciumChange * 0.8f);
}

void Synapse::updateResourcePools(TimestepDuration dt) {
    // Synaptic vesicle depletion and recycling
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        // Vesicle depletion during presynaptic spike
        pImpl->vesiclePool = std::max(
            Impl::MIN_RESOURCE_POOL,
            pImpl->vesiclePool - pImpl->shortTermDepression * pImpl->efficacy * 0.1f
        );
        // Update use history
        pImpl->useHistory += 0.1f * pImpl->efficacy;
    }
    
    // Vesicle recycling
    pImpl->vesiclePool += (Impl::MAX_RESOURCE_POOL - pImpl->vesiclePool) * 
        pImpl->vesicleRecyclingRate * dt;
    
    // Receptor turnover
    pImpl->receptorPool += (Impl::MAX_RESOURCE_POOL - pImpl->receptorPool) * 
        pImpl->receptorTurnoverRate * dt;
    
    // Clamp resource pools
    pImpl->vesiclePool = std::clamp(pImpl->vesiclePool, Impl::MIN_RESOURCE_POOL, Impl::MAX_RESOURCE_POOL);
    pImpl->receptorPool = std::clamp(pImpl->receptorPool, Impl::MIN_RESOURCE_POOL, Impl::MAX_RESOURCE_POOL);
}

void Synapse::updateVoltageDependencies(Timestamp currentTime, TimestepDuration dt) {
    // Update membrane potential trace with low-pass filtering
    float voltageFilter = 0.9f;  // Filter coefficient
    pImpl->membranePotentialTrace = pImpl->membranePotentialTrace * (1.0f - voltageFilter) +
        pImpl->efficacy * voltageFilter;
    
    // Voltage-dependent conductance modulation
    pImpl->voltageDependentG = pImpl->efficacy * (1.0f + pImpl->membranePotentialTrace * 0.5f);
}

void Synapse::updateMetabolicEnergy(TimestepDuration dt) {
    // Metabolic energy dynamics
    float energyChange = 0.0f;
    
    // Energy cost for synaptic activity
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        energyChange -= pImpl->energyCostPerSpike * dt;
    }
    
    // Energy harvest from presynaptic spikes
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->metabolicEnergy > 0.0f) {
        energyChange += pImpl->energyHarvestPerSpike * pImpl->metabolicEnergy * 0.5f * dt;
    }
    
    // Energy efficiency factor
    energyChange *= pImpl->energyEfficiency;
    
    pImpl->metabolicEnergy += energyChange;
    pImpl->metabolicEnergy = std::clamp(
        pImpl->metabolicEnergy, 
        Impl::MIN_ENERGY, 
        Impl::MAX_ENERGY
    );
}

void Synapse::updateEnhancedSTP(Timestamp currentTime, TimestepDuration dt) {
    // Enhanced STP with vesicle depletion model (Markram-Segev)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float deltaT = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        
        // Facilitation: residual calcium buildup
        pImpl->shortTermFacilitation = std::min(
            Impl::STP_U_MAX,
            pImpl->shortTermFacilitation + pImpl->facilitationIncrement - 
            pImpl->shortTermFacilitation * dt / pImpl->facilitationTimeConstant
        );
        
        // Depression: vesicle depletion
        pImpl->shortTermDepression = std::max(
            Impl::MIN_RESOURCE_POOL,
            pImpl->shortTermDepression * std::exp(-dt / pImpl->depressionTimeConstant)
        );
    }
    
    // Recovery towards baseline
    float recoveryRate = 1.0f / pImpl->depressionTimeConstant;
    pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * recoveryRate * dt;
    pImpl->shortTermFacilitation *= std::exp(-dt / pImpl->facilitationTimeConstant);
}

void Synapse::implementAdvancedSTDP(Timestamp currentTime) {
    if (!pImpl->plasticityFlags.stdp) return;
    
    // Analyze recent spike pairs for STDP
    std::vector<Timestamp> effectivePreSpikes;
    std::vector<Timestamp> effectivePostSpikes;
    
    // Filter spikes for plasticity (keep recent ones)
    for (auto& spikeTime : pImpl->preSpikeHistory) {
        if (currentTime - spikeTime <= 100.0f) {  // 100ms window
            effectivePreSpikes.push_back(spikeTime);
        }
    }
    
    for (auto& spikeTime : pImpl->postSpikeHistory) {
        if (currentTime - spikeTime <= 100.0f) {  // 100ms window
            effectivePostSpikes.push_back(spikeTime);
        }
    }
    
    // Implement triplet STDP rules
    applyTripletSTDP(effectivePreSpikes, effectivePostSpikes, currentTime);
    
    // Implement classic STDP rules (pre-post and post-pre)
    applyClassicSTDP(effectivePreSpikes, effectivePostSpikes, currentTime);
}

void Synapse::applyTripletSTDP(const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, Timestamp currentTime) {
    // Triplet STDP: pre-post-post and post-post-pre interactions
    
    // Pre-post-post triplets (causal strengthening)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        float delta_t_prepost = static_cast<float>(preSpikes[i] - currentTime);
        if (delta_t_prepost >= 0.0f && delta_t_prepost <= 20.0f) {  // 20ms window
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                float delta_t_postpost = static_cast<float>(postSpikes[j] - preSpikes[i]);
                if (delta_t_postpost >= 0.0f && delta_t_postpost <= 20.0f) {
                    // Pre-post-post triplet rule
                    float weightChange = pImpl->tripletLearningRatePre * 
                        std::exp(-delta_t_postpost / pImpl->tripletTau);
                    weightChange *= pImpl->efficacy;
                    pImpl->weight += weightChange * pImpl->stdpMaxWeightChange;
                }
            }
        }
    }
    
    // Post-post-pre triplets (anti-causal depression)
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        float delta_t_postpre = static_cast<float>(currentTime - postSpikes[i]);
        if (delta_t_postpre >= 0.0f && delta_t_postpre <= 20.0f) {
            for (size_t j = 0; j < preSpikes.size(); ++j) {
                float delta_t_prepre = static_cast<float>(preSpikes[j] - postSpikes[i]);
                if (delta_t_prepre >= 20.0f) {  // Different time window
                    // Post-post-pre triplet rule
                    float weightChange = pImpl->tripletLearningRatePrePost * 
                        std::exp(-(delta_t_postpre - 20.0f) / pImpl->tripletTau);
                    weightChange *= pImpl->efficacy;
                    pImpl->weight -= weightChange * pImpl->stdpMaxWeightChange;
                }
            }
        }
    }
}

void Synapse::applyClassicSTDP(const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, Timestamp currentTime) {
    // Classic STDP: pre-post and post-pre interactions
    
    // Pre-post interactions (causal strengthening)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        float delta_t = static_cast<float>(currentTime - preSpikes[i]);
        if (delta_t >= 0.0f && delta_t <= 50.0f) {  // 50ms window
            float weightChange = pImpl->stdpLearningRatePre * 
                std::exp(-delta_t / pImpl->stdpTauPre);
            weightChange *= pImpl->efficacy;
            pImpl->weight += weightChange * pImpl->stdpMaxWeightChange;
        }
    }
    
    // Post-pre interactions (anti-causal depression)
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        float delta_t = static_cast<float>(postSpikes[i] - currentTime);
        if (delta_t >= 0.0f && delta_t <= 50.0f) {  // 50ms window
            float weightChange = pImpl->stdpLearningRatePost * 
                std::exp(-delta_t / pImpl->stdpTauPost);
            weightChange *= pImpl->efficacy;
            pImpl->weight -= weightChange * pImpl->stdpMaxWeightChange;
        }
    }
}

void Synapse::updateEligibilityTrace(float decayRate) {
    if (!pImpl->plasticityFlags.reward_modulated) return;
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    
    // Ensure trace doesn't become too small
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

void Synapse::updateEfficacy(Timestamp currentTime, TimestepDuration dt) {
    // Synaptic efficacy modulation based on usage history
    float efficacyChange = 0.0f;
    
    // Use-dependent increase in efficacy
    efficacyChange += pImpl->useHistory * 0.001f * dt;
    
    // Activity-dependent increase
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        efficacyChange += 0.05f * dt;
    }
    
    // Decay towards baseline
    efficacyChange -= (pImpl->efficacy - pImpl->efficacyBaseline) * 
        dt / pImpl->efficacyTimeConstant;
    
    pImpl->efficacy += efficacyChange;
    pImpl->efficacy = std::clamp(pImpl->efficacy, 0.0f, 2.0f);
}

void Synapse::applyStochasticNoise(Timestamp currentTime, TimestepDuration dt) {
    // Add stochastic noise to weight updates
    static float noisePhase = 0.0f;
    noisePhase += 0.1f * dt;
    
    // Generate colored noise (correlated in time)
    pImpl->currentNoiseValue = pImpl->currentNoiseValue * 0.9f + 
        std::sin(noisePhase) * pImpl->weightNoiseStdDev * 0.1f;
    
    // Apply noise to weight with rate limiting
    if (currentTime - pImpl->lastPreSpikeTime <= 10.0f) {  // Only during recent activity
        pImpl->weight += pImpl->currentNoiseValue * 0.01f;
    }
}

void Synapse::updateRefractoryInteractions(Timestamp currentTime) {
    // Interaction with postsynaptic refractory period
    if (pImpl->lastPostSpikeTime >= 0.0f) {
        float timeSincePostSpike = static_cast<float>(currentTime - pImpl->lastPostSpikeTime);
        
        // Refractory synergy factor
        float refractoryFactor = std::exp(-timeSincePostSpike / pImpl->refractoryTimeConstant);
        
        // Modulate synaptic efficacy based on refractory state
        pImpl->efficacy *= (1.0f - pImpl->refractorySynergy * refractoryFactor);
    }
}

void Synapse::applyHomeostaticAdaptation(Timestamp currentTime) {
    // Homeostatic plasticity for weight stability
    float homeostaticError = pImpl->homeostaticTarget - pImpl->weight;
    
    // Apply homeostatic adjustment
    pImpl->weight += homeostaticError * pImpl->homeostaticLearningRate;
    
    // Adaptation variable tracks recent plasticity history
    pImpl->adaptationVariable += std::abs(homeostaticError) * 0.001f;
    
    // Cap adaptation variable
    if (pImpl->adaptationVariable > 1.0f) {
        pImpl->adaptationVariable = 1.0f;
    }
}

void Synapse::updateStructuralPlasticityPotential(Timestamp currentTime, TimestepDuration dt) {
    // Update potential for structural changes (synaptogenesis/pruning)
    
    float structuralInput = 0.0f;
    
    // Activity-dependent structural potential increase
    if (pImpl->lastPreSpikeTime >= 0.0f && pImpl->metabolicEnergy > 0.0f) {
        structuralInput += pImpl->efficacy * pImpl->metabolicEnergy * 0.01f;
    }
    
    // Calcium-dependent structural changes
    if (pImpl->calciumConcentration > Impl::CALCIUM_CONCENTRATION_PEAK * 0.5f) {
        structuralInput += (pImpl->calciumConcentration - 
            Impl::CALCIUM_CONCENTRATION_PEAK * 0.5f) * 0.001f;
    }
    
    // Decay structural potential
    pImpl->structuralPotential += structuralInput * dt - 
        pImpl->structuralPotential * 0.001f * dt;
    
    // Clamp structural potential
    pImpl->structuralPotential = std::clamp(pImpl->structuralPotential, 0.0f, 1.0f);
}

void Synapse::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // 1. Update calcium dynamics (calcium dynamics for plasticity induction)
    updateCalciumDynamics(dt);
    
    // 2. Update synaptic resource pools
    updateResourcePools(dt);
    
    // 3. Update voltage-dependent conductances
    updateVoltageDependencies(currentTime, dt);
    
    // 4. Update metabolic energy
    updateMetabolicEnergy(dt);
    
    // 5. Update short-term plasticity (enhanced Tsodyks-Markram model)
    updateEnhancedSTP(currentTime, dt);
    
    // 6. Implement advanced STDP (pre-post and triplet interactions)
    implementAdvancedSTDP(currentTime);
    
    // 7. Update eligibility traces for reward-modulated learning
    updateEligibilityTrace(dt);
    
    // 8. Update synaptic efficacy (use-dependent modulation)
    updateEfficacy(currentTime, dt);
    
    // 9. Add stochastic noise to weight updates
    applyStochasticNoise(currentTime, dt);
    
    // 10. Update refractory period interactions
    updateRefractoryInteractions(currentTime);
    
    // 11. Implement homeostatic adaptation
    applyHomeostaticAdaptation(currentTime);
    
    // 12. Update structural plasticity potential
    updateStructuralPlasticityPotential(currentTime, dt);
    
    // 13. Apply weight bounds and clamping
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // 14. Update calcium concentration bounds
    pImpl->calciumConcentration = std::clamp(
        pImpl->calciumConcentration, 
        Impl::MIN_CALCIUM, 
        Impl::MAX_CALCIUM
    );
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize advanced fields
    pImpl->calciumConcentration = Impl::MIN_CALCIUM;
    pImpl->calciumTrapped = 0.0f;
    pImpl->vesiclePool = Impl::MAX_RESOURCE_POOL;
    pImpl->receptorPool = Impl::MAX_RESOURCE_POOL;
    pImpl->metabolicEnergy = Impl::MAX_ENERGY * 0.7f;
    
    // Initialize STP state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Initialize plasticity state
    pImpl->eligibilityTrace = 0.0f;
    pImpl->useHistory = 0.0f;
    pImpl->adaptationVariable = 0.0f;
    pImpl->structuralPotential = 0.0f;
    pImpl->membranePotentialTrace = 0.0f;
    
    // Initialize metaplasticity parameters with some variability
    pImpl->facilitationU = rng.uniformReal(0.3f, 0.7f);
    pImpl->depressionU = rng.uniformReal(0.3f, 0.7f);
    pImpl->facilitationTau = rng.uniformReal(50.0f, 150.0f);
    pImpl->depressionTau = rng.uniformReal(100.0f, 300.0f);
    
    // Initialize STDP parameters
    pImpl->stdpLearningRatePre = rng.uniformReal(Impl::STDV_STDP_LR * 0.5f, Impl::STDV_STDP_LR * 1.5f);
    pImpl->stdpLearningRatePost = rng.uniformReal(Impl::STDV_STDP_LR * 0.5f, Impl::STDV_STDP_LR * 1.5f);
    
    // Initialize noise
    pImpl->currentNoiseValue = rng.normal(0.0f, pImpl->weightNoiseStdDev);
}

} // namespace nlm
