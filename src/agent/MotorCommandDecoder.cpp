#include "MotorCommandDecoder.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

MotorCommandDecoder::MotorCommandDecoder(std::shared_ptr<Brain> brain)
    : brain_(brain)
{
}

MotorCommand MotorCommandDecoder::decode(
    const std::vector<Neuron*>& motorForward,
    const std::vector<Neuron*>& motorBackward,
    const std::vector<Neuron*>& motorTurnLeft,
    const std::vector<Neuron*>& motorTurnRight,
    const std::vector<Neuron*>& motorInteract,
    const std::vector<Neuron*>& motorWait,
    bool curiosityEnabled,
    float curiosityLevel) {
    
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
    
    float forwardAct = calcActivity(motorForward);
    float backwardAct = calcActivity(motorBackward);
    float leftAct = calcActivity(motorTurnLeft);
    float rightAct = calcActivity(motorTurnRight);
    float interactAct = calcActivity(motorInteract);
    float waitAct = calcActivity(motorWait);
    
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
    
    // Apply curiosity-based exploration
    if (curiosityEnabled && curiosityLevel > 0.3f) {
        best = selectWithCuriosity(best, curiosityEnabled, curiosityLevel, brain_);
    }
    
    return best;
}

MotorCommand MotorCommandDecoder::selectWithCuriosity(MotorCommand defaultCmd, bool curiosityEnabled,
                                                      float curiosityLevel, std::shared_ptr<Brain> brain) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled && curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain->getRandomGenerator()->uniformInt(0, 7);
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

} // namespace nlm
