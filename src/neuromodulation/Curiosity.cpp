#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    // Intrinsic motivation components
    float intrinsicMotivation;
    float predictionErrorMagnitude;
    float noveltySurprise;
    
    // Temporal dynamics
    float explorationDrive;
    float timeSinceLastExploration;
    
    // Learning parameters
    float learningRate;
    float explorationRate;
    float uncertaintyWeight;
    
    // Reward prediction for curiosity
    float expectedReward;
    float actualReward;
    
    // Novelty prediction model
    std::vector<float> noveltyPrediction;
    std::vector<float> sensoryHistory;
    
    // Curiosity-specific dynamics
    float burstPhase;
    float decayPhase;
    float integrationTimeConstant;
    float predictionConfidence;
    
    // Exploration-exploitation balance
    float explorationThreshold;
    float exploitationConfidence;
    
    // Time constants for temporal dynamics
    float burstTimeConstant;
    float decayTimeConstant;
    float recoveryTimeConstant;
    
    Impl() : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , intrinsicMotivation(0.0f)
        , predictionErrorMagnitude(0.0f)
        , noveltySurprise(0.0f)
        , explorationDrive(0.0f)
        , timeSinceLastExploration(0.0f)
        , learningRate(0.01f)
        , explorationRate(0.1f)
        , uncertaintyWeight(0.5f)
        , expectedReward(0.0f)
        , actualReward(0.0f)
        , burstPhase(0.0f)
        , decayPhase(0.0f)
        , integrationTimeConstant(1.0f)
        , predictionConfidence(0.5f)
        , explorationThreshold(0.5f)
        , exploitationConfidence(0.5f)
        , burstTimeConstant(0.1f)
        , decayTimeConstant(1.0f)
        , recoveryTimeConstant(2.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float floatDt = static_cast<float>(dt);
    
    // Store previous values for temporal dynamics
    float previousLevel = pImpl->level;
    pImpl->burstPhase = (pImpl->level > 0.5f) ? 1.0f : 0.0f;
    pImpl->decayPhase = (pImpl->level < 0.2f) ? 1.0f : 0.0f;
    
    // Compute intrinsic motivation components with biological realism
    // Prediction error magnitude for intrinsic motivation
    pImpl->predictionErrorMagnitude = std::abs(predictionError);
    
    // Novelty surprise - bell-shaped function based on prediction uncertainty
    pImpl->noveltySurprise = novelty * (1.0f - novelty) * 2.0f * pImpl->uncertaintyWeight;
    
    // Novelty prediction error based on forward model
    float noveltyPredictionError = 0.0f;
    if (!pImpl->noveltyPrediction.empty()) {
        noveltyPredictionError = std::abs(novelty - pImpl->noveltyPrediction.back());
    }
    
    // Intrinsic motivation = weighted combination of prediction error and novelty
    float intrinsicTarget = pImpl->predictionErrorMagnitude * pImpl->predictionErrorWeight +
                            pImpl->noveltySurprise * pImpl->noveltyWeight +
                            noveltyPredictionError * 0.3f;
    
    // Smooth integration towards target with time constant
    float integrationRate = static_cast<float>(1.0) / pImpl->integrationTimeConstant;
    pImpl->intrinsicMotivation += (intrinsicTarget - pImpl->intrinsicMotivation) * integrationRate * floatDt;
    
    // Current curiosity level as weighted combination of inputs with burst dynamics
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * predictionError;
    
    // Burst phase for high prediction errors (phasic curiosity response)
    if (std::abs(predictionError) > 0.5f) {
        float burst = predictionErrorComponent * pImpl->predictionErrorWeight * 2.0f;
        pImpl->burstPhase = 1.0f - std::exp(-floatDt / pImpl->burstTimeConstant);
        pImpl->level += burst * pImpl->burstPhase;
    }
    
    // Decay phase for low levels (tonic curiosity baseline)
    if (pImpl->level < 0.1f) {
        pImpl->decayPhase = 1.0f - std::exp(-floatDt / pImpl->decayTimeConstant);
        pImpl->level -= 0.1f * pImpl->decayPhase * floatDt;
    }
    
    // Recovery phase from baseline
    if (pImpl->level < 0.1f) {
        float recovery = (0.1f - pImpl->level) * integrationRate * floatDt;
        pImpl->level += recovery;
    }
    
    // Smooth update towards target with exploration weighting
    pImpl->level += (pImpl->intrinsicMotivation - pImpl->level) * 0.05f * floatDt;
    
    // Apply exploration drive dynamics with threshold
    pImpl->explorationDrive = std::max(0.0f, pImpl->level - pImpl->explorationThreshold) * pImpl->explorationRate;
    
    // Time since last exploration increases
    pImpl->timeSinceLastExploration += floatDt;
    
    // Curiosity reward prediction integration with temporal difference
    if (pImpl->predictionErrorMagnitude > 0.0f) {
        // Update expected reward using temporal difference learning
        float rewardDelta = predictionError - pImpl->expectedReward;
        pImpl->expectedReward += pImpl->learningRate * rewardDelta * floatDt;
        
        // Increase exploration drive when prediction error exceeds threshold
        if (pImpl->predictionErrorMagnitude > 0.3f) {
            pImpl->explorationDrive *= (1.0f + pImpl->predictionErrorMagnitude * 0.8f);
        }
        
        // Boost prediction confidence for novel inputs
        pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + 0.1f * floatDt);
    }
    
    // Update novelty prediction model
    pImpl->noveltyPrediction.push_back(novelty);
    if (pImpl->noveltyPrediction.size() > 10) {
        pImpl->noveltyPrediction.erase(pImpl->noveltyPrediction.begin());
    }
    
    // Decay curiosity level with exponential decay
    pImpl->level = std::max(0.0f, pImpl->level * std::exp(-pImpl->decayRate * floatDt));
    
    // Decay intrinsic motivation
    pImpl->intrinsicMotivation = std::max(0.0f, pImpl->intrinsicMotivation - pImpl->decayRate * 0.3f * floatDt);
    
    // Exploration-exploitation balance update
    pImpl->exploitationConfidence = std::max(0.0f, pImpl->exploitationConfidence - 0.05f * floatDt);
    if (pImpl->explorationDrive > 0.7f) {
        pImpl->exploitationConfidence = std::min(1.0f, pImpl->exploitationConfidence + 0.1f * floatDt);
    }
    
    // Update history for statistical analysis
    pImpl->sensoryHistory.push_back(novelty);
    if (pImpl->sensoryHistory.size() > 100) {
        pImpl->sensoryHistory.erase(pImpl->sensoryHistory.begin());
    }
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(1.0f, weight));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(1.0f, weight));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->intrinsicMotivation = 0.0f;
    pImpl->explorationDrive = 0.0f;
    pImpl->predictionErrorMagnitude = 0.0f;
    pImpl->noveltySurprise = 0.0f;
    pImpl->timeSinceLastExploration = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->actualReward = 0.0f;
    pImpl->noveltyPrediction.clear();
    pImpl->sensoryHistory.clear();
    pImpl->burstPhase = 0.0f;
    pImpl->decayPhase = 0.0f;
    pImpl->predictionConfidence = 0.5f;
    pImpl->explorationThreshold = 0.5f;
    pImpl->exploitationConfidence = 0.5f;
}

} // namespace nlm
