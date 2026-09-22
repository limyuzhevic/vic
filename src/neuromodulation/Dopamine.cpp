#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    // State variables
    float level;
    float tonicLevel;
    float phasicResponse;
    float baseline;
    
    // Dynamics parameters
    float decayRate;
    float releaseRate;
    float temporalFilter;  // Time constant for filtering
    float phasicGain;      // Sensitivity to rewards/prediction errors
    float attentionCoupling;
    float attentionWeight;
    float memoryIntegration;
    
    // Historical data
    std::vector<float> recentActivity;
    std::vector<float> predictionHistory;
    std::vector<std::string> contextHistory;
    
    // Timing for phasic responses
    Timestamp lastPhasicTime;
    float timeSinceLastPhasic;
    
    // Novelty detection integration
    float noveltySensitivity;
    
    // Random number generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> rewardDist;
    
    Impl() 
        : level(0.0f), tonicLevel(0.5f), phasicResponse(0.0f), baseline(0.2f)
        , decayRate(0.05f), releaseRate(1.0f), temporalFilter(0.5f)
        , phasicGain(1.0f), attentionCoupling(0.3f), attentionWeight(0.5f)
        , memoryIntegration(0.4f), noveltySensitivity(0.2f)
        , lastPhasicTime(0.0f), timeSinceLastPhasic(0.0f)
    {
        std::random_device rd;
        rng.seed(rd());
        rewardDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        recentActivity.reserve(100);
        predictionHistory.reserve(100);
        contextHistory.reserve(100);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized with real dynamics");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);  // Allow levels above 1.0 for burst responses
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: 
    // High dopamine potentiates LTP, low dopamine facilitates LTD
    // Nonlinear relationship based on dopaminergic signaling
    float normalized = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Bell-shaped curve: moderate dopamine maximizes plasticity
    // With increased sensitivity at high levels for learning
    float peakPlasticity = 0.8f;  // Optimal dopamine for plasticity
    float width = 0.6f;           // Width of plasticity window
    
    float factor = std::exp(-std::pow(normalized - peakPlasticity, 2) / (2 * width * width));
    
    // Add baseline plasticity
    factor = 0.2f + 0.8f * factor;
    
    // Modulate by novelty sensitivity
    factor *= (1.0f + pImpl->noveltySensitivity * 0.5f);
    
    return factor;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Decay toward baseline
    float decay = pImpl->decayRate * timeStep;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update tonic component
    pImpl->tonicLevel = std::max(pImpl->baseline, pImpl->tonicLevel - decay * 0.5f);
    
    // Update time since last phasic response
    pImpl->timeSinceLastPhasic += timeStep;
    
    // Maintain activity history
    pImpl->recentActivity.push_back(pImpl->level);
    if (pImpl->recentActivity.size() > 100) {
        pImpl->recentActivity.erase(pImpl->recentActivity.begin());
    }
    
    // Modulate neural excitability based on attention coupling
    float attentionMod = 1.0f + pImpl->attentionCoupling * pImpl->attentionWeight;
    
    // Apply effects on neural circuits (to be integrated in Brain.cpp)
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic burst proportional to reward magnitude
    float phasicBurst = reward * pImpl->phasicGain * pImpl->releaseRate;
    
    // Add temporal filtering for realistic dynamics
    if (pImpl->timeSinceLastPhasic > 0.1f) {  // Minimum interval between bursts
        pImpl->level += phasicBurst;
        pImpl->level = std::min(pImpl->level, 2.0f);  // Cap at 2.0 for burst
        
        pImpl->phasicResponse = phasicBurst;
        pImpl->lastPhasicTime = pImpl->timeSinceLastPhasic;
        pImpl->timeSinceLastPhasic = 0.0f;
        
        // Update context history
        pImpl->contextHistory.push_back("reward");
        if (pImpl->contextHistory.size() > 50) {
            pImpl->contextHistory.erase(pImpl->contextHistory.begin());
        }
        
        NLM_LOG_DEBUG("Dopamine burst: reward={} magnitude={}", reward, phasicBurst);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: positive for better-than-expected, negative for worse
    float errorResponse = error * pImpl->phasicGain * pImpl->releaseRate;
    
    // D1-like neurons respond to positive prediction errors (better than expected)
    if (error > 0.0f) {
        pImpl->level += errorResponse * 1.5f;  // Enhanced response to positive errors
        pImpl->phasicResponse = errorResponse * 1.5f;
        
        // Update prediction history
        pImpl->predictionHistory.push_back(error);
        if (pImpl->predictionHistory.size() > 50) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
        
        NLM_LOG_DEBUG("Dopamine RPE (positive): error={} response={}", error, errorResponse);
    }
    // D2-like neurons respond to negative prediction errors (worse than expected)
    else if (error < 0.0f) {
        pImpl->level += errorResponse * 0.8f;  // Smaller response to negative errors
        pImpl->phasicResponse = errorResponse * 0.8f;
        
        NLM_LOG_DEBUG("Dopamine RPE (negative): error={} response={}", error, errorResponse);
    }
    
    pImpl->level = std::max(0.0f, pImpl->level);
    pImpl->timeSinceLastPhasic = 0.0f;
}

void Dopamine::processRewardContext(const std::string& context) {
    // Process contextual information for prediction
    pImpl->contextHistory.push_back(context);
    if (pImpl->contextHistory.size() > 50) {
        pImpl->contextHistory.erase(pImpl->contextHistory.begin());
    }
}

const std::vector<float>& Dopamine::getRecentActivity() const {
    return pImpl->recentActivity;
}

float Dopamine::getPhasicResponse() const {
    return pImpl->phasicResponse;
}

float Dopamine::getTonicLevel() const {
    return pImpl->tonicLevel;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = gain;
}

void Dopamine::setTemporalFilter(float tau) {
    pImpl->temporalFilter = tau;
}

void Dopamine::setAttentionCoupling(float coupling) {
    pImpl->attentionCoupling = coupling;
}

void Dopamine::updateAttentionWeight(float weight) {
    pImpl->attentionWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Dopamine::updateAttention(float currentLevel, float &attentionWeight) {
    // Dopamine modulates attentional selectivity and focus
    // Higher dopamine enhances attention to rewarding stimuli
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    
    // Dopamine increases attentional gain for relevant stimuli
    // Stronger dopamine leads to greater focus and reduced distractibility
    attentionWeight = 0.5f + normalized * pImpl->attentionCoupling;
    attentionWeight = std::clamp(attentionWeight, 0.1f, 1.0f);
    
    // At very high levels, dopamine can cause attentional deficits (hyperfocus)
    if (normalized > 1.5f) {
        attentionWeight *= 0.8f;  // Reduced breadth of attention
    }
}

void Dopamine::updateWorkingMemory(float currentLevel, float &memoryStability) {
    // Dopamine modulates working memory through D1 and D2 receptor effects
    float normalized = std::clamp(currentLevel, 0.0f, 2.0f);
    
    // D1 receptor activation (moderate-high levels) enhances working memory maintenance
    if (normalized >= 0.5f && normalized < 1.5f) {
        memoryStability = 0.6f + (normalized - 0.5f) * 0.3f;
        memoryStability = std::min(memoryStability, 1.0f);
    }
    // Low dopamine (below 0.3) impairs working memory
    else if (normalized < 0.3f) {
        memoryStability = 0.3f + normalized * 0.3f;
    }
    // Very high dopamine can impair working memory (D2-like effects)
    else if (normalized >= 1.5f) {
        memoryStability = 0.9f - (normalized - 1.5f) * 0.2f;
    }
}

void Dopamine::updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) {
    // Dopamine plays a critical role in developmental plasticity and circuit refinement
    float normalized = std::clamp(currentLevel, 0.0f, 2.0f);
    
    // During development, dopamine promotes synaptic pruning and circuit maturation
    developmentalFactor = 0.5f + normalized * 0.3f;
    
    // High dopamine enhances activity-dependent plasticity for circuit refinement
    if (normalized > 1.0f) {
        developmentalFactor = std::min(developmentalFactor, 1.0f);
    }
    
    // Modulate by novelty sensitivity for developmental exploration
    developmentalFactor *= (1.0f + pImpl->noveltySensitivity);
}

} // namespace nlm