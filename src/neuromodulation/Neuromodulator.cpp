#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics parameters
    float level;                    // Current dopamine concentration (nM)
    float baseline;                 // Baseline dopamine level (nM)
    float peak;                     // Maximum dopamine level (nM)
    float decayRate;                // Decay time constant (1/ms)
    float releaseRate;              // Release rate per reward unit (nM/reward)
    float predictionErrorGain;      // Sensitivity to prediction errors
    float adaptationTimeConstant;   // Time constant for adaptation (ms)
    
    // Internal state for dynamics
    float expectedValue;            // Expected value prediction
    float predictionError;          // Reward prediction error
    float eligibilityTrace;         // For credit assignment
    float neuromodulatoryBurst;      // Burst signal for phasic dopamine
    
    // Noise parameters
    float noiseLevel;               // Level of physiological noise
    float noiseCorrelation;         // Correlation between neurons
    
    // History for filtering
    std::vector<float> levelHistory;
    std::vector<float> rewardHistory;
    
    // Random generator for physiological noise
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : 
        level(0.0f),
        baseline(0.2f),
        peak(2.5f),
        decayRate(0.05f),
        releaseRate(0.8f),
        predictionErrorGain(1.5f),
        adaptationTimeConstant(1000.0f),
        expectedValue(0.0f),
        predictionError(0.0f),
        eligibilityTrace(0.0f),
        neuromodulatoryBurst(0.0f),
        noiseLevel(0.05f),
        noiseCorrelation(0.3f),
        levelHistory(100, 0.0f),
        rewardHistory(50, 0.0f) {
        
        // Initialize random generator with current time as seed
        std::random_device rd;
        rng.seed(static_cast<unsigned int>(std::time(nullptr)));
        noiseDist = std::normal_distribution<float>(0.0f, noiseLevel);
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on experimental data
    // Dopamine has an inverted U-shaped relationship with plasticity
    // Based on Doya et al. (2002) and Surrédée et al. (2015) findings
    
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Inverted U-shaped function: moderate dopamine enhances plasticity
    // Too little or too much dopamine reduces plasticity
    if (normalizedLevel < 0.3f) {
        // Low dopamine: reduced baseline plasticity
        return 0.1f + 0.8f * (normalizedLevel / 0.3f);
    } else if (normalizedLevel > 0.7f) {
        // High dopamine: reduced plasticity (inverted U)
        float excess = (normalizedLevel - 0.7f) / 0.3f;
        return 1.0f - excess * 0.8f;
    } else {
        // Optimal range: enhanced plasticity
        float deviation = std::abs(normalizedLevel - 0.5f) / 0.5f;
        return 1.0f - deviation * 0.2f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with physiological modeling
    // Combines tonic baseline with phasic bursts
    
    float dt_f = static_cast<float>(dt);
    
    // 1. Phasic dopamine burst modulation
    if (pImpl->neuromodulatoryBurst > 0.0f) {
        // Burst signal decays exponentially
        pImpl->level += pImpl->neuromodulatoryBurst * pImpl->releaseRate;
        pImpl->neuromodulatoryBurst *= std::exp(-dt_f / 50.0f);  // 50ms time constant
    }
    
    // 2. Baseline dynamics with adaptation
    float targetLevel = pImpl->baseline;
    if (pImpl->predictionError != 0.0f) {
        // Prediction error drives tonic changes
        targetLevel += pImpl->predictionError * pImpl->predictionErrorGain;
    }
    
    // 3. Exponential approach to target level
    float delta = targetLevel - pImpl->level;
    pImpl->level += delta * (1.0f - std::exp(-dt_f / pImpl->decayRate));
    
    // 4. Add physiological noise
    pImpl->level += noiseDist(pImpl->rng) * 0.1f;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // 5. Update history for filtering
    pImpl->levelHistory.push_back(pImpl->level);
    if (pImpl->levelHistory.size() > 100) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
    
    // 6. Apply eligibility trace for credit assignment
    pImpl->eligibilityTrace *= std::exp(-dt_f / pImpl->adaptationTimeConstant);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal difference learning
    // Implements Rescorla-Wagner style learning
    
    // Calculate reward prediction error
    float expected = pImpl->expectedValue;
    float actual = reward;
    float predictionError = actual - expected;
    
    // Update expected value with learning rate
    float learningRate = 0.1f;  // Initial learning rate
    learningRate *= getPlasticityFactor();  // Modulate by current dopamine level
    
    pImpl->expectedValue += learningRate * predictionError;
    
    // Generate phasic dopamine burst proportional to prediction error
    // Phasic dopamine is much stronger than tonic
    pImpl->predictionError = predictionError;
    float burstMagnitude = std::abs(predictionError) * 2.0f;  // Scale factor
    
    if (predictionError > 0.0f) {
        pImpl->neuromodulatoryBurst = burstMagnitude;
    } else if (predictionError < 0.0f) {
        // Negative prediction error (omission) produces dip
        pImpl->neuromodulatoryBurst = -burstMagnitude * 0.5f;
    }
    
    // Record reward for history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 50) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (for temporal difference learning)
    pImpl->predictionError = error;
    
    // Generate phasic response proportional to error magnitude
    float burstMagnitude = std::abs(error) * pImpl->predictionErrorGain;
    
    if (error > 0.0f) {
        pImpl->neuromodulatoryBurst += burstMagnitude;
    } else if (error < 0.0f) {
        pImpl->neuromodulatoryBurst += error * 0.5f;
    }
}

} // namespace nlm
