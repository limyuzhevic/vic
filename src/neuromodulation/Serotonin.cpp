#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float rewardSensitivity;
    float impulsivityFactor;
    float learningRateModulation;
    float decisionBias;
    std::vector<NeuronId> highSerotoninNeurons;
    std::unordered_map<uint64_t, float> rewardPredictionErrors;
    
    Impl() : level(0.0f), baseline(0.03f), rewardSensitivity(1.0f),
             impulsivityFactor(1.0f), learningRateModulation(1.0f),
             decisionBias(0.5f) {}
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
    // Serotonin modulates plasticity based on reward history
    float rewardHistoryFactor = 1.0f;
    if (!pImpl->rewardPredictionErrors.empty()) {
        float avgError = 0.0f;
        for (const auto& pair : pImpl->rewardPredictionErrors) {
            avgError += pair.second;
        }
        avgError /= pImpl->rewardPredictionErrors.size();
        rewardHistoryFactor = std::max(0.1f, 1.0f - std::abs(avgError));
    }
    
    return 0.5f + 0.5f * pImpl->level * pImpl->rewardSensitivity * rewardHistoryFactor;
}

void Serotonin::update(TimestepDuration dt, const std::vector<NeuronId>& neurons,
                      const std::unordered_map<uint64_t, float>& rewardErrors) {
    // Update serotonin dynamics based on reward prediction errors
    pImpl->rewardPredictionErrors = rewardErrors;
    
    // Clear old high serotonin neurons
    std::vector<NeuronId> newHighLevelNeurons;
    
    for (NeuronId neuron : neurons) {
        // Find associated reward prediction error
        auto it = rewardErrors.find(static_cast<uint64_t>(neuron));
        float rewardError = it != rewardErrors.end() ? it->second : 0.0f;
        
        if (rewardError > 0.5f) {
            // Large positive prediction error -> high serotonin (learning positive outcome)
            pImpl->level = std::min(1.0f, pImpl->level + 0.1f * dt);
            newHighLevelNeurons.push_back(neuron);
        } else if (rewardError < -0.5f) {
            // Large negative prediction error -> low serotonin (punishment)
            pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f * dt);
        }
    }
    
    pImpl->highSerotoninNeurons = newHighLevelNeurons;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void Serotonin::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::max(0.1f, std::min(3.0f, sensitivity));
}

float Serotonin::getImpulsivityFactor() const {
    return pImpl->impulsivityFactor;
}

void Serotonin::setImpulsivityFactor(float factor) {
    pImpl->impulsivityFactor = std::max(0.1f, std::min(3.0f, factor));
}

float Serotonin::getLearningRateModulation() const {
    return pImpl->learningRateModulation;
}

void Serotonin::setLearningRateModulation(float modulation) {
    pImpl->learningRateModulation = std::max(0.1f, std::min(3.0f, modulation));
}

float Serotonin::getDecisionBias() const {
    return pImpl->decisionBias;
}

void Serotonin::setDecisionBias(float bias) {
    pImpl->decisionBias = std::clamp(bias, 0.0f, 1.0f);
}

const std::vector<NeuronId>& Serotonin::getHighSerotoninNeurons() const {
    return pImpl->highSerotoninNeurons;
}

void Serotonin::clearHighSerotoninNeurons() {
    pImpl->highSerotoninNeurons.clear();
}

float Serotonin::computeDecisionModulation(float baselineValue, bool isActionSelection) const {
    // Serotonin affects decision making and impulsivity
    float modulation = 1.0f;
    
    if (isActionSelection) {
        // In action selection, serotonin biases toward more conservative choices
        if (pImpl->level > 0.5f) {
            // High serotonin -> more conservative decisions
            modulation *= (1.0f - pImpl->decisionBias * (pImpl->level - 0.5f) * 2.0f);
        }
        
        // Impulsivity factor affects action selection
        modulation *= pImpl->impulsivityFactor;
    }
    
    return baselineValue * modulation;
}

float Serotonin::computeLearningRateModulation() const {
    // Serotonin affects learning rate based on reward history
    return pImpl->learningRateModulation * (1.0f + pImpl->level * 0.5f);
}

} // namespace nlm