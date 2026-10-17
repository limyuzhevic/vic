#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Forward declarations for implementation
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

// Dopamine implementation with full integration
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidation;
    float attentionModulation;
    float structuralPlasticity;
    float rewardPredictionError;
    float learningRate;
    
    // Memory consolidation tracking
    float memoryStrength;
    float encodingPriority;
    float consolidationTime;
    
    // Attention modulation
    float focusEnhancement;
    float selectiveAttention;
    
    // Structural plasticity
    bool promoteSynaptogenesis;
    bool inhibitPruning;
    float structuralWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             memoryConsolidation(0.0f), attentionModulation(0.0f), structuralPlasticity(0.0f),
             rewardPredictionError(0.0f), learningRate(1.0f),
             memoryStrength(0.0f), encodingPriority(0.0f), consolidationTime(0.0f),
             focusEnhancement(0.0f), selectiveAttention(0.0f),
             promoteSynaptogenesis(false), inhibitPruning(false), structuralWeight(1.0f) {}
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
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Decay other effects
    pImpl->memoryConsolidation *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->attentionModulation *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->structuralPlasticity *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->focusEnhancement *= (1.0f - pImpl->decayRate * dtFloat);
    
    // Update consolidation timer
    if (pImpl->consolidationTime > 0.0f) {
        pImpl->consolidationTime -= dtFloat;
    }
}

void Dopamine::interactWith(const Neuromodulator& other) {
    if (other.getName() == "ACh") {
        // Dopamine + Acetylcholine for enhanced learning
        pImpl->attentionModulation = std::min(1.0f, pImpl->attentionModulation + other.getLevel() * 0.3f);
        pImpl->learningRate = std::min(1.5f, pImpl->learningRate + other.getLevel() * 0.2f);
    } else if (other.getName() == "NE") {
        // Dopamine + Norepinephrine for motivation
        if (pImpl->level > 0.5f && other.getLevel() > 0.5f) {
            pImpl->structuralPlasticity = std::min(1.0f, pImpl->structuralPlasticity + 0.2f);
        }
    } else if (other.getName() == "5-HT") {
        // Dopamine + Serotonin for behavioral flexibility
        if (pImpl->rewardPredictionError > 0.5f) {
            pImpl->attentionModulation = std::min(1.0f, pImpl->attentionModulation + other.getLevel() * 0.1f);
        }
    }
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Trigger memory consolidation
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + reward * 0.3f);
    
    // Enhance attention for reward-paired stimuli
    pImpl->focusEnhancement = std::min(1.0f, pImpl->focusEnhancement + reward * 0.2f);
    
    // Update reward prediction error (simplified)
    pImpl->rewardPredictionError = std::abs(reward - 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->rewardPredictionError = error;
}

void Dopamine::signalMemoryConsolidation() {
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.3f);
    pImpl->consolidationTime = 5.0f; // 5 time units for consolidation
    pImpl->encodingPriority = std::min(1.0f, pImpl->encodingPriority + 0.2f);
}

void Dopamine::enhanceMemoryEncoding(float encodingStrength) {
    pImpl->memoryStrength = std::min(1.0f, pImpl->memoryStrength + encodingStrength * 0.1f);
    pImpl->encodingPriority = std::min(1.0f, pImpl->encodingPriority + encodingStrength * 0.15f);
}

void Dopamine::modulateAttention(float currentAttention) {
    pImpl->attentionModulation = std::min(1.0f, currentAttention * pImpl->level * 1.5f);
    pImpl->focusEnhancement = std::min(1.0f, pImpl->focusEnhancement + pImpl->level * 0.2f);
}

void Dopamine::enhanceFocus(float focusLevel) {
    pImpl->focusEnhancement = std::min(1.0f, focusLevel * pImpl->level * 1.2f);
    pImpl->selectiveAttention = std::min(1.0f, pImpl->selectiveAttention + pImpl->level * 0.1f);
}

void Dopamine::influenceStructuralPlasticity() {
    if (pImpl->level > 0.7f) {
        pImpl->promoteSynaptogenesis = true;
        pImpl->structuralWeight = std::min(1.5f, pImpl->structuralWeight + pImpl->level * 0.3f);
    }
}

void Dopamine::guideSynaptogenesis(bool strengthen, bool prune) {
    if (strengthen && pImpl->level > 0.5f) {
        pImpl->structuralPlasticity = std::min(1.0f, pImpl->structuralPlasticity + 0.2f);
    }
    if (!prune && pImpl->level > 0.5f) {
        pImpl->inhibitPruning = true;
    }
}

float Dopamine::getRewardSignal() const {
    return pImpl->rewardPredictionError;
}

float Dopamine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Dopamine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionalGain;
    float memoryEncoding;
    float sensoryGating;
    float neuralExcitability;
    float attentionSpan;
    float noveltyThreshold;
    
    // For theatrical attention switching
    bool theatricalSwitchEnabled;
    float attentionalSwitching;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), attentionalGain(0.0f),
             memoryEncoding(0.0f), sensoryGating(0.0f), neuralExcitability(0.0f),
             attentionSpan(0.0f), noveltyThreshold(0.5f),
             theatricalSwitchEnabled(false), attentionalSwitching(0.0f) {}
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
    return 1.0f + pImpl->level * 0.3f; // ACh enhances plasticity
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Decay other effects
    pImpl->attentionalGain *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->memoryEncoding *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->sensoryGating *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->neuralExcitability *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->attentionSpan *= (1.0f - pImpl->decayRate * dtFloat);
}

void Acetylcholine::interactWith(const Neuromodulator& other) {
    if (other.getName() == "DA") {
        // Strong synergy for learning and attention
        pImpl->attentionalGain = std::min(1.0f, pImpl->attentionalGain + other.getLevel() * 0.4f);
        pImpl->memoryEncoding = std::min(1.0f, pImpl->memoryEncoding + other.getLevel() * 0.3f);
        pImpl->theatricalSwitchEnabled = true;
    } else if (other.getName() == "NE") {
        // ACh + NE for alertness
        pImpl->neuralExcitability = std::min(1.0f, pImpl->neuralExcitability + other.getLevel() * 0.2f);
        pImpl->attentionSpan = std::min(1.0f, pImpl->attentionSpan + other.getLevel() * 0.2f);
    }
}

void Acetylcholine::enhanceSelectiveAttention() {
    pImpl->attentionalGain = std::min(1.0f, pImpl->attentionalGain + 0.3f);
}

void Acetylcholine::enhanceSensoryGating(const std::vector<float>& sensoryInput) {
    float inputStrength = 0.0f;
    for (float val : sensoryInput) {
        inputStrength += std::abs(val);
    }
    inputStrength /= sensoryInput.size();
    
    pImpl->sensoryGating = std::min(1.0f, pImpl->sensoryGating + inputStrength * 0.2f);
}

void Acetylcholine::increaseAttentionSpan(float duration) {
    pImpl->attentionSpan = std::min(1.0f, pImpl->attentionSpan + duration * 0.1f);
}

void Acetylcholine::enhanceMemoryEncoding() {
    pImpl->memoryEncoding = std::min(1.0f, pImpl->memoryEncoding + 0.3f);
}

void Acetylcholine::strengthenWorkingMemory(float capacity) {
    pImpl->attentionSpan = std::min(1.0f, pImpl->attentionSpan + capacity * 0.2f);
    pImpl->attentionalSwitching = std::min(1.0f, pImpl->attentionalSwitching + capacity * 0.1f);
}

void Acetylcholine::enhanceEpisodicEncoding(float eventSalience) {
    pImpl->memoryEncoding = std::min(1.0f, pImpl->memoryEncoding + eventSalience * 0.3f);
    pImpl->sensoryGating = std::min(1.0f, pImpl->sensoryGating + eventSalience * 0.2f);
}

void Acetylcholine::modulateNeuralExcitability() {
    pImpl->neuralExcitability = std::min(1.0f, pImpl->neuralExcitability + 0.2f);
}

void Acetylcholine::increaseSensoryGain(float gainFactor) {
    pImpl->attentionalGain = std::min(1.0f, pImpl->attentionalGain + gainFactor * 0.3f);
}

void Acetylcholine::decreaseNoise(float noiseReduction) {
    pImpl->sensoryGating = std::min(1.0f, pImpl->sensoryGating + noiseReduction * 0.5f);
}

void Acetylcholine::enableTheatricalAttentionSwitching() {
    pImpl->theatricalSwitchEnabled = true;
    pImpl->attentionalSwitching = 1.0f;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float alertness;
    float vigilance;
    float stressLevel;
    float noveltyDetection;
    float arousalState;
    float behavioralActivation;
    
    // Response tracking
    float stressResponseTime;
    float noveltyResponseStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), alertness(0.0f),
             vigilance(0.0f), stressLevel(0.0f), noveltyDetection(0.0f),
             arousalState(0.0f), behavioralActivation(0.0f),
             stressResponseTime(0.0f), noveltyResponseStrength(0.0f) {}
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
    return 1.0f + pImpl->level * 0.2f; // NE enhances plasticity moderately
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Decay other effects
    pImpl->alertness *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->vigilance *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->stressLevel *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->noveltyDetection *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->arousalState *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->behavioralActivation *= (1.0f - pImpl->decayRate * dtFloat);
    
    // Stress response recovery
    if (pImpl->stressResponseTime > 0.0f) {
        pImpl->stressResponseTime -= dtFloat * 0.5f;
        if (pImpl->stressResponseTime <= 0.0f) {
            pImpl->stressLevel = std::max(pImpl->baseline, pImpl->stressLevel - 0.1f);
        }
    }
}

void Norepinephrine::interactWith(const Neuromodulator& other) {
    if (other.getName() == "DA") {
        // DA + NE for motivation and action
        pImpl->behavioralActivation = std::min(1.0f, pImpl->behavioralActivation + other.getLevel() * 0.3f);
        if (pImpl->level > 0.5f && other.getLevel() > 0.5f) {
            pImpl->arousalState = std::min(1.0f, pImpl->arousalState + 0.2f);
        }
    } else if (other.getName() == "ACh") {
        // NE + ACh for alertness and attention
        pImpl->alertness = std::min(1.0f, pImpl->alertness + other.getLevel() * 0.2f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + other.getLevel() * 0.2f);
    } else if (other.getName() == "5-HT") {
        // NE + Serotonin for balanced approach-avoidance
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + other.getLevel() * 0.1f);
    }
}

void Norepinephrine::increaseAlertness(float level) {
    pImpl->alertness = std::min(1.0f, level * pImpl->level * 1.5f);
}

void Norepinephrine::enhanceVigilance(float duration) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + duration * 0.1f);
}

void Norepinephrine::modulateStressResponse(float stressLevel) {
    pImpl->stressLevel = std::min(1.0f, stressLevel);
    pImpl->stressResponseTime = 5.0f; // 5 time units of stress response
}

void Norepinephrine::enhanceFocusedAttention(float focusDuration) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + focusDuration * 0.1f);
}

void Norepinephrine::improveReactionTime(float improvementFactor) {
    pImpl->alertness = std::min(1.0f, pImpl->alertness + improvementFactor * 0.3f);
}

void Norepinephrine::respondToNovelty(float noveltyScore) {
    pImpl->noveltyDetection = std::min(1.0f, noveltyScore * pImpl->level * 1.2f);
    pImpl->noveltyResponseStrength = std::min(1.0f, pImpl->noveltyResponseStrength + noveltyScore * 0.3f);
}

void Norepinephrine::enhanceNoveltyDetection() {
    pImpl->noveltyDetection = std::min(1.0f, pImpl->noveltyDetection + 0.3f);
}

void Norepinephrine::modulateArousalState(float state) {
    pImpl->arousalState = std::min(1.0f, state * pImpl->level * 1.1f);
}

void Norepinephrine::activateBehavioralResponse() {
    pImpl->behavioralActivation = std::min(1.0f, pImpl->behavioralActivation + 0.3f);
}

void Norepinephrine::enhanceMotivation(float drive) {
    pImpl->behavioralActivation = std::min(1.0f, pImpl->behavioralActivation + drive * 0.2f);
    pImpl->alertness = std::min(1.0f, pImpl->alertness + drive * 0.1f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodRegulation;
    float impulsivityControl;
    float emotionalStability;
    float decisionMaking;
    float behavioralFlexibility;
    float socialBehavior;
    
    // Response tracking
    float moodState;
    float socialResponseStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), moodRegulation(0.0f),
             impulsivityControl(0.0f), emotionalStability(0.0f), decisionMaking(0.0f),
             behavioralFlexibility(0.0f), socialBehavior(0.0f),
             moodState(0.5f), socialResponseStrength(0.0f) {}
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
    return 1.0f - pImpl->level * 0.1f; // Serotonin may reduce plasticity for stability
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Decay other effects
    pImpl->moodRegulation *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->impulsivityControl *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->emotionalStability *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->decisionMaking *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->behavioralFlexibility *= (1.0f - pImpl->decayRate * dtFloat);
    pImpl->socialBehavior *= (1.0f - pImpl->decayRate * dtFloat);
    
    // Mood state recovery
    pImpl->moodState = std::max(0.0f, pImpl->moodState - dtFloat * 0.05f);
}

void Serotonin::interactWith(const Neuromodulator& other) {
    if (other.getName() == "DA") {
        // DA + 5-HT for balanced approach-avoidance
        pImpl->decisionMaking = std::min(1.0f, pImpl->decisionMaking + other.getLevel() * 0.2f);
        pImpl->moodRegulation = std::min(1.0f, pImpl->moodRegulation + other.getLevel() * 0.1f);
    } else if (other.getName() == "NE") {
        // 5-HT + NE for stress regulation
        pImpl->emotionalStability = std::min(1.0f, pImpl->emotionalStability + other.getLevel() * 0.2f);
    } else if (other.getName() == "ACh") {
        // 5-HT + ACh for cognitive control
        pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + other.getLevel() * 0.2f);
    }
}

void Serotonin::regulateMood(float moodState) {
    pImpl->moodState = moodState;
    pImpl->moodRegulation = std::min(1.0f, moodState * pImpl->level * 1.2f);
}

void Serotonin::reduceImpulsivity(float reductionFactor) {
    pImpl->impulsivityControl = std::min(1.0f, reductionFactor * pImpl->level * 1.5f);
}

void Serotonin::enhanceEmotionalStability(float stability) {
    pImpl->emotionalStability = std::min(1.0f, stability * pImpl->level * 1.3f);
}

void Serotonin::influenceDecisionMaking(float decisionQuality) {
    pImpl->decisionMaking = std::min(1.0f, decisionQuality * pImpl->level * 1.2f);
}

void Serotonin::modulateRiskTaking(float riskLevel) {
    pImpl->impulsivityControl = std::min(1.0f, pImpl->impulsivityControl + (1.0f - riskLevel) * 0.3f);
}

void Serotonin::enhanceCognitiveControl(float controlStrength) {
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + controlStrength * 0.2f);
    pImpl->decisionMaking = std::min(1.0f, pImpl->decisionMaking + controlStrength * 0.3f);
}

void Serotonin::enhanceBehavioralFlexibility() {
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + 0.3f);
}

void Serotonin::reduceHabitualResponses() {
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + 0.2f);
}

void Serotonin::increaseExplorationDrive(float driveLevel) {
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + driveLevel * 0.3f);
}

void Serotonin::modulateSocialBehavior(float socialityLevel) {
    pImpl->socialBehavior = std::min(1.0f, socialityLevel * pImpl->level * 1.2f);
}

void Serotonin::enhanceSocialLearning() {
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.3f);
}

float Serotonin::getAttentionInfluence() const {
    return pImpl->moodRegulation * 0.3f;
}

float Serotonin::getMemoryInfluence() const {
    return pImpl->emotionalStability * 0.4f;
}

float Serotonin::getArousalInfluence() const {
    return pImpl->behavioralFlexibility * 0.2f;
}

float Serotonin::getDecisionInfluence() const {
    return pImpl->decisionMaking * 0.5f;
}

float Serotonin::getFlexibilityInfluence() const {
    return pImpl->behavioralFlexibility * 0.6f;
}

float Serotonin::getLearningRate() const {
    return 1.0f - pImpl->impulsivityControl * 0.2f; // Stable learning rate
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodRegulation = 0.0f;
    pImpl->impulsivityControl = 0.0f;
    pImpl->emotionalStability = 0.0f;
    pImpl->decisionMaking = 0.0f;
    pImpl->behavioralFlexibility = 0.0f;
    pImpl->socialBehavior = 0.0f;
    pImpl->moodState = 0.5f;
}

} // namespace nlm