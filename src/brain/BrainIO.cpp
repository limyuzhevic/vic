#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void Brain::Impl::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (workingMemory && normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::Impl::injectCurrent(NeuronId neuron, CurrentInjection current) {
    auto it = neuronById.find(neuron.index());
    if (it != neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::Impl::injectCurrentToNeurons(NeuronType type, CurrentInjection current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<class Action> Brain::Impl::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

} // namespace nlm