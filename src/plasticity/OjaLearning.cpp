#include "OjaLearning.hpp"
#include "src/core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct OjaLearning::Impl {
    float learningRate;
    float weightLimit;
    float minWeight;
    
    Impl(float lr, float limit) : learningRate(lr), weightLimit(limit), minWeight(-limit) {}
};

OjaLearning::OjaLearning(float learningRate, float weightLimit) 
    : pImpl(new Impl(learningRate, weightLimit)) {}

void OjaLearning::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    /*
     * Oja Learning Rule Implementation
     * 
     * Mathematical formulation:
     * Δw = η * y * (x - y * w)
     * 
     * Where:
     *   x = pre-synaptic activity (firing rate)
     *   y = post-synaptic activity (firing rate)  
     *   w = synaptic weight
     *   η = learning rate
     * 
     * Biological inspiration:
     *   - Normalizes weight updates to prevent runaway potentiation
     *   - Implements competitive learning for synaptic competition
     *   - Reflects synaptic normalization mechanisms in biology
     *   - Used in self-organizing maps and neural development
     *   
     * Advantages over Hebbian:
     *   - Prevents weight explosion through normalization
     *   - Maintains stable representations
     *   - Allows for competitive synaptic development
     *   - Biologically realistic weight constraints
     */
    
    // Calculate firing rates (simplified for spike-based systems)
    float preRate = static_cast<float>(preSpikes.size()) / dt;  // spikes per timestep
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    if (preRate <= 0.0f || postRate <= 0.0f) {
        return; // No meaningful activity
    }
    
    // Oja learning rule: Δw = η * y * (x - y * w)
    float delta = pImpl->learningRate * postRate * (preRate - postRate * synapse->getWeight());
    
    // Apply with bounds
    applyWeightChange(synapse, delta);
    
    // Update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
}

void OjaLearning::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->weightLimit);
    synapse->setWeight(newWeight);
}

const char* OjaLearning::getName() const {
    return "Oja";
}

void OjaLearning::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void OjaLearning::setMaxWeight(float max) {
    pImpl->weightLimit = std::clamp(max, 0.1f, 10.0f);
    pImpl->minWeight = -pImpl->weightLimit;
}

void OjaLearning::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
    pImpl->weightLimit = std::abs(pImpl->minWeight);
}

float OjaLearning::getLearningRate() const {
    return pImpl->learningRate;
}

float OjaLearning::getMaxWeight() const {
    return pImpl->weightLimit;
}

float OjaLearning::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm