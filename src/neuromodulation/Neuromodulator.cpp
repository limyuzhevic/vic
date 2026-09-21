struct Acetylcholine::Impl {
    float level;
    float attentionSignal;
    float memoryConsolidationSignal;
    float learningRateModulation;
    float baseline;
    
    Impl() : level(0.0f), attentionSignal(0.0f), memoryConsolidationSignal(0.0f), 
             learningRateModulation(1.0f), baseline(0.0f) {}
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
    // ACh enhances plasticity and learning
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Acetylcholine::signalMemoryConsolidation(const class Observation& observation) {
    // Signal to consolidate working memory into long-term storage
    pImpl->memoryConsolidationSignal = 1.0f;
    pImpl->level = std::min(pImpl->level + 0.2f, 1.0f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Modulate attention and focus
    pImpl->attentionSignal = attentionStrength;
    pImpl->level = std::min(pImpl->level + attentionStrength * 0.1f, 1.0f);
}

float Acetylcholine::getLearningRateModulation() const {
    // ACh enhances learning rate
    return 1.0f + pImpl->level * 0.5f;
}

struct Norepinephrine::Impl {
    float level;
    float arousalSignal;
    float vigilanceSignal;
    float noiseReduction;
    float baseline;
    
    Impl() : level(0.0f), arousalSignal(0.0f), vigilanceSignal(0.0f), 
             noiseReduction(0.0f), baseline(0.0f) {}
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
    // NE enhances signal-to-noise ratio, optimizing plasticity
    return 0.9f + 0.2f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Enhance arousal and alertness
    pImpl->arousalSignal = arousalLevel;
    pImpl->level = std::min(pImpl->level + arousalLevel * 0.15f, 1.0f);
}

void Norepinephrine::enhanceVigilance(float vigilance) {
    // Enhance focused attention and vigilance
    pImpl->vigilanceSignal = vigilance;
    pImpl->level = std::min(pImpl->level + vigilance * 0.1f, 1.0f);
}

void Norepinephrine::reduceNoise(float noiseReduction) {
    // Reduce neural noise and increase signal clarity
    pImpl->noiseReduction = noiseReduction;
    pImpl->level = std::min(pImpl->level + noiseReduction * 0.05f, 1.0f);
}

struct Serotonin::Impl {
    float level;
    float moodSignal;
    float impulsivityModulation;
    float socialBehaviorSignal;
    float baseline;
    
    Impl() : level(0.0f), moodSignal(0.0f), impulsivityModulation(0.0f),
             socialBehaviorSignal(0.0f), baseline(0.0f) {}
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
    // Serotonin modulates emotional processing and social learning
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.03f * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // Modulate emotional state and motivation
    pImpl->moodSignal = moodLevel;
    pImpl->level = std::min(pImpl->level + moodLevel * 0.12f, 1.0f);
}

void Serotonin::modulateImpulsivity(float impulsivity) {
    // Reduce impulsivity and increase behavioral inhibition
    pImpl->impulsivityModulation = impulsivity;
    pImpl->level = std::min(pImpl->level + (1.0f - impulsivity) * 0.08f, 1.0f);
}

void Serotonin::signalSocialBehavior(float socialDrive) {
    // Enhance social bonding and cooperative behavior
    pImpl->socialBehaviorSignal = socialDrive;
    pImpl->level = std::min(pImpl->level + socialDrive * 0.1f, 1.0f);
}

} // namespace nlm
