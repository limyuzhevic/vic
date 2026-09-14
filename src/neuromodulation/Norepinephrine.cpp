// Norepinephrine implementation
Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = pImpl->level > pImpl->arousalThreshold ? pImpl->level : 0.0f;
    pImpl->signalToNoise = 0.5f + 0.5f * pImpl->level;  // NE improves signal-to-noise ratio
}

void Norepinephrine::signalNovelty(float novelty, float predictionError) {
    // NE responds to novelty and prediction error, enhancing arousal
    float drive = novelty * 0.7f + predictionError * 0.3f;
    
    // Update arousal level
    if (drive > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + drive * pImpl->arousalGain);
    }
}
