#include "AgentBrainNeuronManager.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void AgentBrainNeuronManager::addNeurons(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot add neurons to null brain pointer");
        return;
    }
    
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
    internalNeurons_.clear();
    
    // Add neurons to appropriate groups
    if (brain) {
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {  // Check for null pointer
                            // Distribute motor neurons to different action groups
                            size_t idx = motorForward_.size() + motorBackward_.size() + 
                                        motorTurnLeft_.size() + motorTurnRight_.size() +
                                        motorInteract_.size() + motorWait_.size();
                            
                            switch (idx % 6) {
                                case 0: motorForward_.push_back(n); break;
                                case 1: motorBackward_.push_back(n); break;
                                case 2: motorTurnLeft_.push_back(n); break;
                                case 3: motorTurnRight_.push_back(n); break;
                                case 4: motorInteract_.push_back(n); break;
                                case 5: motorWait_.push_back(n); break;
                            }
                        }
                    }
                } else if (type == NeuronType::Internal) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {  // Check for null pointer
                            internalNeurons_.push_back(n);
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {  // Check for null pointer
                            // Distribute sensory neurons
                            size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                        sensoryInternal_.size() + sensoryProprioception_.size();
                            
                            switch (idx % 4) {
                                case 0: sensoryVision_.push_back(n); break;
                                case 1: sensoryTouch_.push_back(n); break;
                                case 2: sensoryInternal_.push_back(n); break;
                                case 3: sensoryProprioception_.push_back(n); break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Neuron distribution complete:");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(getMotorNeuronsCount()));
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(getSensoryNeuronsCount()));
    NLM_LOG_INFO("  Internal neurons: " + std::to_string(getInternalNeuronsCount()));
}

} // namespace nlm