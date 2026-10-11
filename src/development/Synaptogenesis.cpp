// Synaptogenesis.cpp - Implementation
#include "Synaptogenesis.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Synaptogenesis::Synaptogenesis() : enabled_(true), rate_(0.0001f), ageThreshold_(10.0f) {}

Synaptogenesis::~Synaptogenesis() {}

void Synaptogenesis::update(float dt, Brain* brain) {
    if (!enabled_ || !brain) return;
    
    // Age-dependent synaptogenesis
    if (brain->getAverageAge() >= ageThreshold_) {
        performSynaptogenesis(brain, dt);
    }
}

void Synaptogenesis::performSynaptogenesis(Brain* brain, float dt) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    size_t neuronCount = neurons.size();
    
    if (neuronCount < 2) return;
    
    // Calculate number of new connections based on rate
    size_t maxNewConnections = static_cast<size_t>(neuronCount * rate_ * dt);
    
    for (size_t i = 0; i < maxNewConnections; ++i) {
        // Random neuron pair
        size_t preIdx = rand() % neuronCount;
        size_t postIdx = rand() % neuronCount;
        
        if (preIdx == postIdx) continue;
        
        auto* preNeuron = neurons[preIdx];
        auto* postNeuron = neurons[postIdx];
        
        // Check if connection already exists
        auto& outgoing = preNeuron->getOutgoingSynapses();
        bool exists = false;
        
        for (const auto& synHandle : outgoing) {
            // Need to check if this synapse connects to postNeuron
            // This is a simplified check - real implementation would be more complex
        }
        
        if (!exists) {
            // Create new synapse
            createNewSynapse(preNeuron, postNeuron, brain);
        }
    }
}

void Synaptogenesis::createNewSynapse(Neuron* pre, Neuron* post, Brain* brain) {
    if (!pre || !post || !brain) return;
    
    // Create synapse connection
    auto synapse = std::make_shared<Synapse>(pre->getId(), post->getId());
    synapse->setWeight(1.0f); // Initial weight
    synapse->enablePlasticity(true, true, false);
    
    // Add to neurons
    pre->addOutgoingSynapse(synapse->getHandle());
    post->addIncomingSynapse(synapse->getHandle());
    
    // Add to region
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        region->addSynapse(synapse);
    }
}

void Synaptogenesis::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool Synaptogenesis::isEnabled() const {
    return enabled_;
}

void Synaptogenesis::setRate(float rate) {
    rate_ = rate;
}

float Synaptogenesis::getRate() const {
    return rate_;
}

} // namespace nlm