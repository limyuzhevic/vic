#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float phasicGain;
    float tonicGain;
    float predictionErrorAccumulator;
    float rewardAccumulator;
    float noveltyAccumulator;
    float learningRate;
    float predictionError;
    float rewardPredictionError;
    
    // Phasic and tonic components
    float phasicComponent;
    float tonicComponent;
    float mode;  // 0.0f = tonic only, 1.0f = phasic only, >0 = mixed
    
    // Temporal dynamics
    float timeConstant;
    float integrationFactor;
    
    // Plasticity modulation
    float plasticityModulation;
    
    // Reward history for prediction error calculation
    std::vector<float> rewardHistory;
    std::vector<float> predictionHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(2.0f)
        , decayRate(0.1f)
        , phasicGain(1.0f)
        , tonicGain(0.5f)
        , predictionErrorAccumulator(0.0f)
        , rewardAccumulator(0.0f)
        , noveltyAccumulator(0.0f)
        , learningRate(0.01f)
        , predictionError(0.0f)
        , rewardPredictionError(0.0f)
        , phasicComponent(0.0f)
        , tonicComponent(baseline)
        , mode(0.5f)
        , timeConstant(0.5f)
        , integrationFactor(0.9f)
        , plasticityModulation(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Clamp to reasonable bounds for dopamine levels
    pImpl->level = std::clamp(level, 0.0f, 10.0f);
    // Update components based on mode
    updateComponents();
}

void Dopamine::setMode(float mode) {
    pImpl->mode = std::clamp(mode, 0.0f, 1.0f);
    updateComponents();
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = gain;
}

void Dopamine::setTonicGain(float gain) {
    pImpl->tonicGain = gain;
}

void Dopamine::setTimeConstant(float tc) {
    pImpl->timeConstant = tc;
}

void Dopamine::setLearningRate(float lr) {
    pImpl->learningRate = lr;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on Schultz 1997 model
    // D1 receptor (high DA): facilitates LTP, strengthening synapses
    // D2 receptor (low DA): facilitates LTD, weakening synapses
    // Integrates with prediction error for credit assignment
    
    // Normalize dopamine level for receptor effects
    float normalizedDA = std::clamp(pImpl->level / pImpl->baseline, 0.0f, 2.0f);
    
    // D1 receptor effect (LTP): sigmoidal response to DA
    float d1Effect = std::tanh((normalizedDA - 1.0f) * 2.0f) * 0.5f + 0.5f;
    
    // D2 receptor effect (LTD): inverse sigmoidal response to DA
    float d2Effect = std::tanh(1.0f - normalizedDA) * 0.5f + 0.5f;
    
    // Prediction error modulation for credit assignment
    // RPE gates plasticity learning (Schultz delta rule)
    float peEffect = std::abs(pImpl->rewardPredictionError) * 0.7f;
    
    // Combine D1/D2 effects weighted by neurobiological evidence
    // D1 plays stronger role in reward learning
    pImpl->plasticityModulation = d1Effect * 0.55f + d2Effect * 0.25f + peEffect * 0.2f;
    
    // Clamp to biologically plausible range for synaptic weights
    pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.1f, 2.0f);
    
    return pImpl->plasticityModulation;
}

void Dopamine::updateComponents() {
    // Update phasic and tonic components based on mode
    // Real biological coupling between tonic and phasic dopamine
    if (pImpl->mode > 0.5f) {
        // Phasic mode: phasic component dominates
        pImpl->phasicComponent = pImpl->level * (1.0f - pImpl->mode) * 2.0f;
        pImpl->tonicComponent = pImpl->baseline * pImpl->tonicGain * 0.5f;  // Reduced tonic
    } else {
        // Tonic mode: tonic component dominates
        pImpl->phasicComponent = 0.0f;
        pImpl->tonicComponent = pImpl->baseline * pImpl->tonicGain;
    }
    
    // Combine components to get total level
    pImpl->level = pImpl->tonicComponent + pImpl->phasicComponent;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with exponential decay
    if (dt > 0.0) {
        // Exponential decay towards baseline (Schultz model)
        float decayFactor = std::exp(-pImpl->decayRate * dt);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
        
        // Clamp to reasonable bounds
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
    
    // Low-pass filter for tonic dopamine (temporal filtering)
    pImpl->predictionErrorAccumulator = 
        pImpl->integrationFactor * pImpl->predictionErrorAccumulator + 
        (1.0f - pImpl->integrationFactor) * pImpl->rewardPredictionError;
    
    // Update history
    pImpl->predictionError = pImpl->predictionErrorAccumulator;
    
    // Add small random fluctuations to tonic component for biological realism
    if (pImpl->mode < 0.3f) {
        // Add low-frequency noise to tonic component
        pImpl->tonicComponent = pImpl->baseline + (rand() % 1000) / 1000.0f * pImpl->baseline * 0.1f;
    }
}

void Dopamine::signalReward(float reward) {
    // Integrate reward with temporal filtering (low-pass filter)
    pImpl->rewardAccumulator = 
        pImpl->integrationFactor * pImpl->rewardAccumulator + 
        (1.0f - pImpl->integrationFactor) * reward;
    
    // Store reward history for prediction error calculation
    pImpl->rewardHistory.push_back(pImpl->rewardAccumulator);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Phasic dopamine response (burst) with biological scaling
    // Positive reward produces phasic burst (Schultz 1997)
    float phasicBurst = reward * pImpl->phasicGain * 15.0f;  // Enhanced burst amplitude
    
    // Apply thresholding - minimal response for small rewards
    if (std::abs(phasicBurst) < 0.1f) {
        phasicBurst = 0.0f;
    }
    
    // Update level with phasic component
    pImpl->phasicComponent = phasicBurst;
    updateComponents();
}

void Dopamine::signalRewardPredictionError(float error) {
    // Store prediction error
    pImpl->rewardPredictionError = error;
    
    // Compute prediction error based on accumulated rewards (delta rule)
    if (!pImpl->rewardHistory.empty()) {
        // Simple prediction error: difference between actual reward and predicted
        // For now, predict based on moving average of recent rewards
        size_t historySize = std::min(size_t(10), pImpl->rewardHistory.size());
        float predicted = 0.0f;
        for (size_t i = 0; i < historySize; ++i) {
            predicted += pImpl->rewardHistory[pImpl->rewardHistory.size() - 1 - i];
        }
        predicted /= historySize;
        
        // Real reward prediction error (delta rule) - temporal difference
        float tdError = pImpl->rewardAccumulator - predicted;
        pImpl->rewardPredictionError = tdError * pImpl->learningRate;
    }
    
    // Phasic dopamine response to prediction error (Schultz 1997)
    // Positive RPE: phasic burst (reward prediction)
    // Negative RPE: phasic dip (prediction error aversion)
    pImpl->phasicComponent = pImpl->rewardPredictionError * pImpl->phasicGain * 8.0f;
    
    // Apply biological thresholding
    if (std::abs(pImpl->phasicComponent) < 0.05f) {
        pImpl->phasicComponent = 0.0f;
    }
    
    updateComponents();
}

void Dopamine::signalNovelty(float novelty) {
    // Novelty signal modulates dopamine for exploration
    pImpl->noveltyAccumulator = 
        pImpl->integrationFactor * pImpl->noveltyAccumulator + 
        (1.0f - pImpl->integrationFactor) * novelty;
    
    // Novelty adds to phasic component (exploratory burst)
    // Novelty enhances exploration motivation
    pImpl->phasicComponent += pImpl->noveltyAccumulator * pImpl->phasicGain * 1.5f;
    
    // Apply biological constraints - novelty has limited effect
    if (pImpl->phasicComponent > pImpl->level * 0.3f) {
        pImpl->phasicComponent = pImpl->level * 0.3f;
    }
    
    updateComponents();
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasicComponent;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonicComponent;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float Dopamine::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Dopamine::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->predictionErrorAccumulator = 0.0f;
    pImpl->rewardAccumulator = 0.0f;
    pImpl->noveltyAccumulator = 0.0f;
}

float Dopamine::getExplorationValue() const {
    // Exploration value based on novelty and prediction error
    float noveltyFactor = std::abs(pImpl->noveltyAccumulator);
    float peFactor = std::abs(pImpl->rewardPredictionError);
    return (noveltyFactor + peFactor) * 0.5f;
}

} // namespace nlm
