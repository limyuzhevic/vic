// AgentBrain initializer module
// Contains initialization logic for AgentBrain

#include "AgentBrainInitializer.hpp"
#include "AgentBody.hpp"
#include "SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void AgentBrainInitializer::distributeMotorNeurons(
    const std::vector<std::shared_ptr<Neuron>>& neurons,
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait) {
    
    for (Neuron* n : neurons) {
        size_t idx = motorForward.size() + motorBackward.size() + 
                     motorTurnLeft.size() + motorTurnRight.size() +
                     motorInteract.size() + motorWait.size();
        
        switch (idx % 6) {
            case 0: motorForward.push_back(n); break;
            case 1: motorBackward.push_back(n); break;
            case 2: motorTurnLeft.push_back(n); break;
            case 3: motorTurnRight.push_back(n); break;
            case 4: motorInteract.push_back(n); break;
            case 5: motorWait.push_back(n); break;
        }
    }
}

void AgentBrainInitializer::distributeSensoryNeurons(
    const std::vector<std::shared_ptr<Neuron>>& neurons,
    std::vector<Neuron*>& visionNeurons,
    std::vector<Neuron*>& touchNeurons,
    std::vector<Neuron*>& internalNeurons,
    std::vector<Neuron*>& proprioceptionNeurons) {
    
    for (Neuron* n : neurons) {
        size_t idx = visionNeurons.size() + touchNeurons.size() +
                     internalNeurons.size() + proprioceptionNeurons.size();
        
        switch (idx % 4) {
            case 0: visionNeurons.push_back(n); break;
            case 1: touchNeurons.push_back(n); break;
            case 2: internalNeurons.push_back(n); break;
            case 3: proprioceptionNeurons.push_back(n); break;
        }
    }
}

void AgentBrainInitializer::initializeMotorGroups(
    AgentBrain& agentBrain,
    const Brain& brain,
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait,
    std::vector<Neuron*>& internalNeurons,
    std::vector<Neuron*>& visionNeurons,
    std::vector<Neuron*>& touchNeurons,
    std::vector<Neuron*>& proprioceptionNeurons) {
    
    for (const auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons(),
                    motorForward, motorBackward, motorTurnLeft,
                    motorTurnRight, motorInteract, motorWait);
            } else if (type == NeuronType::Internal) {
                internalNeurons.insert(internalNeurons.end(),
                    pop->getNeurons().begin(), pop->getNeurons().end());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons(),
                    visionNeurons, touchNeurons, internalNeurons,
                    proprioceptionNeurons);
            }
        }
    }
}

void AgentBrainInitializer::initializeAgentBrain(AgentBrain& agentBrain, const SimpleWorld& world) {
    // Start with no prediction (all zeros), so the first scene is
    // novel — matching the old step-0 novelty spike — and the
    // prediction is then built from actual percepts.
    agentBrain.predictedVision_.clear();
    agentBrain.developmentalAge_ = 0.0;
    agentBrain.plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(agentBrain.sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(agentBrain.sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(agentBrain.sensoryInternal_.size()) + " internal sensory neurons");
}

} // namespace nlm
