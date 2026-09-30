#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <stdexcept>

namespace nlm {

// Helper function to safely get neuron by index with bounds checking
Neuron* getNeuronSafely(const std::vector<Neuron*>& neurons, size_t index) {
    if (neurons.empty() || index >= neurons.size()) {
        NLM_LOG_WARNING("Neuron access out of bounds: index=" + std::to_string(index) + 
                       ", size=" + std::to_string(neurons.size()));
        return nullptr;
    }
    return neurons[index];
}

// Helper function for motor command activity calculation
float calculateGroupActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        if (!n) {
            continue; // Skip null pointers
        }
        const auto& state = n->getState();
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(state.membranePotential - state.restingPotential);
    }
    return sum / neurons.size();
}

// Helper function for exploration-based motor selection
MotorCommand exploreWithCuriosity(RandomGenerator* rng, float exploreChance, const std::vector<MotorCommand>& validCommands) {
    if (!rng || exploreChance <= 0.0f) {
        return MotorCommand::Wait;
    }
    
    float r = rng->uniformReal(0.0f, 1.0f);
    if (r >= exploreChance) {
        return MotorCommand::Wait; // No exploration this time
    }
    
    // Random motor command selection from valid commands
    size_t choice = rng->uniformInt(0, validCommands.size() - 1);
    if (choice < validCommands.size()) {
        return validCommands[choice];
    }
    
    return MotorCommand::Wait;
}

// Validation helper for brain pointers
bool validateBrainPtr(const std::shared_ptr<Brain>& brain, const char* operation) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null during " + std::string(operation));
        return false;
    }
    return true;
}

// Helper to safely get neuron groups from brain regions
void initializeNeuronGroups(std::vector<Neuron*>& motorForward, 
                           std::vector<Neuron*>& motorBackward,
                           std::vector<Neuron*>& motorTurnLeft,
                           std::vector<Neuron*>& motorTurnRight,
                           std::vector<Neuron*>& motorInteract,
                           std::vector<Neuron*>& motorWait,
                           std::vector<Neuron*>& sensoryVision,
                           std::vector<Neuron*>& sensoryTouch,
                           std::vector<Neuron*>& sensoryInternal,
                           std::vector<Neuron*>& sensoryProprioception,
                           const Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize neuron groups: brain is null");
        return;
    }
    
    for (const auto& region : brain->getRegions()) {
        if (!region) continue;
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;
            NeuronType type = pop->getNeuronType();
            
            for (Neuron* n : pop->getNeurons()) {
                if (!n) continue; // Skip null neurons
                
                if (type == NeuronType::Motor) {
                    // Distribute motor neurons to different action groups
                    size_t groupSize = motorForward.size() + motorBackward.size() + 
                                      motorTurnLeft.size() + motorTurnRight.size() +
                                      motorInteract.size() + motorWait.size();
                    
                    switch (groupSize % 6) {
                        case 0: motorForward.push_back(n); break;
                        case 1: motorBackward.push_back(n); break;
                        case 2: motorTurnLeft.push_back(n); break;
                        case 3: motorTurnRight.push_back(n); break;
                        case 4: motorInteract.push_back(n); break;
                        case 5: motorWait.push_back(n); break;
                    }
                } else if (type == NeuronType::Sensory) {
                    // Distribute sensory neurons
                    size_t groupSize = sensoryVision.size() + sensoryTouch.size() +
                                      sensoryInternal.size() + sensoryProprioception.size();
                    
                    switch (groupSize % 4) {
                        case 0: sensoryVision.push_back(n); break;
                        case 1: sensoryTouch.push_back(n); break;
                        case 2: sensoryInternal.push_back(n); break;
                        case 3: sensoryProprioception.push_back(n); break;
                    }
                }
            }
        }
    }
}

// Helper to clamp values within range
float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

} // namespace nlm
