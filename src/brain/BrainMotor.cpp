#include "BrainMotor.hpp"
#include "Brain.hpp"

namespace nlm {

// Brain::Impl motor - BrainMotor class implementation

BrainMotor::BrainMotor(Brain::Impl* impl) : pImpl(impl) {}

std::unique_ptr<class Action> BrainMotor::produceAction() const {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

} // namespace nlm
