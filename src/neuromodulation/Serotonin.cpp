// Serotonin: Mood, impulsivity, and social behavior
// Phase 6 implementation - complete 5-HT system

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rapheNucleiActivity;
    float baselineLevel;
    float decayRate;
    float releaseRate;
    float s1vs2Ratio;  // Ratio of 5-HT1A to 5-HT2A receptor activation
    std::vector<float> history;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , socialBehavior(0.5f)
        , rapheNucleiActivity(0.0f)
        , baselineLevel(0.02f)
        , decayRate(0.06f)
        , releaseRate(1.0f)
        , s1vs2Ratio(0.5f) {} // Default balance
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
    pImpl->level = std::clamp(level, 0.0f, 2.5f);
    pImpl->rapheNucleiActivity = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin has complex effects on plasticity
    // Optimal mood/impulsivity balance enhances learning
    float moodFactor = 1.0f - std::abs(pImpl->mood - 0.5f) * 2.0f;  // Peak at 0.5
    float impFactor = 1.0f - std::abs(pImpl->impulsivity - 0.5f) * 2.0f;
    float balanceFactor = pImpl->s1vs2Ratio;  // 5-HT1A enhances plasticity
    
    float plasticity = 0.5f + (moodFactor + impFactor) * 0.25f * balanceFactor;
    return std::max(0.1f, std::min(2.0f, plasticity));
}

void Serotonin::update(TimestepDuration dt) {
    // Decay with mood-dependent rate
    float decayRate = pImpl->decayRate * (1.0f + (1.0f - std::abs(pImpl->mood - 0.5f)));
    pImpl->level = std::max(pImpl->baselineLevel, 
                          pImpl->level - decayRate * static_cast<float>(dt));
    
    // Update social behavior based on level
    pImpl->socialBehavior = 0.3f + pImpl->level * 0.3f;
    pImpl->impulsivity = 0.5f + pImpl->level * 0.2f - std::abs(pImpl->level - 1.0f) * 0.3f;
    
    pImpl->history.push_back(pImpl->level);
}

void Serotonin::signalPositiveEvent(float reward) {
    // Raphe nuclei fire in response to positive outcomes
    float burst = reward * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(2.5f, pImpl->level + burst);
    
    // Update mood positively
    pImpl->mood = std::min(1.0f, pImpl->mood + reward * 0.3f);
}

void Serotonin::signalNegativeEvent(float punishment) {
    // Suppress firing in response to negative outcomes
    float suppression = punishment * 0.4f;
    pImpl->level = std::max(pImpl->baselineLevel, 
                          pImpl->level - suppression);
    
    // Update mood negatively
    pImpl->mood = std::max(0.0f, pImpl->mood - punishment * 0.4f);
}

void Serotonin::setBrainReference(class Brain* brain) {
    pImpl->brain = brain;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->mood = 0.5f;
    pImpl->impulsivity = 0.5f;
    pImpl->socialBehavior = 0.5f;
    pImpl->rapheNucleiActivity = 0.0f;
    pImpl->s1vs2Ratio = 0.5f;
    pImpl->history.clear();
}

// Serotonin.cpp implementation completed
}
namespace nlm