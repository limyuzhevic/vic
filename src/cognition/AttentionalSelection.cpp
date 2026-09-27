#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    float attentionLevel;
    float attentionThreshold;
    float noveltyWeight;
    float predictionErrorWeight;
    float timePressureWeight;
    float restingThreshold;
    float decayRate;
    float timeInResting;
    float accumulatedStress;
    
    Impl() 
        : brain(nullptr)
        , attentionLevel(0.0f)
        , attentionThreshold(0.5f)
        , noveltyWeight(0.3f)
        , predictionErrorWeight(0.3f)
        , timePressureWeight(0.2f)
        , restingThreshold(0.3f)
        , decayRate(0.05f)
        , timeInResting(0.0f)
        , accumulatedStress(0.0f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

float AttentionalSelection::evaluateEvent(const std::vector<float>& neuralActivations,
                                        float novelty,
                                        float predictionError,
                                        float timeSinceLastEpisode,
                                        bool isResting) {
    if (neuralActivations.empty()) return 0.0f;
    
    // Calculate neural activation salience
    float totalActivation = std::accumulate(neuralActivations.begin(), 
                                           neuralActivations.end(), 0.0f);
    float avgActivation = totalActivation / neuralActivations.size();
    float activationSalience = std::min(1.0f, avgActivation * 2.0f);
    
    // Calculate attention from various sources
    float attention = 0.0f;
    
    // Novelty contribution
    if (novelty > 0.0f) {
        attention += pImpl->noveltyWeight * novelty;
    }
    
    // Prediction error contribution (important learning signal)
    if (predictionError > 0.0f) {
        attention += pImpl->predictionErrorWeight * predictionError;
    }
    
    // Time pressure (urgency of storing experience)
    float timePressure = std::min(1.0f, timeSinceLastEpisode / 100.0f);
    attention += pImpl->timePressureWeight * timePressure;
    
    // Neural salience contribution
    attention += 0.3f * activationSalience;  // Fixed weight for activation salience
    
    // Adjust during resting state
    if (isResting) {
        // During rest, prioritize consolidation of important memories
        float restWeight = std::max(0.5f, 1.0f - pImpl->restingThreshold);
        attention *= restWeight;
        
        // Increase attention to prediction errors during rest (important for consolidation)
        if (predictionError > 0.0f) {
            attention += 0.2f * predictionError;
        }
    }
    
    // Apply decay and smooth update
    pImpl->attentionLevel += (attention - pImpl->attentionLevel) * 0.2f;
    pImpl->attentionLevel = std::max(0.0f, 
        pImpl->attentionLevel - pImpl->decayRate * 0.01f);  // Small decay
    
    return pImpl->attentionLevel;
}

void AttentionalSelection::update(float curiosity,
                                 float predictionError,
                                 float novelty,
                                 TimestepDuration dt) {
    // Update attention level based on neuromodulation signals
    float targetAttention = pImpl->attentionThreshold;
    
    // Curiosity increases attention (exploration drive)
    if (curiosity > 0.0f) {
        targetAttention += curiosity * 0.3f;
    }
    
    // Prediction error should increase attention (learning signal)
    if (predictionError > 0.0f) {
        targetAttention += predictionError * 0.4f;
    }
    
    // Novelty increases attention (new information)
    if (novelty > 0.0f) {
        targetAttention += novelty * 0.2f;
    }
    
    // Smooth the update
    pImpl->attentionLevel += (targetAttention - pImpl->attentionLevel) * 0.1f * dt;
    
    // Add stress during prolonged activation
    float activationStress = std::min(1.0f, pImpl->attentionLevel / pImpl->attentionThreshold);
    pImpl->accumulatedStress += activationStress * 0.001f * dt;
    
    // Cap accumulated stress
    pImpl->accumulatedStress = std::min(1.0f, pImpl->accumulatedStress);
}

float AttentionalSelection::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void AttentionalSelection::setAttentionThreshold(float threshold) {
    pImpl->attentionThreshold = std::max(0.0f, std::min(1.0f, threshold));
}

void AttentionalSelection::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void AttentionalSelection::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void AttentionalSelection::setTimePressureWeight(float weight) {
    pImpl->timePressureWeight = weight;
}

void AttentionalSelection::setRestingThreshold(float threshold) {
    pImpl->restingThreshold = std::max(0.0f, std::min(1.0f, threshold));
}

bool AttentionalSelection::shouldEnterResting(const std::vector<float>& neuralActivations,
                                            float averageFiringRate,
                                            float timeSinceLastEpisode) {
    // High stress and low activation can trigger rest
    bool highStress = pImpl->accumulatedStress > pImpl->restingThreshold;
    bool lowActivation = averageFiringRate < 0.1f;  // Below 10% of max
    
    // Enter rest if stressed and not too active
    bool enterRest = highStress && lowActivation;
    
    // Also enter rest if it's been a long time since last episode and we're accumulating stress
    if (timeSinceLastEpisode > 500.0f && pImpl->accumulatedStress > 0.5f) {
        enterRest = true;
    }
    
    // Reset stress when entering rest
    if (enterRest) {
        pImpl->timeInResting = 0.0f;
    }
    
    return enterRest;
}

bool AttentionalSelection::shouldExitResting(float timeInResting,
                                            float accumulatedStress,
                                            float averageFiringRate) {
    // Exit rest if we have been resting long enough
    bool timeReady = timeInResting > 100.0f;  // Rest for at least 100 steps
    
    // Exit rest if stress has been released
    bool stressReleased = accumulatedStress < 0.1f;
    
    // Exit rest if activation is high enough
    bool activationHigh = averageFiringRate > 0.3f;  // Above 30% of max
    
    return timeReady && (stressReleased || activationHigh);
}

void AttentionalSelection::reset() {
    pImpl->attentionLevel = 0.0f;
    pImpl->timeInResting = 0.0f;
    pImpl->accumulatedStress = 0.0f;
}

} // namespace nlm
