    // Signal social interaction (positive or negative)
    void signalSocialInteraction(float intensity, bool positive);

    // Get behavioral flexibility
    float getBehavioralFlexibility() const;

    // Set mood state, impulsivity, social cohesion
    void setMoodState(float mood);
    float getMoodState() const;

    void setImpulsivity(float impulsivity);
    float getImpulsivity() const;

    void setSocialCohesion(float cohesion);
    float getSocialCohesion() const;

private:
    struct Impl;
    Impl* pImpl;
};

// Implementation

struct Serotonin::Impl {
    float level;
    float moodState;
    float impulsivity;
    float socialCohesion;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    
    Impl() : level(0.0f), moodState(0.5f), impulsivity(0.5f), socialCohesion(0.5f),
             baseline(0.0f), peak(1.0f), decayRate(0.1f), recoveryRate(0.05f) {}
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
    // Serotonin modulates plasticity in complex ways
    // Low serotonin increases impulsivity and reduces plasticity stability
    // High serotonin enhances stable memory formation
    if (pImpl->level < 0.3f) {
        // Low serotonin: increased volatility, reduced stability
        return 1.2f - pImpl->level * 0.5f;
    } else {
        // High serotonin: stable consolidation
        return 0.8f + pImpl->level * 0.3f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay and recovery to baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recover mood state and other parameters
    pImpl->moodState = std::min(1.0f, pImpl->moodState + pImpl->recoveryRate * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.3f, pImpl->impulsivity - pImpl->decayRate * static_cast<float>(dt));
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + pImpl->recoveryRate * static_cast<float>(dt));
}

void Serotonin::signalSocialInteraction(float intensity, bool positive) {
    if (positive) {
        // Positive social interactions boost serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 1.5f);
        pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + intensity * 0.3f);
        pImpl->moodState = std::min(1.0f, pImpl->moodState + intensity * 0.4f);
    } else {
        // Negative social interactions reduce serotonin
        pImpl->level = std::max(pImpl->baseline, pImpl->level - intensity * 1.0f);
        pImpl->socialCohesion = std::max(0.0f, pImpl->socialCohesion - intensity * 0.3f);
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + intensity * 0.3f);
    }
}

void Serotonin::setMoodState(float mood) {
    pImpl->moodState = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getMoodState() const {
    return pImpl->moodState;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::setSocialCohesion(float cohesion) {
    pImpl->socialCohesion = std::clamp(cohesion, 0.0f, 1.0f);
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getBehavioralFlexibility() const {
    // Behavioral flexibility is inversely related to serotonin
    return 1.0f - pImpl->level;
}

} // namespace nlm
