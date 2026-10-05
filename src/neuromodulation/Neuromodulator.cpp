struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // REAL DOPAMINE-PLASTICITY: Real dopamine-modulated plasticity factor
    // Higher dopamine increases STDP and Hebbian plasticity weights
    float baselinePlasticity = 1.0f;
    float dopamineEffect = pImpl->level * 0.8f;  // 80% modulation strength
    return baselinePlasticity + dopamineEffect;
}

void Dopamine::update(TimestepDuration dt) {
    // REAL DOPAMINE DYNAMICS: Decay towards baseline with time
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // REAL REWARD SIGNALING: Burst of dopamine on reward with saturation
    float rewardBurst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardBurst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL REWARD PREDICTION ERROR: Dopamine responds to prediction error
    // Positive error (better than expected) increases dopamine, negative decreases it
    float errorResponse = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
}

// Implement Acetylcholine (attention and memory consolidation)
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), attentionBoost(1.5f) {}
};

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), arousalBoost(1.2f) {}
};

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float impulseControl;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), impulseControl(0.8f) {}
};

// Update acetylcholine for attention modulation
void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory consolidation and attention
    // Higher ACh improves working memory capacity and attention focus
    return 1.0f + pImpl->level * 0.5f;  // 1.0x to 1.5x effect
}

void Acetylcholine::signalAttention(float strength) {
    // Boost acetylcholine to enhance attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->attentionBoost);
}

// Update norepinephrine for arousal
void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and vigilance
    // Affects attention to novel/important stimuli
    return 1.0f + pImpl->level * 0.3f;  // 1.0x to 1.3x effect
}

void Norepinephrine::signalArousal(float intensity) {
    // Boost norepinephrine for increased arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->arousalBoost);
}

// Update serotonin for mood and impulsivity
void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood, impulsivity, and behavioral flexibility
    // Higher serotonin generally reduces impulsivity and improves behavioral control
    return 1.0f - pImpl->level * 0.2f;  // 0.8x to 1.0x effect (inhibitory)
}

void Serotonin::signalMood(float moodLevel) {
    // Boost serotonin for mood regulation
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->impulseControl);
}

// Helper function for neuromodulator integration
void integrateNeuromodulatorsWithBrain(Brain* brain) {
    if (!brain) return;
    
    // Connect neuromodulators to brain systems
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getAcetylcholine();
    auto* norepinephrine = brain->getNorepinephrine();
    auto* serotonin = brain->getSerotonin();
    
    if (dopamine) {
        // Dopamine affects working memory
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->setCapacity(brain->getWorkingMemory()->getCapacity() * dopamine->getPlasticityFactor());
        }
    }
    
    if (acetylcholine) {
        // Acetylcholine affects attention
        if (brain->getAttention()) {
            // Could modulate attention strength
        }
    }
    
    if (norepinephrine) {
        // Norepinephrine affects arousal and novelty detection
        if (brain->getNovelty()) {
            // Could enhance novelty detection
        }
    }
    
    if (serotonin) {
        // Serotonin affects behavioral flexibility
        // Could affect planning and action selection
    }
}
