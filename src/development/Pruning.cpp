// Pruning.cpp - Implementation
#include "Pruning.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Pruning::Pruning() : enabled_(true), rate_(0.00001f), strengthThreshold_(0.1f) {}

Pruning::~Pruning() {}

void Pruning::update(float dt, Brain* brain) {
    if (!enabled_ || !brain) return;
    
    performPruning(brain, dt);
}

void Pruning::performPruning(Brain* brain, float dt) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    size_t neuronCount = neurons.size();
    
    // Calculate number of synapses to prune based on rate
    size_t totalSynapses = 0;
    for (auto* neuron : neurons) {
        totalSynapses += neuron->getOutgoingSynapses().size();
    }
    
    size_t synapsesToPrune = static_cast<size_t>(totalSynapses * rate_ * dt);
    
    for (size_t i = 0; i < synapsesToPrune; ++i) {
        // Select random neuron
        if (neuronCount == 0) break;
        
        size_t neuronIdx = rand() % neuronCount;
        auto* neuron = neurons[neuronIdx];
        
        auto& outgoing = neuron->getOutgoingSynapses();
        if (outgoing.empty()) continue;
        
        // Select random outgoing synapse
        size_t synIdx = rand() % outgoing.size();
        auto synHandle = outgoing[synIdx];
        
        // Check if synapse should be pruned based on strength
        // This requires access to the actual synapse object
        // Simplified: always prune for now
        pruneSynapse(neuron, synHandle, brain);
    }
}

void Pruning::pruneSynapse(Neuron* pre, SynapseHandle handle, Brain* brain) {
    if (!pre || !brain) return;
    
    // Find and remove synapse from pre-synaptic neuron
    auto& outgoing = pre->getOutgoingSynapses();
    
    for (auto it = outgoing.begin(); it != outgoing.end(); ++it) {
        if (*it == handle) {
            outgoing.erase(it);
            break;
        }
    }
    
    // Find post-synaptic neuron and remove from its incoming list
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (const auto& synapse : region->getSynapses()) {
            if (synapse->getHandle() == handle) {
                // Find post-synaptic neuron and remove from its incoming list
                // This is simplified - real implementation would need neuron lookup
                break;
            }
        }
    }
}

void Pruning::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool Pruning::isEnabled() const {
    return enabled_;
}

void Pruning::setRate(float rate) {
    rate_ = rate;
}

float Pruning::getRate() const {
    return rate_;
}

void Pruning::setStrengthThreshold(float threshold) {
    strengthThreshold_ = threshold;
}

float Pruning::getStrengthThreshold() const {
    return strengthThreshold_;
}

} // namespace nlm