#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Motor neuron groups are now organized dynamically in the constructor
}

void AgentBrain::organizeNeuronsByType() {
    // Organize motor neurons dynamically based on brain configuration
    if (!brain_) return;
    
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // First pass: collect all neurons by type
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            auto neurons = pop->getNeurons();
            
            switch (type) {
                case NeuronType::Motor:
                    // Distribute motor neurons based on population characteristics
                    if (neurons.size() >= 6) {
                        // Even distribution for balanced control
                        size_t idx = 0;
                        for (Neuron* n : neurons) {
                            switch (idx % 6) {
                                case 0: motorForward_.push_back(n); break;
                                case 1: motorBackward_.push_back(n); break;
                                case 2: motorTurnLeft_.push_back(n); break;
                                case 3: motorTurnRight_.push_back(n); break;
                                case 4: motorInteract_.push_back(n); break;
                                case 5: motorWait_.push_back(n); break;
                            }
                            idx++;
                        }
                    } else {
                        // Distribute as best as possible for smaller populations
                        for (size_t i = 0; i < neurons.size(); ++i) {
                            switch (i % 6) {
                                case 0: motorForward_.push_back(neurons[i]); break;
                                case 1: motorBackward_.push_back(neurons[i]); break;
                                case 2: motorTurnLeft_.push_back(neurons[i]); break;
                                case 3: motorTurnRight_.push_back(neurons[i]); break;
                                case 4: motorInteract_.push_back(neurons[i]); break;
                                case 5: motorWait_.push_back(neurons[i]); break;
                            }
                        }
                    }
                    break;
                    
                case NeuronType::Sensory:
                    // Organize sensory neurons based on function
                    auto config = brain_->getConfig();
                    if (config) {
                        int visionNeurons = config->getOr<int>("sensory.vision_neurons", 256 / 25); // Default 1 per vision pixel
                        int touchNeurons = config->getOr<int>("sensory.touch_neurons", 8);
                        int internalNeurons = config->getOr<int>("sensory.internal_neurons", 4);
                        int proprioceptionNeurons = config->getOr<int>("sensory.proprioception_neurons", 6);
                        
                        int assigned = 0;
                        for (Neuron* n : neurons) {
                            if (assigned < visionNeurons) {
                                sensoryVision_.push_back(n);
                            } else if (assigned < visionNeurons + touchNeurons) {
                                sensoryTouch_.push_back(n);
                            } else if (assigned < visionNeurons + touchNeurons + internalNeurons) {
                                sensoryInternal_.push_back(n);
                            } else {
                                sensoryProprioception_.push_back(n);
                            }
                            assigned++;
                        }
                    } else {
                        // Default distribution
                        size_t idx = 0;
                        for (Neuron* n : neurons) {
                            switch (idx % 4) {
                                case 0: sensoryVision_.push_back(n); break;
                                case 1: sensoryTouch_.push_back(n); break;
                                case 2: sensoryInternal_.push_back(n); break;
                                case 3: sensoryProprioception_.push_back(n); break;
                            }
                            idx++;
                        }
                    }
                    break;
                    
                default:
                    // Non-motor/sensory neurons are not organized here
                    break;
            }
        }
    }
    
    NLM_LOG_INFO("Organized " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                motorTurnLeft_.size() + motorTurnRight_.size() + 
                motorInteract_.size() + motorWait_.size()) + 
                " motor neurons and " +
                std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                sensoryInternal_.size() + sensoryProprioception_.size()) + 
                " sensory neurons");
}

std::string AgentBrain::motorCommandToString(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

void AgentBrain::resetActionStatistics() {
    actionSuccessCount_.assign(actionSuccessCount_.size(), 0);
    actionTotalCount_.assign(actionTotalCount_.size(), 0);
    actionHistory_.clear();
    
    NLM_LOG_INFO("Action statistics reset");
}
