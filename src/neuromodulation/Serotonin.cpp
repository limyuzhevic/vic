#include "Serotonin.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float moodWeight;
    float impulsivityControl;
    float socialLearningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f), brain(nullptr),
             moodWeight(1.1f), impulsivityControl(1.4f), socialLearningRate(0.9f) {}
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
    // 5-HT modulates plasticity based on mood/state
    float modulation = getLevel();
    if (modulation > 0.7f) {
        // High serotonin reduces certain types of plasticity (prevents over-learning)
        return 0.6f - 0.4f * (modulation - 0.7f) * 2.0f;
    } else {
        // Moderate serotonin enhances adaptive plasticity
        return 0.7f + 0.3f * modulation;
    }
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.03f;
    }
}

void Serotonin::regulateMood(bool socialPositive, float socialValue) {
    // Serotonin regulates mood based on social feedback
    float moodChange = socialValue;
    if (!socialPositive) {
        moodChange -= 0.5f;  // Social defeat reduces serotonin
    }
    
    setLevel(std::min(1.0f, getLevel() + moodChange * pImpl->releaseRate * 0.7f));
}

void Serotonin::controlImpulsivity(bool hasWaitOption) {
    // Serotonin helps control impulsive behavior
    if (hasWaitOption) {
        // Having a wait option reduces impulsivity
        setLevel(getLevel() * 0.95f);
    } else {
        // No wait option may increase impulsivity
        setLevel(std::min(1.0f, getLevel() + 0.2f * pImpl->releaseRate));
    }
}

float Serotonin::getBehavioralModulation() const {
    // Serotonin modulates behavioral responses (patience, mood)
    return 1.0f + pImpl->moodWeight * getLevel() - pImpl->impulsivityControl * (1.0f - getLevel()) * 0.5f;
}

float Serotonin::getAttentionModulation() const {
    // Serotonin modulates attentional focus and impulsivity
    return 1.0f - 0.3f * getLevel();  // High serotonin reduces distractibility
}

} // namespace nlm