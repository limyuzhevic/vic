#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "BrainCore.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain;

// Forward declarations for I/O
class SensoryInput;
class Action;

// BrainIO implementation - Phase 6: Integrated Artificial Brain
// Handles sensory/motor input/output operations
struct BrainIO {
    // ========== SENSORY INPUT HANDLING ==========
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(Brain* parent, const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(Brain* parent, NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(Brain* parent, NeuronType type, MembranePotential current);
    
    // ========== ACTION PRODUCTION ==========
    
    // Produce motor/action output based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    std::unique_ptr<Action> produceAction(Brain* parent);
    
    // Helper methods for I/O operations
    void distributeInputToSensoryNeurons(Brain* parent, const std::vector<float>& values);
    size_t getSensoryNeuronCount(Brain* parent) const;
    size_t getMotorNeuronCount(Brain* parent) const;
    size_t calculateFiringMotorCount(Brain* parent) const;
};

} // namespace nlm