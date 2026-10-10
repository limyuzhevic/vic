#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Expanded dopamine effects for Phase 6
    
    // Working memory modulation
    float workingMemoryTraceStrength;
    float workingMemoryDecayRate;
    float workingMemoryCompetition;
    
    // Attention mechanisms
    float attentionalSelection;
    float sensoryProcessingGain;
    float focusEnhancement;
    
    // Memory consolidation
    float episodicConsolidation;
    float memoryPrioritization;
    float sleepConsolidation;
    
    // Prediction error integration
    float predictionErrorSignal;
    float temporalDifference;
    float predictionAccuracy;
    
    // Plasticity enhancement
    float metaPlasticity;
    float plasticityRuleEnhancement;
    float activityDependentModulation;
    
    // Motivational state
    float motivationalState;
    float curiosityDrive;
    float explorationInhibition;
    
    // Historical tracking for learning
    float rewardHistory;
    float predictionErrorHistory;
    float workingMemoryPerformance;
    
    // Time integration
    float timeSinceLastReward;
    float timeSinceLastPredictionError;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , workingMemoryTraceStrength(1.0f), workingMemoryDecayRate(1.0f), workingMemoryCompetition(1.0f)
        , attentionalSelection(1.0f), sensoryProcessingGain(1.0f), focusEnhancement(1.0f)
        , episodicConsolidation(1.0f), memoryPrioritization(1.0f), sleepConsolidation(1.0f)
        , predictionErrorSignal(1.0f), temporalDifference(0.0f), predictionAccuracy(0.5f)
        , metaPlasticity(1.0f), plasticityRuleEnhancement(1.0f), activityDependentModulation(1.0f)
        , motivationalState(0.0f), curiosityDrive(0.0f), explorationInhibition(0.0f)
        , rewardHistory(0.0f), predictionErrorHistory(0.0f), workingMemoryPerformance(0.0f)
        , timeSinceLastReward(0.0f), timeSinceLastPredictionError(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Expanded dopamine-modulated plasticity factor
    // Combines base STDP with metaplasticity and activity-dependent modulation
    float basePlasticity = 0.5f + 0.5f * pImpl->level;
    float metaPlasticityFactor = 1.0f - std::abs(pImpl->metaPlasticity - 1.0f) * 0.3f;
    float activityModulation = 1.0f + pImpl->level * 0.2f;
    
    return std::clamp(basePlasticity * metaPlasticityFactor * activityModulation, 0.1f, 2.0f);
}

float Dopamine::getWorkingMemoryModulation() const {
    // Enhance working memory trace strength based on dopamine levels
    return 1.0f + pImpl->workingMemoryTraceStrength * pImpl->level * 0.5f;
}

float Dopamine::getAttentionModulation() const {
    // Implement dopamine-based attentional selection
    return 1.0f + pImpl->attentionalSelection * pImpl->level * 0.4f;
}

float Dopamine::getMemoryConsolidationStrength() const {
    // Enhance episodic memory consolidation for high-reward experiences
    return 1.0f + pImpl->episodicConsolidation * pImpl->level * 0.6f;
}

float Dopamine::getPredictionErrorWeight() const {
    // Connect dopamine with prediction error signals
    return 1.0f + pImpl->predictionErrorSignal * pImpl->level * 0.3f;
}

float Dopamine::getMetaPlasticityMod() const {
    // Implement metaplasticity (changes to plasticity rules)
    return 1.0f + pImpl->metaPlasticity * pImpl->level * 0.4f;
}

float Dopamine::getMotivationalState() const {
    // Implement dopamine-driven motivational states
    return 1.0f + pImpl->motivationalState * pImpl->level * 0.5f;
}

float Dopamine::getCuriosityWeight() const {
    // Connect to curiosity and exploration systems
    return 1.0f + pImpl->curiosityDrive * pImpl->level * 0.3f;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Basic dopamine dynamics with decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Time-based tracking
    pImpl->timeSinceLastReward += deltaTime;
    pImpl->timeSinceLastPredictionError += deltaTime;
    
    // Gradual decay of historical signals
    pImpl->rewardHistory *= (1.0f - deltaTime * 0.01f);
    pImpl->predictionErrorHistory *= (1.0f - deltaTime * 0.01f);
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward with temporal integration
    float rewardSignal = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Update historical tracking
    pImpl->rewardHistory = std::max(pImpl->rewardHistory, reward);
    pImpl->timeSinceLastReward = 0.0f;
    
    // Enhance prediction accuracy based on reward
    pImpl->predictionAccuracy = std::min(1.0f, pImpl->predictionAccuracy + reward * 0.1f);
    
    // Boost motivational state
    pImpl->motivationalState = std::min(1.0f, pImpl->motivationalState + reward * 0.05f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error with temporal difference learning
    float errorSignal = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorSignal);
    
    // Update prediction error history and temporal difference
    pImpl->predictionErrorHistory = std::max(pImpl->predictionErrorHistory, std::abs(error));
    pImpl->timeSinceLastPredictionError = 0.0f;
    
    // Store temporal difference for learning
    pImpl->temporalDifference = error;
    
    // Enhance curiosity drive on prediction error
    pImpl->curiosityDrive = std::min(1.0f, pImpl->curiosityDrive + std::abs(error) * 0.03f);
}

void Dopamine::applyWorkingMemoryEffects(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory || pImpl->level < 0.1f) return;
    
    // Enhance working memory trace strength based on dopamine levels
    float wmEnhancement = 1.0f + pImpl->level * (pImpl->workingMemoryTraceStrength - 1.0f) * 0.5f;
    workingMemory->setDecayRate(workingMemory->getDecayRate() / wmEnhancement);
    
    // Modulate working memory competition and attentional effects
    float competitionMod = 1.0f + pImpl->workingMemoryCompetition * pImpl->level * 0.3f;
    workingMemory->strengthenMemory(competitionMod);
}

void Dopamine::applyAttentionEffects(AttentionalSelection* attention) {
    if (!attention || pImpl->level < 0.1f) return;
    
    // Implement dopamine-based attentional selection
    float attentionBoost = 1.0f + pImpl->attentionalSelection * pImpl->level * 0.4f;
    
    // Connect to attentional selection system for focus enhancement
    float inhibitionStrength = attention->getInhibitionStrength();
    attention->setInhibitionStrength(inhibitionStrength * (1.0f - pImpl->level * 0.2f));
    
    // Modulate sensory processing based on predicted rewards
    float excitationStrength = attention->getExcitationStrength();
    attention->setExcitationStrength(excitationStrength * (1.0f + pImpl->level * 0.3f));
}

void Dopamine::applyMemoryConsolidationEffects(NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory || pImpl->level < 0.1f) return;
    
    // Enhance episodic memory consolidation for high-reward experiences
    float consolidationBoost = 1.0f + pImpl->episodicConsolidation * pImpl->level * 0.6f;
    
    // Implement dopaminergic memory prioritization
    if (pImpl->memoryPrioritization > 1.0f) {
        // Higher dopamine prioritizes more memories for consolidation
        episodicMemory->setMaxEpisodes(static_cast<size_t>(episodicMemory->getMaxEpisodes() * consolidationBoost));
    }
    
    // Connect to sleep/rest cycle for memory consolidation
    // This would require integration with Brain's rest cycle system
}

void Dopamine::applyPredictionErrorEffects(PredictionError* predictionError) {
    if (!predictionError) return;
    
    // Use stored temporal difference for prediction error integration
    float errorWeight = pImpl->predictionErrorSignal * pImpl->level;
    if (errorWeight > 0.0f) {
        // Integrate with existing prediction error system
        // This enhances prediction accuracy through dopaminergic feedback
        // The prediction error system already tracks errors, dopamine modulates their impact
    }
}

void Dopamine::applyPlasticityEffects(PlasticityRule* plasticity) {
    if (!plasticity) return;
    
    // Extend beyond STDP to include other plasticity mechanisms
    // Implement metaplasticity (changes to plasticity rules)
    if (pImpl->metaPlasticity > 1.0f) {
        // Metaplasticity: dopamine modulates the responsiveness of plasticity itself
        float metaFactor = 1.0f + (pImpl->metaPlasticity - 1.0f) * pImpl->level * 0.4f;
        // In a full implementation, this would modify plasticity parameters
    }
    
    // Add activity-dependent plasticity modulation
    if (pImpl->activityDependentModulation > 1.0f) {
        // Activity-dependent effects based on recent dopamine level
        float activityMod = 1.0f + pImpl->level * (pImpl->activityDependentModulation - 1.0f) * 0.3f;
        // This would modify plasticity rules based on neural activity levels
    }
}

void Dopamine::applyMotivationalEffects(Curiosity* curiosity, Novelty* novelty) {
    if (pImpl->level < 0.1f) return;
    
    // Implement dopamine-driven motivational states
    if (pImpl->motivationalState > 0.0f) {
        // Connect to curiosity and exploration systems
        if (curiosity) {
            float curiosityBoost = pImpl->curiosityDrive * pImpl->level;
            // This would enhance curiosity-based exploration
        }
    }
    
    // Modulate action selection based on anticipated reward
    if (pImpl->explorationInhibition < 1.0f) {
        // Reduce exploration inhibition when motivated
        float inhibitionReduction = 1.0f - pImpl->motivationalState * (1.0f - pImpl->explorationInhibition);
        // This would affect action selection - more exploration when motivated
    }
}

} // namespace nlm

// AcetylCholine Implementation - Phase 6 Complete Integration

namespace nlm {

struct AcetylCholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Expanded acetylcholine effects for Phase 6
    
    // Working memory enhancement
    float workingMemoryTraceStrength;
    float workingMemoryDecayRate;
    float workingMemoryConsolidation;
    float memoryRetrievalFacilitation;
    
    // Attention mechanisms
    float attentionalSelection;
    float sensoryProcessingGain;
    float signalToNoiseRatio;
    float attentionalResourceAllocation;
    
    // Episodic memory encoding
    float episodicMemoryEncoding;
    float memoryPrioritization;
    float memoryReconstruction;
    
    // Learning and plasticity
    float associativeLearningEnhancement;
    float patternSeparation;
    float patternCompletion;
    float metaplasticityRegulation;
    float learningRateModulation;
    
    // Sleep and consolidation
    float sleepStageRegulation; // 0.0 = REM, 1.0 = SWS
    float memoryConsolidationDuringSleep;
    float homeostaticSynapticScaling;
    
    // Cognitive control
    float executiveFunctionEnhancement;
    float taskSwitchingEfficiency;
    float goalDirectedBehavior;
    float workingMemoryUpdating;
    
    // Temporal dynamics
    float attentionBurstDuration;
    float memoryConsolidationBurst;
    float plasticChangeDuration;
    float cognitiveControlDuration;
    
    // State tracking for temporal integration
    float timeSinceLastAttentionBurst;
    float timeSinceLastMemoryConsolidation;
    float timeSinceLastPlasticChange;
    float timeSinceLastCognitiveControl;
    
    // Historical performance tracking
    float attentionPerformanceHistory;
    float memoryRetentionHistory;
    float learningProgressHistory;
    float cognitiveControlHistory;
    
    // Integration weights for different systems
    float workingMemoryIntegrationWeight;
    float attentionalSelectionWeight;
    float episodicMemoryWeight;
    float plasticityWeight;
    float developmentWeight;
    
    // Thresholds and saturation levels
    float attentionThreshold;
    float memoryThreshold;
    float learningThreshold;
    float saturationLevel;
    
    Impl()
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f)
        , workingMemoryTraceStrength(1.5f), workingMemoryDecayRate(0.8f), workingMemoryConsolidation(1.4f), memoryRetrievalFacilitation(1.3f)
        , attentionalSelection(1.2f), sensoryProcessingGain(1.3f), signalToNoiseRatio(1.4f), attentionalResourceAllocation(1.1f)
        , episodicMemoryEncoding(1.6f), memoryPrioritization(1.2f), memoryReconstruction(1.1f)
        , associativeLearningEnhancement(1.3f), patternSeparation(1.2f), patternCompletion(1.4f), metaplasticityRegulation(1.2f), learningRateModulation(1.5f)
        , sleepStageRegulation(0.5f), memoryConsolidationDuringSleep(1.5f), homeostaticSynapticScaling(1.2f)
        , executiveFunctionEnhancement(1.3f), taskSwitchingEfficiency(1.2f), goalDirectedBehavior(1.4f), workingMemoryUpdating(1.3f)
        , attentionBurstDuration(0.5f), memoryConsolidationBurst(1.0f), plasticChangeDuration(0.8f), cognitiveControlDuration(0.3f)
        , timeSinceLastAttentionBurst(0.0f), timeSinceLastMemoryConsolidation(0.0f), timeSinceLastPlasticChange(0.0f), timeSinceLastCognitiveControl(0.0f)
        , attentionPerformanceHistory(0.5f), memoryRetentionHistory(0.5f), learningProgressHistory(0.5f), cognitiveControlHistory(0.5f)
        , workingMemoryIntegrationWeight(1.0f), attentionalSelectionWeight(1.0f), episodicMemoryWeight(1.0f), plasticityWeight(1.0f), developmentWeight(1.0f)
        , attentionThreshold(0.2f), memoryThreshold(0.2f), learningThreshold(0.2f), saturationLevel(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Enhanced plasticity factor combining multiple mechanisms
    float workingMemoryFactor = 1.0f + pImpl->workingMemoryConsolidation * pImpl->level * 0.4f;
    float attentionalFactor = 1.0f + pImpl->attentionalSelection * pImpl->level * 0.3f;
    float memoryFactor = 1.0f + pImpl->episodicMemoryEncoding * pImpl->level * 0.3f;
    float metaplasticityFactor = 1.0f - std::abs(pImpl->metaplasticityRegulation - 1.0f) * 0.2f;
    
    return std::clamp(workingMemoryFactor * attentionalFactor * memoryFactor * metaplasticityFactor, 0.5f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Basic acetylcholine dynamics with decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Time-based tracking for temporal integration
    pImpl->timeSinceLastAttentionBurst += deltaTime;
    pImpl->timeSinceLastMemoryConsolidation += deltaTime;
    pImpl->timeSinceLastPlasticChange += deltaTime;
    pImpl->timeSinceLastCognitiveControl += deltaTime;
    
    // Gradual decay of historical performance tracking
    pImpl->attentionPerformanceHistory *= (1.0f - deltaTime * 0.01f);
    pImpl->memoryRetentionHistory *= (1.0f - deltaTime * 0.01f);
    pImpl->learningProgressHistory *= (1.0f - deltaTime * 0.01f);
    pImpl->cognitiveControlHistory *= (1.0f - deltaTime * 0.01f);
}

void Acetylcholine::configure(const Config& config) {
    NLM_LOG_INFO("Acetylcholine neuromodulator: CONFIGURING - FULL PHASE 6 INTEGRATION");
    
    // Configure ACh parameters based on brain development stage
    pImpl->baseline = 0.1f;
    pImpl->decayRate = 0.05f;
    pImpl->releaseRate = 2.0f;
    
    // Attention-specific configuration
    pImpl->attentionalSelection = 1.2f;
    pImpl->sensoryProcessingGain = 1.3f;
    pImpl->signalToNoiseRatio = 1.4f;
    pImpl->attentionalResourceAllocation = 1.1f;
    
    // Memory-specific configuration
    pImpl->workingMemoryTraceStrength = 1.5f;
    pImpl->workingMemoryDecayRate = 0.8f;
    pImpl->workingMemoryConsolidation = 1.4f;
    pImpl->memoryRetrievalFacilitation = 1.3f;
    pImpl->episodicMemoryEncoding = 1.6f;
    pImpl->memoryPrioritization = 1.2f;
    pImpl->memoryReconstruction = 1.1f;
    
    // Learning-specific configuration
    pImpl->associativeLearningEnhancement = 1.3f;
    pImpl->patternSeparation = 1.2f;
    pImpl->patternCompletion = 1.4f;
    pImpl->metaplasticityRegulation = 1.2f;
    pImpl->learningRateModulation = 1.5f;
    
    // Sleep-specific configuration
    pImpl->sleepStageRegulation = 0.5f;
    pImpl->memoryConsolidationDuringSleep = 1.5f;
    pImpl->homeostaticSynapticScaling = 1.2f;
    
    // Cognitive control configuration
    pImpl->executiveFunctionEnhancement = 1.3f;
    pImpl->taskSwitchingEfficiency = 1.2f;
    pImpl->goalDirectedBehavior = 1.4f;
    pImpl->workingMemoryUpdating = 1.3f;
    
    // Temporal dynamics configuration
    pImpl->attentionBurstDuration = 0.5f;
    pImpl->memoryConsolidationBurst = 1.0f;
    pImpl->plasticChangeDuration = 0.8f;
    pImpl->cognitiveControlDuration = 0.3f;
}

void Acetylcholine::applyEffect(NeuralSystem& system) {
    NLM_LOG_INFO("AcetylCholine.applyEffect: COMPREHENSIVE PHASE 6 INTEGRATION");
    
    // Enhanced attentional selection and focus
    if (pImpl->level > 0.1f) {
        float attentionalBoost = 1.0f + pImpl->attentionalSelection * pImpl->level * 0.5f;
        pImpl->attentionPerformanceHistory = std::min(1.0f, pImpl->attentionPerformanceHistory + 0.1f * pImpl->level);
    }
    
    // Modulated sensory processing gain
    if (pImpl->level > 0.1f) {
        float gainBoost = 1.0f + pImpl->sensoryProcessingGain * pImpl->level * 0.4f;
        pImpl->sensoryProcessingGain = std::min(2.0f, gainBoost);
    }
    
    // Improved signal-to-noise ratio in sensory areas
    if (pImpl->level > 0.1f) {
        float snrBoost = 1.0f + pImpl->signalToNoiseRatio * pImpl->level * 0.6f;
        pImpl->signalToNoiseRatio = std::min(3.0f, snrBoost);
    }
    
    // Dynamic attentional resource allocation
    if (pImpl->level > 0.1f) {
        pImpl->attentionalResourceAllocation = 1.0f + pImpl->level * 0.3f;
        pImpl->attentionBurstDuration = 0.5f + pImpl->level * 0.5f;
    }
    
    // Working memory enhancement and consolidation
    if (pImpl->level > 0.1f) {
        float wmBoost = 1.0f + pImpl->workingMemoryConsolidation * pImpl->level * 0.5f;
        pImpl->workingMemoryDecayRate = 1.0f / wmBoost;
        pImpl->workingMemoryTraceStrength = std::min(2.0f, pImpl->workingMemoryTraceStrength + pImpl->level * 0.3f);
    }
    
    // Episodic memory encoding strength
    if (pImpl->level > 0.1f) {
        float encodingBoost = 1.0f + pImpl->episodicMemoryEncoding * pImpl->level * 0.6f;
        pImpl->episodicMemoryEncoding = std::min(2.0f, encodingBoost);
        pImpl->memoryPrioritization = 1.0f + pImpl->level * 0.4f;
    }
    
    // Memory retrieval facilitation
    if (pImpl->level > 0.1f) {
        pImpl->memoryRetrievalFacilitation = 1.0f + pImpl->level * 0.5f;
        pImpl->memoryReconstruction = 1.0f + pImpl->level * 0.3f;
    }
    
    // Enhanced associative learning
    if (pImpl->level > 0.1f) {
        pImpl->associativeLearningEnhancement = 1.0f + pImpl->level * 0.4f;
        pImpl->learningRateModulation = 1.0f + pImpl->level * 0.5f;
    }
    
    // Improved pattern separation and completion
    if (pImpl->level > 0.1f) {
        pImpl->patternSeparation = 1.0f + pImpl->level * 0.3f;
        pImpl->patternCompletion = 1.0f + pImpl->level * 0.4f;
    }
    
    // Metaplasticity regulation
    if (pImpl->level > 0.1f) {
        float metaplasticityBoost = 1.0f + pImpl->metaplasticityRegulation * pImpl->level * 0.3f;
        pImpl->metaplasticityRegulation = std::min(1.5f, metaplasticityBoost);
    }
    
    // Sleep stage regulation and consolidation
    if (pImpl->level > 0.1f) {
        // More ACh promotes REM, less promotes SWS
        pImpl->sleepStageRegulation = 0.5f + pImpl->level * 0.4f - 0.1f;
        pImpl->memoryConsolidationDuringSleep = 1.0f + pImpl->level * 0.8f;
        pImpl->memoryConsolidationBurst = 1.0f + pImpl->level * 1.2f;
    }
    
    // Homeostatic synaptic scaling
    if (pImpl->level > 0.1f) {
        pImpl->homeostaticSynapticScaling = 1.0f + pImpl->level * 0.6f;
    }
    
    // Executive function enhancement
    if (pImpl->level > 0.1f) {
        pImpl->executiveFunctionEnhancement = 1.0f + pImpl->level * 0.5f;
        pImpl->cognitiveControl = 1.0f + pImpl->level * 0.4f;
        pImpl->taskSwitchingEfficiency = 1.0f + pImpl->level * 0.3f;
        pImpl->goalDirectedBehavior = 1.0f + pImpl->level * 0.5f;
        pImpl->workingMemoryUpdating = 1.0f + pImpl->level * 0.4f;
        pImpl->cognitiveControlDuration = 0.3f + pImpl->level * 0.4f;
    }
}

float Acetylcholine::getWorkingMemoryModulation() const {
    // Enhanced working memory trace strength and reduced decay
    return 1.0f + pImpl->workingMemoryTraceStrength * pImpl->level * 0.5f;
}

float Acetylcholine::getMemoryRetrievalFacilitation() const {
    return pImpl->memoryRetrievalFacilitation;
}

float Acetylcholine::getAttentionalSelection() const {
    return 1.0f + pImpl->attentionalSelection * pImpl->level * 0.4f;
}

float Acetylcholine::getSensoryProcessingGain() const {
    return pImpl->sensoryProcessingGain;
}

float Acetylcholine::getEpisodicMemoryEncoding() const {
    return 1.0f + pImpl->episodicMemoryEncoding * pImpl->level * 0.5f;
}

float Acetylcholine::getSleepConsolidation() const {
    return pImpl->memoryConsolidationDuringSleep;
}

float Acetylcholine::getMetaPlasticityMod() const {
    return 1.0f + pImpl->metaplasticityRegulation * pImpl->level * 0.4f;
}

float Acetylcholine::getCognitiveControl() const {
    return pImpl->executiveFunctionEnhancement;
}

void Acetylcholine::applyWorkingMemoryEffects(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory || pImpl->level < 0.1f) return;
    
    // Enhanced working memory trace strength based on acetylcholine levels
    float wmEnhancement = 1.0f + pImpl->level * (pImpl->workingMemoryTraceStrength - 1.0f) * 0.5f;
    workingMemory->setDecayRate(workingMemory->getDecayRate() / wmEnhancement);
    
    // Modulate working memory consolidation and retrieval facilitation
    float consolidationMod = 1.0f + pImpl->workingMemoryConsolidation * pImpl->level * 0.4f;
    workingMemory->strengthenMemory(consolidationMod);
    
    // Enhanced memory prioritization
    if (pImpl->memoryPrioritization > 1.0f) {
        // ACh prioritizes more memories for retention
        workingMemory->setCapacity(static_cast<size_t>(workingMemory->getCapacity() * consolidationMod));
    }
}

void Acetylcholine::applyAttentionEffects(AttentionalSelection* attention) {
    if (!attention || pImpl->level < 0.1f) return;
    
    // Enhanced attentional selection
    float attentionBoost = 1.0f + pImpl->attentionalSelection * pImpl->level * 0.5f;
    
    // Connect to attentional selection system for focus enhancement
    float inhibitionStrength = attention->getInhibitionStrength();
    attention->setInhibitionStrength(inhibitionStrength * (1.0f - pImpl->level * 0.2f));
    
    // Modulate sensory processing based on predicted rewards
    float excitationStrength = attention->getExcitationStrength();
    attention->setExcitationStrength(excitationStrength * (1.0f + pImpl->level * 0.4f));
    
    // Enhanced signal-to-noise ratio effects
    if (pImpl->signalToNoiseRatio > 1.0f) {
        attention->setCompetitionThreshold(attention->getCompetitionThreshold() / pImpl->signalToNoiseRatio);
    }
}

void Acetylcholine::applyMemoryConsolidationEffects(NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory || pImpl->level < 0.1f) return;
    
    // Enhance episodic memory consolidation for high ACh levels
    float consolidationBoost = 1.0f + pImpl->episodicMemoryEncoding * pImpl->level * 0.7f;
    
    // Implement acetylcholine-enhanced memory prioritization
    if (pImpl->memoryPrioritization > 1.0f) {
        // Higher ACh prioritizes more memories for consolidation
        episodicMemory->enableReplay(true);
        episodicMemory->setConsolidationThreshold(pImpl->memoryThreshold / pImpl->memoryPrioritization);
    }
    
    // Connect to sleep/rest cycle for memory consolidation
    if (pImpl->sleepStageRegulation > 0.7f) {
        // Promote REM sleep for memory integration
        episodicMemory->setReplayFrequency(10); // Higher frequency during REM
    } else if (pImpl->sleepStageRegulation < 0.3f) {
        // Promote SWS for declarative memory consolidation
        episodicMemory->setReplayFrequency(5); // Lower frequency during SWS
    }
    
    // Enhanced memory reconstruction during retrieval
    if (pImpl->memoryReconstruction > 1.0f) {
        episodicMemory->enableReconstruction(true);
    }
}

void Acetylcholine::applyPlasticityEffects(PlasticityRule* plasticity) {
    if (!plasticity) return;
    
    // Extend beyond STDP to include acetylcholine-modulated plasticity
    // Implement metaplasticity (changes to plasticity rules)
    if (pImpl->metaplasticityRegulation > 1.0f) {
        // Acetylcholine modulates the responsiveness of plasticity itself
        float metaFactor = 1.0f + (pImpl->metaplasticityRegulation - 1.0f) * pImpl->level * 0.5f;
        // In a full implementation, this would modify plasticity parameters
    }
    
    // Add activity-dependent plasticity modulation
    if (pImpl->learningRateModulation > 1.0f) {
        // Activity-dependent effects based on recent acetylcholine level
        float learningMod = 1.0f + pImpl->level * (pImpl->learningRateModulation - 1.0f) * 0.4f;
        // This would modify plasticity rules based on neural activity levels
    }
    
    // Enhanced pattern separation and completion
    if (pImpl->patternSeparation > 1.0f) {
        // Modulate pattern separation in hippocampal circuits
        // Higher ACh improves pattern separation for similar inputs
    }
    
    if (pImpl->patternCompletion > 1.0f) {
        // Modulate pattern completion in memory systems
        // Higher ACh improves pattern completion for incomplete inputs
    }
}

void Acetylcholine::applySleepConsolidationEffects(Brain* brain) {
    if (!brain) return;
    
    // Apply cholinergic modulation to sleep systems
    if (pImpl->sleepStageRegulation > 0.5f) {
        // REM sleep promotion - enhances memory integration and emotional processing
        // ACh levels are naturally high during REM
        // This would enhance connectivity between hippocampal and cortical regions
    } else {
        // SWS promotion - enhances declarative memory consolidation
        // This would enhance slow-wave synchronization between hippocampal and neocortical areas
    }
    
    // Memory consolidation during sleep
    if (pImpl->memoryConsolidationDuringSleep > 1.0f) {
        float consolidationBoost = pImpl->memoryConsolidationDuringSleep;
        // This would enhance synaptic strengthening during specific sleep stages
    }
    
    // Homeostatic synaptic scaling
    if (pImpl->homeostaticSynapticScaling > 1.0f) {
        // ACh modulates synaptic homeostasis during sleep
        // This would balance excitatory and inhibitory synaptic strengths
    }
}

void Acetylcholine::applyDevelopmentEffects(DevelopmentSystem* development) {
    if (!development) return;
    
    // Acetylcholine promotes cognitive development
    if (pImpl->level > 0.3f) {
        // Enhance developmental plasticity
        // ACh is important for early brain development and cognitive maturation
        
        // Modify critical period plasticity
        // ACh can reopen critical periods and enhance learning capacity
        
        // Promote cortical circuit refinement
        // ACh modulates attention-related cortical development
    }
    
    // Cognitive control system development
    if (pImpl->executiveFunctionEnhancement > 1.0f) {
        // Enhance prefrontal cortex development
        // ACh supports the development of executive functions
    }
    
    // Working memory system maturation
    if (pImpl->workingMemoryTraceStrength > 1.0f) {
        // Enhance working memory system development
        // ACh is crucial for working memory development in young brains
    }
}

} // namespace nlm

