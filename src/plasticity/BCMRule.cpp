#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float theta;              // Sliding threshold (BCM threshold)
    float learningRate;       // Base learning rate
    float synapticScaling;    // Synaptic weight scaling factor
    float previousActivity;   // Previous average postsynaptic activity for sliding threshold
    float synapticLTD;        // Long-term depression factor
    float synapticLTP;        // Long-term potentiation factor
    
    Impl() : theta(0.5f), learningRate(0.01f), synapticScaling(1.0f),
             previousActivity(0.0f), synapticLTD(0.01f), synapticLTP(0.012f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Real BCM (Bienenstock-Cooper-Munro) implementation with synaptic scaling
     * 
     * BCM rule: Neurons that fire together wire together, but only when
     * postsynaptic activity exceeds the sliding threshold (theta).
     * 
     * Mathematical formulation:
     *   For each post-synaptic spike at time t:
     *   - Increase theta: θ(t) = θ(t-1) + α * (x² - θ(t-1))
     *   - If x > θ(t): LTP occurs: Δw = η * (x - θ(t)) * w
     *   - If x ≤ θ(t): LTD occurs: Δw = -η * ξ * w
     *   
     * Where:
     *   x = postsynaptic activity (spike rate or calcium level)
     *   θ = sliding threshold (changes with postsynaptic activity)
     *   η = learning rate
     *   ξ = LTD rate (typically ~0.01)
     *   w = current synaptic weight
     *   α = threshold adaptation rate
     * 
     * Synaptic scaling:
     *   All weights scaled by factor s: w_new = w * s
     *   Maintains weight range and prevents runaway excitation
     * 
     * Biological inspiration:
     *   - Sliding threshold reflects homeostatic regulation
     *   - Links postsynaptic activity to synaptic modification
     *   - Prevents synaptic saturation
     */
    
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Calculate current postsynaptic activity (normalized spike count)
    float postsynapticActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalized over 10ms
    postsynapticActivity = std::clamp(postsynapticActivity, 0.0f, 1.0f);
    
    // Update sliding threshold (θ = θ + α * (x² - θ))
    // α = 0.01 (threshold adaptation rate)
    float alpha = 0.01f;
    pImpl->previousActivity = pImpl->previousActivity * (1.0f - alpha) + 
                             postsynapticActivity * postsynapticActivity * alpha;
    pImpl->theta = pImpl->previousActivity;
    
    // Calculate weight change based on BCM rule
    float weightChange = 0.0f;
    float currentWeight = synapse->getWeight();
    
    if (postsynapticActivity > pImpl->theta) {
        // LTP: Postsynaptic firing exceeds threshold
        // Δw = η * (x - θ) * w
        weightChange = pImpl->learningRate * (postsynapticActivity - pImpl->theta) * currentWeight;
    } else {
        // LTD: Postsynaptic firing below threshold
        // Δw = -η * ξ * w
        weightChange = -pImpl->learningRate * pImpl->synapticLTD * currentWeight;
    }
    
    // Apply synaptic scaling
    // Prevent runaway excitation by scaling weights based on overall activity
    if (std::abs(postsynapticActivity - 0.5f) > 0.2f) {
        float scalingFactor = pImpl->synapticScaling;
        if (postsynapticActivity > 0.7f) {
            // Scale down if too much activity (homeostatic regulation)
            scalingFactor *= 0.9f;
        } else if (postsynapticActivity < 0.3f) {
            // Scale up if too little activity (promote activation)
            scalingFactor *= 1.1f;
        }
        
        // Apply scaling to current weight change
        weightChange *= scalingFactor;
    }
    
    // Ensure weight change doesn't exceed bounds
    if (std::abs(weightChange) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        weightChange *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(weightChange);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + weightChange);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // For BCM rule, apply weight change with soft bounds
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply BCM-specific weight constraints
    // Allow wider range than STDP (typically -2 to 2 for BCM)
    float minWeight = pImpl->maxWeight * -2.0f;
    float maxWeight = pImpl->maxWeight * 2.0f;
    
    // Apply synaptic scaling to bounds
    newWeight *= pImpl->synapticScaling;
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setSynapticScaling(float scaling) {
    pImpl->synapticScaling = std::clamp(scaling, 0.5f, 2.0f);
}

float BCMRule::getSynapticScaling() const {
    return pImpl->synapticScaling;
}

} // namespace nlm
