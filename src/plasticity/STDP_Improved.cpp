#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float timeConstant;           // Tau for exponential window (ms)
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    float globalLearningRate;     // Adaptive learning rate based on network activity
    float saturationThreshold;    // Threshold for reducing learning when network is saturated
    uint64_t stepsSinceLastAdjustment;
    
    Impl() : ltpWeight(0.02f), ltdWeight(0.015f), timeConstant(30.0f),
             minWeight(-0.5f), maxWeight(0.5f),
             globalLearningRate(1.0f), saturationThreshold(0.9f),
             stepsSinceLastAdjustment(0) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    /*
     * Improved STDP implementation with adaptive learning rate
     * and better saturation control
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Nearest-neighbour STDP with timing window
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Apply adaptive learning rate based on global saturation
    float adaptiveFactor = pImpl->globalLearningRate;
    
    // Time-dependent component
    if (dtMs > 0) {
        // Potentiation with reduced amplitude for small time differences
        float timeFactor = std::exp(-dtMs / tau);
        totalDelta += pImpl->ltpWeight * timeFactor * adaptiveFactor;
    } else if (dtMs < 0) {
        // Depression with reduced amplitude for small time differences
        float timeFactor = std::exp(dtMs / tau);
        totalDelta += -pImpl->ltdWeight * timeFactor * adaptiveFactor;
    }
    
    // Apply weight change with improved bounds
    if (std::abs(totalDelta) > 1e-7f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Improved sign handling for inhibitory synapses
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;  // Strengthen inhibition (make more negative)
        }
        
        // Enhanced soft bounds for better learning selectivity
        float w = synapse->getWeight();
        bool isPositive = w > 0.0f;
        bool isNegative = w < 0.0f;
        
        if (isPositive) {
            // For positive weights: scale by (1 - w/maxWeight) for subtler potentiation
            float saturationFactor = (pImpl->maxWeight - w) / (pImpl->maxWeight - pImpl->minWeight);
            totalDelta *= saturationFactor * 0.5f;  // Reduced from 1.0f
        } else if (isNegative) {
            // For negative weights: scale by (-w - minWeight) for subtler depression
            float saturationFactor = (std::abs(w) - std::abs(pImpl->minWeight)) / 
                                     (pImpl->maxWeight - pImpl->minWeight);
            totalDelta *= saturationFactor * 0.5f;  // Reduced from 1.0f
        } else {
            // For near-zero weights: use a reduced learning rate
            totalDelta *= 0.1f;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Improved sign clamping
        if (synapse->getType() == SynapseType::Inhibitory) {
            if (synapse->getWeight() > -0.02f) {  // Looser bound
                synapse->setWeight(-0.02f);
            }
        } else if (synapse->getType() == SynapseType::Excitatory) {
            if (synapse->getWeight() < 0.02f) {  // Looser bound
                synapse->setWeight(0.02f);
            }
        }
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.5f);  // Reduced trace
        
        // Increment counter for learning rate adjustment
        pImpl->stepsSinceLastAdjustment++;
        
        // Adapt learning rate based on network activity
        if (pImpl->stepsSinceLastAdjustment >= 1000) {
            adjustGlobalLearningRate();
        }
    }
}

void STDP::adjustGlobalLearningRate() {
    // Implement adaptive learning rate logic
    // This would need access to network-wide statistics
    // For now, we'll keep it simple
    
    // Smooth learning rate adjustment
    float targetRate = 1.0f;
    float currentRate = pImpl->globalLearningRate;
    
    if (currentRate > targetRate) {
        currentRate *= 0.95f;  // Gradual reduction
    } else {
        currentRate *= 1.05f;  // Gradual increase
    }
    
    currentRate = std::clamp(currentRate, 0.1f, 2.0f);
    pImpl->globalLearningRate = currentRate;
    
    // Reset counter
    pImpl->stepsSinceLastAdjustment = 0;
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to improved bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 50.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
