// Serotonin implementation
Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel = pImpl->level * pImpl->moodGain;
    pImpl->impulseControl = std::max(0.0f, 1.0f - pImpl->level * 0.8f);  // Serotonin reduces impulsivity
    pImpl->socialModulation = 0.5f + 0.5f * pImpl->level;  // Serotonin enhances social behavior
}

void Serotonin::signalSocialContext(float socialCohesion, float frustration) {
    // Serotonin responds to social context and frustration
    float drive = socialCohesion * 0.6f - frustration * 0.4f;
    
    // Update level
    if (drive > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + drive * pImpl->moodGain);
    } else if (drive < 0.0f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + drive * 0.5f);
    }
}
