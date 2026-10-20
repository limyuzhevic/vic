#include "brain/BrainAction.hpp"
#include "brain/Brain.hpp"
#include "brain/Action.hpp"
#include <memory>

namespace nlm {

std::unique_ptr<Action> BrainAction::produceAction(const std::vector<Neuron*>& motorNeurons) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron && neuron->isFiring()) {
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

void BrainAction::applySensoryInput(Brain* brain,
                                   const SensoryInput& input,
                                   std::vector<Neuron*>& sensoryNeurons) {
    if (!brain || sensoryNeurons.empty()) return;
    
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
    }
}

void BrainAction::injectCurrentToNeurons(Brain* brain,
                                        NeuronId neuron,
                                        MembranePotential current) {
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n && n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainAction::injectCurrentToType(Brain* brain,
                                     NeuronType type,
                                     MembranePotential current) {
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        if (!region) continue;
        
        for (auto& population : region->getPopulations()) {
            if (!population || population->getNeuronType() != type) continue;
            
            for (auto* neuron : population->getNeurons()) {
                if (neuron) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

} // namespace nlm
