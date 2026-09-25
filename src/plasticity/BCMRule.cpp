#include "../../plasticity/PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float theta;      // BCM threshold
    float tau;        // Activity variable time constant
    float learningRate;  // BCM learning rate
    
    Impl() : theta(0.5f), tau(10.0f), learningRate(0.01f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    // Bienenstock-Cooper-Munro (BCM) learning rule
    // Implements "slide” learning with a dynamic threshold
    //
    // Mathematical formulation:
    // v = τ * dv/dt + ∑ pre(t) * post(t)
    // θ = ⟨v⟩ (moving average of v)
    // Δw = η * (v - θ) * pre(t) * post(t)
    //
    // Where:
    //   v = postsynaptic activity variable
    //   θ = dynamic threshold (updated from running average)
    //   η = learning rate
    //
    // Biological inspiration:
    //   - Addresses stability-plasticity dilemma
    //   - Implements neuronal threshold regulation
    //   - Found in visual cortex development
    //   - Sliding threshold prevents runaway potentiation
    //
    // Limitations:
    //   - Assumes postsynaptic firing rate as proxy for v
    //   - Simple activity variable estimation
    //   - Doesn't account for spike timing precision
    //
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Estimate postsynaptic activity variable v
    // Simplified: sum of products of pre and post spike timing
    float v = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            v += static_cast<float>(preTime) * static_cast<float>(postTime);
        }
    }
    v *= pImpl->learningRate;
    
    // Get current threshold
    float theta = pImpl->theta;
    
    // Update threshold based on recent activity
    // Running average approximation
    static float thetaMean = theta;
    thetaMean = thetaMean * 0.9f + v * 0.1f;
    theta = thetaMean;
    
    // Update threshold parameter
    setTheta(theta);
    
    // Calculate weight change based on BCM formula
    float delta = pImpl->learningRate * (v - theta);
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // BCM rules typically operate within narrower bounds to maintain stability
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -0.5f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 2.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setTau(float tau) {
    pImpl->tau = std::clamp(tau, 1.0f, 100.0f);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.05f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
