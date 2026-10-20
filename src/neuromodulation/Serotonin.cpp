#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float decayRate;
    float rewardSensitivity;
    std::vector<float> moodHistory;
    float lastReward;
    
    Impl()
        : brain(nullptr)
        , level(0.5f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , decayRate(0.015f)
        , rewardSensitivity(1.0f)
        , lastReward(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    // Initialize random generator for stochastic behavior
    std::random_device rd;
    rng_.seed(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized (mood & impulsivity)");
}

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and impulsivity
    float moodModulation = pImpl->mood * 0.5f;
    float impulseModulation = pImpl->impulsivity * 0.3f;
    return 1.0f + moodModulation + impulseModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: regulates mood and impulsivity
    float currentMood = calculateMoodLevel();
    
    // Smooth update towards target mood level
    float targetLevel = currentMood * pImpl->level;
    pImpl->level += (targetLevel - pImpl->level) * 0.02f;
    
    // Decay and stochastic fluctuations
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add some stochastic variation (physiological reality)
    std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
    pImpl->level += dist(rng_);
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    
    // Store mood level
    pImpl->moodHistory.push_back(pImpl->mood);
    if (pImpl->moodHistory.size() > 100) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
}

void Serotonin::adjustMood(float rewardPrediction) {
    // Mood changes based on reward prediction
    float moodChange = (rewardPrediction - pImpl->lastReward) * 0.3f;
    pImpl->mood += moodChange;
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
    
    pImpl->lastReward = rewardPrediction;
}

void Serotonin::modifyImpulsivity(float urgency) {
    // Urgency increases impulsivity
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + urgency * 0.5f);
}

float Serotonin::calculateMoodLevel() const {
    if (!pImpl->brain) return 0.5f;
    
    float mood = 0.5f;  // Baseline mood
    
    // Positive mood from rewards and success
    if (auto* dopamine = pImpl->brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        mood += dopamineLevel * 0.3f;
    }
    
    // Negative mood from prediction errors
    if (auto* predictionError = pImpl->brain->getPredictionErrorSignal()) {
        float error = predictionError->getError();
        mood -= std::min(0.5f, std::abs(error) * 0.8f);
    }
    
    // Social learning effects
    if (auto* socialLearning = pImpl->brain->getSocialLearning()) {
        float socialReward = socialLearning->getSocialReward();
        mood += socialReward * 0.4f;
    }
    
    return std::clamp(mood, 0.0f, 1.0f);
}

void Serotonin::reset() {
    pImpl->level = 0.5f;
    pImpl->mood = 0.5f;
    pImpl->impulsivity = 0.5f;
    pImpl->moodHistory.clear();
}

} // namespace nlm