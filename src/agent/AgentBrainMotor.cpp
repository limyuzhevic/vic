#include "AgentBrainMotor.hpp"
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {
namespace motor {

MotorCommand AgentBrainMotor::decodeMotorCommand(AgentBrain& agentBrain) {
    if (!agentBrain.brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = AgentBrainMotor::decodeFromMotorNeurons(agentBrain);
    
    // Apply curiosity-based exploration
    if (agentBrain.curiosityEnabled_ && agentBrain.curiosityLevel_ > 0.3f) {
        decoded = AgentBrainMotor::selectWithCuriosity(agentBrain, decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrainMotor::decodeFromMotorNeurons(AgentBrain& agentBrain) {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(agentBrain.motorForward_);
    float backwardAct = calcActivity(agentBrain.motorBackward_);
    float leftAct = calcActivity(agentBrain.motorTurnLeft_);
    float rightAct = calcActivity(agentBrain.motorTurnRight_);
    float interactAct = calcActivity(agentBrain.motorInteract_);
    float waitAct = calcActivity(agentBrain.motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrainMotor::selectWithCuriosity(AgentBrain& agentBrain, MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (agentBrain.curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = agentBrain.curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = agentBrain.brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = agentBrain.brain_->getRandomGenerator()->uniformInt(0, 7);
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

} // namespace motor
} // namespace nlm
