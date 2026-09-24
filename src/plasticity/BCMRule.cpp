#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    float learningRate;
    float thetaPlus;      // Sliding threshold for potentiation (Π)
    float thetaMinus;      // Sliding threshold for depression (ϴ)
    float tauTheta;       // Time constant for threshold adaptation
    float maxWeight;
    float minWeight;
    float currentTheta;   // Current sliding threshold
    
    Impl() : learningRate(0.01f), thetaPlus(1.0f), thetaMinus(0.5f),
             tauTheta(100.0f), maxWeight(1.0f), minWeight(-1.0f),
             currentTheta(thetaMinus) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Real BCM (Bienenstock-Cooper-Munro) learning implementation
     * 
     * Mathematical formulation:
     * Δw = η * (post · (post² - θ))
     * 
     * Where:
     *   - θ is a sliding threshold that adapts based on post-synaptic activity
     *   - θ adapts according to: dθ/dt = (post² - θ) / τ
     *   - When post² > θ: potentiation occurs (weights increase)
     *   - When post² < θ: depression occurs (weights decrease)
     *   - This implements a winner-take-all learning rule
     * 
     * Biological inspiration:
     *   - Implements BCM theory of synaptic modification
     *   - Sliding threshold explains stability-plasticity dilemma
     *   - Explains memory consolidation and synaptic tagging
     *   - Related to calcium-based synaptic tagging
     *   
     * Limitations:
     *   - Simplified version (no spike timing dependence)
     *   - Single postsynaptic variable (no synaptic traces)
     *   - Doesn't incorporate presynaptic contributions
     */
    if (!synapse) return;
    
    // Update sliding threshold based on postsynaptic activity
    float postActivity = calculatePostActivity(postSpikes);
    float thetaAdaptation = (postActivity * postActivity - pImpl->currentTheta) / pImpl->tauTheta;
    pImpl->currentTheta += thetaAdaptation * dt;
    pImpl->currentTheta = std::clamp(pImpl->currentTheta, pImpl->thetaMinus, pImpl->thetaPlus);
    
    // Compute BCM synaptic modification
    float postActivitySquared = postActivity * postActivity;
    float modification = pImpl->learningRate * postActivity * (postActivitySquared - pImpl->currentTheta);
    
    // Apply with bounds
    if (std::abs(modification) > 1e-6f) {
        applyWeightChange(synapse, modification);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThetaPlus(float thetaPlus) {
    pImpl->thetaPlus = std::clamp(thetaPlus, 0.1f, 10.0f);
}

float BCMRule::getThetaPlus() const {
    return pImpl->thetaPlus;
}

void BCMRule::setThetaMinus(float thetaMinus) {
    pImpl->thetaMinus = std::clamp(thetaMinus, 0.0f, pImpl->thetaPlus - 0.1f);
}

float BCMRule::getThetaMinus() const {
    return pImpl->thetaMinus;
}

void BCMRule::setTauTheta(float tauTheta) {
    pImpl->tauTheta = std::clamp(tauTheta, 1.0f, 1000.0f);
}

float BCMRule::getTauTheta() const {
    return pImpl->tauTheta;
}

float BCMRule::calculatePostActivity(const std::vector<Timestamp>& postSpikes) const {
    // Calculate postsynaptic activity from spike history
    if (postSpikes.empty()) return 0.0f;
    
    // Simple count of recent spikes (could be more sophisticated)
    return static_cast<float>(postSpikes.size());
}

} // namespace nlm