#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryTrace;
    float eligibilityTrace;
    float learningRateMod;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             memoryTrace(0.0f), eligibilityTrace(0.0f), learningRateMod(1.0f) {}
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
    // Real dopamine-modulated plasticity: higher dopamine increases LTP, lower dopamine increases LTD
    float rewardHistory = std::min(1.0f, pImpl->memoryTrace * 2.0f);
    float predictionError = std::max(0.0f, pImpl->eligibilityTrace);
    return 0.5f + 0.3f * rewardHistory + 0.2f * predictionError;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline with homeostatic regulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 10.0f);
    
    // Update eligibility trace with decay
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace * 0.95f);
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward prediction
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->memoryTrace = std::min(1.0f, pImpl->memoryTrace + reward * 0.3f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + std::abs(error) * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error, float confidence) {
    // Enhanced prediction error with confidence weighting
    float weightedError = error * confidence;
    signalRewardPredictionError(weightedError);
    pImpl->learningRateMod = 1.0f + weightedError * 0.5f;
}

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Attention system integration
    float attentionalFocus;
    float noveltySensitivity;
    float predictionErrorResponse;
    
    // Memory integration
    float workingMemoryStrength;
    float episodicConsolidation;
    float corticalPlasticity;
    
    // Hippocampal coupling
    float hippocampalLearningMod;
    float corticalLearningMod;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f),
             attentionalFocus(0.5f), noveltySensitivity(0.5f), predictionErrorResponse(0.5f),
             workingMemoryStrength(0.5f), episodicConsolidation(0.5f), corticalPlasticity(0.5f),
             hippocampalLearningMod(0.5f), corticalLearningMod(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity during encoding, reduces during recall
    float encodingFactor = pImpl->workingMemoryStrength * 0.7f;
    float consolidationFactor = pImpl->episodicConsolidation * 0.3f;
    return 0.3f + encodingFactor + consolidationFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Biological dynamics: slow rise, moderate decay
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.01f * static_cast<float>(dt));
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 5.0f);
    
    // Update dynamic parameters based on current level
    pImpl->attentionalFocus = 0.5f + pImpl->level * 0.4f;
    pImpl->noveltySensitivity = 0.3f + pImpl->level * 0.6f;
    pImpl->workingMemoryStrength = 0.4f + pImpl->level * 0.6f;
}

void Acetylcholine::modulateAttention(float relevance, float novelty) {
    // Real attention modulation based on behavioral relevance
    float attentionalGain = relevance * pImpl->noveltySensitivity;
    pImpl->attentionalFocus = std::clamp(attentionalGain, 0.0f, 1.0f);
}

void Acetylcholine::encodeToWorkingMemory(float activationPattern) {
    if (pImpl->brain && pImpl->workingMemoryStrength > 0.5f) {
        // Stronger ACh enhances working memory encoding
        float encodingBoost = pImpl->workingMemoryStrength * 2.0f;
        pImpl->brain->getWorkingMemory()->storeToNeuron(0, activationPattern * encodingBoost);
    }
}

void Acetylcholine::consolidateToEpisodicMemory() {
    if (pImpl->brain && pImpl->episodicConsolidation > 0.5f) {
        // ACh promotes memory consolidation during rest
        pImpl->brain->getEpisodicMemory()->consolidate(pImpl->episodicConsolidation);
    }
}

void Acetylcholine::updateNoveltySensitivity(float predictionError) {
    // ACh connects to novelty detection and prediction error
    pImpl->predictionErrorResponse = std::min(1.0f, pImpl->predictionErrorResponse + predictionError * 0.1f);
}

void Acetylcholine::updatePredictionError(float predictionError) {
    pImpl->predictionErrorResponse = predictionError;
}

void Acetylcholine::modulateHippocampalLearning(float stability) {
    // ACh modulates learning rates in hippocampus (episodic memory)
    pImpl->hippocampalLearningMod = 0.5f + (pImpl->level - 0.5f) * stability * 0.8f;
}

void Acetylcholine::modulateCorticalLearning(float novelty) {
    // ACh modulates learning rates in cortex (working memory)
    pImpl->corticalLearningMod = 0.3f + pImpl->level * novelty * 0.7f;
}

void Acetylcholine::updateAttentionalFocus(const std::vector<float>& attentionWeights) {
    if (attentionWeights.empty()) return;
    
    float totalFocus = 0.0f;
    for (float weight : attentionWeights) {
        totalFocus += weight;
    }
    pImpl->attentionalFocus = totalFocus / attentionWeights.size();
}

void Acetylcholine::setAttentionalPriority(float priority) {
    pImpl->attentionalFocus = std::clamp(priority, 0.0f, 1.0f);
}

void Acetylcholine::signalOther(Neuromodulator* target) {
    if (!target) return;
    
    // ACh inhibits NE for focus vs exploration trade-off
    if (strcmp(target->getName(), "NE") == 0) {
        float currentNELevel = target->getLevel();
        target->setLevel(std::max(0.0f, currentNELevel - pImpl->level * 0.3f));
    }
    
    // ACh excites 5-HT for mood regulation
    if (strcmp(target->getName(), "5-HT") == 0) {
        float current5HTLevel = target->getLevel();
        target->setLevel(std::min(1.0f, current5HTLevel + pImpl->level * 0.2f));
    }
}

void Acetylcholine::respondToLearning(float predictionError, float reward) {
    updatePredictionError(predictionError);
    modulateAttention(reward, predictionError);
    encodeToWorkingMemory(reward);
}

void Acetylcholine::affectNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float excitabilityMod = pImpl->level * 0.3f;
    pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, excitabilityMod);
}

void Acetylcholine::influenceMemoryConsolidation() {
    consolidateToEpisodicMemory();
    modulateHippocampalLearning(1.0f - pImpl->level);
}

void Acetylcholine::modulateCognition() {
    affectNeuralExcitability();
    
    if (pImpl->brain && pImpl->brain->getAttention()) {
        pImpl->brain->getAttention()->setInhibitionStrength(0.5f - pImpl->attentionalFocus * 0.3f);
        pImpl->brain->getAttention()->setExcitationStrength(1.5f + pImpl->attentionalFocus * 0.5f);
    }
}

void Acetylcholine::connectToBrain(class Brain* brain) {
    pImpl->brain = brain;
}

class Brain* Acetylcholine::getConnectedBrain() const {
    return pImpl->brain;
}

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Arousal and vigilance
    float arousalLevel;
    float vigilance;
    float stressResponse;
    
    // Attention breadth
    float attentionBreadth;
    float focusVsExploration;
    
    // Locus coerulean integration
    float locusCoeruleanActivity;
    float noveltyDetectionGain;
    
    // Motivation
    float motivationLevel;
    float curiosityDrive;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f),
             arousalLevel(0.5f), vigilance(0.5f), stressResponse(0.5f),
             attentionBreadth(0.5f), focusVsExploration(0.5f),
             locusCoeruleanActivity(0.5f), noveltyDetectionGain(0.5f),
             motivationLevel(0.5f), curiosityDrive(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity: high arousal enhances LTP, low arousal enhances LTD
    float arousalFactor = pImpl->arousalLevel * 0.7f;
    float explorationFactor = (1.0f - pImpl->focusVsExploration) * 0.3f;
    return 0.4f + arousalFactor + explorationFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: rapid rise, slower decay
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.02f * static_cast<float>(dt));
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 3.0f);
    
    // Update vigilance based on NE level
    pImpl->vigilance = 0.3f + pImpl->level * 0.7f;
    pImpl->arousalLevel = 0.2f + pImpl->level * 0.8f;
}

void Norepinephrine::modulateArousal(float stimulusStrength) {
    // NE increases arousal in response to stimuli
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + stimulusStrength * 0.1f);
}

void Norepinephrine::updateVigilance(float importance) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + importance * 0.1f);
}

void Norepinephrine::setAttentionBreadth(float breadth) {
    pImpl->attentionBreadth = std::clamp(breadth, 0.0f, 1.0f);
    pImpl->focusVsExploration = 1.0f - breadth;
}

void Norepinephrine::modulateFocusVsExploration() {
    // High NE promotes focus, low NE promotes exploration
    if (pImpl->level > 0.7f) {
        pImpl->focusVsExploration = std::max(0.3f, pImpl->focusVsExploration - 0.1f);
    } else {
        pImpl->focusVsExploration = std::min(0.7f, pImpl->focusVsExploration + 0.1f);
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stressLevel * 0.1f);
}

void Norepinephrine::signalMotivation(float motivationLevel) {
    pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + motivationLevel * 0.1f);
}

void Norepinephrine::detectNoveltyForLC(float novelty) {
    pImpl->noveltyDetectionGain = std::min(1.0f, pImpl->noveltyDetectionGain + novelty * 0.2f);
}

void Norepinephrine::updateLocusCoeruleanActivity() {
    // LC activity tracks NE level with adaptive response
    pImpl->locusCoeruleanActivity = std::min(1.0f, pImpl->locusCoeruleanActivity + pImpl->level * 0.05f);
}

void Norepinephrine::updateCuriosityDrive(float curiosityLevel) {
    pImpl->curiosityDrive = curiosityLevel;
}

void Norepinephrine::modulateExplorationBehavior() {
    // NE promotes exploration when novelty is detected
    if (pImpl->noveltyDetectionGain > 0.7f) {
        pImpl->focusVsExploration = std::min(1.0f, pImpl->focusVsExploration + 0.05f);
    }
}

void Norepinephrine::signalOther(Neuromodulator* target) {
    if (!target) return;
    
    // NE excites ACh for exploration
    if (strcmp(target->getName(), "ACh") == 0) {
        float currentAChLevel = target->getLevel();
        target->setLevel(std::min(1.0f, currentAChLevel + pImpl->level * 0.2f));
    }
    
    // NE modulates 5-HT for stress regulation
    if (strcmp(target->getName(), "5-HT") == 0) {
        float current5HTLevel = target->getLevel();
        target->setLevel(std::max(0.0f, current5HTLevel - pImpl->stressResponse * 0.3f));
    }
}

void Norepinephrine::respondToLearning(float predictionError, float reward) {
    modulateArousal(reward);
    updateVigilance(predictionError);
    detectNoveltyForLC(predictionError);
}

void Norepinephrine::affectNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float excitabilityMod = pImpl->arousalLevel * 0.4f;
    pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, excitabilityMod);
}

void Norepinephrine::influenceMemoryConsolidation() {
    updateLocusCoeruleanActivity();
    modulateExplorationBehavior();
}

void Norepinephrine::modulateCognition() {
    affectNeuralExcitability();
    
    if (pImpl->brain && pImpl->brain->getCuriosity()) {
        pImpl->brain->getCuriosity()->update(pImpl->noveltyDetectionGain, 0.0f, 0.001f);
    }
    
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // High NE narrows attention (focus), low NE widens attention (exploration)
        pImpl->brain->getAttention()->setInhibitionStrength(0.7f - pImpl->attentionBreadth * 0.4f);
        pImpl->brain->getAttention()->setExcitationStrength(1.2f + pImpl->attentionBreadth * 0.3f);
    }
}

void Norepinephrine::connectToBrain(class Brain* brain) {
    pImpl->brain = brain;
}

class Brain* Norepinephrine::getConnectedBrain() const {
    return pImpl->brain;
}

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Mood regulation
    float moodLevel;
    float socialValue;
    float stressLevel;
    
    // Reward prediction error (inverted relationship)
    float rpeSensitivity;
    float rewardResponse;
    
    // Decision making
    float impulsivity;
    float decisionThreshold;
    
    // Social behavior
    float socialCohesion;
    float trustLevel;
    
    // Novelty detection for approach/avoidance
    float approachBias;
    float avoidanceBias;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f),
             moodLevel(0.5f), socialValue(0.5f), stressLevel(0.5f),
             rpeSensitivity(0.5f), rewardResponse(0.5f),
             impulsivity(0.5f), decisionThreshold(0.5f),
             socialCohesion(0.5f), trustLevel(0.5f),
             approachBias(0.5f), avoidanceBias(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: high 5-HT stabilizes, low 5-HT destabilizes
    float moodFactor = pImpl->moodLevel * 0.4f;
    float socialFactor = pImpl->socialCohesion * 0.3f;
    return 0.4f + moodFactor + socialFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: moderate rise, stable decay
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.008f * static_cast<float>(dt));
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 4.0f);
    
    // Update mood based on 5-HT level
    pImpl->moodLevel = 0.3f + pImpl->level * 0.7f;
    pImpl->socialValue = 0.4f + pImpl->level * 0.6f;
}

void Serotonin::updateMood(float socialValue, float stressLevel) {
    pImpl->socialValue = socialValue;
    pImpl->stressLevel = stressLevel;
    pImpl->moodLevel = 0.3f + pImpl->level * 0.7f - pImpl->stressLevel * 0.3f;
}

void Serotonin::regulateBehavior(float approachLevel) {
    pImpl->approachBias = approachLevel;
    pImpl->avoidanceBias = 1.0f - approachLevel;
}

void Serotonin::integrateWithRPE(float rewardPredictionError) {
    // Inverted relationship with RPE
    pImpl->rpeSensitivity = 1.0f - std::abs(rewardPredictionError);
    pImpl->rewardResponse = std::max(0.0f, 1.0f - rewardPredictionError * 2.0f);
}

void Serotonin::updatePredictionErrorResponse(float error) {
    pImpl->rpeSensitivity = 1.0f - std::abs(error);
}

void Serotonin::modulateImpulsivity(float impulsivityLevel) {
    pImpl->impulsivity = impulsivityLevel;
    pImpl->decisionThreshold = 1.0f - impulsivityLevel;
}

void Serotonin::updateDecisionThresholds(float threshold) {
    pImpl->decisionThreshold = std::clamp(threshold, 0.0f, 1.0f);
    pImpl->impulsivity = 1.0f - threshold;
}

void Serotonin::updateSocialBonding(float socialCohesion) {
    pImpl->socialCohesion = socialCohesion;
    pImpl->trustLevel = socialCohesion;
}

void Serotonin::updateApproachAvoidanceBias(float novelty, float threatLevel) {
    pImpl->approachBias = std::min(1.0f, novelty * (1.0f - threatLevel) * 1.5f);
    pImpl->avoidanceBias = 1.0f - pImpl->approachBias;
}

void Serotonin::integrateWithNoveltySystem(float noveltyLevel) {
    updateApproachAvoidanceBias(noveltyLevel, pImpl->avoidanceBias);
}

void Serotonin::signalOther(Neuromodulator* target) {
    if (!target) return;
    
    // 5-HT inhibits ACh for behavioral control
    if (strcmp(target->getName(), "ACh") == 0) {
        float currentAChLevel = target->getLevel();
        target->setLevel(std::max(0.0f, currentAChLevel - pImpl->avoidanceBias * 0.3f));
    }
    
    // 5-HT stabilizes NE for stress regulation
    if (strcmp(target->getName(), "NE") == 0) {
        float currentNELevel = target->getLevel();
        target->setLevel(std::max(pImpl->baseline, currentNELevel - pImpl->stressLevel * 0.4f));
    }
}

void Serotonin::respondToLearning(float predictionError, float reward) {
    integrateWithRPE(predictionError);
    updateMood(reward, predictionError);
    modulateImpulsivity(predictionError);
}

void Serotonin::affectNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float excitabilityMod = (1.0f - pImpl->moodLevel) * 0.2f;
    pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, excitabilityMod);
}

void Serotonin::influenceMemoryConsolidation() {
    updateSocialBonding(pImpl->socialCohesion);
    modulateImpulsivity(pImpl->impulsivity);
}

void Serotonin::modulateCognition() {
    affectNeuralExcitability();
    
    if (pImpl->brain) {
        // Apply to attention system
        if (pImpl->brain->getAttention()) {
            // Stable mood from 5-HT promotes consistent attentional performance
            pImpl->brain->getAttention()->setInhibitionStrength(0.5f + pImpl->moodLevel * 0.2f);
            pImpl->brain->getAttention()->setExcitationStrength(1.5f - pImpl->moodLevel * 0.2f);
        }
        
        // Apply to planner for decision making
        if (pImpl->brain->getPlanner()) {
            // 5-HT modulates action selection consistency
            float decisionBias = 1.0f - pImpl->impulsivity;
            // This would require modifications to NeuralPlanner for implementation
        }
    }
}

void Serotonin::connectToBrain(class Brain* brain) {
    pImpl->brain = brain;
}

class Brain* Serotonin::getConnectedBrain() const {
    return pImpl->brain;
}

} // namespace nlm