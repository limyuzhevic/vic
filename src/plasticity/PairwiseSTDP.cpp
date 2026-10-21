#include "PairwiseSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PairwiseSTDP::Impl {
    // Basic STDP parameters
    float ltpWeight;         // Potentiation weight (A+)
    float ltdWeight;         // Depression weight (A-)
    
    // Separate time constants for potentiation and depression
    float timeConstantPotentiation;  // τ+ time constant for potentiation
    float timeConstantDepression;   // τ- time constant for depression
    
    // Weight bounds
    float maxWeight;          // Maximum synaptic weight
    float minWeight;          // Minimum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstantPotentiation(20.0f),
             timeConstantDepression(20.0f), maxWeight(1.0f), minWeight(-1.0f) {}
};

PairwiseSTDP::PairwiseSTDP() : pImpl(new Impl) {}

PairwiseSTDP::~PairwiseSTDP() = default;

void PairwiseSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // For each pre-synaptic spike
    for (Timestamp preTime : preSpikes) {
        // For each post-synaptic spike
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt = t_post - t_pre
            
            if (delta_t > 0) {
                // Pre before post (positive Δt): POTENTIATION
                // Uses separate time constant for potentiation
                float delta = pImpl->ltpWeight * std::exp(-delta_t / pImpl->timeConstantPotentiation);
                totalDelta += delta;
            } else if (delta_t < 0) {
                // Post before pre (negative Δt): DEPRESSION
                // Uses separate time constant for depression (note: using absolute value for exponential)
                float delta = -pImpl->ltdWeight * std::exp(delta_t / pImpl->timeConstantDepression);
                totalDelta += delta;
            }
            // If delta_t == 0 (simultaneous spikes), no change
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void PairwiseSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* PairwiseSTDP::getName() const {
    return "PairwiseSTDP";
}

void PairwiseSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float PairwiseSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void PairwiseSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float PairwiseSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void PairwiseSTDP::setTimeConstantPotentiation(float tauPlus) {
    pImpl->timeConstantPotentiation = std::clamp(tauPlus, 1.0f, 100.0f);
}

float PairwiseSTDP::getTimeConstantPotentiation() const {
    return pImpl->timeConstantPotentiation;
}

void PairwiseSTDP::setTimeConstantDepression(float tauMinus) {
    pImpl->timeConstantDepression = std::clamp(tauMinus, 1.0f, 100.0f);
}

float PairwiseSTDP::getTimeConstantDepression() const {
    return pImpl->timeConstantDepression;
}

void PairwiseSTDP::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float PairwiseSTDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

void PairwiseSTDP::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float PairwiseSTDP::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm