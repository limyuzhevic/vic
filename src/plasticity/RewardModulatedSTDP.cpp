#include "RewardModulatedSTDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float learningRate;      // Overall learning rate (A)
    float discountFactor;    // Eligibility trace decay (γ)
    float predictionError;   // Reward prediction error (δ)
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    
    Impl() : learningRate(0.01f), discountFactor(0.95f), predictionError(0.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // Compute STDP eligibility trace contribution
    float eligibilityTrace = 0.0f;
    
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Only consider the most recent spike pair (efficient implementation)
        Timestamp preTime = preSpikes.back();
        Timestamp postTime = postSpikes.back();
        
        float dtMs = static_cast<float>((postTime - preTime) * 1000.0);
        
        if (dtMs > 0) {
            // Pre-before-post: potentiation contribution
            eligibilityTrace += getLTPWeight(pImpl->learningRate) * 
                               std::exp(-dtMs / getTimeConstant(pImpl->discountFactor));
        } else if (dtMs < 0) {
            // Post-before-pre: depression contribution  
            eligibilityTrace += -getLTDWeight(pImpl->learningRate) * 
                               std::exp(dtMs / getTimeConstant(pImpl->discountFactor));
        }
    }
    
    // Store eligibility trace in synapse
    synapse->setEligibilityTrace(eligibilityTrace);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Check reward prediction error
    if (pImpl->predictionError <= 0.0f) {
        // Only learn when prediction error is positive
        return;
    }
    
    // Get current eligibility trace
    float eligibilityTrace = synapse->getEligibilityTrace();
    if (std::abs(eligibilityTrace) < 1e-6f) {
        return; // No eligibility trace
    }
    
    // Compute weight change using eligibility trace
    float weightChange = pImpl->learningRate * eligibilityTrace * pImpl->predictionError;
    
    // Apply weight change with bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Decay eligibility trace for next step
    synapse->decayEligibilityTrace(pImpl->discountFactor);
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

void RewardModulatedSTDP::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.1f, 0.999f);
}

float RewardModulatedSTDP::getDiscountFactor() const {
    return pImpl->discountFactor;
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = std::clamp(error, -1.0f,  আটf);
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedSTDP::reset() {
    pImpl->predictionError = 0.0f;
    // Clear eligibility traces
    // Note: Synapse eligibility traces are handled by synapse class
}

// Helper methods for accessing pImpl parameters
float RewardModulatedSTDP::getLTPWeight(float rate) const {
    return rate * 0.01f; // Default LTP weight
}

float RewardModulatedSTDP::getLTDWeight(float rate) const {
    return rate * 0.012f; // Default LTD weight
}

float RewardModulatedSTDP::getTimeConstant(float factor) const {
    return 20.0f * factor; // Default time constant
}

} // namespace nlm
