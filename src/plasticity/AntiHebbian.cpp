// AntiHebbian.cpp - Implementation of anti-Hebbian plasticity

#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float inhibitionStrength;
    float maxWeight;
    float minWeight;
    float activityBaseline;
    
    Impl() : learningRate(0.01f), inhibitionStrength(0.5f), maxWeight(1.0f), 
             minWeight(-1.0f), activityBaseline(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() {
    delete pImpl;
}

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   The negative sign means correlation decreases synaptic weight
     *   "neurons that fire together, disconnect together"
     *   
     * Biological inspiration:
     *   - Opposite-pole stabilization in cortical circuits
     *   - Homeostatic plasticity mechanisms
     *   - Prevents runaway excitation in neural networks
     *   
     * Applications:
     *   - Preventing synchronization seizures
     *   - Maintaining network stability
     *   - Resetting synaptic weights after learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate current correlation between pre- and post-synaptic activity
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window
            if (std::abs(dt) < 100.0f) {  // 100ms window
                ++correlationCount;
            }
        }
    }
    
    // Estimate average firing rates
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float expectedCorrelation = preRate * postRate * dt;
    
    // Calculate anti-Hebbian weight change (negative correlation effect)
    float correlation = static_cast<float>(correlationCount) / (preSpikes.size() * postSpikes.size());
    float delta = -pImpl->learningRate * pImpl->inhibitionStrength * (correlation - expectedCorrelation);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = std::clamp(strength, 0.0f, 2.0f);
}

float AntiHebbianRule::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

} // namespace nlm