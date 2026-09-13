// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with dynamic modulation of mood and behavioral inhibition

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodModulation;
    float inhibitionStrength;
    float rewardResponse;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , moodModulation(1.0f)
        , inhibitionStrength(1.5f)
        , rewardResponse(0.8f) {}
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
    // Serotonin modulates mood and behavioral inhibition
    // High serotonin = more stable learning, less impulsive
    return 1.0f - pImpl->level * 0.5f;  // Inhibits plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline (tonic 5-HT)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodSignal) {
    // Phasic 5-HT signals for mood changes
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->releaseRate);
}

void Serotonin::signalInhibition(float inhibitionSignal) {
    // 5-HT increases behavioral inhibition and impulse control
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionSignal * pImpl->releaseRate);
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    // 5-HT responds to social feedback and modulates social learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * pImpl->releaseRate * 0.6f);
}

void Serotonin::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Serotonin::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

void Serotonin::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = strength;
}

} // namespace nlm
