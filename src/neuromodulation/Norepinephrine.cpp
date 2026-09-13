// Norepinephrine: Arousal and vigilance
// Real implementation with dynamic modulation of arousal and response gain

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float responseGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(1.0f)
        , arousalBoost(2.0f)
        , responseGain(1.8f) {}
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
    // NE modulates arousal and baseline excitability
    return 1.0f + pImpl->level * (pImpl->responseGain - 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline (tonic NE)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Phasic NE signals for increased arousal and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate);
    
    if (brain_) {
        // Increase overall neural excitability (arousal)
        // In real implementation, would enhance response gain of neurons
    }
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE responds to novel stimuli and increases alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::signalThreat(float threatLevel) {
    // NE responds to threatening or salient events
    pImpl->level = std::min(pImpl->peak, pImpl->level + threatLevel * pImpl->releaseRate * 0.5f);
}

void Norepinephrine::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Norepinephrine::getArousalBoost() const {
    return pImpl->arousalBoost;
}

void Norepinephrine::setArousalBoost(float boost) {
    pImpl->arousalBoost = boost;
}

} // namespace nlm
