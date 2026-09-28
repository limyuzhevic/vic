#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// I/O System Integration
// Handles sensory input and motor output integration

class BrainIO {
public:
    // Receive sensory input from environment
    void receiveSensoryInput(Brain::Impl& pImpl, const SensoryInput& input);
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<Action> produceAction(const Brain::Impl& pImpl);
    
    // Get I/O system statistics
    void logIOStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for I/O integration
    void injectCurrentFromSensoryInput(Brain::Impl& pImpl, const SensoryInput& input);
    void updateMotorNeurons(Brain::Impl& pImpl);
    void handleActionSelection(Brain::Impl& pImpl);
};

} // namespace nlm
