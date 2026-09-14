    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);

private:
    // History for reward prediction error
    std::deque<float> rpeHistory;
};

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation effects
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), memoryBoost(1.0f) {}
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
    // ACh enhances memory consolidation at moderate levels
    pImpl->memoryBoost = 1.0f + pImpl->level * 0.5f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh facilitates attention and memory encoding
    // Higher ACh increases plasticity for salient inputs
    float factor = 0.5f + 0.5f * pImpl->level;
    return factor * pImpl->memoryBoost;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Decay memory boost
    pImpl->memoryBoost = std::max(1.0f, pImpl->memoryBoost - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Respond to attentional demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate * 2.0f);
    pImpl->memoryBoost = 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::signalNovelty(float novelty) {
    // Enhance encoding of novel information
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance effects
    float vigilance;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.1f), releaseRate(1.0f), vigilance(1.0f), arousal(1.0f) {}
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
    // NE modulates arousal and vigilance
    pImpl->vigilance = 1.0f + pImpl->level * 0.8f;
    pImpl->arousal = 1.0f + pImpl->level * 0.6f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity during high arousal
    float vigilanceModifier = 1.0f + (pImpl->vigilance - 1.0f) * 0.3f;
    return 0.5f + 0.5f * pImpl->level * vigilanceModifier;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->vigilance = std::max(1.0f, pImpl->vigilance - 0.01f * static_cast<float>(dt));
    pImpl->arousal = std::max(1.0f, pImpl->arousal - 0.01f * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressLevel) {
    // Respond to stress with increased arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate);
    pImpl->vigilance = 1.0f + pImpl->level * 0.8f;
    pImpl->arousal = 1.0f + pImpl->level * 0.6f;
}

void Norepinephrine::signalFocus(float focus) {
    // Enhance focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus * pImpl->releaseRate * 1.5f);
    pImpl->vigilance = 1.0f + pImpl->level * 0.8f;
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and impulsivity effects
    float mood;
    float impulseControl;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.1f), releaseRate(1.0f), mood(1.0f), impulseControl(1.0f) {}
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
    // 5-HT regulates mood and impulse control
    pImpl->mood = 0.5f + 0.5f * pImpl->level;
    pImpl->impulseControl = 1.0f - pImpl->level * 0.7f;  // Higher 5-HT = better impulse control
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for stable learning
    float moodModifier = pImpl->mood;  // Stable mood = more stable learning
    return 0.6f + 0.4f * pImpl->level * moodModifier;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->mood = std::max(0.5f, pImpl->mood - 0.01f * static_cast<float>(dt));
    pImpl->impulseControl = std::max(0.3f, pImpl->impulseControl - 0.01f * static_cast<float>(dt));
}

void Serotonin::signalSocialContext(float socialCohesion) {
    // Respond to social context
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->releaseRate);
    pImpl->mood = 0.5f + 0.5f * pImpl->level;
}

void Serotonin::signalWellbeing(float wellbeing) {
    // Enhance learning from positive experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + wellbeing * pImpl->releaseRate * 1.5f);
    pImpl->impulseControl = 1.0f - pImpl->level * 0.7f;
}

} // namespace nlm
