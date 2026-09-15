struct Acetylcholine::Impl {
    float level;
    float baseline;
    
    // Attention modulation parameters
    float attentionLevel;
    float modulationDepth;
    
    // Memory consolidation state
    bool consolidating;
    float consolidationProgress;
    float consolidationThreshold;
    
    // Learning rate modulation
    float attentionModulationRate;
    float memoryModulationRate;
    
    Impl() : level(0.0f), baseline(0.0f), attentionLevel(0.0f), modulationDepth(0.5f),
             consolidating(false), consolidationProgress(0.0f), consolidationThreshold(0.8f),
             attentionModulationRate(0.1f), memoryModulationRate(0.05f) {}
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
    pImpl->attentionLevel = pImpl->level * pImpl->modulationDepth;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates attention and memory consolidation
    // High ACh enhances attention and stabilizes memories
    return 1.0f + pImpl->attentionLevel * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Slow decay towards baseline
    float decay = pImpl->baseline * timeStep * 0.1f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update consolidation state
    if (pImpl->consolidating) {
        pImpl->consolidationProgress += timeStep * 0.01f;
        if (pImpl->consolidationProgress >= pImpl->consolidationThreshold) {
            pImpl->consolidating = false;
            pImpl->consolidationProgress = 0.0f;
        }
    }
}

float Acetylcholine::calculateAttentionModulation(float attentionLevel) const {
    // Attention enhances Hebbian learning specificity
    return 1.0f + attentionLevel * 0.8f; // Up to 80% enhancement
}

float Acetylcholine::calculateMemoryConsolidation() const {
    // Memory consolidation stabilizes recent plasticity
    return pImpl->consolidating ? pImpl->consolidationProgress : 0.0f;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    
    // Arousal state parameters
    float arousalLevel;
    float vigilanceLevel;
    float modulationStrength;
    
    // Learning modulation parameters
    float arousalModulationRate;
    float vigilanceModulationRate;
    
    // Burst firing patterns
    float burstRate;
    float burstDuration;
    float interburstInterval;
    
    // State tracking
    bool isBursting;
    float burstTimer;
    float interburstTimer;
    
    Impl() : level(0.0f), baseline(0.0f), arousalLevel(0.0f), vigilanceLevel(0.0f),
             modulationStrength(0.5f), arousalModulationRate(0.1f), vigilanceModulationRate(0.05f),
             burstRate(5.0f), burstDuration(50.0f), interburstInterval(200.0f),
             isBursting(false), burstTimer(0.0f), interburstTimer(0.0f) {}
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
    
    // Update arousal and vigilance based on level
    pImpl->arousalLevel = std::min(1.0f, pImpl->level * 1.5f);
    pImpl->vigilanceLevel = 1.0f - std::exp(-pImpl->level * 2.0f);
    
    // Start or stop bursting based on level
    if (pImpl->level > 0.3f && !pImpl->isBursting) {
        pImpl->isBursting = true;
        pImpl->interburstTimer = interburstInterval;
    } else if (pImpl->level < 0.1f && pImpl->isBursting) {
        pImpl->isBursting = false;
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // NE has complex effects on plasticity: moderate arousal enhances, high arousal impairs
    float arousal = pImpl->arousalLevel;
    
    // Inverted U-shaped response: optimal plasticity at moderate arousal
    float optimalArousal = 0.5f;
    float width = 0.3f;
    float plasticity = std::exp(-std::pow((arousal - optimalArousal) / width, 2.0f));
    
    // Apply vigilance modulation (high vigilance enhances focused learning)
    float vigilanceFactor = 1.0f + pImpl->vigilanceLevel * 0.3f;
    
    return std::clamp(plasticity * vigilanceFactor, 0.1f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Natural decay towards baseline
    float decay = pImpl->baseline * timeStep * 0.05f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update burst state
    if (pImpl->isBursting) {
        pImpl->burstTimer += timeStep;
        if (pImpl->burstTimer >= pImpl->burstDuration) {
            pImpl->isBursting = false;
            pImpl->burstTimer = 0.0f;
            pImpl->interburstTimer = interburstInterval;
        }
    } else {
        pImpl->interburstTimer -= timeStep;
        if (pImpl->interburstTimer <= 0.0f) {
            pImpl->isBursting = true;
            pImpl->interburstTimer = 0.0f;
        }
    }
}

float Norepinephrine::calculateArousalModulation(float arousalLevel) const {
    // Arousal enhances detection of salient stimuli and strengthens memory encoding
    float modulation = 1.0f + arousalLevel * 0.5f;
    
    // High arousal can also enhance stress responses
    if (arousalLevel > 0.8f) {
        modulation *= 1.2f;
    }
    
    return std::clamp(modulation, 0.5f, 2.0f);
}

float Norepinephrine::calculateVigilanceControl() const {
    // Vigilance enhances focused attention and reduces distractibility
    float modulation = 1.0f + pImpl->vigilanceLevel * 0.4f;
    
    // High vigilance can impair flexible thinking
    if (pImpl->vigilanceLevel > 0.7f) {
        modulation *= 0.9f;
    }
    
    return std::clamp(modulation, 0.5f, 1.5f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    
    // Mood state parameters
    float moodLevel;
    float impulsivityLevel;
    float rewardSensitivity;
    
    // Social behavior parameters
    float socialCohesion;
    float aggressionLevel;
    float dominance;
    
    // Plasticity modulation parameters
    float moodModulationRate;
    float impulseControlRate;
    float socialModulationRate;
    
    // State tracking
    float moodHistory[50];
    size_t moodIndex;
    size_t moodSize;
    
    Impl() : level(0.0f), baseline(0.0f), moodLevel(0.5f), impulsivityLevel(0.5f),
             rewardSensitivity(0.5f), socialCohesion(0.5f), aggressionLevel(0.5f),
             dominance(0.5f), moodModulationRate(0.1f), impulseControlRate(0.05f),
             socialModulationRate(0.05f), moodIndex(0), moodSize(0) {
        
        // Initialize mood history
        for (size_t i = 0; i < 50; ++i) {
            moodHistory[i] = moodLevel;
        }
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
    
    // Update mood based on serotonin level
    // Higher serotonin generally improves mood, reduces impulsivity
    pImpl->moodLevel = 0.3f + pImpl->level * 0.7f;
    pImpl->impulsivityLevel = 1.0f - pImpl->moodLevel;
    
    // Update social parameters
    pImpl->socialCohesion = 0.5f + pImpl->moodLevel * 0.4f;
    pImpl->aggressionLevel = 1.0f - pImpl->socialCohesion;
    
    // Store in mood history
    pImpl->moodHistory[pImpl->moodIndex] = pImpl->moodLevel;
    pImpl->moodIndex = (pImpl->moodIndex + 1) % 50;
    pImpl->moodSize = std::min(pImpl->moodSize + 1, 50);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood, impulsivity, and social behavior effects on plasticity
    float moodFactor = 1.0f + pImpl->moodLevel * 0.3f; // Better mood enhances learning
    float impulseFactor = 1.0f - pImpl->impulsivityLevel * 0.5f; // Less impulsivity enhances learning
    float socialFactor = 1.0f + pImpl->socialCohesion * 0.2f; // Social cohesion enhances learning
    
    float factor = moodFactor * impulseFactor * socialFactor;
    
    return std::clamp(factor, 0.5f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Slow decay towards baseline
    float decay = pImpl->baseline * timeStep * 0.02f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update mood based on level
    pImpl->moodLevel = 0.3f + pImpl->level * 0.7f;
    pImpl->impulsivityLevel = 1.0f - pImpl->moodLevel;
}

float Serotonin::calculateMoodModulation() const {
    // Mood affects learning: positive mood enhances memory formation, negative mood impairs
    // But moderate negative mood can enhance attention to negative stimuli
    float moodFactor = 1.0f + pImpl->moodLevel * 0.4f;
    
    // Very high mood might reduce critical thinking
    if (pImpl->moodLevel > 0.8f) {
        moodFactor *= 0.9f;
    }
    
    return std::clamp(moodFactor, 0.5f, 1.8f);
}

float Serotonin::calculateImpulseControl() const {
    // Good impulse control enables sustained attention and working memory
    float impulseFactor = 1.0f - pImpl->impulsivityLevel * 0.7f;
    
    // Very low impulsivity might reduce flexibility
    if (pImpl->impulsivityLevel < 0.2f) {
        impulseFactor *= 1.1f;
    }
    
    return std::clamp(impulseFactor, 0.5f, 1.5f);
}

float Serotonin::calculateSocialModulation() const {
    // Social context modulates learning: high social cohesion enhances cooperation
    float socialFactor = 1.0f + pImpl->socialCohesion * 0.5f;
    
    // Very high social cohesion might reduce exploration of novel options
    if (pImpl->socialCohesion > 0.8f) {
        socialFactor *= 0.95f;
    }
    
    return std::clamp(socialFactor, 0.5f, 1.6f);
}

} // namespace nlm