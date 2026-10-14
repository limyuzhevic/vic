#pragma once

#include "MotorCommandGroup.hpp"
#include "AgentBrainConfig.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

MotorDecoder(const std::map<MotorCommandGroup::Action, std::vector<Neuron*>>& motorGroups)
        : motorGroups_(motorGroups) {
        validateSetup();
    }
    
    MotorCommandGroup::Action decodeFromMotorNeurons() {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            
            float sum = 0.0f;
            for (Neuron* neuron : neurons) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(neuron->getState().membranePotential - 
                               neuron->getState().restingPotential);
            }
            return sum / neurons.size();
        };
        
        float forwardAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::MoveForward));
        float backwardAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::MoveBackward));
        float leftAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::TurnLeft));
        float rightAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::TurnRight));
        float interactAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::Interact));
        float waitAct = calcActivity(motorGroups_.at(MotorCommandGroup::Action::Wait));
        
        // Find maximum activity
        struct CommandWithActivity {
            MotorCommandGroup::Action action;
            float activity;
        };
        
        CommandWithActivity commands[] = {
            {MotorCommandGroup::Action::MoveForward, forwardAct},
            {MotorCommandGroup::Action::MoveBackward, backwardAct},
            {MotorCommandGroup::Action::TurnLeft, leftAct},
            {MotorCommandGroup::Action::TurnRight, rightAct},
            {MotorCommandGroup::Action::Interact, interactAct},
            {MotorCommandGroup::Action::Wait, waitAct}
        };
        
        MotorCommandGroup::Action bestAction = MotorCommandGroup::Action::Wait;
        float bestActivity = waitAct;  // Default to wait if nothing stronger
        
        for (const auto& cmd : commands) {
            if (cmd.activity > bestActivity) {
                bestActivity = cmd.activity;
                bestAction = cmd.action;
            }
        }
        
        // Only act if there's meaningful activity
        if (bestActivity < AgentBrainConfig::MOTOR_THRESHOLD) {
            return MotorCommandGroup::Action::Wait;
        }
        
        return bestAction;
    }
    
    MotorCommandGroup::Action selectWithCuriosity(MotorCommandGroup::Action defaultAction, 
                                                  float curiosityLevel,
                                                  class RandomGenerator* randomGenerator) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel > AgentBrainConfig::EXPLORATION_THRESHOLD) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel * AgentBrainConfig::EXPLORATION_CHANCE_MAX;
            
            float r = randomGenerator->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = randomGenerator->uniformInt(
                    AgentBrainConfig::RANDOM_ACTION_RANGE_MIN,
                    AgentBrainConfig::RANDOM_ACTION_RANGE_MAX);
                
                switch (choice) {
                    case 0: return MotorCommandGroup::Action::MoveForward;
                    case 1: return MotorCommandGroup::Action::MoveBackward;
                    case 2: return MotorCommandGroup::Action::TurnLeft;
                    case 3: return MotorCommandGroup::Action::TurnRight;
                    case 4: return MotorCommandGroup::Action::LookLeft;
                    case 5: return MotorCommandGroup::Action::LookRight;
                    case 6: return MotorCommandGroup::Action::Interact;
                    default: return MotorCommandGroup::Action::Wait;
                }
            }
        }
        
        return defaultAction;
    }
    
    float getMotorActivity(MotorCommandGroup::Action action) const {
        const auto& it = motorGroups_.find(action);
        if (it != motorGroups_.end()) {
            return calcActivity(it->second);
        }
        return 0.0f;
    }
    
    bool validateSetup() {
        bool isValid = true;
        
        for (const auto& pair : motorGroups_) {
            const auto& action = pair.first;
            const auto& neurons = pair.second;
            
            if (neurons.empty()) {
                throw AgentBrainException::nullPointerError(
                    "MotorCommandGroup initialization for " + std::to_string(static_cast<int>(action)));
            }
            
            for (Neuron* neuron : neurons) {
                if (!neuron) {
                    throw AgentBrainException::nullPointerError(
                        "Invalid neuron in MotorCommandGroup for " + std::to_string(static_cast<int>(action)));
                }
            }
        }
        
        return isValid;
    }
    
    // Reset method for testing and state clearing
    void reset() {
        // Clear any accumulated state
    }
    
private:
    float calcActivity(const std::vector<Neuron*>& neurons) const {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (const Neuron* neuron : neurons) {
            sum += std::abs(neuron->getState().membranePotential - 
                           neuron->getState().restingPotential);
        }
        return sum / neurons.size();
    }
    
    std::map<MotorCommandGroup::Action, std::vector<Neuron*>> motorGroups_;
};

} // namespace nlm
