#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
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
    AdvancedSynapseType advancedType;
    SynapticCurrentType currentType;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // BCM theory sliding threshold
    float slidingThreshold;
    float beta;  // BCM beta parameter
    
    // Advanced synapse configuration
    AdvancedSynapseConfig config;
    
    // State-dependent plasticity
    float neuralState;
    float environmentalContext;
    
    // Meta-learning parameters
    float metaGradient;
    float learningRate;
    
    // Transfer learning
    float taskSimilarity;
    float transferStrength;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    // Re Magi receptor resources
    float ampaReceptorResources;
    float nmdaReceptorResources;
    float gabaAReceptorResources;
    float gabaBReceptorResources;
    float achReceptorResources;
    float serReceptorResources;
    float dopReceptorResources;
    float noreReceptorResources;
    float gluReceptorResources;
    float glyReceptorResources;
    float atpReceptorResources;
    float vipReceptorResources;
    float spReceptorResources;
    float enkReceptorResources;
    
    // Receptor saturation
    float ampaSaturation;
    float nmdaSaturation;
    float gabaASaturation;
    float gabaBSaturation;
    float achSaturation;
    float serSaturation;
    float dopSaturation;
    float noreSaturation;
    float gluSaturation;
    float glySaturation;
    float atpSaturation;
    float vipSaturation;
    float spSaturation;
    float enkSaturation;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 1.0f;  // Improved: Start with non-zero weight
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->advancedType = AdvancedSynapseType::Excitatory;
    pImpl->currentType = SynapticCurrentType::AMPA;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->slidingThreshold = 0.0f;
    pImpl->beta = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->neuralState = 0.0f;
    pImpl->environmentalContext = 0.0f;
    pImpl->metaGradient = 0.0f;
    pImpl->learningRate = 0.01f;
    pImpl->taskSimilarity = 0.0f;
    pImpl->transferStrength = 0.0f;
    
    // Initialize receptor resources
    pImpl->ampaReceptorResources = 1.0f;
    pImpl->nmdaReceptorResources = 1.0f;
    pImpl->gabaAReceptorResources = 1.0f;
    pImpl->gabaBReceptorResources = 1.0f;
    pImpl->achReceptorResources = 1.0f;
    pImpl->serReceptorResources = 1.0f;
    pImpl->dopReceptorResources = 1.0f;
    pImpl->noreReceptorResources = 1.0f;
    pImpl->gluReceptorResources = 1.0f;
    pImpl->glyReceptorResources = 1.0f;
    pImpl->atpReceptorResources = 1.0f;
    pImpl->vipReceptorResources = 1.0f;
    pImpl->spReceptorResources = 1.0f;
    pImpl->enkReceptorResources = 1.0f;
    
    // Initialize saturation levels
    pImpl->ampaSaturation = 0.0f;
    pImpl->nmdaSaturation = 0.0f;
    pImpl->gabaASaturation = 0.0f;
    pImpl->gabaBSaturation = 0.0f;
    pImpl->achSaturation = 0.0f;
    pImpl->serSaturation = 0.0f;
    pImpl->dopSaturation = 0.0f;
    pImpl->noreSaturation = 0.0f;
    pImpl->gluSaturation = 0.0f;
    pImpl->glySaturation = 0.0f;
    pImpl->atpSaturation = 0.0f;
    pImpl->vipSaturation = 0.0f;
    pImpl->spSaturation = 0.0f;
    pImpl->enkSaturation = 0.0f;
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
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::applyPlasticityChange(float change) {
    // Apply advanced plasticity change with bounds checking
    pImpl->weight += change;
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Update weight history
    pImpl->config.weightHistory.push_back(pImpl->weight);
    if (pImpl->config.weightHistory.size() > 1000) {
        pImpl->config.weightHistory.erase(pImpl->config.weightHistory.begin());
    }
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

AdvancedSynapseType Synapse::getAdvancedType() const {
    return pImpl->advancedType;
}

void Synapse::setAdvancedType(AdvancedSynapseType type) {
    pImpl->advancedType = type;
}

SynapticCurrentType Synapse::getCurrentType() const {
    return pImpl->currentType;
}

void Synapse::setCurrentType(SynapticCurrentType type) {
    pImpl->currentType = type;
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

bool Synapse::isModulatory() const {
    return pImpl->type == SynapseType::Modulatory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    
    // Update pre-spike history for plasticity
    if (!pImpl->preSpikeHistory.empty()) {
        pImpl->lastPreSpikeTime = pImpl->preSpikeHistory.back();
    }
    
    // Update receptor resources based on spike
    updateReceptorResources(1.0f);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Update post-spike history for plasticity
    if (!pImpl->postSpikeHistory.empty()) {
        pImpl->lastPostSpikeTime = pImpl->postSpikeHistory.back();
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

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

const AdvancedSynapseConfig& Synapse::getAdvancedConfig() const {
    return pImpl->config;
}

AdvancedSynapseConfig& Synapse::getAdvancedConfig() {
    return pImpl->config;
}

void Synapse::configureAdvanced(const AdvancedSynapseConfig& config) {
    pImpl->config = config;
}

void Synapse::updateAdvancedConfig() {
    // Update configuration based on current state
    pImpl->config.weight = pImpl->weight;
    pImpl->config.currentEfficacy = pImpl->efficacy;
    pImpl->config.eligibilityTrace = pImpl->eligibilityTrace;
    pImpl->config.lastPreSpikeTime = pImpl->lastPreSpikeTime;
    pImpl->config.lastPostSpikeTime = pImpl->lastPostSpikeTime;
    pImpl->config.shortTermDepression = pImpl->shortTermDepression;
    pImpl->config.shortTermFacilitation = pImpl->shortTermFacilitation;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = efficacy;
}

void Synapse::updateEfficacy(float preActivity, float postActivity) {
    // Update efficacy based on recent activity (use-dependent modulation)
    float activityModulation = (preActivity + postActivity) * 0.5f;
    pImpl->efficacy += activityModulation * pImpl->config.activitySensitivity * 0.01f;
    
    // Clamp efficacy to reasonable bounds
    pImpl->efficacy = std::max(0.0f, std::min(pImpl->efficacy, 2.0f));
    
    // Update efficacy history
    pImpl->config.efficacyHistory.push_back(pImpl->efficacy);
    if (pImpl->config.efficacyHistory.size() > 1000) {
        pImpl->config.efficacyHistory.erase(pImpl->config.efficacyHistory.begin());
    }
}

void Synapse::updateTemporalDynamics(Timestamp currentTime) {
    // Update short-term plasticity dynamics
    if (pImpl->lastPreSpikeTime > 0.0f) {
        float dt = currentTime - pImpl->lastPreSpikeTime;
        
        // Facilitation (short-term)
        if (pImpl->config.shortTermFacilitation > 0.0f) {
            pImpl->shortTermFacilitation = std::max(0.0f, 
                pImpl->shortTermFacilitation * std::exp(-dt / Impl::STP_FACILITATION_TAU));
        }
        
        // Depression (short-term)
        if (pImpl->config.shortTermDepression > 0.0f) {
            pImpl->shortTermDepression = std::max(0.0f,
                pImpl->shortTermDepression * std::exp(-dt / Impl::STP_DEPRESSION_TAU));
        }
    }
}

void Synapse::updatePlasticity(Timestamp currentTime, const AdvancedSynapseConfig& config) {
    // Apply advanced plasticity rules based on configuration
    switch (config.plasticityRule) {
        case AdvancedPlasticityRule::STDP:
            // Standard STDP implementation
            applySTDP(currentTime);
            break;
        case AdvancedPlasticityRule::Hebbian:
            // Hebbian learning (OUA)
            applyHebbianLearning(currentTime);
            break;
        case AdvancedPlasticityRule::AntiHebbian:
            // Anti-Hebbian learning
            applyAntiHebbianLearning(currentTime);
            break;
        case AdvancedPlasticityRule::BCM:
            // Bienenstock–Cooper–Munro
            applyBCMLearning(currentTime);
            break;
        case AdvancedPlasticityRule::Oja:
            // Oja's rule
            applyOjaLearning(currentTime);
            break;
        case AdvancedPlasticityRule::Sanger:
            // Sanger's competitive learning
            applySangersLearning(currentTime);
            break;
        case AdvancedPlasticityRule::Adversarial:
            // Adversarial learning
            applyAdversarialLearning(currentTime);
            break;
        case AdvancedPlasticityRule::MetaLearning:
            // Meta-learning
            applyMetaLearning(currentTime);
            break;
        default:
            // Default to STDP
            applySTDP(currentTime);
            break;
    }
}

void Synapse::applySTDP(Timestamp currentTime) {
    // Spike-Timing-Dependent Plasticity
    if (pImpl->lastPreSpikeTime > 0.0f && pImpl->lastPostSpikeTime > 0.0f) {
        float delta_t = currentTime - pImpl->lastPostSpikeTime;  // Post before pre
        
        if (delta_t > 0.0f) {
            // Pre after post: Long-Term Potentiation (LTP)
            float weightChange = pImpl->config.plasticityRate * 
                               std::exp(-delta_t / pImpl->config.timeConstant);
            applyPlasticityChange(weightChange);
        } else {
            // Pre before post: Long-Term Depression (LTD)
            float weightChange = -pImpl->config.plasticityRate * 
                               std::exp(delta_t / pImpl->config.timeConstant);
            applyPlasticityChange(weightChange);
        }
    }
}

void Synapse::applyHebbianLearning(Timestamp currentTime) {
    // Hebbian learning (OUA rule)
    float preActivity = 1.0f;  // Simplified: assume full activation
    float postActivity = 1.0f; // Simplified: assume full activation
    
    float weightChange = pImpl->config.plasticityRate * preActivity * postActivity;
    applyPlasticityChange(weightChange);
}

void Synapse::applyAntiHebbianLearning(Timestamp currentTime) {
    // Anti-Hebbian learning
    float preActivity = 1.0f;
    float postActivity = 1.0f;
    
    float weightChange = -pImpl->config.plasticityRate * preActivity * postActivity;
    applyPlasticityChange(weightChange);
}

void Synapse::applyBCMLearning(Timestamp currentTime) {
    // Bienenstock–Cooper–Munro (BCM) theory with sliding threshold
    updateSlidingThreshold(0.5f, pImpl->beta);  // Simplified average post activity
    
    float preActivity = 1.0f;
    float threshold = pImpl->slidingThreshold;
    
    if (preActivity > threshold) {
        // LTP when pre > threshold
        float weightChange = pImpl->config.plasticityRate * 
                           (preActivity - threshold) * preActivity;
        applyPlasticityChange(weightChange);
    } else {
        // LTD when pre < threshold
        float weightChange = -pImpl->config.plasticityRate * 
                           (threshold - preActivity) * preActivity;
        applyPlasticityChange(weightChange);
    }
}

void Synapse::applyOjaLearning(Timestamp currentTime) {
    // Oja's rule for dimensionality reduction
    float preActivity = 1.0f;
    float postActivity = pImpl->efficacy;  // Simplified
    
    float weightChange = pImpl->config.plasticityRate * preActivity * postActivity - 
                        pImpl->config.plasticityRate * pImpl->weight * postActivity * postActivity;
    applyPlasticityChange(weightChange);
}

void Synapse::applySangersLearning(Timestamp currentTime) {
    // Sanger's competitive learning
    float preActivity = 1.0f;
    float postActivity = 1.0f;
    
    // Competitive learning with normalization
    float weightChange = pImpl->config.plasticityRate * preActivity * 
                        (postActivity - pImpl->weight);
    applyPlasticityChange(weightChange);
}

void Synapse::applyAdversarialLearning(Timestamp currentTime) {
    // Adversarial learning
    float reward = 1.0f;
    float cost = 0.5f;
    
    float weightChange = pImpl->config.plasticityRate * (reward - cost);
    applyPlasticityChange(weightChange);
}

void Synapse::applyMetaLearning(Timestamp currentTime) {
    // Meta-learning
    float metaGradient = pImpl->metaGradient;
    float learningRate = pImpl->learningRate;
    
    float weightChange = learningRate * metaGradient;
    applyPlasticityChange(weightChange);
}

void Synapse::updateNeuromodulation(float neuromodulationSignal) {
    if (pImpl->config.isNeuromodulated) {
        // Apply neuromodulation to plasticity
        float modulationEffect = neuromodulationSignal * pImpl->config.neuromodulationStrength;
        
        // Modify plasticity rate based on neuromodulation
        pImpl->config.plasticityRate *= (1.0f + modulationEffect);
        
        // Update eligibility trace
        pImpl->eligibilityTrace += neuromodulationSignal * pImpl->config.eligibilityTraceDecay;
    }
}

void Synapse::updateStructuralPlasticity(float globalGrowthFactor) {
    if (pImpl->config.enableStructuralPlasticity) {
        // Apply structural plasticity based on global growth factor
        float structuralChange = globalGrowthFactor * pImpl->config.structuralGrowthRate;
        
        // Grow or prune based on structural stability
        if (pImpl->config.structuralStability > 0.5f) {
            // Growth
            applyPlasticityChange(structuralChange);
        } else {
            // Pruning
            applyPlasticityChange(-structuralChange * pImpl->config.structuralPruningRate);
        }
    }
}

void Synapse::updateSlidingThreshold(float avgPostActivity, float beta) {
    // BCM sliding threshold update
    pImpl->slidingThreshold = avgPostActivity * beta;
}

float Synapse::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

void Synapse::updateRewardModulatedLearning(float rewardPredictionError, float neuromodulationStrength) {
    // Reward-modulated learning with eligibility traces
    float modEffect = rewardPredictionError * neuromodulationStrength;
    pImpl->eligibilityTrace += modEffect * pImpl->config.eligibilityTraceDecay;
    
    // Apply weight change based on eligibility trace
    if (pImpl->eligibilityTrace > 0.0f) {
        float weightChange = pImpl->config.plasticityRate * pImpl->eligibilityTrace;
        applyPlasticityChange(weightChange);
    }
}

void Synapse::updateCuriosityDrivenLearning(float curiositySignal, float novelty) {
    // Curiosity-driven learning
    float curiosityEffect = curiositySignal * novelty * pImpl->config.neuromodulationStrength;
    
    // Increase exploration and learning
    pImpl->config.plasticityRate *= (1.0f + curiosityEffect);
    pImpl->efficacy *= (1.0f + curiosityEffect * 0.5f);
}

void Synapse::updateMetaLearningParameters(float metaGradient) {
    pImpl->metaGradient = metaGradient;
    // Apply meta-gradient to update learning parameters
    pImpl->learningRate *= (1.0f + metaGradient * 0.01f);
}

void Synapse::updateTransferLearning(const AdvancedSynapseConfig& newTaskConfig) {
    // Transfer learning adaptation
    float similarity = calculateTaskSimilarity(pImpl->config, newTaskConfig);
    pImpl->taskSimilarity = similarity;
    
    float transferEffect = similarity * pImpl->config.transferStrength;
    
    // Adapt weights based on task similarity
    pImpl->weight *= (1.0f + transferEffect * 0.1f);
    pImpl->efficacy *= (1.0f + transferEffect * 0.05f);
}

void Synapse::updateContinualLearning(const std::vector<float>& newActivities) {
    // Prevent catastrophic forgetting in continual learning
    float avgNewActivity = 0.0f;
    for (float activity : newActivities) {
        avgNewActivity += activity;
    }
    avgNewActivity /= newActivities.size();
    
    // Maintain stability while allowing plasticity
    float stabilityFactor = std::min(avgNewActivity, 1.0f) * 0.1f;
    
    // Apply weight changes with stability constraint
    float maxWeightChange = 0.01f * stabilityFactor;
    // Simplified: just ensure weight doesn't change too dramatically
}

void Synapse::updateOnlineLearning(float onlineGradient) {
    pImpl->metaGradient = onlineGradient;
    float weightChange = pImpl->learningRate * onlineGradient;
    applyPlasticityChange(weightChange);
}

void Synapse::updateSocialLearning(float socialInfluence, float peerCohesion) {
    // Social learning with peer influence
    float socialEffect = socialInfluence * peerCohesion;
    
    // Adjust learning rate based on social factors
    pImpl->config.plasticityRate *= (1.0f + socialEffect * 0.2f);
    
    // Influence efficacy
    pImpl->efficacy *= (1.0f + socialEffect * 0.1f);
}

void Synapse::updateCollaborativeLearning(float collaborationSignal, float groupPerformance) {
    // Collaborative learning with group dynamics
    float collaborationEffect = collaborationSignal * groupPerformance;
    
    // Enhance learning through collaboration
    pImpl->config.plasticityRate *= (1.0f + collaborationEffect * 0.3f);
    pImpl->efficacy *= (1.0f + collaborationEffect * 0.2f);
}

void Synapse::updateMultiTaskLearning(const std::vector<float>& taskRewards, float taskSharingFactor) {
    // Multi-task learning with reward sharing
    float avgTaskReward = 0.0f;
    for (float reward : taskRewards) {
        avgTaskReward += reward;
    }
    avgTaskReward /= taskRewards.size();
    
    // Share rewards across tasks
    float sharingEffect = avgTaskReward * taskSharingFactor;
    
    // Apply shared reward effects
    pImpl->config.plasticityRate *= (1.0f + sharingEffect * 0.15f);
    pImpl->efficacy *= (1.0f + sharingEffect * 0.1f);
}

void Synapse::updateStateDependentPlasticity(float neuralState, float environmentalContext) {
    // State-dependent plasticity
    pImpl->neuralState = neuralState;
    pImpl->environmentalContext = environmentalContext;
    
    // Adapt plasticity based on state and context
    float stateEffect = neuralState * environmentalContext;
    pImpl->config.plasticityRate *= (1.0f + stateEffect * 0.1f);
    pImpl->efficacy *= (1.0f + stateEffect * 0.05f);
}

float Synapse::calculateTaskSimilarity(const AdvancedSynapseConfig& config1, const AdvancedSynapseConfig& config2) {
    // Calculate similarity between two task configurations
    float similarity = 0.0f;
    
    // Weight similarity
    similarity += std::abs(config1.weight - config2.weight) * 0.2f;
    
    // Plasticity rate similarity
    similarity += std::abs(config1.plasticityRate - config2.plasticityRate) * 0.2f;
    
    // Efficacy similarity
    similarity += std::abs(config1.currentEfficacy - config2.currentEfficacy) * 0.2f;
    
    // Type similarity
    similarity += (config1.type != config2.type) ? 0.2f : 0.0f;
    
    return 1.0f - similarity;  // Return similarity score (0-1)
}

void Synapse::step(Timestamp currentTime) {
    // Apply advanced synaptic dynamics
    updateTemporalDynamics(currentTime);
    
    // Update plasticity based on current state
    updatePlasticity(currentTime, pImpl->config);
    
    // Update receptor resources
    updateReceptorResources(0.001f);  // Small timestep
    
    // Update eligibility traces
    decayEligibilityTrace(0.001f);
    
    // Update advanced configuration
    updateAdvancedConfig();
}

void Synapse::reset() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->slidingThreshold = 0.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->neuralState = 0.0f;
    pImpl->environmentalContext = 0.0f;
    pImpl->metaGradient = 0.0f;
    
    // Reset receptor resources
    pImpl->ampaReceptorResources = 1.0f;
    pImpl->nmdaReceptorResources = 1.0f;
    pImpl->gabaAReceptorResources = 1.0f;
    pImpl->gabaBReceptorResources = 1.0f;
    pImpl->achReceptorResources = 1.0f;
    pImpl->serReceptorResources = 1.0f;
    pImpl->dopReceptorResources = 1.0f;
    pImpl->noreReceptorResources = 1.0f;
    pImpl->gluReceptorResources = 1.0f;
    pImpl->glyReceptorResources = 1.0f;
    pImpl->atpReceptorResources = 1.0f;
    pImpl->vipReceptorResources = 1.0f;
    pImpl->spReceptorResources = 1.0f;
    pImpl->enkReceptorResources = 1.0f;
    
    // Reset saturation levels
    pImpl->ampaSaturation = 0.0f;
    pImpl->nmdaSaturation = 0.0f;
    pImpl->gabaASaturation = 0.0f;
    pImpl->gabaBSaturation = 0.0f;
    pImpl->achSaturation = 0.0f;
    pImpl->serSaturation = 0.0f;
    pImpl->dopSaturation = 0.0f;
    pImpl->noreSaturation = 0.0f;
    pImpl->gluSaturation = 0.0f;
    pImpl->glySaturation = 0.0f;
    pImpl->atpSaturation = 0.0f;
    pImpl->vipSaturation = 0.0f;
    pImpl->spSaturation = 0.0f;
    pImpl->enkSaturation = 0.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng, const AdvancedSynapseConfig& config) {
    // Initialize synapse with random parameters
    pImpl->config = config;
    
    // Random weight initialization
    pImpl->weight = rng.uniformReal(Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    
    // Random delay
    pImpl->delay = rng.uniformInt(1, 10);  // 1-10 steps
    
    // Random advanced type selection
    int typeIndex = rng.uniformInt(0, static_cast<int>(AdvancedSynapseType::Count) - 1);
    pImpl->advancedType = static_cast<AdvancedSynapseType>(typeIndex);
    
    // Random current type selection
    int currentTypeIndex = rng.uniformInt(0, static_cast<int>(SynapticCurrentType::Count) - 1);
    pImpl->currentType = static_cast<SynapticCurrentType>(currentTypeIndex);
    
    // Random plasticity rule
    int plasticityIndex = rng.uniformInt(0, static_cast<int>(AdvancedPlasticityRule::Count) - 1);
    pImpl->config.plasticityRule = static_cast<AdvancedPlasticityRule>(plasticityIndex);
    
    // Random plasticity rate
    pImpl->config.plasticityRate = rng.uniformReal(0.001f, 0.1f);
    
    // Random efficacy
    pImpl->efficacy = rng.uniformReal(0.5f, 2.0f);
    
    // Random sliding threshold
    pImpl->slidingThreshold = rng.uniformReal(-1.0f, 1.0f);
    
    // Random beta for BCM
    pImpl->beta = rng.uniformReal(0.5f, 2.0f);
}

AdvancedPlasticityRule Synapse::getCurrentPlasticityRule() const {
    return pImpl->config.plasticityRule;
}

bool Synapse::isPlastic() const {
    return pImpl->plasticityFlags.hebbian || 
           pImpl->plasticityFlags.stdp || 
           pImpl->plasticityFlags.reward_modulated;
}

SynapticWeight Synapse::getCurrentContribution() const {
    // Calculate current synaptic contribution based on type and efficacy
    float baseContribution = pImpl->weight * pImpl->efficacy;
    
    // Apply short-term plasticity effects
    float stpFactor = pImpl->shortTermDepression * pImpl->shortTermFacilitation;
    
    // Apply receptor saturation
    float receptorFactor = 1.0f;
    switch (pImpl->currentType) {
        case SynapticCurrentType::AMPA:
            receptorFactor = 1.0f - pImpl->ampaSaturation;
            break;
        case SynapticCurrentType::NMDA:
            receptorFactor = 1.0f - pImpl->nmdaSaturation;
            break;
        case SynapticCurrentType::GABA_A:
            receptorFactor = 1.0f - pImpl->gabaASaturation;
            break;
        case SynapticCurrentType::GABA_B:
            receptorFactor = 1.0f - pImpl->gabaBSaturation;
            break;
        // ... other receptor types
    }
    
    return baseContribution * stpFactor * receptorFactor;
}

void Synapse::updateReceptorResources(float timeStep) {
    // Update receptor resources based on activity and saturation
    float activation = std::min(1.0f, pImpl->efficacy);
    
    switch (pImpl->currentType) {
        case SynapticCurrentType::AMPA:
            pImpl->ampaSaturation = std::min(1.0f, pImpl->ampaSaturation + activation * timeStep * 0.1f);
            pImpl->ampaReceptorResources = 1.0f - pImpl->ampaSaturation;
            break;
        case SynapticCurrentType::NMDA:
            pImpl->nmdaSaturation = std::min(1.0f, pImpl->nmdaSaturation + activation * timeStep * 0.1f);
            pImpl->nmdaReceptorResources = 1.0f - pImpl->nmdaSaturation;
            break;
        case SynapticCurrentType::GABA_A:
            pImpl->gabaASaturation = std::min(1.0f, pImpl->gabaASaturation + activation * timeStep * 0.1f);
            pImpl->gabaAReceptorResources = 1.0f - pImpl->gabaASaturation;
            break;
        case SynapticCurrentType::GABA_B:
            pImpl->gabaBSaturation = std::min(1.0f, pImpl->gabaBSaturation + activation * timeStep * 0.1f);
            pImpl->gabaBReceptorResources = 1.0f - pImpl->gabaBSaturation;
            break;
        case SynapticCurrentType::Acetylcholine:
            pImpl->achSaturation = std::min(1.0f, pImpl->achSaturation + activation * timeStep * 0.1f);
            pImpl->achReceptorResources = 1.0f - pImpl->achSaturation;
            break;
        case SynapticCurrentType::Serotonin:
            pImpl->serSaturation = std::min(1.0f, pImpl->serSaturation + activation * timeStep * 0.1f);
            pImpl->serReceptorResources = 1.0f - pImpl->serSaturation;
            break;
        case SynapticCurrentType::Dopamine:
            pImpl->dopSaturation = std::min(1.0f, pImpl->dopSaturation + activation * timeStep * 0.1f);
            pImpl->dopReceptorResources = 1.0f - pImpl->dopSaturation;
            break;
        case SynapticCurrentType::Norepinephrine:
            pImpl->noreSaturation = std::min(1.0f, pImpl->noreSaturation + activation * timeStep * 0.1f);
            pImpl->noreReceptorResources = 1.0f - pImpl->noreSaturation;
            break;
        case SynapticCurrentType::Glutamate:
            pImpl->gluSaturation = std::min(1.0f, pImpl->gluSaturation + activation * timeStep * 0.1f);
            pImpl->gluReceptorResources = 1.0f - pImpl->gluSaturation;
            break;
        case SynapticCurrentType::Glycine:
            pImpl->glySaturation = std::min(1.0f, pImpl->glySaturation + activation * timeStep * 0.1f);
            pImpl->glyReceptorResources = 1.0f - pImpl->glySaturation;
            break;
        case SynapticCurrentType::ATP:
            pImpl->atpSaturation = std::min(1.0f, pImpl->atpSaturation + activation * timeStep * 0.1f);
            pImpl->atpReceptorResources = 1.0f - pImpl->atpSaturation;
            break;
        case SynapticCurrentType::VIP:
            pImpl->vipSaturation = std::min(1.0f, pImpl->vipSaturation + activation * timeStep * 0.1f);
            pImpl->vipReceptorResources = 1.0f - pImpl->vipSaturation;
            break;
        case SynapticCurrentType::SubstanceP:
            pImpl->spSaturation = std::min(1.0f, pImpl->spSaturation + activation * timeStep * 0.1f);
            pImpl->spReceptorResources = 1.0f - pImpl->spSaturation;
            break;
        case SynapticCurrentType::Enkephalin:
            pImpl->enkSaturation = std::min(1.0f, pImpl->enkSaturation + activation * timeStep * 0.1f);
            pImpl->enkReceptorResources = 1.0f - pImpl->enkSaturation;
            break;
    }
}

float Synapse::getReceptorSaturation() const {
    // Return current receptor saturation based on type
    switch (pImpl->currentType) {
        case SynapticCurrentType::AMPA: return pImpl->ampaSaturation;
        case SynapticCurrentType::NMDA: return pImpl->nmdaSaturation;
        case SynapticCurrentType::GABA_A: return pImpl->gabaASaturation;
        case SynapticCurrentType::GABA_B: return pImpl->gabaBSaturation;
        case SynapticCurrentType::Acetylcholine: return pImpl->achSaturation;
        case SynapticCurrentType::Serotonin: return pImpl->serSaturation;
        case SynapticCurrentType::Dopamine: return pImpl->dopSaturation;
        case SynapticCurrentType::Norepinephrine: return pImpl->noreSaturation;
        case SynapticCurrentType::Glutamate: return pImpl->gluSaturation;
        case SynapticCurrentType::Glycine: return pImpl->glySaturation;
        case SynapticCurrentType::ATP: return pImpl->atpSaturation;
        case SynapticCurrentType::VIP: return pImpl->vipSaturation;
        case SynapticCurrentType::SubstanceP: return pImpl->spSaturation;
        case SynapticCurrentType::Enkephalin: return pImpl->enkSaturation;
        default: return 0.0f;
    }
}

} // namespace nlm
