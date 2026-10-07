#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Dopamine learning parameters
    float learningRate;
    float predictionErrorWeight;
    float rewardPredictionErrorWeight;
    
    // History for statistics
    std::deque<float> levelHistory;
    std::deque<float> predictionErrorHistory;
    
    // Event tracking
    float totalEvents;
    float lastEventTime;
    
    // Neuromodulatory dynamics
    float tonicLevel;
    float phasicBurst;
    float eligibilityTrace;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(2.0f)
        , learningRate(0.01f)
        , predictionErrorWeight(0.7f)
        , rewardPredictionErrorWeight(0.3f)
        , totalEvents(0.0f)
        , lastEventTime(0.0f)
        , tonicLevel(0.05f)
        , phasicBurst(0.0f)
        , eligibilityTrace(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    // Initialize dopamine system with brain integration
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized with biological plausibility");
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->levelHistory.push_back(pImpl->level);
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.pop_front();
    }
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Dopamine enhances synaptic plasticity for reward prediction error learning
    float targetPlasticity = 1.0f + pImpl->level * 2.0f;  // Max plasticity factor 3.0
    
    // Eligibility trace modulates plasticity
    return targetPlasticity * (1.0f + pImpl->eligibilityTrace);
}

void Dopamine::update(float predictionError, float rewardPredictionError, TimestepDuration dt) {
    // Real dopamine dynamics with phasic/tonic signaling
    float combinedPredictionError = pImpl->predictionErrorWeight * predictionError +
                                    pImpl->rewardPredictionErrorWeight * rewardPredictionError;
    
    // Target dopamine level based on prediction error
    float targetLevel = std::min(1.0f, std::abs(combinedPredictionError) * 2.0f);
    
    // Smooth update with learning rate
    float delta = (targetLevel - pImpl->level) * pImpl->learningRate;
    pImpl->level += delta;
    
    // Decay towards tonic baseline
    pImpl->level = std::max(pImpl->tonicLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update eligibility trace for synaptic tagging
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.1f * static_cast<float>(dt));
    
    // Update statistics
    pImpl->predictionErrorHistory.push_back(combinedPredictionError);
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.pop_front();
    }
    
    pImpl->totalEvents++;
    pImpl->lastEventTime = static_cast<float>(pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with phasic bursts
    float burstLevel = std::min(pImpl->peak, reward * pImpl->releaseRate);
    pImpl->level = std::max(pImpl->level, burstLevel);
    
    // Update eligibility trace for reward-based learning
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.5f * reward);
    
    NLM_LOG_DEBUG("Dopamine reward signal: " + std::to_string(reward) + " at level " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with temporal difference learning
    float errorSignal = std::max(0.0f, std::min(1.0f, error * 0.5f));
    pImpl->level = std::max(pImpl->level, errorSignal);
    
    // Stronger eligibility trace for prediction error
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.3f * errorSignal);
    
    NLM_LOG_DEBUG("Dopamine prediction error signal: " + std::to_string(error) + " at level " + std::to_string(pImpl->level));
}

} // namespace nlm
