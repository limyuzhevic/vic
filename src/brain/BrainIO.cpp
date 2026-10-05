#include "BrainIO.hpp"
#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainIO::receiveSensoryInput(Brain* parent, const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = getSensoryNeuronCount(parent);
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        injectCurrent(parent, parent->getSensoryNeurons()[i]->getId(), normalizedValue);
        
        // Also store in working memory
        if (auto workingMemory = parent->getWorkingMemory()) {
            if (normalizedValue > 0.5f) {
                workingMemory->storeToNeuron(parent->getSensoryNeurons()[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
}

void BrainIO::injectCurrent(Brain* parent, NeuronId neuron, MembranePotential current) {
    for (const auto& region : parent->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainIO::injectCurrentToNeurons(Brain* parent, NeuronType type, MembranePotential current) {
    for (const auto& region : parent->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> BrainIO::produceAction(Brain* parent) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    size_t numMotor = getMotorNeuronCount(parent);
    if (numMotor == 0) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = calculateFiringMotorCount(parent);
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void BrainIO::distributeInputToSensoryNeurons(Brain* parent, const std::vector<float>& values) {
    size_t numSensory = getSensoryNeuronCount(parent);
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = values[i] * 10.0f;
        }
        injectCurrent(parent, parent->getSensoryNeurons()[i]->getId(), normalizedValue);
    }
}

size_t BrainIO::getSensoryNeuronCount(Brain* parent) const {
    return parent->getSensoryNeurons().size();
}

size_t BrainIO::getMotorNeuronCount(Brain* parent) const {
    return parent->getMotorNeurons().size();
}

size_t BrainIO::calculateFiringMotorCount(Brain* parent) const {
    size_t firingMotor = 0;
    for (auto* neuron : parent->getMotorNeurons()) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    return firingMotor;
}

} // namespace nlm