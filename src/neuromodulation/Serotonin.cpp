#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>
#include <deque>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood state (0.0 = very sad, 1.0 = very happy)
    float moodState;
    float moodDecayRate;
    
    // Anxiety level (0.0 = calm, 1.0 = very anxious)
    float anxietyLevel;
    float anxietyFeedback;
    
    // Anhedonia (reduced reward responsiveness)
    std::deque<float> anhedoniaHistory;
    size_t maxHistorySize;
    
    // Extinction learning enhancement
    std::unordered_map<uint64_t, float> extinctionCoefficients; // SynapseId -> coefficient
    float extinctionLearningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodState(0.5f), moodDecayRate(0.05f)
        , anxietyLevel(0.0f), anxietyFeedback(0.1f)
        , maxHistorySize(100)
        , extinctionLearningRate(0.3f) {}
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
    // Serotonin modulates learning rate based on mood
    // High serotonin (happy mood) promotes LTP
    // Low serotonin (sad mood) promotes LTD
    float moodFactor = pImpl->moodState; // 0.0 = sad -> more LTD, 1.0 = happy -> more LTP
    return 0.5f + 0.5f * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood state with decay
    pImpl->moodState = std::max(0.0f, pImpl->moodState - pImpl->moodDecayRate * static_cast<float>(dt));
    
    // Clean up history
    while (pImpl->anhedoniaHistory.size() > pImpl->maxHistorySize) {
        pImpl->anhedoniaHistory.pop_front();
    }
}

void Serotonin::setMoodState(float mood) {
    pImpl->moodState = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getMoodState() const {
    return pImpl->moodState;
}

void Serotonin::setAnxietyLevel(float anxiety) {
    pImpl->anxietyLevel = std::clamp(anxiety, 0.0f, 1.0f);
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getModulatedLearningRate(float baseRate) const {
    // Learning rate modulation based on mood and anxiety
    // Happy mood (high serotonin) -> increased learning
    // Anxiety -> decreased learning (conservative)
    float moodModulation = 1.0f + (pImpl->moodState - 0.5f) * 0.5f; // 0.75 to 1.25
    float anxietyModulation = 1.0f - pImpl->anxietyLevel * 0.5f; // 1.0 to 0.5
    return baseRate * moodModulation * anxietyModulation;
}

void Serotonin::signalAnhedonia(float magnitude) {
    // Store anhedonia response (reduced reward response)
    pImpl->anhedoniaHistory.push_back(magnitude);
}

void Serotonin::facilitateExtinctionLearning() {
    // Increase extinction coefficients for all synapses
    for (auto& pair : pImpl->extinctionCoefficients) {
        pair.second *= 1.1f; // Enhance extinction learning
    }
}

} // namespace nlm
