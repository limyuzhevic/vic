// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.05f), peak(2.0f), decayRate(0.03f), releaseRate(1.5f), arousal(0.5f) {}
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
    // Norepinephrine increases learning rate and plasticity - higher NE = more learning
    return 1.0f + pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine tracks arousal and vigilance
    // High NE increases alertness and learning rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousal = std::max(0.0f, pImpl->arousal - 0.01f * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Arousal from novel stimuli, stress, or salient events
    pImpl->arousal = std::min(1.0f, pImpl->arousal + arousal * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Vigilance from attention and focus on task-relevant stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->releaseRate * 0.5f);
}