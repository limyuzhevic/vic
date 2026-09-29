#include "MotorController.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace nlm {

float MotorController::calculateNeuronActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (!n) {
            continue; // Skip null pointers
        }
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand MotorController::selectBestMotorCommand(
    const std::vector<float>& activities,
    const std::vector<MotorCommand>& commands) {
    
    if (activities.empty() || commands.empty()) {
        return MotorCommand::Wait;
    }
    
    if (activities.size() != commands.size()) {
        return MotorCommand::Wait;
    }
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commandsWithActivity[6] = {
        {MotorCommand::MoveForward, activities[0]},
        {MotorCommand::MoveBackward, activities[1]},
        {MotorCommand::TurnLeft, activities[2]},
        {MotorCommand::TurnRight, activities[3]},
        {MotorCommand::Interact, activities[4]},
        {MotorCommand::Wait, activities[5]}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = activities.back();  // Default to wait
    
    for (int i = 0; i < 6; ++i) {
        if (commandsWithActivity[i].activity > bestActivity) {
            bestActivity = commandsWithActivity[i].activity;
            best = commandsWithActivity[i].cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConstants::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand MotorController::decodeMotorCommand(
    const std::vector<Neuron*>& forward,
    const std::vector<Neuron*>& backward,
    const std::vector<Neuron*>& left,
    const std::vector<Neuron*>& right,
    const std::vector<Neuron*>& interact,
    const std::vector<Neuron*>& wait) {
    
    // Calculate activities for each motor group
    float forwardAct = calculateNeuronActivity(forward);
    float backwardAct = calculateNeuronActivity(backward);
    float leftAct = calculateNeuronActivity(left);
    float rightAct = calculateNeuronActivity(right);
    float interactAct = calculateNeuronActivity(interact);
    float waitAct = calculateNeuronActivity(wait);
    
    // Store activities and commands for selection
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    std::vector<MotorCommand> commands = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    return selectBestMotorCommand(activities, commands);
}

MotorCommand MotorController::applyCuriosityFilter(
    MotorCommand defaultCmd,
    float curiosityLevel,
    class RandomGenerator* rng) {
    
    if (!rng || curiosityLevel <= 0.0f) {
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel > AgentBrainConstants::EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = rng->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = rng->uniformInt(0, 7);
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

bool MotorController::validateNeuronGroups(
    const std::vector<Neuron*>& group,
    const std::string& groupName) {
    // Check for null pointers
    for (Neuron* n : group) {
        if (!n) {
            return false;
        }
    }
    return true;
}

} // namespace nlm