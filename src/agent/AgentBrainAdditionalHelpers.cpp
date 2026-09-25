// Additional helper classes for AgentBrain refactoring
// This file contains the remaining helper classes that were not in the original AgentBrain.cpp

#pragma once

#include "AgentBrain.hpp"
#include "Constants.hpp"
#include <algorithm>

namespace nlm {

// Additional helper classes to complete the AgentBrain refactoring

// Curiosity processor - handles curiosity-based exploration
class CuriosityProcessor {
public:
    static MotorCommand SelectWithCuriosity(
        MotorCommand defaultCmd,
        float curiosityLevel,
        float explorationChance,
        std::shared_ptr<Brain> brain) {
        
        if (curiosityLevel > Constants::CURIOSITY_EXPLORATION_MAX_LEVEL) {
            float exploreChance = curiosityLevel * explorationChance;
            float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                int choice = brain->getRandomGenerator()->uniformInt(
                    Constants::RANDOM_CHOICE_MIN,
                    Constants::RANDOM_CHOICE_MAX);
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    case 6: return MotorCommand::Interact;
                    default: return MotorCommand::Wait;
                }
            }
        }
        
        return defaultCmd;
    }
};

// Brain processor - handles integration with Brain class
class BrainProcessor {
public:
    static void ApplyRewardModulation(
        std::vector<Neuron*>& sensoryVisionNeurons,
        std::vector<Neuron*>& sensoryTouchNeurons,
        std::vector<Neuron*>& sensoryInternalNeurons,
        std::vector<Neuron*>& sensoryProprioceptionNeurons,
        float reward,
        float predictedReward,
        float& predictionError,
        float& expectedReward,
        float plasticityModifier,
        std::shared_ptr<Brain> brain,
        bool rewardModulationEnabled) {
        
        if (!brain || !rewardModulationEnabled) return;
        
        predictionError = reward - predictedReward;
        expectedReward = Constants::REWARD_DECAY_FACTOR * expectedReward +
                         Constants::REWARD_UPDATE_FACTOR * reward;
        
        float dopamineLevel = predictionError;
        dopamineLevel = Math::clamp(dopamineLevel,
                                   Constants::DOPAMINE_CLAMP_MIN,
                                   Constants::DOPAMINE_CLAMP_MAX);
        
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(eligibility) > Constants::ELIGIBILITY_THRESHOLD) {
                    float delta = eligibility * dopamineLevel * plasticityModifier;
                    syn->addToWeight(delta);
                    syn->decayEligibilityTrace(Constants::ELIGIBILITY_DECAY);
                }
            }
        }
        
        float plasticityFactor = Constants::PLASTICITY_BASE_FACTOR +
                                 Constants::PLASTICITY_BASE_FACTOR * dopamineLevel;
        plasticityFactor = Math::clamp(plasticityFactor,
                                      Constants::PLASTICITY_CLAMP_MIN,
                                      Constants::PLASTICITY_CLAMP_MAX);
        
        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(Constants::STDP_BASE_WEIGHT_LTP * plasticityFactor);
            stdp->setLTDWeight(Constants::STDP_BASE_WEIGHT_LTD * plasticityFactor);
        }
    }
};

// Factories and utilities for AgentBrain
namespace AgentBrainFactories {
    // Factory function to create AgentBrain with default configuration
    std::unique_ptr<AgentBrain> CreateDefaultAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_unique<AgentBrain>(brain);
    }
    
    // Factory function to create AgentBrain with custom configuration
    std::unique_ptr<AgentBrain> CreateAgentBrain(
        std::shared_ptr<Brain> brain,
        bool rewardModulationEnabled,
        bool structuralPlasticityEnabled,
        bool developmentEnabled,
        bool curiosityEnabled) {
        
        auto agentBrain = std::make_unique<AgentBrain>(brain);
        agentBrain->enableRewardModulation(rewardModulationEnabled);
        agentBrain->enableStructuralPlasticity(structuralPlasticityEnabled);
        agentBrain->enableDevelopment(developmentEnabled);
        agentBrain->enableCuriosity(curiosityEnabled);
        
        return agentBrain;
    }
    
    // Utility function to create sensory percept from world state
    SensoryPercept CreateSensoryPercept(
        const std::vector<float>& vision,
        const std::vector<float>& touch,
        const std::vector<float>& internal,
        const std::vector<float>& proprioception,
        double timestamp) {
        
        return SensoryPercept(vision, touch, internal, proprioception, timestamp);
    }
}

} // namespace nlm