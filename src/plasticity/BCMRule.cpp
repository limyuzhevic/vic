#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float theta;           // Sliding threshold for BCM learning
    float learningRate;    // BCM learning rate
    float timeConstant;    // Time constant for threshold adaptation
    float currentTheta;    // Current threshold value
    float synapticActivity; // Running average of postsynaptic activity
    
    Impl() : theta(0.2f), learningRate(0.01f), timeConstant(100.0f),
             currentTheta(0.2f), synapticActivity(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update synaptic activity estimate (simplified running average)
    float postsynapticActivity = 0.0f;
    for (Timestamp post : postSpikes) {
        postsynapticActivity += std::abs(static_cast<float>(post));
    }
    postsynapticActivity /= static_cast<float>(postSpikes.size());
    
    // Exponential moving average of activity
    pImpl->synapticActivity = 0.9f * pImpl->synapticActivity + 
                             0.1f * postsynapticActivity;
    
    // Update threshold (theta) based on recent activity
    // Theta increases with high postsynaptic activity to prevent runaway potentiation
    float activityFactor = std::tanh(pImpl->synapticActivity / pImpl->theta);
    pImpl->currentTheta = pImpl->theta * (1.0f + 0.5f * activityFactor);
    
    // Calculate BCM weight change
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION (if above threshold)
                float potentiation = pImpl->learningRate * std::exp(-dt / tau);
                if (pImpl->synapticActivity > pImpl->currentTheta) {
                    totalDelta += potentiation;
                }
            } else if (dt < 0) {
                // Post before pre: DEPRESSION (always occurs)
                float depression = pImpl->learningRate * std::exp(dt / tau);
                totalDelta -= depression;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds with BCM-specific bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // BCM has additional constraint: prevents weights from going too low after high activity
    if (pImpl->synapticActivity > pImpl->currentTheta && newWeight < -0.5f) {
        newWeight = -0.5f;  // Prevent too strong depression during high activity
    }
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.05f, 0.5f);
    pImpl->currentTheta = pImpl->theta;
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setBCMLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.05f);
}

float BCMRule::getBCMLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setBCMTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000.0f);
}

float BCMRule::getBCMTimeConstant() const {
    return pImpl->timeConstant;
}

} // namespace nlm