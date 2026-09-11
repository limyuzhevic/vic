#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian learning: "cells that fire together, wire apart"
     * 
     * This implementation decreases synaptic weight when neurons fire together
     * (pre and post spikes occur close in time), implementing the opposite of
     * Hebbian learning. This helps stabilize the network and prevent runaway
     * excitation.
     * 
     * Mathematical formulation:
     * For each correlated spike pair:
     * Δw = -η * exp(-Δt / τ)
     * 
     * Where η is the learning rate and τ is a time constant.
     * 
     * Biological inspiration:
     * - Homosynaptic depression
     * - Negative feedback in synaptic strength
     * - Prevents excessive excitation in neural circuits
     */
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_ms = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window
            if (std::abs(dt_ms) < 100.0f) {
                // Anti-Hebbian: decrease weight for correlated activity
                float delta = -pImpl->learningRate * std::exp(-std::abs(dt_ms) / 20.0f);
                totalDelta += delta;
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
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

} // namespace nlm
