#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    // ACh dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory-related parameters
    float memoryConsolidationRate;
    float attentionBoost;
    float workingMemoryEnhancement;
    
    // State management
    float attentionScore;
    float memoryPriority;
    std::vector<float> memoryTrace;
    
    // Learning-related parameters
    float encodingRate;
    float consolidationRate;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.5f), decayRate(0.05f), 
             releaseRate(1.0f), memoryConsolidationRate(0.3f),
             attentionBoost(1.5f), workingMemoryEnhancement(1.2f),
             attentionScore(0.0f), memoryPriority(0.0f), encodingRate(0.5f),
             consolidationRate(0.2f) {}
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
    // ACh primarily affects encoding and consolidation, not raw plasticity
    // It enhances NMDA receptor function and promotes LTP at certain synapses
    float factor = 1.0f;
    
    // Low ACh: reduced attention, poor encoding
    if (pImpl->level < 0.2f) {
        factor = 0.5f + pImpl->level * 0.5f;
    } else if (pImpl->level < 0.6f) {
        // Moderate ACh: optimal attention and memory formation
        factor = 1.0f + pImpl->level * 0.4f;
    } else {
        // High ACh: may cause over-focusing on some stimuli
        factor = 1.3f + pImpl->level * 0.2f;
    }
    
    // Apply attention modulation
    factor *= (1.0f + pImpl->attentionScore * 0.3f);
    
    return factor;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update attention score based on level
    pImpl->attentionScore = pImpl->attentionScore * 0.9f + pImpl->level * 0.1f;
    
    // Update memory trace (slower decay than level)
    pImpl->memoryPriority = pImpl->memoryPriority * 0.95f;
    
    // Apply attention-based enhancement
    if (pImpl->level > 0.3f) {
        // Neural encoding enhancement
        float encodingBoost = pImpl->level * pImpl->encodingRate;
        
        // Memory consolidation
        float consolidationBoost = pImpl->level * pImpl->consolidationRate;
        
        // Update level with encoding/consolidation effects
        pImpl->level = std::min(pImpl->peak, pImpl->level + encodingBoost * 0.01f);
    }
}

void Acetylcholine::signalAttention(float attentionStrength, float memoryPriority) {
    // Enhance ACh release based on attentional demands
    if (attentionStrength > 0.0f) {
        // Phasic response to attentional focus
        float attentionResponse = attentionStrength * pImpl->attentionBoost;
        pImpl->level = std::max(pImpl->level, pImpl->baseline + attentionResponse);
        
        // Set memory priority
        pImpl->memoryPriority = std::max(pImpl->memoryPriority, memoryPriority);
        
        // Enhance working memory
        pImpl->workingMemoryEnhancement = 1.0f + attentionStrength * 0.5f;
    }
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryPattern, float strength) {
    // Store memory pattern for consolidation
    if (memoryPattern.size() > 0) {
        // Average pattern strength
        float patternStrength = 0.0f;
        for (float val : memoryPattern) {
            patternStrength += val;
        }
        patternStrength /= static_cast<float>(memoryPattern.size());
        
        // Scale by consolidation rate
        float consolidation = patternStrength * strength * pImpl->memoryConsolidationRate;
        
        // Enhance memory trace
        pImpl->memoryTrace.push_back(consolidation);
        
        // If consolidation is strong, enhance ACh level for future consolidation
        if (consolidation > 0.5f) {
            pImpl->level = std::min(pImpl->peak, pImpl->level + consolidation * 0.1f);
        }
    }
}

float Acetylcholine::getAttentionScore() const {
    return pImpl->attentionScore;
}

float Acetylcholine::getMemoryPriority() const {
    return pImpl->memoryPriority;
}

const std::vector<float>& Acetylcholine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

struct Norepinephrine::Impl {
    // NE dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilanceScore;
    float stressLevel;
    
    // Alertness parameters
    float alertness;
    float reactiveness;
    float inhibition;
    
    // State management
    float phasicArousal;
    float tonicVigilance;
    std::vector<float> alertHistory;
    std::vector<float> stressHistory;
    
    // Learning-related parameters
    float explorationBias;
    float riskAssessment;
    float noveltySensitivity;
    
    Impl() : level(0.02f), baseline(0.02f), peak(0.3f), decayRate(0.03f), 
             releaseRate(1.0f), arousalLevel(0.0f), vigilanceScore(0.0f),
             stressLevel(0.0f), alertness(0.0f), reactiveness(0.0f), inhibition(0.0f),
             phasicArousal(0.0f), tonicVigilance(0.0f), explorationBias(0.5f),
             riskAssessment(0.5f), noveltySensitivity(0.7f) {}
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
    // NE modulates plasticity in complex ways depending on level
    float factor = 1.0f;
    
    if (pImpl->level < 0.1f) {
        // Low NE: reduced alertness, slower learning
        factor = 0.5f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.3f) {
        // Moderate NE: optimal alertness and learning
        factor = 0.8f + pImpl->level * 2.5f;
    } else if (pImpl->level < 0.6f) {
        // High NE: increased arousal, enhanced attention but potential interference
        factor = 1.0f + pImpl->level * 1.5f;
    } else {
        // Very high NE: stress overload, reduced learning efficiency
        factor = 0.7f + pImpl->level * 0.5f;
    }
    
    // Apply vigilance modulation
    factor *= (1.0f + pImpl->vigilanceScore * 0.4f);
    
    // Apply arousal modulation
    factor *= (1.0f + pImpl->arousalLevel * 0.3f);
    
    return factor;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update arousal and vigilance
    pImpl->arousalLevel = pImpl->arousalLevel * 0.9f + pImpl->level * 0.2f;
    pImpl->tonicVigilance = pImpl->tonicVigilance * 0.95f + pImpl->level * 0.05f;
    
    // Update alertness based on NE level
    pImpl->alertness = pImpl->alertness * 0.9f + pImpl->level * 0.15f;
    pImpl->reactiveness = pImpl->reactiveness * 0.9f + pImpl->level * 0.1f;
    
    // Inhibition decreases with NE level
    pImpl->inhibition = 0.9f + pImpl->level * 0.2f;
    
    // Update stress level
    if (pImpl->level > 0.4f) {
        // Stress accumulation at high NE levels
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + pImpl->level * 0.01f);
    } else {
        // Stress recovery at moderate NE levels
        pImpl->stressLevel *= 0.98f;
    }
    
    // Store history
    pImpl->alertHistory.push_back(pImpl->alertness);
    if (pImpl->alertHistory.size() > 100) {
        pImpl->alertHistory.erase(pImpl->alertHistory.begin());
    }
    
    pImpl->stressHistory.push_back(pImpl->stressLevel);
    if (pImpl->stressHistory.size() > 100) {
        pImpl->stressHistory.erase(pImpl->stressHistory.begin());
    }
}

void Norepinephrine::signalArousal(float intensity, float novelty) {
    // Phasic NE response to arousing stimuli
    if (intensity > 0.0f) {
        // Arousal response
        float arousalResponse = intensity * 2.0f;
        pImpl->level = std::max(pImpl->level, pImpl->baseline + arousalResponse);
        
        // Update phasic arousal
        pImpl->phasicArousal = pImpl->phasicArousal * 0.8f + intensity * 0.2f;
        
        // Novelty enhances NE response
        if (novelty > 0.0f) {
            float noveltyBoost = novelty * pImpl->noveltySensitivity;
            pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyBoost);
            
            // Novelty increases exploration bias
            pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + novelty * 0.1f);
        }
        
        // Update risk assessment
        pImpl->riskAssessment = std::min(1.0f, pImpl->riskAssessment + intensity * 0.05f);
    }
}

void Norepinephrine::signalStress(float stressor, float duration) {
    // Stress response from environmental challenges
    if (stressor > 0.0f) {
        // Acute stress response
        float stressResponse = stressor * duration * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
        
        // Stress increases arousal and vigilance
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressResponse);
        pImpl->vigilanceScore = std::min(1.0f, pImpl->vigilanceScore + stressor * 0.3f);
        
        // High stress impairs learning
        pImpl->inhibition = std::min(2.0f, pImpl->inhibition + stressResponse);
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceScore() const {
    return pImpl->vigilanceScore;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

float Norepinephrine::getExplorationBias() const {
    return pImpl->explorationBias;
}

struct Serotonin::Impl {
    // 5-HT dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and affect parameters
    float moodScore;
    float satietyLevel;
    float impatience;
    float impulsivity;
    
    // Social behavior parameters
    float socialCohesion;
    float territoriality;
    float aggression;
    
    // State management
    float emotionalState;
    float socialBonding;
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    // Learning-related parameters
    float riskAversion;
    float patience;
    float rewardSensitivity;
    
    // Random number generator for stochastic behavior
    std::mt19937 rng;
    
    Impl() : level(0.02f), baseline(0.02f), peak(0.4f), decayRate(0.04f), 
             releaseRate(1.0f), moodScore(0.5f), satietyLevel(0.5f),
             impatience(0.5f), impulsivity(0.5f), socialCohesion(0.5f),
             territoriality(0.5f), aggression(0.3f), emotionalState(0.5f),
             socialBonding(0.5f), riskAversion(0.5f), patience(0.5f),
             rewardSensitivity(0.7f) {
        // Initialize random generator
        std::random_device rd;
        rng.seed(rd());
    }
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
    // Serotonin modulates plasticity in complex, context-dependent ways
    float factor = 1.0f;
    
    if (pImpl->level < 0.1f) {
        // Low serotonin: increased impulsivity, faster learning
        factor = 1.2f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.3f) {
        // Moderate serotonin: balanced learning
        factor = 0.8f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.6f) {
        // High serotonin: improved mood, enhanced stable learning
        factor = 1.0f + pImpl->level * 1.0f;
    } else {
        // Very high serotonin: over-satiated, reduced exploration
        factor = 0.6f + pImpl->level * 0.5f;
    }
    
    // Apply mood modulation
    factor *= (1.0f + pImpl->moodScore * 0.3f);
    
    // Apply satiety modulation
    factor *= (1.0f - pImpl->satietyLevel * 0.2f);
    
    // Apply social bonding modulation
    factor *= (1.0f + pImpl->socialBonding * 0.2f);
    
    return factor;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update mood score based on level
    pImpl->moodScore = pImpl->moodScore * 0.9f + pImpl->level * 0.2f;
    
    // Update satiety
    pImpl->satietyLevel = pImpl->satietyLevel * 0.95f;
    
    // Update emotional state
    pImpl->emotionalState = pImpl->emotionalState * 0.9f + pImpl->level * 0.1f;
    
    // Update social bonding
    pImpl->socialBonding = pImpl->socialBonding * 0.95f;
    
    // Store history
    pImpl->moodHistory.push_back(pImpl->moodScore);
    if (pImpl->moodHistory.size() > 100) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    pImpl->socialHistory.push_back(pImpl->socialBonding);
    if (pImpl->socialHistory.size() > 100) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
}

void Serotonin::signalReward(float reward, float socialContext) {
    // Serotonin response to reward, modulated by social context
    if (reward > 0.0f) {
        // Reward response (tonic)
        float rewardResponse = reward * 0.8f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
        
        // Satiety increases with reward
        pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + reward * 0.05f);
        
        // Social context modulates response
        if (socialContext > 0.0f) {
            float socialMod = socialContext * 0.5f;
            pImpl->level = std::min(pImpl->peak, pImpl->level + socialMod);
            
            // Social bonding increases
            pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + socialContext * 0.1f);
        }
        
        // Update patience based on reward history
        pImpl->patience = std::min(1.0f, pImpl->patience + reward * 0.01f);
    }
}

void Serotonin::signalSocialEvent(float socialValue, float groupCohesion) {
    // Serotonin response to social events
    if (socialValue > 0.0f) {
        // Positive social event
        float socialResponse = socialValue * 1.2f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialResponse);
        
        // Increase social cohesion
        pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialValue * 0.1f);
        
        // Decrease aggression
        pImpl->aggression *= (1.0f - socialValue * 0.05f);
        
        // Increase territoriality (protect group)
        pImpl->territoriality = std::min(1.0f, pImpl->territoriality + socialValue * 0.05f);
    } else if (socialValue < 0.0f) {
        // Negative social event (conflict, exclusion)
        float conflictResponse = std::abs(socialValue) * 1.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + conflictResponse);
        
        // Decrease social cohesion
        pImpl->socialCohesion *= (1.0f - std::abs(socialValue) * 0.1f);
        
        // Increase aggression
        pImpl->aggression = std::min(1.0f, pImpl->aggression + std::abs(socialValue) * 0.1f);
        
        // Increase territoriality (defensive)
        pImpl->territoriality = std::min(1.0f, pImpl->territoriality + std::abs(socialValue) * 0.05f);
        
        // Decrease patience
        pImpl->patience *= (1.0f - std::abs(socialValue) * 0.05f);
    }
}

void Serotonin::signalStress(float stressLevel) {
    // Serotonin response to stress (serotonin often decreases under chronic stress)
    if (stressLevel > 0.5f) {
        // Chronic stress reduces serotonin
        float stressImpact = (stressLevel - 0.5f) * 0.8f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - stressImpact);
        
        // Increase impulsivity
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + stressLevel * 0.1f);
        
        // Decrease patience
        pImpl->patience *= (1.0f - stressLevel * 0.05f);
    } else {
        // Acute stress can actually increase serotonin in some contexts
        float stressImpact = stressLevel * 0.3f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressImpact);
    }
}

float Serotonin::getMoodScore() const {
    return pImpl->moodScore;
}

float Serotonin::getSatietyLevel() const {
    return pImpl->satietyLevel;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getAggression() const {
    return pImpl->aggression;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorGain;
    float rewardHistoryGain;
    std::queue<float> rewardQueue;  // For smoothing rewards
    std::queue<float> errorQueue;  // For prediction error smoothing
    float integratedReward;
    float integratedError;
    float eligibilityTrace;
    float plasticityModulation;
    
    // DA dynamics parameters (realistic)
    float tonicLevel;           // Baseline dopamine level
    float phasicPeak;           // Phasic response amplitude
    float phasicDecay;          // Phasic response decay time constant
    float burstFrequency;       // Burst firing frequency (Hz)
    float pauseDuration;         // Pause duration between bursts
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), predictionErrorGain(0.5f), rewardHistoryGain(0.2f),
             integratedReward(0.0f), integratedError(0.0f), eligibilityTrace(0.0f),
             plasticityModulation(1.0f), tonicLevel(0.05f), phasicPeak(2.0f),
             phasicDecay(0.05f), burstFrequency(4.0f), pauseDuration(100.0f) {}
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
    // Real dopamine-modulated plasticity: more complex than linear
    float modulation = pImpl->plasticityModulation;
    
    // Nonlinear relationship: low DA reduces plasticity, moderate DA increases it,
    // high DA can decrease plasticity (overlearning prevention)
    if (pImpl->level < 0.2f) {
        modulation = 0.3f + pImpl->level * 0.7f;  // Low DA: very low plasticity
    } else if (pImpl->level < 0.6f) {
        modulation = 0.8f + pImpl->level * 0.5f;  // Moderate DA: increased plasticity
    } else {
        modulation = 1.2f - pImpl->level * 0.6f;  // High DA: decreased plasticity
    }
    
    return modulation;
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline with temporal difference learning
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate smoothed reward and error signals
    if (!pImpl->rewardQueue.empty()) {
        pImpl->integratedReward += pImpl->rewardQueue.front() * pImpl->rewardHistoryGain * dtFloat;
        pImpl->rewardQueue.pop();
    }
    
    if (!pImpl->errorQueue.empty()) {
        pImpl->integratedError += pImpl->errorQueue.front() * pImpl->predictionErrorGain * dtFloat;
        pImpl->errorQueue.pop();
    }
    
    // Update level based on integrated signals (state-space model)
    float rewardInput = pImpl->integratedReward * pImpl->releaseRate;
    float errorInput = pImpl->integratedError * pImpl->predictionErrorGain;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = pImpl->eligibilityTrace * 0.95f + 
                              (rewardInput + errorInput) * 0.05f;
    
    // Apply input with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardInput + errorInput);
}

void Dopamine::signalReward(float reward) {
    // More realistic reward signaling with burst-pause dynamics
    if (reward > 0.0f) {
        // Phasic burst response
        float burstAmplitude = std::min(reward, 2.0f) * pImpl->phasicPeak;
        burstAmplitude = std::min(burstAmplitude, 1.0f);  // Cap to 1.0
        
        pImpl->level = std::max(pImpl->level, pImpl->tonicLevel + burstAmplitude);
        
        // Queue for smoothing
        pImpl->rewardQueue.push(reward);
        
        // Update eligibility trace for reward-modulated plasticity
        pImpl->eligibilityTrace += reward * 0.1f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TPC model)
    float scaledError = error * pImpl->predictionErrorGain;
    
    if (error > 0.0f) {
        // Positive RPE (better than expected): increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + scaledError * 2.0f);
        pImpl->plasticityModulation *= (1.0f + scaledError * 0.1f);
    } else if (error < 0.0f) {
        // Negative RPE (worse than expected): decrease dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level + scaledError);
        pImpl->plasticityModulation *= (1.0f + scaledError * 0.2f);  // Stronger depression
    }
    
    // Queue for smoothing
    pImpl->errorQueue.push(error);
}

} // namespace nlm
