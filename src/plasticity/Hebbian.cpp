#include "HebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;
    float maxWeightChange;
    float baseline;
    
    Impl() : learningRate(0.01f), maxWeightChange(0.5f), baseline(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse, 
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    // Real Hebbian learning: "neurons that fire together, wire together"
    // Implements both pairwise and aggregate Hebbian learning
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Pairwise Hebbian: consider each pre-post spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Temporal window for Hebbian learning
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0 && dt < 50.0f) { // Within 50ms window
                // Positive correlation strengthens synapse
                float weightChange = pImpl->learningRate * 
                                    std::exp(-dt / 20.0f) * // Temporal decay
                                    synapse->getEfficacy();
                totalDelta += weightChange;
            }
        }
    }
    
    // Apply bounds to prevent runaway excitation
    totalDelta = std::clamp(totalDelta, -pImpl->maxWeightChange, pImpl->maxWeightChange);
    
    // Apply weight change
    applyWeightChange(synapse, totalDelta);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Hebbian weight change with homeostatic bounds
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Constrain to physiological range
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // Add small homeostatic component
    float homeostaticComponent = (newWeight - currentWeight) * 0.1f;
    newWeight = currentWeight + homeostaticComponent;
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeightChange(float maxChange) {
    pImpl->maxWeightChange = std::max(0.0f, maxChange);
}

float HebbianRule::getMaxWeightChange() const {
    return pImpl->maxWeightChange;
}

void HebbianRule::initializeRandom(class RandomGenerator& rng) {
    // Initialize with biologically plausible Hebbian parameters
    pImpl->learningRate = rng.uniformReal(0.005f, 0.05f);
    pImpl->maxWeightChange = rng.uniformReal(0.2f, 0.8f);
    pImpl->baseline = rng.uniformReal(-0.1f, 0.1f);
}

} // namespace nlm
