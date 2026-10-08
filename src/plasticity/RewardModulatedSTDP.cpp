#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;      // Base LTP weight
    float baseLTDWeight;      // Base LTD weight
    float timeConstant;       // STDP time constant (ms)
    float learningRate;       // Learning rate for reward modulation
    float modulationStrength; // How strongly reward affects plasticity
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    float eligibilityTrace;   // Current eligibility trace
    float rewardHistory[10];  // Recent reward values for prediction
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.012f), timeConstant(20.0f),
             learningRate(0.001f), modulationStrength(1.0f),
             minWeight(-1.0f), maxWeight(1.0f), eligibilityTrace(0.0f) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl), currentPredictionError_(0.0f) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // For each pre-post spike pair, compute STDP-like weight change
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: potential potentiation
                float delta = pImpl->baseLTPWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: potential depression
                float delta = -pImpl->baseLTDWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply reward modulation if we have a prediction error
    if (std::abs(currentPredictionError_) > 1e-6f) {
        float modulation = pImpl->learningRate * currentPredictionError_ * pImpl->modulationStrength;
        totalDelta *= (1.0f + modulation);
        
        // Update eligibility trace
        pImpl->eligibilityTrace += totalDelta;
        
        // Update recent reward history
        for (int i = 9; i > 0; --i) {
            pImpl->rewardHistory[i] = pImpl->rewardHistory[i-1];
        }
        pImpl->rewardHistory[0] = currentPredictionError_;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setModulationStrength(float strength) {
    pImpl->modulationStrength = std::clamp(strength, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getModulationStrength() const {
    return pImpl->modulationStrength;
}

void RewardModulatedSTDP::applyReward(float reward, Timestamp time) {
    // Simple reward prediction: current reward is prediction error
    currentPredictionError_ = reward;
}

float RewardModulatedSTDP::getCurrentPredictionError() const {
    return currentPredictionError_;
}

} // namespace nlm