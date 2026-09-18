#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRatePositive;
    float learningRateNegative;
    float moodLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.03f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(2.5f)
        , learningRatePositive(1.0f)
        , learningRateNegative(1.0f)
        , moodLevel(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // 5-HT affects learning rates and mood
    pImpl->learningRatePositive = 1.0f + pImpl->level * 0.8f;
    pImpl->learningRateNegative = 1.0f + pImpl->level * 0.5f;
    pImpl->moodLevel = 1.0f + pImpl->level * 0.5f;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on learning requirements
    // Higher 5-HT enhances both positive and negative learning plasticity
    return 1.0f + pImpl->level * 0.6f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // 5-HT affects learning and mood
    if (pImpl->brain) {
        // 5-HT modulates learning rates
        if (pImpl->brain->getSTDP()) {
            pImpl->brain->getSTDP()->setLTPWeight(0.01f * pImpl->learningRatePositive);
            pImpl->brain->getSTDP()->setLTDWeight(0.012f * pImpl->learningRateNegative);
        }
        
        if (pImpl->brain->getHebbian()) {
            pImpl->brain->getHebbian()->setLearningRate(0.5f * pImpl->learningRatePositive);
        }
        
        // 5-HT influences social behavior and decision making
        // It affects reward prediction error processing
        if (pImpl->brain->getPredictionErrorSignal()) {
            // 5-HT modulates prediction error sensitivity
            float errorMod = 1.0f - pImpl->level * 0.3f;  // Higher 5-HT reduces error sensitivity
            pImpl->brain->getPredictionErrorSignal()->setLevel(pImpl->level * errorMod);
        }
        
        // 5-HT affects impulsivity and decision making
        // It can modulate attentional selection and decision thresholds
        if (pImpl->brain->getAttention()) {
            // 5-HT influences attentional control
            float attentionMod = 1.0f - pImpl->level * 0.2f;  // Moderate 5-HT improves focus
            pImpl->brain->getAttention()->setInhibitionStrength(0.5f * attentionMod);
        }
    }
}

void Serotonin::modulateLearning(float reward, float predictionError) {
    if (pImpl->brain) {
        // 5-HT modulates learning rates based on prediction errors
        float learningMod = pImpl->level * (1.0f - std::abs(predictionError));
        
        if (pImpl->brain->getSTDP()) {
            // Adjust STDP weights based on 5-HT level
            if (reward > 0.0f) {
                float ltpWeight = pImpl->brain->getSTDP()->getLTPWeight() * pImpl->learningRatePositive;
                pImpl->brain->getSTDP()->setLTPWeight(ltpWeight);
            } else if (reward < 0.0f) {
                float ltdWeight = pImpl->brain->getSTDP()->getLTDWeight() * pImpl->learningRateNegative;
                pImpl->brain->getSTDP()->setLTDWeight(ltdWeight);
            }
        }
        
        // 5-HT influences social behavior through prediction error
        if (predictionError != 0.0f) {
            // Stronger prediction errors with higher 5-HT create stronger learning
            if (pImpl->brain->getPredictionErrorSignal()) {
                pImpl->brain->getPredictionErrorSignal()->setLevel(std::abs(predictionError) * pImpl->level);
            }
        }
    }
}

void Serotonin::affectDecisionMaking(float uncertainty) {
    if (pImpl->brain) {
        // 5-HT influences decision making under uncertainty
        float decisionMod = 1.0f / (1.0f + uncertainty * pImpl->level);
        
        // Adjust attentional selection based on decision making
        if (pImpl->brain->getAttention()) {
            pImpl->brain->getAttention()->setExcitationStrength(decisionMod);
        }
    }
}

} // namespace nlm