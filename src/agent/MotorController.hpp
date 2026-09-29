#pragma once

#include "../core/Types/Types.hpp"
#include "AgentBrainConstants.h"
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

class MotorController {
public:
    MotorController() = default;
    ~MotorController() = default;
    
    // Calculate average activity across a group of neurons
    static float calculateNeuronActivity(const std::vector<Neuron*>& neurons);
    
    // Find the motor command with highest activity
    static MotorCommand selectBestMotorCommand(
        const std::vector<float>& activities,
        const std::vector<MotorCommand>& commands);
    
    // Decode motor command from neuron groups
    MotorCommand decodeMotorCommand(
        const std::vector<Neuron*>& forward,
        const std::vector<Neuron*>& backward,
        const std::vector<Neuron*>& left,
        const std::vector<Neuron*>& right,
        const std::vector<Neuron*>& interact,
        const std::vector<Neuron*>& wait);
    
    // Apply curiosity-based exploration to motor selection
    MotorCommand applyCuriosityFilter(
        MotorCommand defaultCmd,
        float curiosityLevel,
        class RandomGenerator* rng);
    
    // Validate motor group bounds
    static bool validateNeuronGroups(
        const std::vector<Neuron*>& group,
        const std::string& groupName);
};

} // namespace nlm