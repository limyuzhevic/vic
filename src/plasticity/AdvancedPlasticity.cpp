#include "AdvancedPlasticity.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// RewardModulatedSTDP implementation
struct RewardModulatedSTDP::Impl {
    float learningRate;           // Base learning rate
    float rewardDecayRate;         // Rate of reward signal decay
    float baselineReward;         // Baseline reward level
    float rewardPredictionError;   // RPE signal
    
    // Neuromodulatory effects
    std::vector<float> neuromodulatorEffects;  // Effects for each neuromodulator type
    
    // Eligibility traces
    float eligibilityTrace;       // For reward-modulated learning
    float traceDecay;             // Trace decay rate
    
    // Time constants
    float rewardTimeConstant;     // Reward signal time constant
    
    Impl() : learningRate(0.01f), rewardDecayRate(0.1f), baselineReward(0.1f),
             rewardPredictionError(0.0f), eligibilityTrace(0.0f), traceDecay(0.01f),
             rewardTimeConstant(100.0f) {
        neuromodulatorEffects = std::vector<float>(8, 1.0f); // One per neuromodulator type
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Standard STDP-like update
    float totalDelta = 0.0f;
    float tau = 20.0f; // STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->learningRate * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -pImpl->learningRate * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply reward modulation
    totalDelta *= (1.0f + pImpl->rewardPredictionError);
    
    // Apply neuromodulatory effects
    for (size_t i = 0; i < pImpl->neuromodulatorEffects.size(); ++i) {
        totalDelta *= pImpl->neuromodulatorEffects[i];
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace = pImpl->eligibilityTrace * (1.0f - pImpl->traceDecay) + totalDelta;
    
    // Apply weight change if eligibility trace is significant
    if (std::abs(pImpl->eligibilityTrace) > 1e-6f) {
        applyWeightChange(synapse, pImpl->eligibilityTrace);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setRewardPredictionError(float rpe) {
    pImpl->rewardPredictionError = rpe;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void RewardModulatedSTDP::setRewardDecayRate(float decay) {
    pImpl->rewardDecayRate = std::clamp(decay, 0.0f, 1.0f);
}

void RewardModulatedSTDP::setBaselineReward(float baseline) {
    pImpl->baselineReward = baseline;
}

void RewardModulatedSTDP::setNeuromodulator(NeuromodulatorType type, float concentration) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(pImpl->neuromodulatorEffects.size())) {
        pImpl->neuromodulatorEffects[index] = concentration;
    }
}

void RewardModulatedSTDP::setNeuromodulatorEffect(NeuromodulatorType type, float effect) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(pImpl->neuromodulatorEffects.size())) {
        pImpl->neuromodulatorEffects[index] = effect;
    }
}

// HomosynapticPlasticity implementation
struct HomosynapticPlasticity::Impl {
    float homosynapticStrength;   // Strength of homosynaptic plasticity
    float targetWeight;          // Target weight for homeostasis
    float outputDependency;      // Output dependence factor
    
    bool heterosynapticEnabled;   // Whether heterosynaptic plasticity is enabled
    float heterosynapticRange;    // Spatial range for heterosynaptic effects
    float heterosynapticStrength; // Strength of heterosynaptic plasticity
    
    // Network-wide variables
    std::vector<float> averageWeights;  // Average weights across synapses
    std::vector<float> outputActivity;  // Recent output activity levels
    
    Impl() : homosynapticStrength(0.01f), targetWeight(0.0f), outputDependency(0.5f),
             heterosynapticEnabled(true), heterosynapticRange(5.0f),
             heterosynapticStrength(0.005f) {}
};

HomosynapticPlasticity::HomosynapticPlasticity() : pImpl(new Impl) {}

HomosynapticPlasticity::~HomosynapticPlasticity() = default;

void HomosynapticPlasticity::update(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    if (!synapse) return;
    
    // Homosynaptic: depends on postsynaptic activity
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f; // Normalize
    
    // Activity-dependent weight change
    float delta = pImpl->homosynapticStrength * postActivity;
    
    // Homeostatic component (target weight)
    float weightError = pImpl->targetWeight - synapse->getWeight();
    delta += pImpl->outputDependency * weightError;
    
    // Output dependence
    delta *= pImpl->outputDependency;
    
    applyWeightChange(synapse, delta);
}

void HomosynapticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomosynapticPlasticity::getName() const {
    return "HomosynapticPlasticity";
}

void HomosynapticPlasticity::setHomosynapticStrength(float strength) {
    pImpl->homosynapticStrength = std::clamp(strength, 0.0f, 1.0f);
}

void HomosynapticPlasticity::setTargetWeight(float target) {
    pImpl->targetWeight = std::clamp(target, -1.0f, 1.0f);
}

void HomosynapticPlasticity::setOutputDependency(float outputDep) {
    pImpl->outputDependency = std::clamp(outputDep, 0.0f, 1.0f);
}

void HomosynapticPlasticity::setHeterosynapticRange(float range) {
    pImpl->heterosynapticRange = std::max(0.0f, range);
}

void HomosynapticPlasticity::setHeterosynapticStrength(float strength) {
    pImpl->heterosynapticStrength = std::clamp(strength, 0.0f, 1.0f);
}

void HomosynapticPlasticity::enableHeterosynaptic(bool enable) {
    pImpl->heterosynapticEnabled = enable;
}

// SynapticTaggingAndCapture implementation
struct SynapticTaggingAndCapture::Impl {
    float tagPersistence;        // How long tags last
    float captureThreshold;      // Threshold for capture
    float tagDecayRate;           // Rate at which tags decay
    float captureRange;           // Range for capture
    
    bool multiNeuromodulatorEnabled; // Enable multi-neuromodulator integration
    std::vector<float> modulatorWeights; // Weights for each neuromodulator
    
    // Tag and capture state
    struct TagState {
        float tagValue;           // Current tag value
        float captureValue;       // Current capture value
        bool hasTag;              // Whether synapse has a tag
        Timestamp tagTime;        // When tag was set
    };
    
    std::vector<TagState> tagStates; // Per-synapse tag state
    
    Impl() : tagPersistence(100.0f), captureThreshold(0.5f), tagDecayRate(0.01f),
             captureRange(2.0f), multiNeuromodulatorEnabled(false) {
        modulatorWeights = std::vector<float>(8, 1.0f);
    }
};

SynapticTaggingAndCapture::SynapticTaggingAndCapture() : pImpl(new Impl) {}

SynapticTaggingAndCapture::~SynapticTaggingAndCapture() = default;

void SynapticTaggingAndCapture::update(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    if (!synapse) return;
    
    // Get synapse ID for tag state lookup
    SynapseId synId = synapse->getId();
    
    // Simplified tag and capture implementation
    if (preSpikes.size() > 0 && postSpikes.size() > 0) {
        // Check for strong coactivity that sets a tag
        float coactivity = static_cast<float>(std::min(preSpikes.size(), postSpikes.size()));
        
        if (coactivity > pImpl->captureThreshold) {
            // Apply tag
            if (synId < pImpl->tagStates.size()) {
                pImpl->tagStates[synId].tagValue = std::min(1.0f, pImpl->tagStates[synId].tagValue + 0.1f);
                pImpl->tagStates[synId].hasTag = true;
                pImpl->tagStates[synId].tagTime = static_cast<Timestamp>(0.0); // Simplified
            }
        }
        
        // Apply capture with neuromodulatory integration
        if (pImpl->multiNeuromodulatorEnabled) {
            float modulatorEffect = 1.0f;
            for (size_t i = 0; i < pImpl->modulatorWeights.size(); ++i) {
                modulatorEffect *= pImpl->modulatorWeights[i];
            }
            
            if (synId < pImpl->tagStates.size() && pImpl->tagStates[synId].hasTag) {
                float delta = 0.05f * modulatorEffect;
                applyWeightChange(synapse, delta);
            }
        }
    }
}

void SynapticTaggingAndCapture::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* SynapticTaggingAndCapture::getName() const {
    return "SynapticTaggingAndCapture";
}

void SynapticTaggingAndCapture::setTagPersistence(float persistence) {
    pImpl->tagPersistence = std::max(0.0f, persistence);
}

void SynapticTaggingAndCapture::setCaptureThreshold(float threshold) {
    pImpl->captureThreshold = std::max(0.0f, threshold);
}

void SynapticTaggingAndCapture::setTagDecayRate(float decay) {
    pImpl->tagDecayRate = std::clamp(decay, 0.0f, 1.0f);
}

void SynapticTaggingAndCapture::setCaptureRange(float range) {
    pImpl->captureRange = std::max(0.0f, range);
}

void SynapticTaggingAndCapture::setMultiNeuromodulatorIntegration(bool enable) {
    pImpl->multiNeuromodulatorEnabled = enable;
}

void SynapticTaggingAndCapture::setModulatorWeights(const std::vector<float>& weights) {
    pImpl->modulatorWeights = weights;
}

// Metaplasticity implementation
struct Metaplasticity::Impl {
    float plasticityThreshold;   // Threshold for plasticity
    size_t plasticityHistorySize; // Size of plasticity history
    float metaplasticLearningRate; // Learning rate for metaplasticity
    float homeostasisTarget;     // Target for homeostatic regulation
    
    // Plasticity history per synapse
    std::vector<float> plasticityHistory;
    
    // Metaplastic state
    float metaplasticState;      // Current metaplastic state
    float homeostaticPressure;   // Homeostatic pressure
    float historyDecayRate;      // Rate at which history decays
    
    Impl() : plasticityThreshold(0.1f), plasticityHistorySize(100),
             metaplasticLearningRate(0.001f), homeostasisTarget(0.0f),
             metaplasticState(1.0f), homeostaticPressure(0.0f), historyDecayRate(0.01f) {
        plasticityHistory.resize(plasticityHistorySize, 0.0f);
    }
};

Metaplasticity::Metaplasticity() : pImpl(new Impl) {}

Metaplasticity::~Metaplasticity() = default;

void Metaplasticity::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate current plasticity level
    float plasticityLevel = calculatePlasticityLevel(synapse);
    
    // Metaplastic regulation
    float homeostaticError = pImpl->homeostasisTarget - plasticityLevel;
    float metaplasticDelta = pImpl->metaplasticLearningRate * homeostaticError;
    
    // Apply metaplastic weight change to the synapse's learning rate
    float currentLearningRate = synapse->getEfficacy();
    currentLearningRate += metaplasticDelta;
    currentLearningRate = std::clamp(currentLearningRate, 0.0f, 1.0f);
    synapse->setEfficacy(currentLearningRate);
    
    // Record plasticity history
    recordPlasticityHistory(synapse, plasticityLevel);
}

void Metaplasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Scale weight change by current metaplastic state
    float scalingFactor = pImpl->metaplasticState;
    float scaledDelta = delta * scalingFactor;
    
    float newWeight = synapse->getWeight() + scaledDelta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* Metaplasticity::getName() const {
    return "Metaplasticity";
}

void Metaplasticity::setPlasticityThreshold(float threshold) {
    pImpl->plasticityThreshold = std::max(0.0f, threshold);
}

void Metaplasticity::setPlasticityHistorySize(size_t size) {
    pImpl->plasticityHistorySize = size;
    pImpl->plasticityHistory.resize(size, 0.0f);
}

void Metaplasticity::setMetaplasticLearningRate(float rate) {
    pImpl->metaplasticLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Metaplasticity::setHomeostasisTarget(float target) {
    pImpl->homeostasisTarget = std::clamp(target, -1.0f, 1.0f);
}

void Metaplasticity::recordPlasticityHistory(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    SynapseId synId = synapse->getId();
    if (synId < pImpl->plasticityHistory.size()) {
        pImpl->plasticityHistory[synId] = delta;
    }
}

float Metaplasticity::getPlasticityHistory(SynapseId synapseId) const {
    if (synapseId < pImpl->plasticityHistory.size()) {
        return pImpl->plasticityHistory[synapseId];
    }
    return 0.0f;
}

void Metaplasticity::resetPlasticityHistory(SynapseId synapseId) {
    if (synapseId < pImpl->plasticityHistory.size()) {
        pImpl->plasticityHistory[synapseId] = 0.0f;
    }
}

float Metaplasticity::calculatePlasticityLevel(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Calculate plasticity level based on recent weight changes
    // Simplified implementation
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Scale by metaplastic state
    return std::abs(weight) * efficacy * pImpl->metaplasticState;
}

// ShortTermPlasticity implementation
struct ShortTermPlasticity::Impl {
    float depressionU;           // Depression utilization parameter
    float depressionTauD;        // Depression time constant
    
    float facilitationU;         // Facilitation utilization parameter
    float facilitationTauF;      // Facilitation time constant
    
    bool depressionEnabled;      // Whether depression is enabled
    bool facilitationEnabled;    // Whether facilitation is enabled
    
    // STP state
    float depressionFactor;       // Current depression factor
    float facilitationFactor;     // Current facilitation factor
    float lastPreSpikeTime;       // When pre-synaptic neuron last fired
    float lastPostSpikeTime;      // When post-synaptic neuron last fired
    
    // Use-dependent modulation
    bool useDependentModulation;  // Whether use-dependent modulation is enabled
    float maxModulation;          // Maximum modulation level
    float modulationDecay;        // Modulation decay rate
    
    Impl() : depressionU(0.5f), depressionTauD(100.0f),
             facilitationU(0.5f), facilitationTauF(10.0f),
             depressionEnabled(true), facilitationEnabled(true),
             depressionFactor(1.0f), facilitationFactor(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             useDependentModulation(true), maxModulation(1.0f),
             modulationDecay(0.01f) {}
};

ShortTermPlasticity::ShortTermPlasticity() : pImpl(new Impl) {}

ShortTermPlasticity::~ShortTermPlasticity() = default;

void ShortTermPlasticity::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse) return;
    
    Timestamp currentTime = static_cast<Timestamp>(0.0); // Simplified
    
    // Update short-term plasticity state
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = currentTime - pImpl->lastPreSpikeTime;
        pImpl->facilitationFactor *= std::exp(-timeSincePre / pImpl->facilitationTauF);
    }
    
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? currentTime - pImpl->lastPostSpikeTime : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? currentTime - pImpl->lastPreSpikeTime : 0.0f
        );
        pImpl->depressionFactor += (1.0f - pImpl->depressionFactor) * 
                                   (1.0f - std::exp(-timeSinceActivity / pImpl->depressionTauD));
    }
    
    // Apply use-dependent modulation
    if (pImpl->useDependentModulation) {
        updateSynapticEfficacy(synapse, currentTime);
    }
}

void ShortTermPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* ShortTermPlasticity::getName() const {
    return "ShortTermPlasticity";
}

void ShortTermPlasticity::setDepressionParameters(float u, float tauD) {
    pImpl->depressionU = std::clamp(u, 0.0f, 1.0f);
    pImpl->depressionTauD = std::max(0.0f, tauD);
}

void ShortTermPlasticity::setFacilitationParameters(float U, float tauF) {
    pImpl->facilitationU = std::clamp(U, 0.0f, 1.0f);
    pImpl->facilitationTauF = std::max(0.0f, tauF);
}

void ShortTermPlasticity::enableDepression(bool enable) {
    pImpl->depressionEnabled = enable;
}

void ShortTermPlasticity::enableFacilitation(bool enable) {
    pImpl->facilitationEnabled = enable;
}

void ShortTermPlasticity::setUseDependentModulation(bool enable) {
    pImpl->useDependentModulation = enable;
}

void ShortTermPlasticity::setModulationParameters(float maxModulation) {
    pImpl->maxModulation = std::max(0.0f, maxModulation);
}

void ShortTermPlasticity::updateSynapticEfficacy(Synapse* synapse, Timestamp currentTime) {
    if (!synapse) return;
    
    // Calculate current efficacy based on STP state
    float efficacy = 1.0f;
    
    if (pImpl->facilitationEnabled) {
        efficacy *= pImpl->facilitationFactor;
    }
    
    if (pImpl->depressionEnabled) {
        efficacy *= pImpl->depressionFactor;
    }
    
    // Clamp to modulation range
    efficacy = std::clamp(efficacy, 0.0f, pImpl->maxModulation);
    
    // Apply efficacy to synapse
    float currentEfficacy = synapse->getEfficacy();
    currentEfficacy = std::lerp(currentEfficacy, efficacy, pImpl->modulationDecay);
    synapse->setEfficacy(currentEfficacy);
}

// NeuromodulatoryIntegration implementation
struct NeuromodulatoryIntegration::Impl {
    // Neuromodulator states
    std::vector<NeuromodulatorState> neuromodulators;
    
    // Integration weights
    std::vector<float> modulationWeights;
    std::vector<std::vector<float>> interactionTerms; // Matrix of interactions
    
    // Time integration
    float integrationTimeConstant; // Time constant for temporal integration
    
    // State variables
    float baselinePlasticity;    // Baseline plasticity level
    float modulatorySum;         // Sum of all modulatory effects
    float homeostaticSetPoint;   // Homeostatic set point
    
    Impl() : integrationTimeConstant(10.0f), baselinePlasticity(0.5f),
             modulatorySum(0.0f), homeostaticSetPoint(0.0f) {
        // Initialize with all 8 neuromodulator types
        neuromodulators.resize(8);
        modulationWeights.resize(8, 1.0f);
        
        // Initialize interaction matrix (8x8)
        interactionTerms.resize(8, std::vector<float>(8, 0.0f));
        for (size_t i = 0; i < 8; ++i) {
            interactionTerms[i][i] = 1.0f; // Self-interaction
        }
    }
};

NeuromodulatoryIntegration::NeuromodulatoryIntegration() : pImpl(new Impl) {}

NeuromodulatoryIntegration::~NeuromodulatoryIntegration() = default;

void NeuromodulatoryIntegration::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Update neuromodulator states
    updateNeuromodulatorState(static_cast<Timestamp>(0.0)); // Simplified
    
    // Calculate combined modulatory effect
    float modulatoryEffect = computeModulatedPlasticity(0.0f); // Simplified base plasticity
    
    // Apply modulated plasticity to synapse
    float scaledDelta = modulatoryEffect * synapse->getEfficacy();
    applyWeightChange(synapse, scaledDelta);
}

void NeuromodulatoryIntegration::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* NeuromodulatoryIntegration::getName() const {
    return "NeuromodulatoryIntegration";
}

void NeuromodulatoryIntegration::addNeuromodulator(NeuromodulatorType type, float baseline,
                                                 float releaseRate, float decayRate) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(pImpl->neuromodulators.size())) {
        pImpl->neuromodulators[index].baseline = baseline;
        pImpl->neuromodulators[index].releaseRate = releaseRate;
        pImpl->neuromodulators[index].decayRate = decayRate;
    }
}

void NeuromodulatoryIntegration::updateNeuromodulator(NeuromodulatorType type, float value) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(pImpl->neuromodulators.size())) {
        pImpl->neuromodulators[index].concentration = value;
    }
}

float NeuromodulatoryIntegration::getNeuromodulatorLevel(NeuromodulatorType type) const {
    int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(pImpl->neuromodulators.size())) {
        return pImpl->neuromodulators[index].concentration;
    }
    return 0.0f;
}

void NeuromodulatoryIntegration::setModulationWeights(const std::vector<float>& weights) {
    pImpl->modulationWeights = weights;
}

void NeuromodulatoryIntegration::setInteractionTerms(const std::vector<std::vector<float>>& interactions) {
    pImpl->interactionTerms = interactions;
}

} // namespace nlm

// Add include for plasticity rule implementations in existing files
// This function can be called from existing plasticity files to integrate advanced rules
namespace nlm {
    
    // Factory function to create advanced plasticity rules
    PlasticityRule* createAdvancedPlasticityRule(const std::string& ruleType) {
        if (ruleType == "RewardModulatedSTDP") {
            return new RewardModulatedSTDP();
        } else if (ruleType == "HomosynapticPlasticity") {
            return new HomosynapticPlasticity();
        } else if (ruleType == "SynapticTaggingAndCapture") {
            return new SynapticTaggingAndCapture();
        } else if (ruleType == "Metaplasticity") {
            return new Metaplasticity();
        } else if (ruleType == "ShortTermPlasticity") {
            return new ShortTermPlasticity();
        } else if (ruleType == "NeuromodulatoryIntegration") {
            return new NeuromodulatoryIntegration();
        } else {
            return nullptr;
        }
    }
    
} // namespace nlm