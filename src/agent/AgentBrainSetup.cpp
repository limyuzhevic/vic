#include "AgentBrainSetup.hpp"
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {
namespace setup {

void AgentBrainSetup::initializeConstructor(AgentBrain& agentBrain, std::shared_ptr<Brain> brain) {
    agentBrain.brain_ = brain;
    agentBrain.dopamineLevel_ = 0.0f;
    agentBrain.noveltyLevel_ = 0.0f;
    agentBrain.curiosityLevel_ = 0.0f;
    agentBrain.predictionError_ = 0.0f;
    agentBrain.expectedReward_ = 0.0f;
    agentBrain.developmentalAge_ = 0.0;
    agentBrain.plasticityModifier_ = 1.0f;
    agentBrain.rewardModulationEnabled_ = true;
    agentBrain.structuralPlasticityEnabled_ = true;
    agentBrain.developmentEnabled_ = true;
    agentBrain.curiosityEnabled_ = true;
    agentBrain.sensoryNoveltyDecay_ = 0.99f;
    
    // Initialize motor and sensory neuron groups
    if (brain) {
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = agentBrain.motorForward_.size() + agentBrain.motorBackward_.size() + 
                                     agentBrain.motorTurnLeft_.size() + agentBrain.motorTurnRight_.size() +
                                     agentBrain.motorInteract_.size() + agentBrain.motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: agentBrain.motorForward_.push_back(n); break;
                            case 1: agentBrain.motorBackward_.push_back(n); break;
                            case 2: agentBrain.motorTurnLeft_.push_back(n); break;
                            case 3: agentBrain.motorTurnRight_.push_back(n); break;
                            case 4: agentBrain.motorInteract_.push_back(n); break;
                            case 5: agentBrain.motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = agentBrain.sensoryVision_.size() + agentBrain.sensoryTouch_.size() +
                                     agentBrain.sensoryInternal_.size() + agentBrain.sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: agentBrain.sensoryVision_.push_back(n); break;
                            case 1: agentBrain.sensoryTouch_.push_back(n); break;
                            case 2: agentBrain.sensoryInternal_.push_back(n); break;
                            case 3: agentBrain.sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

} // namespace setup
} // namespace nlm
