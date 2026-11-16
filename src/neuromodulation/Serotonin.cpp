// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f) {}
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
    // Serotonin modulates learning rate - higher 5-HT decreases plasticity
    // to promote stability of learned behaviors and memory consolidation
    return 1.0f - 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays with tonic activity
    // High serotonin promotes memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Mood signals can be positive (reward) or negative (punishment)
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

void Serotonin::signalSocialReward(float socialReward) {
    // Social rewards (observation, imitation, connection) increase serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialReward * pImpl->releaseRate * 2.0f);
}