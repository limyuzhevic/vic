#pragma once

#include "MotorDecoder.hpp"
#include "../agent/NeuronActivityCalculator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

MotorDecoder::MotorDecoder(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , curiosityEnabled_(true)
    , randomGen_(std::random_device{}())
{
}

float MotorDecoder::calculateActivity(const std::vector<Neuron*>& neurons) {
    return NeuronActivityCalculator::calculatePopulationActivity(neurons);
}

MotorCommand MotorDecoder::decode(const std::vector<Neuron*>& neurons) {
    // This is a simplified version for testing
    // The actual implementation is in the original AgentBrain::decodeFromMotorNeurons
    float maxActivity = -1.0f;
    MotorCommand best = MotorCommand::Wait;
    
    for (Neuron* n : neurons) {
        float activity = NeuronActivityCalculator::calculateActivity(*n);
        if (activity > maxActivity) {
            maxActivity = activity;
            // For testing, map neuron index to command
            int idx = 0; // Placeholder logic
            switch (idx % 6) {
                case 0: best = MotorCommand::MoveForward; break;
                case 1: best = MotorCommand::MoveBackward; break;
                case 2: best = MotorCommand::TurnLeft; break;
                case 3: best = MotorCommand::TurnRight; break;
                case 4: best = MotorCommand::Interact; break;
                default: best = MotorCommand::Wait; break;
            }
        }
    }
    
    return maxActivity > 0.5f ? best : MotorCommand::Wait;
}

MotorCommand MotorDecoder::selectWithCuriosity(MotorCommand defaultCmd, 
                                              float curiosityLevel,
                                              bool enabled) {
    if (!enabled || curiosityLevel <= 0.5f) {
        return defaultCmd;
    }
    
    float exploreChance = curiosityLevel * 0.3f;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(randomGen_) < exploreChance) {
        int choice = std::uniform_int_distribution<int>(0, 7)(randomGen_);
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
    
    return defaultCmd;
}

} // namespace nlm