// Norepinephrine: Arousal and vigilance
// Phase 6 implementation - complete NE system

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float vigilance;
    float arousal;
    float locusCoeruleusActivity;
    float baselineLevel;
    float decayRate;
    float releaseRate;
    float attentionGain;
    std::vector<float> history;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , vigilance(0.5f)
        , arousal(0.5f)
        , locusCoeruleusActivity(0.0f)
        , baselineLevel(0.03f)
        , decayRate(0.05f)
        , releaseRate(1.2f)
        , attentionGain(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
    pImpl->locusCoeruleusActivity = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal
    // Moderate NE enhances plasticity, high NE impairs it
    float arousal = std::min(1.0f, pImpl->level / 2.0f);
    float plasticity = 0.5f + arousal * 0.5f * (1.0f - arousal * 0.3f);
    return std::max(0.1f, plasticity);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with vigilance-dependent rate
    float decayRate = pImpl->decayRate * (1.0f + pImpl->vigilance * 0.5f);
    pImpl->level = std::max(pImpl->baselineLevel, 
                          pImpl->level - decayRate * static_cast<float>(dt));
    
    // Update vigilance based on level
    pImpl->vigilance = std::min(1.0f, pImpl->level / 1.5f);
    pImpl->arousal = std::min(1.0f, pImpl->level / 3.0f);
    
    pImpl->history.push_back(pImpl->level);
}

void Norepinephrine::signalStress(float stress) {
    // LC neurons fire in response to stress/novelty
    float burst = stress * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(3.0f, pImpl->level + burst);
}

void Norepinephrine::increaseVigilance(float vigilance) {
    // Directly increase vigilance state
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + vigilance * 0.3f);
    // Slightly increase NE level
    pImpl->level = std::min(3.0f, pImpl->level + vigilance * 0.5f);
}

void Norepinephrine::setBrainReference(class Brain* brain) {
    pImpl->brain = brain;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->vigilance = 0.5f;
    pImpl->arousal = 0.5f;
    pImpl->locusCoeruleusActivity = 0.0f;
    pImpl->history.clear();
}

// Norepinephrine.cpp implementation completed
}
namespace nlm