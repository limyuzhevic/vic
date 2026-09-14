#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics
    float level;                    // Current dopamine concentration (0-1)
    float baseline;                 // Baseline level
    float peak;                     // Peak achievable level
    float decayRate;                // Decay rate
    float releaseRate;              // Release rate when signaling
    float plasticityGain;           // How much dopamine affects plasticity
    float attentionGain;            // How much dopamine affects attention
    float memoryGain;               // How much dopamine affects memory
    
    // History and tracking
    float lastReward;               // Last reward received
    float predictionErrorHistory[5]; // Rolling prediction error history
    size_t historyIndex;            // Index in history array
    
    // State tracking
    bool isBursting;                // True during dopamine bursts
    TimestepDuration burstDuration;  // How long burst lasts
    TimestepDuration timeSinceBurst;
    
    // Integration state
    class Brain* brain;             // Brain reference for effects
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(2.0f)
        , plasticityGain(0.5f)
        , attentionGain(0.3f)
        , memoryGain(0.2f)
        , lastReward(0.0f)
        , historyIndex(0)
        , isBursting(false)
        , burstDuration(100.0f)
        , timeSinceBurst(0.0f)
        , brain(nullptr) {
        
        // Initialize prediction error history
        for (int i = 0; i < 5; ++i) {
            predictionErrorHistory[i] = 0.0f;
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
    // Dopamine modulates STDP and Hebbian plasticity
    // Higher dopamine increases learning rate but with inverted-U relationship
    float normalizedLevel = std::min(2.0f, pImpl->level * 2.0f);  // Scale to 0-2
    return pImpl->plasticityGain * (1.0f - std::abs(normalizedLevel - 1.0f));
}

float Dopamine::getExcitabilityFactor() const {
    // Dopamine affects neural excitability
    return 1.0f + pImpl->attentionGain * pImpl->level;
}

float Dopamine::getAttentionFactor() const {
    // Dopamine enhances attentional focus
    return 1.0f + pImpl->attentionGain * pImpl->level;
}

float Dopamine::getMemoryConsolidationFactor() const {
    // Dopamine aids memory consolidation
    return 1.0f + pImpl->memoryGain * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine dynamics
    pImpl->timeSinceBurst += dt;
    
    if (pImpl->isBursting) {
        // Decay during burst
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        
        if (pImpl->timeSinceBurst >= pImpl->burstDuration) {
            // End burst
            pImpl->isBursting = false;
            pImpl->timeSinceBurst = 0.0f;
        }
    } else {
        // Natural decay towards baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
}

void Dopamine::signalReward(float reward) {
    // Signal reward with dopamine burst
    pImpl->lastReward = reward;
    
    // Calculate burst magnitude based on reward
    float burstMagnitude = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Start dopamine burst
    pImpl->isBursting = true;
    pImpl->timeSinceBurst = 0.0f;
    
    // Set level to peak burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Update prediction error history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = reward - pImpl->lastReward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
    
    NLM_LOG_INFO("Dopamine burst: reward = " + std::to_string(reward) + ", level = " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Signal reward prediction error (positive for better than expected, negative for worse)
    pImpl->lastReward = error;
    
    // Calculate burst magnitude based on prediction error
    float burstMagnitude = std::min(1.0f, std::abs(error) * pImpl->releaseRate);
    
    // Start dopamine burst (inverted: positive error = decrease, negative error = increase)
    pImpl->isBursting = true;
    pImpl->timeSinceBurst = 0.0f;
    
    // Set level based on prediction error sign
    if (error > 0.0f) {
        // Positive prediction error (better than expected) - decrease dopamine
        pImpl->level = std::max(0.0f, pImpl->level - burstMagnitude);
    } else {
        // Negative prediction error (worse than expected) - increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    }
    
    // Update prediction error history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
    
    NLM_LOG_INFO("Dopamine RPE: error = " + std::to_string(error) + ", level = " + std::to_string(pImpl->level));
}

void Dopamine::enhanceSTDP(class STDP* stdp) {
    // Enhance STDP with dopamine modulation
    if (stdp && pImpl->level > 0.0f) {
        float modFactor = getPlasticityFactor();
        
        // Adjust STDP parameters based on dopamine level
        float ltpWeight = 0.01f * modFactor;
        float ltdWeight = 0.012f * modFactor;
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
        
        // Higher dopamine also affects time constant
        float originalTau = stdp->getTau();
        float tauMod = 1.0f + 0.5f * pImpl->level;
        stdp->setTau(originalTau * tauMod);
        
        NLM_LOG_INFO("Dopamine enhanced STDP: LTP=" + std::to_string(ltpWeight) + 
                    ", LTD=" + std::to_string(ltdWeight) + ", tau factor=" + std::to_string(tauMod));
    }
}

void Dopamine::modulateCuriosity(class Curiosity* curiosity) {
    // Modulate curiosity drive based on dopamine level
    if (curiosity) {
        // Dopamine and curiosity have complex relationship
        // Moderate dopamine increases curiosity, very high or very low decreases it
        float curiosityMod = 1.0f;
        if (pImpl->level < 0.3f) {
            // Low dopamine reduces curiosity (low motivation)
            curiosityMod = 0.5f + 0.5f * (pImpl->level / 0.3f);
        } else if (pImpl->level > 0.7f) {
            // High dopamine may reduce curiosity (satiated)
            curiosityMod = 1.0f - 0.5f * ((pImpl->level - 0.7f) / 0.3f);
        } else {
            // Moderate dopamine increases curiosity
            curiosityMod = 1.0f + 0.5f * ((pImpl->level - 0.3f) / 0.4f);
        }
        
        curiosity->setNoveltyWeight(curiosity->getNoveltyWeight() * curiosityMod);
        curiosity->setPredictionErrorWeight(curiosity->getPredictionErrorWeight() * curiosityMod);
        
        NLM_LOG_INFO("Dopamine modulated curiosity: factor = " + std::to_string(curiosityMod));
    }
}

void Dopamine::resetWithReward(float rewardAmount) {
    // Reset dopamine state with a reward burst
    pImpl->level = std::min(pImpl->peak, rewardAmount * pImpl->releaseRate);
    pImpl->lastReward = rewardAmount;
    pImpl->isBursting = true;
    pImpl->timeSinceBurst = 0.0f;
    
    NLM_LOG_INFO("Dopamine reset with reward: amount = " + std::to_string(rewardAmount) + ", level = " + std::to_string(pImpl->level));
}

} // namespace nlm