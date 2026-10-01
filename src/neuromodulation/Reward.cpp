#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward prediction and error
    float expectedReward;
    float predictionError;
    float predictionGain;
    float rewardSensitivity;
    
    // Neuromodulator integration
    class Brain* brain;
    
    // Reward computation parameters
    float noveltyThreshold;
    float predictionAccuracy;
    float learningRate;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), expectedReward(0.0f),
             predictionError(0.0f), predictionGain(1.0f), rewardSensitivity(1.0f),
             brain(nullptr), noveltyThreshold(0.3f), predictionAccuracy(0.5f),
             learningRate(0.1f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) {
    // Real reward computation from environmental observations
    float reward = 0.0f;
    
    // Get sensory input data from observation
    const auto& sensoryInput = observation.getSensoryInput();
    const auto& observationData = sensoryInput->getData();
    
    if (observationData.empty()) {
        return 0.0f;
    }
    
    // Basic reward computation based on sensory input patterns
    // This can be extended with domain-specific reward functions
    float intensity = 0.0f;
    float novelty = 0.0f;
    
    // Compute intensity (magnitude of response)
    for (float value : observationData) {
        intensity += std::abs(value);
    }
    intensity /= observationData.size();
    
    // Novelty detection using history
    if (!pImpl->history.empty()) {
        float avgRecent = 0.0f;
        size_t count = std::min(size_t(10), pImpl->history.size());
        for (size_t i = 0; i < count; ++i) {
            avgRecent += pImpl->history[pImpl->history.size() - 1 - i];
        }
        avgRecent /= count;
        
        // Novelty is deviation from expected reward
        novelty = std::abs(intensity - avgRecent);
    }
    
    // Neuromodulator influence on reward computation
    float neuromodulatorInfluence = getNeuromodulatorInfluence();
    
    // Compute final reward
    reward = intensity * pImpl->rewardSensitivity;
    
    // Add novelty bonus (enhances exploration)
    if (novelty > pImpl->noveltyThreshold) {
        reward += novelty * 0.5f;
    }
    
    // Update prediction error
    float predictionError = intensity - pImpl->expectedReward;
    pImpl->predictionError = predictionError;
    
    // Update expected reward (learning)
    pImpl->expectedReward += predictionError * pImpl->predictionGain * pImpl->learningRate;
    
    // Store in history
    pImpl->history.push_back(reward);
    
    // Integrate with dopamine system
    integrateWithDopamine(predictionError);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::integrateWithDopamine(float error, float learningRate) {
    // Integrate reward with dopamine system for learning
    if (pImpl->brain) {
        if (auto* dopamine = pImpl->brain->getDopamine()) {
            // Signal reward prediction error to dopamine
            dopamine->signalRewardPredictionError(error);
        }
    }
}

float Reward::getNeuromodulatorInfluence() const {
    // Compute influence of other neuromodulators on reward processing
    if (!pImpl->brain) {
        return 1.0f;
    }
    
    float totalInfluence = 1.0f;  // Baseline
    
    // Acetylcholine influence (attention modulation)
    if (auto* acetylcholine = dynamic_cast<Acetylcholine*>(pImpl->brain->getAcetylcholine())) {
        float attentionLevel = acetylcholine->getLevel();
        totalInfluence *= (1.0f + attentionLevel * 0.2f);
    }
    
    // Norepinephrine influence (arousal modulation)
    if (auto* norepinephrine = dynamic_cast<Norepinephrine*>(pImpl->brain->getNorepinephrine())) {
        float arousalLevel = norepinephrine->getLevel();
        totalInfluence *= (1.0f + arousalLevel * 0.3f);
    }
    
    // Serotonin influence (mood modulation)
    if (auto* serotonin = dynamic_cast<Serotonin*>(pImpl->brain->getSerotonin())) {
        float moodLevel = serotonin->getLevel();
        totalInfluence *= (1.0f - moodLevel * 0.2f);  // Serotonin typically reduces reward sensitivity
    }
    
    return totalInfluence;
}

} // namespace nlm
