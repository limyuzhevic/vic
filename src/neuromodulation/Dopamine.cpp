#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;                    // Current dopamine level (-1 to 1)
    float predictionError;          // Reward prediction error
    float learnedValue;              // Learned value estimate
    float learningRate;              // Dopamine learning rate
    float baseline;                  // Baseline dopamine level
    float tonicLevel;                // Tonic (background) dopamine level
    std::vector<float> phasicHistory;  // Recent phasic dopamine bursts
    
    // Plasticity modulation factors
    float stpdLtpWeight;            // STDP LTP weight
    float stpdLtdWeight;            // STDP LTD weight
    float hebbianLearningRate;      // Hebbian learning rate
    float structuralPlasticityRate; // Structural plasticity rate
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , predictionError(0.0f)
        , learnedValue(0.0f)
        , learningRate(0.01f)
        , baseline(0.1f)
        , tonicLevel(0.1f)
        , stpdLtpWeight(0.01f)
        , stpdLtdWeight(0.012f)
        , hebbianLearningRate(0.05f)
        , structuralPlasticityRate(0.0001f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

void Dopamine::update(float predictionError, TimestepDuration dt) {
    // Calculate reward prediction error (RPE)
    pImpl->predictionError = predictionError;
    
    // Update learned value using RPE
    pImpl->learnedValue += pImpl->learningRate * pImpl->predictionError;
    
    // Phasic dopamine burst/slap response
    if (pImpl->predictionError > 0.0f) {
        // Reward: burst of dopamine
        pImpl->level = std::min(1.0f, pImpl->level + 0.5f + pImpl->predictionError);
    } else {
        // Prediction error: dopamine dip/slap
        pImpl->level = std::max(-1.0f, pImpl->level - 0.3f + pImpl->predictionError);
    }
    
    // Baseline and tonic modulation
    pImpl->level = pImpl->level * 0.7f + pImpl->tonicLevel * 0.3f;
    
    // Decay
    pImpl->level *= (1.0f - 0.05f * static_cast<float>(dt));
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
    
    // Record phasic response
    pImpl->phasicHistory.push_back(pImpl->predictionError);
    if (pImpl->phasicHistory.size() > 100) {
        pImpl->phasicHistory.erase(pImpl->phasicHistory.begin());
    }
    
    // Update plasticity factors based on dopamine level
    updatePlasticityFactors();
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getLearnedValue() const {
    return pImpl->learnedValue;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->phasicHistory.clear();
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    pImpl->tonicLevel = baseline;
}

void Dopamine::setTonicLevel(float level) {
    pImpl->tonicLevel = level;
}

float Dopamine::getPlasticityFactor() const {
    // Positive dopamine increases plasticity, negative decreases
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::updatePlasticityFactors() {
    // Adapt plasticity rules based on dopamine level
    float factor = getPlasticityFactor();
    
    // Scale STDP weights
    pImpl->stpdLtpWeight = 0.01f * factor;
    pImpl->stpdLtdWeight = 0.012f * factor;
    
    // Scale Hebbian learning
    pImpl->hebbianLearningRate = 0.05f * factor;
    
    // Scale structural plasticity
    pImpl->structuralPlasticityRate = 0.0001f * factor;
}

void Dopamine::getPlasticityFactors(float& ltpWeight, float& ltdWeight, float& hebbianRate, float& structuralRate) const {
    ltpWeight = pImpl->stpdLtpWeight;
    ltdWeight = pImpl->stpdLtdWeight;
    hebbianRate = pImpl->hebbianLearningRate;
    structuralRate = pImpl->structuralPlasticityRate;
}

const std::vector<float>& Dopamine::getPhasicHistory() const {
    return pImpl->phasicHistory;
}

bool Dopamine::isBursting() const {
    return pImpl->predictionError > 0.1f;
}

bool Dopamine::isSlapping() const {
    return pImpl->predictionError < -0.1f;
}

} // namespace nlm
