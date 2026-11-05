// BrainIOHandler.cpp - Implementation of BrainIOHandler class

#include "BrainIOHandler.hpp"
#include "Brain.hpp"

namespace nlm {

void BrainIOHandler::receiveSensoryInput(Brain& brain, const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = brain.pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        brain.pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (brain.pImpl->workingMemory && normalizedValue > 0.5f) {
            brain.pImpl->workingMemory->storeToNeuron(brain.pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainIOHandler::injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current) {
    auto it = brain.pImpl->neuronById.find(neuron.index());
    if (it != brain.pImpl->neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
    for (auto& region : brain.pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainIOHandler::injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> BrainIOHandler::produceAction(Brain& brain) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (brain.pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : brain.pImpl->motorNeurons) {
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

void BrainIOHandler::processSensoryNeurons(Brain& brain) {
    // Process sensory neurons for input encoding
    // Placeholder for sensory processing
}

void BrainIOHandler::processMotorNeurons(Brain& brain) {
    // Process motor neurons for output decoding
    // Placeholder for motor processing
}

void BrainIOHandler::normalizeInput(float& normalizedValue) {
    // Normalize input value to range [-10, 10] mV
    // This is a simple normalization function
    // Note: This function is kept for backward compatibility
}
