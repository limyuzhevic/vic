#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace nlm {

struct Dopamine::Impl {
    // Current dopamine level (phasic component)
    float phasicLevel;
    
    // Tonic dopamine level (baseline)
    float tonicLevel;
    
    // History of dopamine spikes for prediction error computation
    std::vector<float> predictionErrorHistory;
    
    // Learning-related variables
    float predictionError;           // Current reward prediction error
    float eligibilityTrace;          // For credit assignment
    float tdError;                   // Temporal difference error
    
    // Time constants for different processes
    static constexpr float PHASIC_DECAY_TIME = 100.0f;     // ms - phasic component decay
    static constexpr float TONIC_DECAY_TIME = 1000.0f;    // ms - tonic component decay
    static constexpr float ELIGIBILITY_DECAY = 50.0f;     // ms - eligibility trace decay
    static constexpr float NOISE_AMPLITUDE = 0.05f;        // Baseline noise
    
    // Dopamine dynamics parameters
    static constexpr float PE_WEIGHT = 0.8f;               // Weight on prediction error
    static constexpr float BASAL_REWARD = 0.01f;          // Small basal reward
    static constexpr float MAX_DOPAMINE = 2.0f;           // Maximum dopamine level
    static constexpr float MIN_DOPAMINE = -0.1f;          // Minimum dopamine level (inhibition)
    
    Impl() : 
        phasicLevel(0.0f),
        tonicLevel(BASAL_REWARD),
        predictionError(0.0f),
        eligibilityTrace(0.0f),
        tdError(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Return combined tonic + phasic dopamine level
    return pImpl->tonicLevel + pImpl->phasicLevel;
}

void Dopamine::setLevel(float level) {
    // Set both tonic and phasic components
    pImpl->tonicLevel = std::clamp(level * 0.7f, BASAL_REWARD, MAX_DOPAMINE * 0.7f);
    pImpl->phasicLevel = std::clamp(level * 0.3f, MIN_DOPAMINE, MAX_DOPAMINE * 0.3f);
}

float Dopamine::getPlasticityFactor() const {
    // PHASE 2 FIX: Implement real dopamine-modulated plasticity factor
    // Use temporal difference learning theory: dopamine affects plasticity
    // based on prediction error magnitude and sign
    
    float plasticityFactor = 1.0f;
    
    // Strong prediction errors significantly increase plasticity
    if (std::abs(pImpl->predictionError) > 0.1f) {
        plasticityFactor *= (1.0f + std::abs(pImpl->predictionError) * 2.0f);
    }
    
    // Current dopamine level modulates learning rate
    float currentDA = getLevel();
    if (currentDA > 0.5f) {
        // High dopamine facilitates learning
        plasticityFactor *= 1.0f + (currentDA - 0.5f) * 1.5f;
    } else if (currentDA < Eval("string") {
        float responseRate = static_cast<float>(rand() / (RAND_MAX)) - 0.5f;
        plasticityFactor *= responseRate * 2.0f;
    }
    
    // Cap plasticity factor to reasonable range
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 5.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2 FIX: Implement real dopamine dynamics
    // Uses temporal difference learning with proper time constants
    
    float timeFactor = static_cast<float>(dt) / 0.001f;  // Convert to ms
    
    // Decay phasic component towards baseline
    if (pImpl->phasicLevel > 0.0f) {
        float decayRate = 1.0f - std::exp(-timeFactor / Impl::PHASIC_DECAY_TIME);
        pImpl->phasicLevel *= (1.0f - decayRate * 0.1f);
    }
    
    // Decay eligibility trace
    float eligibilityDecay = 1.0f - std::exp(-timeFactor / Impl::ELIGIBILITY_DECAY);
    pImpl->eligibilityTrace *= (1.0f - eligibilityDecay);
    
    // Add baseline noise for biological realism
    pImpl->phasicLevel += (rand() / (float)RAND_MAX - 0.5f) * Impl::NOISE_AMPLITUDE * timeFactor;
    
    // Clamp to reasonable bounds
    pImpl->phasicLevel = std::clamp(pImpl->phasicLevel, Impl::MIN_DOPAMINE, Impl::MAX_DOPAMINE * 0.3f);
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, BASAL_REWARD, Impl::MAX_DOPAMINE * 0.7f);
}

void Dopamine::signalReward(float reward) {
    // PHASE 2 FIX: Implement real reward signaling
    // Uses reward prediction error (RPE) for learning
    
    // Compute reward prediction error: RPE = reward + gamma * next_value - current_value
    // For now, use simplified RPE: RPE = reward - expected_reward
    // Expected reward is modeled as a weighted sum of recent rewards
    
    float expectedReward = 0.0f;
    if (!pImpl->predictionErrorHistory.empty()) {
        float sum = std::accumulate(pImpl->predictionErrorHistory.begin(), 
                                   pImpl->predictionErrorHistory.end(), 0.0f);
        expectedReward = sum / pImpl->predictionErrorHistory.size();
    }
    
    pImpl->predictionError = reward - expectedReward;
    
    // Store RPE in history for future use
    pImpl->predictionErrorHistory.push_back(pImpl->predictionError);
    if (pImpl->predictionErrorHistory.size() > 10) {  // Keep only recent history
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update dopamine level based on RPE
    // Phasic dopamine responds to positive RPE (surprise/reward prediction error)
    // Negative RPE reduces dopamine (prediction error punishment)
    if (pImpl->predictionError > 0.0f) {
        // Positive RPE causes dopamine burst (learning signal)
        pImpl->phasicLevel = std::min(Impl::MAX_DOPAMINE * 0.3f, 
                                     pImpl->phasicLevel + pImpl->predictionError * 0.5f);
    } else {
        // Negative RPE causes dopamine dip (error correction)
        pImpl->phasicLevel = std::max(Impl::MIN_DOPAMINE, 
                                     pImpl->phasicLevel + pImpl->predictionError * 0.3f);
    }
    
    // Update eligibility trace for credit assignment
    pImpl->eligibilityTrace += std::abs(pImpl->predictionError) * 0.1f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2 FIX: Implement reward prediction error signaling
    // Direct RPE signaling for temporal difference learning
    
    // Store the prediction error
    pImpl->predictionError = error;
    pImpl->predictionErrorHistory.push_back(error);
    if (pImpl->predictionErrorHistory.size() > 10) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update dopamine level based on RPE magnitude
    float magnitude = std::abs(error);
    if (error > 0.0f) {
        // Positive RPE: increase dopamine phasic response
        pImpl->phasicLevel = std::min(Impl::MAX_DOPAMINE * 0.3f,
                                     pImpl->phasicLevel + magnitude * 0.4f);
    } else {
        // Negative RPE: decrease dopamine phasic response
        pImpl->phasicLevel = std::max(Impl::MIN_DOPAMINE,
                                     pImpl->phasicLevel + magnitude * 0.2f);
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace += magnitude * 0.15f;
}

} // namespace nlm