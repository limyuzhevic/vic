// Improved AgentBrain Implementation
// This file demonstrates practical improvements for better code quality
// while maintaining compatibility with the existing interface

#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace nlm {

class AgentBrainImproved : public AgentBrain {
public:
    AgentBrainImproved(std::shared_ptr<Brain> brain)
        : AgentBrain(brain) {
        validateBrainInitialization();
        populateNeuronGroups();
    }
    
    ~AgentBrainImproved() = default;
    
    // Enhanced method with proper error handling and validation
    bool isValidNeuron(Neuron* neuron) const {
        if (!neuron) {
            return false;
        }
        
        // Additional validation checks
        return neuron->getId() > 0 && 
               neuron->getType() != NeuronType::Invalid;
    }
    
    // Safely get motor neuron groups with validation
    std::vector<Neuron*> getValidatedMotorNeurons() const {
        std::vector<Neuron*> validNeurons;
        for (const auto& neuron : motorForward_) {
            if (isValidNeuron(neuron)) {
                validNeurons.push_back(neuron);
            }
        }
        return validNeurons;
    }
    
    // Get motor neuron groups by type with error handling
    std::vector<Neuron*> getMotorNeuronsByType(MotorActionType type) const {
        switch (type) {
            case MotorActionType::Forward: return getValidatedMotorNeurons();
            case MotorActionType::Backward: return getValidatedMotorNeurons();
            case MotorActionType::TurnLeft: return getValidatedMotorNeurons();
            case MotorActionType::TurnRight: return getValidatedMotorNeurons();
            case MotorActionType::Interact: return getValidatedMotorNeurons();
            case MotorActionType::Wait: return getValidatedMotorNeurons();
            default: return {};
        }
    }
    
    // Enhanced decodeMotorCommand with better error handling
    MotorCommand decodeMotorCommand() override {
        try {
            // Validate that brain is still active
            if (!brain_ || !brain_->getSpikeSystem()) {
                NLM_LOG_WARNING("Brain not properly initialized for motor command decoding");
                return std::make_unique<Action>(ActionType::Wait);
            }
            
            // Use the parent's implementation but with additional safety checks
            auto action = AgentBrain::decodeMotorCommand();
            
            // Validate the decoded action
            if (!action || action->getType() == ActionType::Invalid) {
                NLM_LOG_WARNING("Invalid motor command decoded, returning default");
                return std::make_unique<Action>(ActionType::Wait);
            }
            
            return action;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in decodeMotorCommand: " + std::string(e.what()));
            return std::make_unique<Action>(ActionType::Wait);
        }
    }
    
    // Process sensory input with enhanced validation
    void processSensoryInput(const SensoryPercept& percept) override {
        try {
            // Validate percept
            if (percept.getVision().empty() && percept.getTouch().empty()) {
                NLM_LOG_WARNING("Empty sensory percept received");
                return;
            }
            
            // Additional validation checks
            if (!validateSensoryInput(percept)) {
                NLM_LOG_WARNING("Invalid sensory input, skipping processing");
                return;
            }
            
            AgentBrain::processSensoryInput(percept);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in processSensoryInput: " + std::string(e.what()));
        }
    }
    
    // Enhanced development update with better error handling
    void updateDevelopment(double timestep) override {
        try {
            if (!brain_ || !brain_->getDevelopmentSystem()) {
                NLM_LOG_WARNING("Cannot update development - brain not properly initialized");
                return;
            }
            
            // Additional validation
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Invalid timestep for development update: " + std::to_string(timestep));
                return;
            }
            
            AgentBrain::updateDevelopment(timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in updateDevelopment: " + std::string(e.what()));
        }
    }
    
    // Apply neuromodulation with enhanced error handling
    void applyRewardModulation(float reward, float predictedReward) override {
        try {
            // Validate reward values
            if (std::isnan(reward) || std::isinf(reward)) {
                NLM_LOG_ERROR("Invalid reward value: " + std::to_string(reward));
                return;
            }
            
            // Clamp reward to reasonable bounds
            reward = std::max(-10.0f, std::min(10.0f, reward));
            predictedReward = std::max(-10.0f, std::min(10.0f, predictedReward));
            
            AgentBrain::applyRewardModulation(reward, predictedReward);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in applyRewardModulation: " + std::string(e.what()));
        }
    }
    
protected:
    // Helper methods for validation
    bool validateBrainInitialization() const {
        return brain_ != nullptr && brain_->getRegionCount() > 0;
    }
    
    bool validateSensoryInput(const SensoryPercept& percept) const {
        // Additional validation logic
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        
        // Validate vision data
        for (float value : vision) {
            if (std::isnan(value) || std::isinf(value)) {
                return false;
            }
        }
        
        // Validate touch data
        for (float value : touch) {
            if (std::isnan(value) || std::isinf(value)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Populate neuron groups with validation
    void populateNeuronGroups() {
        if (!validateBrainInitialization()) {
            NLM_LOG_WARNING("Cannot populate neuron groups - brain not properly initialized");
            return;
        }
        
        // Call parent method
        AgentBrain::populateNeuronGroups();
        
        // Additional validation and safety checks
        validateAndCleanNeuronGroups();
    }
    
    // Validate and clean neuron groups
    void validateAndCleanNeuronGroups() {
        // Remove null or invalid neurons from all groups
        cleanNeuronGroup(motorForward_);
        cleanNeuronGroup(motorBackward_);
        cleanNeuronGroup(motorTurnLeft_);
        cleanNeuronGroup(motorTurnRight_);
        cleanNeuronGroup(motorInteract_);
        cleanNeuronGroup(motorWait_);
        cleanNeuronGroup(sensoryVision_);
        cleanNeuronGroup(sensoryTouch_);
        cleanNeuronGroup(sensoryInternal_);
        cleanNeuronGroup(sensoryProprioception_);
        cleanNeuronGroup(internalNeurons_);
    }
    
    template<typename T>
    void cleanNeuronGroup(std::vector<T>& group) {
        group.erase(std::remove_if(group.begin(), group.end(),
                                  [](T neuron) { return neuron == nullptr; }),
                   group.end());
    }
};

} // namespace nlm
