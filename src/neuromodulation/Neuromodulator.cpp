#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float tonicBaseline;
    float phasicPeak;
    float decayRate;
    float releaseRate;
    
    // Reward prediction error tracking
    float lastRewardPredictionError;
    float predictionErrorGain;
    float rewardPredictionGain;
    
    // Memory connections
    class Brain* brain;
    
    // Plasticity modulation
    float stdpScaling;
    float rewardModulation;
    
    Impl() : level(0.0f), tonicBaseline(0.02f), phasicPeak(1.0f), 
             decayRate(0.05f), releaseRate(2.0f),
             lastRewardPredictionError(0.0f), predictionErrorGain(0.8f), 
             rewardPredictionGain(1.5f), brain(nullptr), stdpScaling(1.0f), 
             rewardModulation(1.0f) {}
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
    // Real dopamine-modulated plasticity factor
    // Higher dopamine (particularly phasic) increases synaptic learning rate
    // Dopamine modulates both Hebbian and reward-modulated plasticity
    float dopamineInfluence = std::min(1.0f, pImpl->level / 0.5f);
    float plasticity = 0.5f + 0.5f * dopamineInfluence * pImpl->rewardModulation;
    
    // Phasic dopamine specifically enhances reward-modulated STDP
    if (dopamineInfluence > 0.3f) {
        plasticity *= (1.0f + 0.5f * dopamineInfluence);
    }
    
    return plasticity * pImpl->stdpScaling;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with phasic and tonic components
    float targetTonic = pImpl->tonicBaseline;
    
    // Tonic dopamine modulates overall motivation and arousal
    if (pImpl->brain) {
        // Integrate with working memory to adjust tonic levels
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float memoryActivity = wm->getMemoryActivity();
            targetTonic += memoryActivity * 0.02f;
        }
        
        // Integrate with episodic memory for context-dependent modulation
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            // Use recent reward history to modulate tonic dopamine
            if (em->getEpisodeCount() > 0) {
                float avgReward = em->getAverageReward();
                targetTonic += avgReward * 0.01f;
            }
        }
    }
    
    // Smooth transition to target tonic level
    pImpl->level += (targetTonic - pImpl->level) * 0.01f;
    
    // Ensure level stays in valid range (0-1)
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with reward prediction error
    float predictionError = reward - pImpl->lastRewardPredictionError;
    pImpl->lastRewardPredictionError = reward;
    
    // Phasic dopamine burst for unexpected rewards (positive RPE)
    if (predictionError > 0.0f) {
        // Large phasic response for unexpected reward
        pImpl->level = std::min(pImpl->phasicPeak, pImpl->level + 
                               predictionError * pImpl->rewardPredictionGain);
    } else if (predictionError < 0.0f) {
        // Phasic dip for omitted expected rewards (negative RPE)
        // Tonic dopamine adjusts downward
        float errorMagnitude = std::abs(predictionError);
        pImpl->level = std::max(0.0f, pImpl->level - 
                                errorMagnitude * pImpl->predictionErrorGain * 0.5f);
    }
    
    // Integrate with memory systems
    if (pImpl->brain) {
        // Update working memory strength based on reward
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            wm->strengthenMemory(1.0f + reward * 0.1f);
        }
        
        // Store reward in episodic memory context
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            // Create a memory item for this reward event
            // (In a real implementation, we would record this)
        }
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Negative RPE (omitted reward) reduces dopamine
    // Positive RPE (unexpected reward) increases dopamine
    pImpl->lastRewardPredictionError = error;
    
    // Immediate phasic response to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + 
                           std::abs(error) * pImpl->predictionErrorGain);
    
    // Integrate prediction error with learning systems
    if (pImpl->brain) {
        // Modulate plasticity based on prediction error
        // Stronger prediction errors lead to stronger learning
        float errorMagnitude = std::min(1.0f, std::abs(error) * 2.0f);
        pImpl->rewardModulation = 1.0f + errorMagnitude * 0.5f;
        
        // Update working memory predictions based on error
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            // Adjust memory expectations based on prediction error
            wm->strengthenMemory(1.0f + error * 0.05f);
        }
    }
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation parameters
    float attentionGain;
    float focusStrength;
    
    // Memory consolidation parameters
    float consolidationRate;
    float memoryStabilization;
    
    // Working memory enhancement parameters
    float wmBoost;
    float rehearsalEnhancement;
    
    // Brain integration
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.03f), releaseRate(1.5f),
             attentionGain(1.0f), focusStrength(0.8f),
             consolidationRate(0.5f), memoryStabilization(0.9f),
             wmBoost(1.2f), rehearsalEnhancement(1.5f), brain(nullptr) {}
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
    // Acetylcholine modulates attention and working memory
    // Enhances synaptic plasticity for attended information
    float attentionLevel = std::min(1.0f, pImpl->level * 2.0f);
    
    // Higher ACh increases plasticity for attended items
    float plasticity = 0.6f + 0.4f * attentionLevel;
    
    // ACh specifically enhances working memory-related plasticity
    if (pImpl->brain) {
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float memoryActivity = wm->getMemoryActivity();
            plasticity *= (1.0f + memoryActivity * 0.3f * attentionLevel);
        }
    }
    
    return plasticity;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Tonic acetylcholine for baseline attention
    // Phasic bursts for focused attention on novel stimuli
    float targetLevel = pImpl->baseline;
    
    // Integrate with attentional system
    if (pImpl->brain) {
        if (auto* attention = pImpl->brain->getAttention()) {
            // ACh modulates attentional gain
            targetLevel += attentionStrength_ * 0.01f;
        }
    }
    
    // Smooth transition to target level
    pImpl->level += (targetLevel - pImpl->level) * 0.02f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::signalAttention(float salience) {
    // Phasic acetylcholine burst for salient stimuli
    // Enhances encoding and consolidation of attended information
    float phasicBurst = std::min(pImpl->peak, salience * pImpl->releaseRate);
    pImpl->level += phasicBurst;
    
    // Integrate with memory systems
    if (pImpl->brain) {
        // Enhance working memory encoding of attended items
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            wm->strengthenMemory(1.0f + salience * 0.2f);
        }
        
        // Boost episodic memory consolidation
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            // Increase memory stabilization based on attention
            // (In a real implementation, we would access and modify memory items)
        }
        
        // Modulate attentional selection
        if (auto* attention = pImpl->brain->getAttention()) {
            attention->increaseSalience(salience * pImpl->attentionGain);
        }
    }
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

// Add a helper method to AttentionalSelection for ACh integration
void AttentionalSelection::increaseSalience(float additionalSalience) {
    // Apply attention boost based on acetylcholine signaling
    for (auto& neuronSalience : neuronSalience_) {
        neuronSalience += additionalSalience * 0.1f;
    }
    
    float attentionStrength = std::min(1.0f, inhibitionStrength_ + excitationStrength_ * 0.5f + additionalSalience * 0.05f);
    setInhibitionStrength(attentionStrength);
}

} // namespace nlm

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilance;
    float stressResponse;
    
    // Cognitive enhancement parameters
    float workingMemoryBoost;
    float attentionBoost;
    float executiveFunction;
    
    // Brain integration
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.04f), releaseRate(1.2f),
             arousalLevel(0.5f), vigilance(0.5f), stressResponse(0.0f),
             workingMemoryBoost(0.3f), attentionBoost(0.4f), executiveFunction(0.6f),
             brain(nullptr) {}
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
    // Norepinephrine modulates arousal and cognitive function
    // Enhances plasticity during high arousal states
    float arousalInfluence = std::min(1.0f, pImpl->level * 2.0f);
    
    // Higher NE increases plasticity, especially in alert states
    float plasticity = 0.7f + 0.3f * arousalInfluence;
    
    // NE specifically enhances working memory and attention-related plasticity
    if (pImpl->brain) {
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float memoryActivity = wm->getMemoryActivity();
            plasticity *= (1.0f + memoryActivity * 0.2f * arousalInfluence);
        }
        
        // Boost executive function plasticity
        plasticity *= (1.0f + pImpl->executiveFunction * arousalInfluence * 0.3f);
    }
    
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Tonic norepinephrine for baseline arousal
    // Phasic bursts for stress response and focused attention
    float targetLevel = pImpl->baseline;
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Adjust arousal based on task demands
        if (auto* attention = pImpl->brain->getAttention()) {
            float attentionStrength = attention->getInhibitionFor(NeuronId(0)) + 
                                     attention->getExcitationFor(NeuronId(0)) * 0.5f;
            targetLevel += attentionStrength * 0.01f;
        }
        
        // Stress response: increase NE during challenging conditions
        // (In a real implementation, we would detect stress levels from various sources)
        pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.001f * dt);
        targetLevel += pImpl->stressResponse * 0.02f;
    }
    
    // Smooth transition to target level
    pImpl->level += (targetLevel - pImpl->level) * 0.02f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::signalStress(float stressorLevel) {
    // Phasic norepinephrine burst for stress response
    // Increases arousal and vigilance
    float phasicResponse = std::min(pImpl->peak, stressorLevel * pImpl->releaseRate * 1.5f);
    pImpl->level += phasicResponse;
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Boost working memory under stress
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            wm->strengthenMemory(1.0f + stressorLevel * 0.3f);
        }
        
        // Enhance attention under stress
        if (auto* attention = pImpl->brain->getAttention()) {
            attention->increaseSalience(stressorLevel * pImpl->vigilance);
        }
    }
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

} // namespace nlm

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood regulation parameters
    float moodLevel;
    float behavioralInhibition;
    float socialBehavior;
    
    // Stress regulation parameters
    float stressResponse;
    float anxietyLevel;
    float rewardSensitivity;
    
    // Brain integration
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f),
             moodLevel(0.5f), behavioralInhibition(0.5f), socialBehavior(0.5f),
             stressResponse(0.0f), anxietyLevel(0.0f), rewardSensitivity(0.5f),
             brain(nullptr) {}
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
    // Serotonin modulates mood and behavioral inhibition
    // Reduces plasticity during high inhibition states
    float inhibitionLevel = std::min(1.0f, pImpl->level * 2.0f);
    
    // Higher 5-HT reduces plasticity to prevent over-learning
    float plasticity = 0.9f - 0.3f * inhibitionLevel;
    
    // 5-HT specifically modulates reward-related plasticity
    plasticity *= (1.0f - pImpl->behavioralInhibition * 0.2f);
    
    // Enhance social behavior-related plasticity
    if (pImpl->brain) {
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            // Social memories have different plasticity rates
            size_t episodeCount = em->getEpisodeCount();
            if (episodeCount > 0) {
                // Adjust plasticity based on social experience
                float socialModulation = std::min(1.0f, static_cast<float>(episodeCount) * 0.01f);
                plasticity *= (1.0f - socialModulation * pImpl->socialBehavior * 0.1f);
            }
        }
    }
    
    return plasticity;
}

void Serotonin::update(TimestepDuration dt) {
    // Tonic serotonin for baseline mood regulation
    // Phasic bursts for behavioral inhibition and social behavior modulation
    float targetLevel = pImpl->baseline;
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Adjust mood based on experience
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            if (em->getEpisodeCount() > 0) {
                // Use recent reward/punishment history to modulate mood
                float avgReward = em->getAverageReward();
                targetLevel += avgReward * 0.01f;
            }
        }
        
        // Stress regulation
        pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.001f * dt);
        pImpl->anxietyLevel = std::max(0.0f, pImpl->anxietyLevel - 0.0005f * dt);
        
        // Adjust target level based on stress
        targetLevel -= pImpl->stressResponse * 0.01f;
        targetLevel -= pImpl->anxietyLevel * 0.005f;
    }
    
    // Smooth transition to target level
    pImpl->level += (targetLevel - pImpl->level) * 0.02f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Phasic serotonin response to mood changes
    // Modulates behavioral inhibition and social behavior
    float moodResponse = std::min(pImpl->peak, std::abs(moodDelta) * pImpl->releaseRate);
    
    if (moodDelta > 0.0f) {
        pImpl->level += moodResponse;  // Mood improvement
        pImpl->behavioralInhibition *= 0.8f;  // Reduce inhibition on good moods
        pImpl->socialBehavior += 0.1f;  // Enhance social behavior
    } else {
        pImpl->level += moodResponse;  // Mood decline
        pImpl->behavioralInhibition *= 1.2f;  // Increase inhibition on bad moods
        pImpl->socialBehavior -= 0.1f;  // Reduce social behavior
    }
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Enhance or suppress working memory based on mood
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            wm->strengthenMemory(1.0f + moodDelta * 0.1f);
        }
        
        // Adjust stress response based on mood
        pImpl->stressResponse *= (1.0f + moodDelta * 0.1f);
        pImpl->anxietyLevel *= (1.0f - moodDelta * 0.05f);
    }
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

} // namespace nlm

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood regulation parameters
    float moodLevel;
    float behavioralInhibition;
    float socialBehavior;
    
    // Stress regulation parameters
    float stressResponse;
    float anxietyLevel;
    float rewardSensitivity;
    
    // Brain integration
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f),
             moodLevel(0.5f), behavioralInhibition(0.5f), socialBehavior(0.5f),
             stressResponse(0.0f), anxietyLevel(0.0f), rewardSensitivity(0.5f),
             brain(nullptr) {}
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
    // Serotonin modulates mood and behavioral inhibition
    // Reduces plasticity during high inhibition states
    float inhibitionLevel = std::min(1.0f, pImpl->level * 2.0f);
    
    // Higher 5-HT reduces plasticity to prevent over-learning
    float plasticity = 0.9f - 0.3f * inhibitionLevel;
    
    // 5-HT specifically modulates reward-related plasticity
    plasticity *= (1.0f - pImpl->behavioralInhibition * 0.2f);
    
    // Enhance social behavior-related plasticity
    if (pImpl->brain) {
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            // Social memories have different plasticity rates
            size_t episodeCount = em->getEpisodeCount();
            if (episodeCount > 0) {
                // Adjust plasticity based on social experience
                float socialModulation = std::min(1.0f, static_cast<float>(episodeCount) * 0.01f);
                plasticity *= (1.0f - socialModulation * pImpl->socialBehavior * 0.1f);
            }
        }
    }
    
    return plasticity;
}

void Serotonin::update(TimestepDuration dt) {
    // Tonic serotonin for baseline mood regulation
    // Phasic bursts for behavioral inhibition and social behavior modulation
    float targetLevel = pImpl->baseline;
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Adjust mood based on experience
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            if (em->getEpisodeCount() > 0) {
                // Use recent reward/punishment history to modulate mood
                float avgReward = em->getAverageReward();
                targetLevel += avgReward * 0.01f;
            }
        }
        
        // Stress regulation
        pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.001f * dt);
        pImpl->anxietyLevel = std::max(0.0f, pImpl->anxietyLevel - 0.0005f * dt);
        
        // Adjust target level based on stress
        targetLevel -= pImpl->stressResponse * 0.01f;
        targetLevel -= pImpl->anxietyLevel * 0.005f;
    }
    
    // Smooth transition to target level
    pImpl->level += (targetLevel - pImpl->level) * 0.02f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Phasic serotonin response to mood changes
    // Modulates behavioral inhibition and social behavior
    float moodResponse = std::min(pImpl->peak, std::abs(moodDelta) * pImpl->releaseRate);
    
    if (moodDelta > 0.0f) {
        pImpl->level += moodResponse;  // Mood improvement
        pImpl->behavioralInhibition *= 0.8f;  // Reduce inhibition on good moods
        pImpl->socialBehavior += 0.1f;  // Enhance social behavior
    } else {
        pImpl->level += moodResponse;  // Mood decline
        pImpl->behavioralInhibition *= 1.2f;  // Increase inhibition on bad moods
        pImpl->socialBehavior -= 0.1f;  // Reduce social behavior
    }
    
    // Integrate with brain systems
    if (pImpl->brain) {
        // Enhance or suppress working memory based on mood
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            wm->strengthenMemory(1.0f + moodDelta * 0.1f);
        }
        
        // Adjust stress response based on mood
        pImpl->stressResponse *= (1.0f + moodDelta * 0.1f);
        pImpl->anxietyLevel *= (1.0f - moodDelta * 0.05f);
    }
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

} // namespace nlm
