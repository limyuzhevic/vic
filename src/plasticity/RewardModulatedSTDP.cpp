#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float learningRate;          // Base learning rate (η)
    float traceDecayRate;        // Eligibility trace decay (λ)
    float predictionError;       // TD prediction error (δ)
    float creditAssignmentStrength; // Reward signal integration strength
    float eligibilityTrace;      // Current eligibility trace
    float baselineActivity;      // Baseline firing rate for normalization
    float maxTraceValue;         // Maximum eligibility trace value
    
    Impl() : learningRate(0.01f), traceDecayRate(0.95f), predictionError(0.0f),
             creditAssignmentStrength(1.0f), eligibilityTrace(0.0f), baselineActivity(0.1f),
             maxTraceValue(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse) return;
    
    // Compute standard STDP contribution
    float stdpDelta = 0.0f;
    float tau = 20.0f;  // STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            if (deltaT > 0) {
                // Pre before post: potentiation
                float delta = pImpl->learningRate * std::exp(-deltaT / tau);
                stdpDelta += delta;
            } else if (deltaT < 0) {
                // Post before pre: depression
                float delta = -pImpl->learningRate * std::exp(deltaT / tau);
                stdpDelta += delta;
            }
        }
    }
    
    // Apply eligibility trace: Δw = δ * E(t) * stdpWeight
    if (pImpl->eligibilityTrace > 1e-6f && pImpl->predictionError != 0.0f) {
        float rewardWeightedDelta = pImpl->predictionError * pImpl->eligibilityTrace *
                                   pImpl->creditAssignmentStrength;
        stdpDelta += rewardWeightedDelta;
    }
    
    // Apply weight change with bounds
    if (std::abs(stdpDelta) > 1e-6f) {
        applyWeightChange(synapse, stdpDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Update eligibility trace based on weight change
    pImpl->eligibilityTrace += delta;
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, 
                                        -pImpl->maxTraceValue, 
                                        pImpl->maxTraceValue);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setTraceDecayRate(float decay) {
    pImpl->traceDecayRate = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = error;
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedSTDP::setCreditAssignmentStrength(float strength) {
    pImpl->creditAssignmentStrength = std::clamp(strength, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getCreditAssignmentStrength() const {
    return pImpl->creditAssignmentStrength;
}

void RewardModulatedSTDP::decayEligibilityTrace(float dt) {
    if (dt > 0.0f) {
        pImpl->eligibilityTrace *= std::pow(pImpl->traceDecayRate, dt);
    }
}

float RewardModulatedSTDP::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void RewardModulatedSTDP::addToEligibilityTrace(float delta) {
    pImpl->eligibilityTrace += delta;
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace,
                                        -pImpl->maxTraceValue,
                                        pImpl->maxTraceValue);
}

} // namespace nlm