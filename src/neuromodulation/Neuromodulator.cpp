#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

// ========== DOPAMINE IMPLEMENTATION ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float rewardPredictionError;
    float memoryModulation;
    float attentionBias;
    float learningRate;
    
    // Effects on different brain functions
    float memoryConsolidationStrength;
    float retrievalEnhancement;
    float attentionFocus;
    float motivationLevel;
    
    // Internal state for dynamics
    float velocity;  // For more realistic bursting dynamics
    float lastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f)
           , predictionError(0.0f), rewardPredictionError(0.0f), memoryModulation(1.0f)
           , attentionBias(1.0f), learningRate(1.0f), memoryConsolidationStrength(1.0f)
           , retrievalEnhancement(1.0f), attentionFocus(1.0f), motivationLevel(1.0f)
           , velocity(0.0f), lastUpdate(0.0f) {}
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
    // Apply effects based on level
    pImpl->memoryConsolidationStrength = 0.5f + 0.5f * level;
    pImpl->retrievalEnhancement = 1.0f + 0.5f * level;
    pImpl->attentionFocus = 1.0f + 0.3f * level;
    pImpl->motivationLevel = level;
    pImpl->learningRate = 0.5f + 0.5f * level;
}

float Dopamine::getPlasticityFactor() const {
    // More sophisticated dopamine-modulated plasticity
    float basePlasticity = 0.5f;
    float levelModulation = pImpl->level * 0.5f;
    float predictionErrorMod = std::abs(pImpl->predictionError) * 0.3f;
    float rpeMod = std::abs(pImpl->rewardPredictionError) * 0.2f;
    
    float plasticityFactor = basePlasticity + levelModulation + predictionErrorMod + rpeMod;
    
    // Saturation to prevent runaway plasticity
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    float timeDelta = static_cast<float>(dt);
    
    // More realistic dopamine dynamics with velocity-based bursting
    if (timeDelta > 0.0f) {
        // Velocity-based dynamics
        pImpl->velocity += (0.01f - 0.02f * std::abs(pImpl->velocity)) * timeDelta;
        pImpl->velocity = std::clamp(pImpl->velocity, -0.1f, 0.1f);
        
        // Decay towards baseline with velocity component
        float decay = pImpl->decayRate * timeDelta * (1.0f + std::abs(pImpl->velocity));
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
        
        // Update last update time
        pImpl->lastUpdate = timeDelta;
    }
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error signaling
    // Positive error (reward > expected) -> DA increase
    // Negative error (reward < expected) -> DA decrease
    
    float error = reward - pImpl->predictionError;
    pImpl->rewardPredictionError = error;
    
    // Burst of dopamine with magnitude based on reward size
    float burst = reward * pImpl->releaseRate * (1.0f + 0.5f * std::abs(pImpl->velocity));
    
    // Asymmetric response: stronger for unexpected rewards
    if (reward > 0.0f) {
        burst *= (1.0f + std::abs(error) * 0.5f);
    }
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update prediction error for next step
    pImpl->predictionError = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct RPE signaling for temporal difference learning
    pImpl->predictionError = error;
    pImpl->rewardPredictionError = error;
    
    // Update dopamine level based on RPE
    // Phasic bursts for unexpected prediction errors
    if (std::abs(error) > 0.01f) {
        float burst = error * pImpl->releaseRate * 2.0f;  // Stronger response for unexpected errors
        pImpl->level = std::max(0.0f, pImpl->level + burst);
    }
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention and learning effects
    float attentionGain;
    float encodingStrength;
    float learningRate;
    float memoryFocus;
    
    // State variables
    float attentionStability;
    float cholinergicTone;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , attentionGain(1.0f), encodingStrength(1.0f), learningRate(1.0f), memoryFocus(1.0f)
           , attentionStability(1.0f), cholinergicTone(0.0f) {}
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
    // Update derived parameters
    pImpl->attentionGain = 1.0f + 0.8f * level;
    pImpl->encodingStrength = 1.0f + 0.6f * level;
    pImpl->learningRate = 1.0f + 0.5f * level;
    pImpl->memoryFocus = 1.0f + 0.4f * level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates learning rate and attention
    float basePlasticity = 1.0f;
    float attentionMod = pImpl->attentionGain * 0.3f;
    float encodingMod = pImpl->encodingStrength * 0.2f;
    
    return basePlasticity + attentionMod + encodingMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeDelta = static_cast<float>(dt);
    
    // Tonic cholinergic activity modulation
    if (timeDelta > 0.0f) {
        // Stability of attentional focus
        pImpl->attentionStability += (pImpl->level - pImpl->attentionStability) * 0.1f * timeDelta;
        pImpl->attentionStability = std::clamp(pImpl->attentionStability, 0.5f, 2.0f);
        
        // Tonic level modulation
        float decay = pImpl->decayRate * timeDelta;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
        
        // Cholinergic tone integrates with other signals
        pImpl->cholinergicTone = pImpl->level * pImpl->attentionStability;
    }
}

void Acetylcholine::signalFocus(float novelty, float importance) {
    // Focus signal based on novelty and importance
    float signal = (novelty + importance) * 0.5f;
    float burst = signal * pImpl->releaseRate * 1.5f;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Acetylcholine::signalMemoryEncoding(float encodingQuality) {
    // Enhance memory encoding based on ACh level
    float encodingBoost = encodingQuality * pImpl->encodingStrength * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + encodingBoost);
}

// ========== NORADRENALINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and cognitive effects
    float arousalLevel;
    float alertness;
    float workingMemoryCapacity;
    float stressResponse;
    float locusCoeruleusActivity;
    
    // State tracking
    float tonicActivity;
    float phasicActivity;
    float adaptation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.2f), releaseRate(1.0f)
           , arousalLevel(1.0f), alertness(1.0f), workingMemoryCapacity(1.0f)
           , stressResponse(0.0f), locusCoeruleusActivity(0.0f), tonicActivity(0.0f)
           , phasicActivity(0.0f), adaptation(0.0f) {}
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
    // Update effects
    pImpl->arousalLevel = 1.0f + 1.0f * level;  // Stronger arousal
    pImpl->alertness = 1.0f + 0.8f * level;
    pImpl->workingMemoryCapacity = 1.0f + 0.6f * level;
    pImpl->locusCoeruleusActivity = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and alertness, affecting plasticity
    float basePlasticity = 1.0f;
    float arousalMod = pImpl->arousalLevel * 0.4f;
    float alertnessMod = pImpl->alertness * 0.3f;
    
    return basePlasticity + arousalMod + alertnessMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeDelta = static_cast<float>(dt);
    
    if (timeDelta > 0.0f) {
        // Locus coeruleus adaptation to tonic level
        pImpl->adaptation += (pImpl->level - pImpl->adaptation) * 0.05f * timeDelta;
        pImpl->adaptation = std::clamp(pImpl->adaptation, 0.0f, 2.0f);
        
        // Split into tonic and phasic components
        pImpl->tonicActivity = pImpl->level * 0.7f + pImpl->adaptation * 0.3f;
        
        // Phasic responses to unexpected stimuli
        pImpl->phasicActivity = std::max(0.0f, pImpl->level - pImpl->tonicActivity);
        
        // Decay with arousal modulation
        float decay = pImpl->decayRate * timeDelta * (1.0f + pImpl->alertness * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
        
        // Stress response buildup
        if (pImpl->phasicActivity > 0.3f) {
            pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.1f * timeDelta);
        } else {
            pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.05f * timeDelta);
        }
    }
}

void Norepinephrine::signalArousal(float stimulusIntensity, bool unexpected) {
    // Arousal response to stimuli
    float baseResponse = stimulusIntensity * 0.5f;
    float unexpectedBonus = unexpected ? 1.0f : 0.0f;
    
    float burst = (baseResponse + unexpectedBonus) * pImpl->releaseRate * 2.0f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Norepinephrine::signalWorkingMemoryLoad(float load) {
    // Modulate working memory capacity based on NE level
    pImpl->workingMemoryCapacity = 1.0f + load * 0.5f * pImpl->level;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and behavioral effects
    float moodState;
    float behavioralInhibition;
    float impulsivity;
    float socialBehavior;
    float sleepDrive;
    
    // State variables
    float somatostatinModulation;
    float rapheNucleusActivity;
    float emotionalStability;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f)
           , moodState(1.0f), behavioralInhibition(1.0f), impulsivity(0.0f)
           , socialBehavior(1.0f), sleepDrive(0.0f), somatostatinModulation(1.0f)
           , rapheNucleusActivity(0.0f), emotionalStability(1.0f) {}
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
    // Update effects
    pImpl->moodState = 1.0f - 0.8f * level;  // High serotonin = better mood
    pImpl->behavioralInhibition = 1.0f + 0.7f * level;  // More inhibition
    pImpl->socialBehavior = 1.0f + 0.5f * level;
    pImpl->sleepDrive = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and behavioral inhibition
    float basePlasticity = 1.0f;
    float inhibitionMod = pImpl->behavioralInhibition * 0.3f;
    float moodMod = (1.0f - std::abs(pImpl->moodState - 1.0f)) * 0.2f;
    
    return basePlasticity + inhibitionMod + moodMod;
}

void Serotonin::update(TimestepDuration dt) {
    float timeDelta = static_cast<float>(dt);
    
    if (timeDelta > 0.0f) {
        // Raphe nucleus activity
        pImpl->rapheNucleusActivity = pImpl->level;
        
        // Somatostatin co-release modulation
        pImpl->somatostatinModulation = 1.0f - 0.3f * pImpl->level;
        
        // Emotional stability
        pImpl->emotionalStability = 1.0f - 0.5f * std::abs(pImpl->moodState - 1.0f);
        
        // Sleep-wake cycle modulation
        pImpl->sleepDrive = pImpl->level * (1.0f + pImpl->emotionalStability * 0.3f);
        
        // Decay
        float decay = pImpl->decayRate * timeDelta * (1.0f + pImpl->sleepDrive * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
        
        // Impulse control based on serotonin
        pImpl->impulsivity = 1.0f - pImpl->behavioralInhibition * 0.8f;
    }
}

void Serotonin::signalMood(float moodChange) {
    // Mood modulation
    pImpl->moodState += moodChange * 0.3f;
    pImpl->moodState = std::clamp(pImpl->moodState, 0.0f, 2.0f);
    
    // Update behavioral inhibition
    pImpl->behavioralInhibition = 1.0f + 0.7f * (1.0f - std::abs(pImpl->moodState - 1.0f));
}

void Serotonin::signalBehavioralInhibition(float inhibitionLevel) {
    // Direct behavioral inhibition signaling
    pImpl->behavioralInhibition += inhibitionLevel * 0.5f;
    pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.1f, 2.0f);
}

void Serotonin::signalSleepNeed(float sleepPressure) {
    // Sleep drive modulation
    float sleepBurst = sleepPressure * pImpl->releaseRate * 1.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + sleepBurst);
}

// ========== MEMORY SYSTEM INTEGRATION HELPERS ==========

void applyMemoryEffects(
    std::vector<std::unique_ptr<NeuralWorkingMemory>>& workingMemory,
    std::vector<std::unique_ptr<NeuralEpisodicMemory>>& episodicMemory,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    const Norepinephrine& ne,
    const Serotonin& serotonin,
    float timestep
) {
    // Memory consolidation effects
    float dopamineConsolidation = dopamine.getLevel() * 0.5f;
    float achEncoding = ach.getLevel() * 0.3f;
    
    // Apply memory system modulation
    for (auto& memory : workingMemory) {
        if (memory) {
            // Dopamine enhances working memory maintenance
            float dopamineMaintenance = 1.0f + dopamineConsolidation * 0.5f;
            memory->setMaintenanceFactor(dopamineMaintenance);
            
            // ACh enhances encoding of new items
            memory->setEncodingFactor(1.0f + achEncoding);
            
            // NE enhances updating capacity
            float neUpdating = 1.0f + ne.getLevel() * 0.4f;
            memory->setUpdatingFactor(neUpdating);
        }
    }
    
    for (auto& memory : episodicMemory) {
        if (memory) {
            // Dopamine enhances consolidation strength
            float consolidationStrength = 1.0f + dopamineConsolidation;
            memory->setConsolidationStrength(consolidationStrength);
            
            // ACh enhances pattern completion during retrieval
            float retrievalBoost = 1.0f + ach.getLevel() * 0.3f;
            memory->setRetrievalBoost(retrievalBoost);
            
            // Serotonin modulates forgetting rate
            float forgettingRate = 0.1f * (2.0f - serotonin.getLevel());  // High 5-HT = less forgetting
            memory->setForgettingRate(forgettingRate);
        }
    }
}

void applyAttentionEffects(
    std::vector<std::unique_ptr<AttentionalSelection>>& attentionSystems,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    const Norepinephrine& ne,
    float timestep
) {
    for (auto& attention : attentionSystems) {
        if (attention) {
            // Dopamine modulates selection bias
            float dopamineSelection = 1.0f + dopamine.getLevel() * 0.4f;
            attention->setSelectionBias(dopamineSelection);
            
            // ACh modulates attentional focus
            float achFocus = 1.0f + ach.getLevel() * 0.8f;
            attention->setFocusWidth(1.0f / achFocus);  // Narrower focus with high ACh
            
            // NE modulates alertness and filtering
            float neFiltering = 1.0f + ne.getAlertness() * 0.5f;
            attention->setNoiseFiltering(neFiltering);
            
            // Serotonin modulates competition
            float serotoninCompetition = 1.0f + serotonin.getBehavioralInhibition() * 0.3f;
            attention->setCompetitionIntensity(serotoninCompetition);
        }
    }
}

void applyDevelopmentEffects(
    std::vector<std::unique_ptr<DevelopmentSystem>>& developmentSystems,
    const Dopamine& dopamine,
    const Norepinephrine& ne,
    const Serotonin& serotonin,
    float timestep
) {
    for (auto& devSystem : developmentSystems) {
        if (devSystem) {
            // Dopamine modulates developmental plasticity
            float dopaminePlasticity = dopamine.getPlasticityFactor();
            devSystem->setPlasticityModulation(dopaminePlasticity);
            
            // NE modulates stress response affecting development
            float neStress = ne.getStressResponse() * 0.5f;
            devSystem->setStressImpact(neStress);
            
            // Serotonin modulates emotional development
            float serotoninEmotional = 1.0f - serotonin.getMoodState() * 0.3f;
            devSystem->setEmotionalModulation(serotoninEmotional);
        }
    }
}

void applyPredictionEffects(
    std::vector<std::unique_ptr<PredictionSystem>>& predictionSystems,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    float timestep
) {
    for (auto& predSystem : predictionSystems) {
        if (predSystem) {
            // Dopamine enhances prediction error learning
            float dopaminePrediction = 1.0f + dopamine.getLevel() * 0.6f;
            predSystem->setPredictionLearning(dopaminePrediction);
            
            // ACh enhances model updating based on novelty
            float achUpdating = 1.0f + ach.getLevel() * 0.4f;
            predSystem->setModelUpdating(achUpdating);
        }
    }
}

} // namespace nlm
