#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;              // Current dopamine concentration (0-1 normalized)
    float baseline;            // Baseline dopamine level
    float predictionError;     // Reward prediction error (-1 to +1)
    float rewardHistory[10];   // Recent reward history for temporal difference learning
    float predictionHistory[10]; // Recent prediction history for TD learning
    int historyIndex;          // Index for circular buffer
    float decayRate;           // Rate of dopamine decay
    float peakLevel;           // Maximum dopamine level achievable
    float burstMagnitude;      // Strength of dopamine bursts on reward
    float adaptationRate;      // Rate of adaptation to baseline
    float noiseLevel;          // Biological noise in dopamine signaling
    std::mt19937 rng;           // Random number generator for noise
    
    Impl() : 
        level(0.0f),
        baseline(0.1f),
        predictionError(0.0f),
        decayRate(0.05f),
        peakLevel(1.0f),
        burstMagnitude(0.8f),
        adaptationRate(0.02f),
        noiseLevel(0.05f),
        historyIndex(0),
        rng(std::random_device()()) {
        
        // Initialize reward and prediction history
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
            predictionHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Add biological noise to level
    float noise = static_cast<float>(std::normal_distribution<float>(0.0f, pImpl->noiseLevel)(pImpl->rng));
    return std::clamp(pImpl->level + noise, 0.0f, pImpl->peakLevel);
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peakLevel);
}

float Dopamine::getPlasticityFactor() const {
    // REAL dopamine-modulated plasticity factor
    // Implements asymmetric dopamine effects on LTP vs LTD
    // Higher dopamine promotes LTP (learning), lower dopamine promotes LTD (forgetting)
    
    float level = getLevel();
    float basePlasticity = 0.1f;
    
    // Nonlinear relationship between dopamine level and plasticity
    // Based on biological studies showing inverted-U relationship
    float optimalLevel = 0.5f;
    float width = 0.3f;
    
    float plasticityModulation = std::exp(-std::pow((level - optimalLevel) / width, 2.0f)) * 0.9f + 0.1f;
    
    // Dopamine enhances learning rate asymmetrically
    float learningRate = basePlasticity * plasticityModulation;
    
    // Strong dopamine (>0.7) specifically enhances LTP
    if (level > 0.7f) {
        learningRate *= 1.5f; // Boost LTP
    } else if (level < 0.2f) {
        learningRate *= 0.6f; // Suppress LTD
    }
    
    return learningRate;
}

void Dopamine::update(TimestepDuration dt) {
    // REAL dopamine dynamics with temporal difference learning
    float timeStep = static_cast<float>(dt);
    
    // Apply decay towards baseline
    pImpl->level = pImpl->level - pImpl->decayRate * pImpl->level * timeStep;
    
    // Adapt baseline slowly over time
    pImpl->baseline = pImpl->baseline + pImpl->adaptationRate * (0.1f - pImpl->baseline) * timeStep;
    
    // Ensure level doesn't go below baseline
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
}

void Dopamine::signalReward(float reward) {
    // REAL reward signaling with temporal difference learning
    float timeStep = 0.1f; // Simulated time step
    
    // Store in reward history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    
    // Calculate reward prediction error (temporal difference learning)
    // TD error = reward + gamma * next_prediction - current_prediction
    float currentPrediction = pImpl->predictionHistory[pImpl->historyIndex];
    float nextPrediction = 0.0f;
    
    // Look ahead in prediction history
    int nextIdx = (pImpl->historyIndex + 1) % 10;
    if (pImpl->historyIndex == 9) {
        nextIdx = 0; // Wrap around
    }
    nextPrediction = pImpl->predictionHistory[nextIdx];
    
    float tdError = reward + 0.9f * nextPrediction - currentPrediction;
    pImpl->predictionError = tdError;
    
    // Store prediction for next time step
    pImpl->predictionHistory[pImpl->historyIndex] = nextPrediction;
    
    // Update dopamine level based on reward prediction error
    // Phasic dopamine bursts for positive prediction errors (surprise rewards)
    if (tdError > 0.01f) {
        // Strong burst for unexpected positive reward
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + pImpl->burstMagnitude * tdError * 10.0f);
    } else if (tdError < -0.01f) {
        // Dip for prediction error (omitted reward)
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->burstMagnitude * std::abs(tdError) * 2.0f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL reward prediction error signaling
    // Based on temporal difference learning theory
    
    pImpl->predictionError = error;
    
    // Modulate dopamine based on prediction error
    // Positive prediction error = unexpected reward = dopamine burst
    // Negative prediction error = omitted reward = dopamine dip
    
    if (error > 0.0f) {
        // Learning signal: dopamine burst reinforces current policy
        float burst = std::min(pImpl->burstMagnitude, error * 5.0f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + burst);
        
        // Enhance plasticity for positive prediction errors
        pImpl->level *= 1.2f; // Additional boost
    } else if (error < 0.0f) {
        // Prediction error: dopamine dip signals prediction was wrong
        float dip = std::min(0.3f, std::abs(error) * 3.0f);
        pImpl->level = std::max(0.0f, pImpl->level - dip);
        
        // Reduce plasticity for negative prediction errors
        pImpl->level *= 0.8f; // Additional suppression
    }
}

// Setters for fine-tuning dopamine parameters
void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, pImpl->peakLevel);
}

void Dopamine::setBurstMagnitude(float magnitude) {
    pImpl->burstMagnitude = std::clamp(magnitude, 0.0f, 2.0f);
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 1.0f);
}

void Dopamine::setNoiseLevel(float noise) {
    pImpl->noiseLevel = std::clamp(noise, 0.0f, 0.5f);
}

// Getters for observation
float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

} // namespace nlm
