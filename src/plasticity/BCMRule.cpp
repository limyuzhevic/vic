#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float sigma;      // Threshold parameter (for Merzenich threshold)
    float learningRate;  // Learning rate
    float thetaM;      // Modification threshold (BCM sliding threshold)
    float maxWeight;   // Maximum weight
    float minWeight;   // Minimum weight
    float runningSum;  // For computing postsynaptic activity average
    
    Impl() : sigma(0.1f), learningRate(0.01f), thetaM(0.5f),
             maxWeight(1.0f), minWeight(-1.0f), runningSum(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) Learning Rule
     * 
     * Mathematical formulation (BCM theory):
     * 
     * If |y| < θM: LTP (long-term potentiation) - weight increases
     * If |y| > θM: LTD (long-term depression) - weight decreases
     * 
     * Where:
     *   y = pre-synaptic activity * post-synaptic activity
     *   θM = modification threshold (sliding threshold)
     *   θM = θ0 * Σ(y)  (where θ0 is initial threshold)
     * 
     * Biological basis:
     *   - Proposed by Bienenstock, Cooper, and Munro (1982)
     *   - Explains stability of neural networks during development
     *   - Accounts for concentration of learning during critical periods
     *   - Incorporates "sliding threshold" concept
     *   - Explains why networks don't go to saturation
     *   - Found in visual cortex development
     * 
     * Key features:
     *   - Activity-dependent threshold adjustment
     *   - Prevents runaway excitation
     *   - Enables critical period plasticity
     *   - Self-organized feature extraction
     *   - Competitive learning dynamics
     * 
     * Applications:
     *   - Visual cortex development
     *   - Critical period determination
     *   - Sensory deprivation studies
     *   - Neural network stability
     *   - Developmental neurobiology
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute postsynaptic activity (simplified as average firing rate)
    float postsynapticActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    
    // Compute pre-synaptic activity (simplified)
    float presynapticActivity = static_cast<float>(preSpikes.size()) / 10.0f;  // Normalize
    
    // Compute product (y)
    float y = presynapticActivity * postsynapticActivity;
    
    // Update running sum for sliding threshold (θM = θ0 * Σ(y))
    pImpl->runningSum += y;
    float currentThetaM = pImpl->thetaM * pImpl->runningSum;
    
    // Apply BCM rule
    float delta;
    if (std::abs(y) < currentThetaM) {
        // |y| < θM: LTP (long-term potentiation)
        delta = pImpl->learningRate * y;
    } else {
        // |y| > θM: LTD (long-term depression)
        delta = -pImpl->learningRate * y;
    }
    
    applyWeightChange(synapse, delta);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCMRule";
}

void BCMRule::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 1.0f);
}

float BCMRule::getSigma() const {
    return pImpl->sigma;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThetaM(float thetaM) {
    pImpl->thetaM = std::clamp(thetaM, 0.0f, 10.0f);
}

float BCMRule::getThetaM() const {
    return pImpl->thetaM;
}

} // namespace nlm