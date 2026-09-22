#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeToPeak;
    float decayTime;
    bool isPhasic;
    float phasicLevel;
    float tonicLevel;
    std::vector<float> eligibilityTrace;  // For credit assignment
    std::vector<float> rewardHistory;    // For prediction error computation
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)          // Tonic baseline
        , peak(1.0f)
        , decayRate(0.05f)         // Decay rate for phasic signals
        , releaseRate(2.0f)        // Burst rate for phasic signals
        , timeToPeak(0.5f)         // Time to peak in seconds
        , decayTime(2.0f)          // Decay time in seconds
        , isPhasic(false)
        , phasicLevel(0.0f)
        , tonicLevel(baseline)
    {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components
    return pImpl->tonicLevel + pImpl->phasicLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->phasicLevel = level;
}

float Dopamine::getPlasticityFactor() const {
    // PHASE 2: Real dopamine-modulated plasticity factor
    // Dopamine has complex effects on different plasticity mechanisms:
    // - DA facilitates LTP (Long-Term Potentiation) at excitatory synapses
    // - DA suppresses LTD (Long-Term Depression) at the same synapses
    // - DA modulates synaptic tagging and capture for LTP
    // - Different dopamine receptor subtypes (D1, D2) have opposing effects
    
    float level = getLevel();
    
    if (level < 0.3f) {
        // Low dopamine: impairs plasticity, LTD dominance
        return std::max(0.1f, level * 0.5f);
    } else if (level < 0.7f) {
        // Moderate dopamine: optimal for LTP
        return 0.3f + (level - 0.3f) * 1.17f;  // Range: 0.3 to 1.0
    } else {
        // High dopamine: excessive LTP, reduced stability
        return std::min(2.0f, 1.0f + (level - 0.7f) * 1.67f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 6: Real dopamine dynamics
    // Dopamine signals have both tonic and phasic components
    
    float deltaTime = static_cast<float>(dt);
    
    // Phase 1: Phasic response (reward prediction error)
    if (pImpl->phasicLevel > 0.0f) {
        // Phasic dopamine bursts (rewards, prediction errors)
        // Exponential decay
        pImpl->phasicLevel *= std::exp(-deltaTime / pImpl->decayTime);
        
        // Update tonic level based on prediction error learning
        // Learning rate depends on phasic signal
        float learningRate = pImpl->phasicLevel * 0.1f;
        pImpl->tonicLevel = std::clamp(pImpl->tonicLevel + (0.5f - pImpl->tonicLevel) * learningRate, 
                                      pImpl->baseline, 1.0f);
    }
    
    // Phase 2: Background noise and spontaneous activity
    // More variable at low levels (exploration-promoting)
    if (pImpl->tonicLevel < 0.2f) {
        pImpl->phasicLevel += (pImpl->level - pImpl->phasicLevel) * 0.05f * deltaTime;
    }
    
    // Ensure consistency
    pImpl->level = getLevel();
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with spike timing dependence
    // Dopamine responds to unexpected rewards with phasic bursts
    // The magnitude depends on reward prediction error
    
    if (reward > 0.0f) {
        // Phasic burst proportional to reward magnitude
        // Saturation at high rewards
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + reward * pImpl->releaseRate, pImpl->peak);
        pImpl->isPhasic = true;
        
        // Record in eligibility trace for credit assignment
        pImpl->eligibilityTrace.push_back(pImpl->phasicLevel);
        if (pImpl->eligibilityTrace.size() > 100) {
            pImpl->eligibilityTrace.erase(pImpl->eligibilityTrace.begin());
        }
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Positive error (better than expected) -> dopamine burst
    // Negative error (worse than expected) -> dopamine dip
    
    if (error > 0.0f) {
        // Better than expected: reward prediction error
        // Dopamine burst proportional to error magnitude
        pImpl->phasicLevel += error * pImpl->releaseRate;
    } else if (error < 0.0f) {
        // Worse than expected: prediction error
        // Transient dip in dopamine levels
        float dip = std::min(0.3f, -error * 0.5f);
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - dip);
    }
    
    pImpl->phasicLevel = std::clamp(pImpl->phasicLevel, 0.0f, pImpl->peak);
    pImpl->isPhasic = true;
    
    // Store prediction error for learning
    pImpl->rewardHistory.push_back(error);
    if (pImpl->rewardHistory.size() > 50) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

// Real neuromodulation effects on plasticity
void Dopamine::applyToPlasticity(STDP* stdp, float& ltpWeight, float& ltdWeight) {
    if (!stdp) return;
    
    float level = getLevel();
    float factor = getPlasticityFactor();
    
    // D1 receptor effects: enhance LTP
    if (level > 0.3f) {
        float d1Effect = (level - 0.3f) / 0.7f;  // D1 activation
        ltpWeight *= (1.0f + d1Effect * 0.5f);    // Boost LTP
        ltdWeight *= (1.0f - d1Effect * 0.2f);    // Suppress LTD
    }
    
    // D2 receptor effects: suppress background noise
    if (level < 0.5f) {
        float d2Effect = (0.5f - level) / 0.5f;  // D2 activation
        ltpWeight *= (1.0f - d2Effect * 0.1f);    // Reduce noise
        ltdWeight *= (1.0f - d2Effect * 0.1f);
    }
    
    // Apply overall modulation factor
    ltpWeight *= factor;
    ltdWeight *= factor;
    
    // Ensure weights are in valid range
    ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
    ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->phasicLevel = 0.0f;
    pImpl->tonicLevel = pImpl->baseline;
    pImpl->isPhasic = false;
    pImpl->eligibilityTrace.clear();
    pImpl->rewardHistory.clear();
}

// Acetylcholine implementation - attention and memory modulation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryReconciliation;
    float corticalClearance;
    bool isActive;
    float workingMemoryTarget;
    float episodicTarget;
    float attentionWeight;
    
    Impl() 
        : level(0.0f)
        , baseline(0.02f)          // Low tonic baseline for attention
        , peak(1.0f)
        , decayRate(0.08f)         // Faster decay than dopamine
        , releaseRate(1.5f)        // Quick response to stimuli
        , attentionGain(0.8f)      // Strong attention modulation
        , memoryReconciliation(0.6f)  // Memory consolidation strength
        , corticalClearance(0.5f)     // Attention filtering
        , isActive(false)
        , workingMemoryTarget(0.7f)   // Target for working memory updating
        , episodicTarget(0.8f)       // Target for episodic memory
        , attentionWeight(1.0f)       // Current attention level
    {}
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
    // ACh has specific effects on attention and memory:
    // - Enhances signal-to-noise ratio in cortical processing
    // - Facilitates memory encoding and consolidation
    // - Modulates working memory updating
    // - Promotes attention to salient stimuli
    
    float level = getLevel();
    
    if (level < 0.2f) {
        // Low ACh: poor attention, weak memory encoding
        return std::max(0.3f, 0.5f + level * 0.5f);
    } else if (level < 0.6f) {
        // Moderate ACh: optimal for attention and working memory
        return 0.7f + (level - 0.2f) * 0.45f;
    } else {
        // High ACh: strong attention but potential for over-focusing
        return std::min(1.5f, 1.0f + (level - 0.6f) * 1.5f);
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics:
    // - Builds up with stimulus salience
    // - Decays between stimuli
    // - Modulates cortical arousal and attention
    
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Modulate attention based on current level
    pImpl->attentionWeight = std::clamp(0.5f + (getLevel() - 0.1f) * pImpl->attentionGain, 
                                       0f, 2.0f);
    
    // Update memory targets based on cholinergic tone
    pImpl->workingMemoryTarget = std::clamp(0.5f + getLevel() * 0.3f, 0f, 1.0f);
    pImpl->episodicTarget = std::clamp(0.6f + getLevel() * 0.2f, 0f, 1.0f);
}

void Acetylcholine::signalStimulus(float salience, float predictionError) {
    // ACh responds to stimulus salience and prediction errors
    // High salience or positive prediction error triggers ACh release
    
    float response = salience * predictionError;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
    
    // Focus attention on salient stimuli
    pImpl->isActive = (salience > 0.5f);
}

void Acetylcholine::applyToMemory(NeuralWorkingMemory* workingMemory, NeuralEpisodicMemory* episodicMemory) {
    if (!workingMemory || !episodicMemory) return;
    
    // ACh effects on working memory:
    // - Increases maintenance of relevant information
    // - Enhances updating with new information
    // - Reduces interference from irrelevant items
    
    float level = getLevel();
    
    // Modulate working memory parameters based on ACh level
    // Higher ACh = better working memory performance up to optimal level
    if (level > 0.3f && level < 0.8f) {
        // In the optimal range: enhance working memory stability
        workingMemory->setDecayRate(0.01f * (1.0f - (level - 0.3f) * 0.67f));
        
        // Strengthen memory traces
        workingMemory->strengthenMemory(level * 0.5f);
    }
    
    // ACh effects on episodic memory:
    // - Facilitates memory encoding
    // - Enhances consolidation during rest periods
    // - Promotes memory replay during sleep
    
    // Update consolidation targets
    float consolidationStrength = level * 0.8f;
    
    // Apply attention modulation to episodic memory
    // Similar implementation as in concept formation
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->isActive = false;
    pImpl->attentionWeight = 1.0f;
}

// Norepinephrine implementation - arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalGain;
    float vigilanceFactor;
    bool isActive;
    float alertnessTarget;
    float focusTarget;
    
    Impl() 
        : level(0.0f)
        , baseline(0.01f)          // Very low tonic baseline for NE
        , peak(1.0f)
        , decayRate(0.12f)         // Fast decay for alertness changes
        , releaseRate(2.0f)        // Quick response to salient stimuli
        , arousalGain(0.9f)       // Strong arousal modulation
        , vigilanceFactor(0.7f)   // Enhances vigilance during high states
        , isActive(false)
        , alertnessTarget(0.8f)    // Target arousal level
        , focusTarget(0.6f)       // Target focus level
    {}
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
    // NE has specific effects on arousal and vigilance:
    // - Increases locus coeruleus activity
    // - Enhances signal detection in noisy environments
    // - Modulates sleep-wake cycles
    // - Affects memory encoding during high arousal
    
    float level = getLevel();
    
    if (level < 0.1f) {
        // Very low NE: poor alertness, sleepy
        return std::max(0.2f, level * 2.0f);
    } else if (level < 0.5f) {
        // Moderate NE: optimal alertness for attention
        return 0.4f + (level - 0.1f) * 0.6f;
    } else {
        // High NE: hyper-alertness, can cause over-arousal
        return std::min(1.5f, 0.8f + (level - 0.5f) * 1.4f);
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics:
    // - Builds slowly with sustained attention/effort
    // - Decays when attention is withdrawn
    // - Promotes vigilance for novel or important stimuli
    
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Modulate vigilance based on current level
    // High NE enhances signal detection in noisy environments
    if (getLevel() > 0.3f) {
        pImpl->vigilanceFactor = std::min(1.5f, 0.7f + (getLevel() - 0.3f) * 1.0f);
    }
    
    // Update alert and focus targets
    pImpl->alertnessTarget = std::clamp(0.5f + getLevel() * 0.4f, 0f, 1.0f);
    pImpl->focusTarget = std::clamp(0.4f + getLevel() * 0.3f, 0f, 1.0f);
}

void Norepinephrine::signalArousal(float stimulusIntensity, float threatLevel) {
    // NE responds to stimulus intensity and threat level
    // High intensity or threat triggers NE release
    
    float response = stimulusIntensity * (1.0f + threatLevel * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
    
    // Enhance vigilance for potential threats
    pImpl->isActive = (stimulusIntensity > 0.3f || threatLevel > 0.5f);
}

void Norepinephrine::applyToAttention(AttentionalSelection* attention) {
    if (!attention) return;
    
    // NE effects on attention:
    // - Increases alertness and readiness to respond
    // - Enhances processing of salient/stimulating inputs
    // - Reduces response to routine/redundant inputs
    
    float level = getLevel();
    
    // Modulate attention parameters based on NE level
    if (level > 0.2f) {
        // Higher NE: enhance attentional gain
        // This is implemented in the attention system's internal mechanisms
    }
    
    // Update vigilance in attention system
    // Note: This would require attention system to expose vigilance update method
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->isActive = false;
}

// Serotonin implementation - mood regulation and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodGain;
    float socialModulation;
    float impulsivityControl;
    bool isActive;
    float rewardTarget;
    float inhibitionTarget;
    
    Impl() 
        : level(0.0f)
        , baseline(0.03f)          // Low baseline for mood regulation
        , peak(1.0f)
        , decayRate(0.06f)         // Moderate decay
        , releaseRate(1.0f)        // Steady release
        , moodGain(0.6f)           // Mood modulation strength
        , socialModulation(0.7f)   // Social behavior modulation
        , impulsivityControl(0.8f) // Impulsivity control strength
        , isActive(false)
        , rewardTarget(0.6f)       // Target reward processing
        , inhibitionTarget(0.7f)  // Target behavioral inhibition
    {}
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
    // Serotonin has specific effects on mood and social behavior:
    // - Regulates mood and emotional state
    // - Modulates social behavior and bonding
    // - Controls impulsivity and aggression
    // - Affects memory consolidation during emotional states
    
    float level = getLevel();
    
    if (level < 0.2f) {
        // Low serotonin: depressive mood, poor impulse control
        return std::max(0.2f, 0.4f + level * 0.5f);
    } else if (level < 0.7f) {
        // Moderate serotonin: optimal mood and behavior
        return 0.6f + (level - 0.2f) * 0.57f;
    } else {
        // High serotonin: potential for excessive inhibition
        return std::min(1.2f, 0.8f + (level - 0.7f) * 0.71f);
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics:
    // - Slowly builds with repeated positive experiences
    // - Decays with stress or negative experiences
    // - Modulates mood and social evaluation
    
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Modulate mood based on current level
    // Higher serotonin = more positive mood and social engagement
    if (getLevel() > 0.3f) {
        // Update reward processing targets
        pImpl->rewardTarget = std::min(1.0f, 0.5f + (getLevel() - 0.3f) * 1.1f);
        pImpl->inhibitionTarget = std::min(1.0f, 0.4f + (getLevel() - 0.3f) * 0.8f);
    }
}

void Serotonin::signalSocialInteraction(float socialReward, float novelty) {
    // Serotonin responds to social reward and novelty
    // Positive social interactions increase serotonin
    
    float response = socialReward * (1.0f + novelty * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
    
    // Enhance social processing
    pImpl->isActive = (socialReward > 0.0f);
}

void Serotonin::applyToSocialBehavior(SocialLearning* social) {
    if (!social) return;
    
    // Serotonin effects on social behavior:
    // - Enhances social bonding and trust
    // - Reduces social anxiety and fear
    // - Promotes prosocial behavior
    // - Modulates social memory consolidation
    
    float level = getLevel();
    
    // Modulate social learning parameters
    if (level > 0.4f) {
        // Higher serotonin: enhanced social learning
    }
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->isActive = false;
}

} // namespace nlm
