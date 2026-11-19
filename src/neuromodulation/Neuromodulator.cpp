#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float predictionError;
    float rewardPredictionError;
    
    // Dopamine dynamics state
    float temporalDerivative;
    float eligibilityTrace;
    std::queue<float> recentRewards;
    
    // Learning components
    float phasicComponent;
    float tonicComponent;
    float predictionErrorTrace;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             adaptationRate(0.01f), predictionError(0.0f), rewardPredictionError(0.0f),
             temporalDerivative(0.0f), eligibilityTrace(0.0f), phasicComponent(0.0f),
             tonicComponent(baseline), predictionErrorTrace(0.0f) {
        recentRewards = std::queue<float>();
    }
    
    // Realistic dopamine dynamics with multiple components
    void update(double dt) {
        // Update tonic component (baseline)
        tonicComponent = baseline;
        
        // Update phasic component based on rewards and prediction errors
        updatePhasicComponents(dt);
        
        // Combine phasic and tonic components
        level = tonicComponent + phasicComponent;
        
        // Apply decay towards baseline
        level = std::max(baseline, level * (1.0f - decayRate * static_cast<float>(dt)));
        
        // Update eligibility trace for learning
        eligibilityTrace *= (1.0f - adaptationRate * static_cast<float>(dt));
        
        // Update prediction error trace
        predictionErrorTrace *= (1.0f - adaptationRate * static_cast<float>(dt));
        
        // Maintain reward history
        maintainRewardHistory();
    }
    
    // Update phasic dopamine components
    void updatePhasicComponents(double dt) {
        phasicComponent = 0.0f;
        
        // Phasic response to unexpected rewards
        if (!recentRewards.empty()) {
            float avgReward = calculateAverageReward();
            phasicComponent += avgReward * releaseRate;
        }
        
        // Prediction error response (TD error)
        if (predictionError > 0.0f) {
            float tdError = rewardPredictionError - predictionError;
            phasicComponent += tdError * releaseRate * 0.5f;
        }
        
        // Phasic response to novel stimuli
        phasicComponent += getNoveltySignal() * releaseRate * 0.3f;
        
        // Apply temporal derivative for predictive signaling
        float derivative = calculateTemporalDerivative();
        phasicComponent += derivative * 0.2f;
    }
    
    // Calculate average of recent rewards
    float calculateAverageReward() const {
        if (recentRewards.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t count = 0;
        std::queue<float> temp = recentRewards;
        
        while (!temp.empty()) {
            sum += temp.front();
            temp.pop();
            count++;
        }
        
        return sum / static_cast<float>(count);
    }
    
    // Maintain reward history with bounded memory
    void maintainRewardHistory() {
        recentRewards.push(0.0f); // Placeholder - should be updated with actual reward
        if (recentRewards.size() > 10) { // Keep last 10 rewards
            recentRewards.pop();
        }
    }
    
    // Get novelty signal based on recent activity patterns
    float getNoveltySignal() const {
        // Simulate novelty detection based on signal variance
        return std::min(1.0f, predictionError * 2.0f);
    }
    
    // Calculate temporal derivative
    float calculateTemporalDerivative() const {
        float currentDerivative = 0.0f;
        if (temporalDerivative == 0.0f) {
            currentDerivative = level - baseline;
        } else {
            currentDerivative = level - temporalDerivative;
        }
        temporalDerivative = level;
        return currentDerivative;
    }
    
    // Update eligibility trace for credit assignment
    void updateEligibilityTrace(float reward, float learningRate) {
        eligibilityTrace += reward * learningRate;
        eligibilityTrace = std::max(0.0f, std::min(1.0f, eligibilityTrace));
    }
    
    // Update prediction error tracking
    void updatePredictionError(float error) {
        predictionError = error;
        predictionErrorTrace = std::max(predictionErrorTrace, error);
    }
    
    // Calculate realistic plasticity factor based on dopamine level
    float calculatePlasticityFactor() const {
        // Inverted U-curve relationship: moderate dopamine maximizes plasticity
        float normalizedLevel = (level - baseline) / (peak - baseline);
        
        if (normalizedLevel < 0.2f) {
            return 0.2f * normalizedLevel; // Low dopamine = low plasticity
        } else if (normalizedLevel < 0.8f) {
            return 0.4f +  মুক্তहा * (normalizedLevel - 0.2f) / 0.6f; // Moderate dopamine = high plasticity
        } else {
            return 1.0f - 0.5f * (normalizedLevel - 0.8f) / 0.2f; // High dopamine = reduced plasticity
        }
    }
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Phase 2: Implement real dopamine-modulated plasticity factor
    // Realistic dopamine dynamics with inverted U-curve for optimal learning
    
    return pImpl->calculatePlasticityFactor();
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Implement real dopamine dynamics
    // Sophisticated dopamine dynamics with multiple components
    
    pImpl->update(dt);
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Implement real reward signaling
    // Burst of dopamine on reward with eligibility traces
    
    // Update reward history
    pImpl->recentRewards.push(reward);
    
    // Calculate immediate dopamine response
    float immediateResponse = std::min(pImpl->peak, reward * pImpl->releaseRate);
    pImpl->level = std::max(pImpl->level, immediateResponse);
    
    // Update eligibility trace for learning
    pImpl->updateEligibilityTrace(reward, pImpl->adaptationRate);
    
    // Store reward for prediction error calculation
    pImpl->predictionError = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Implement reward prediction error signaling
    // Dopamine encodes reward prediction errors (TD error)
    
    // Update prediction error
    pImpl->rewardPredictionError = error;
    pImpl->updatePredictionError(error);
    
    // Integrate with reward signal
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        float positiveResponse = error * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->level, pImpl->baseline + positiveResponse);
    } else if (error < -0.5f) {
        // Large negative prediction error (worse than expected)
        float negativeResponse = std::abs(error) * pImpl->releaseRate * 0.3f;
        pImpl->level *= (1.0f - negativeResponse);
    }
}

} // namespace nlm
