#pragma once

#include "../brain/Neuron.hpp"
#include "AgentBody.hpp"
#include <vector>
#include <map>

namespace nlm {

class MotorCommandGroup {
public:
    // Motor action groups for different behaviors
    enum class Action {
        MoveForward,
        MoveBackward,
        TurnLeft,
        TurnRight,
        Interact,
        Wait
    };
    
    MotorCommandGroup(Action action, std::vector<Neuron*>& neurons)
        : action_(action), neurons_(neurons) {
        if (neurons.empty()) {
            throw AgentBrainException::nullPointerError(
                "MotorCommandGroup initialization: empty neuron vector");
        }
    }
    
    Action getAction() const { return action_; }
    size_t size() const { return neurons_.size(); }
    bool empty() const { return neurons_.empty(); }
    
    Neuron* operator[](size_t index) {
        if (index >= neurons_.size()) {
            throw std::out_of_range("MotorCommandGroup index out of range");
        }
        return neurons_[index];
    }
    
    const Neuron* operator[](size_t index) const {
        if (index >= neurons_.size()) {
            throw std::out_of_range("MotorCommandGroup index out of range");
        }
        return neurons_[index];
    }
    
    auto begin() { return neurons_.begin(); }
    auto end() { return neurons_.end(); }
    auto begin() const { return neurons_.begin(); }
    auto end() const { return neurons_.end(); }
    
    bool isValid() const {
        for (Neuron* neuron : neurons_) {
            if (!neuron) {
                return false;
            }
        }
        return !neurons_.empty();
    }
    
    float calculateActivity() const {
        if (empty()) return 0.0f;
        
        float total = 0.0f;
        for (const Neuron* neuron : neurons_) {
            if (neuron) {
                total += std::abs(neuron->getState().membranePotential - 
                                 neuron->getState().restingPotential);
            }
        }
        return total / neurons_.size();
    }
    
    // Static method to distribute neurons into action groups
    static std::map<Action, std::vector<Neuron*>> distributeMotorNeurons(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        std::map<Action, std::vector<Neuron*>> motorGroups;
        
        // Initialize all action groups
        motorGroups[Action::MoveForward] = std::vector<Neuron*>();
        motorGroups[Action::MoveBackward] = std::vector<Neuron*>();
        motorGroups[Action::TurnLeft] = std::vector<Neuron*>();
        motorGroups[Action::TurnRight] = std::vector<Neuron*>();
        motorGroups[Action::Interact] = std::vector<Neuron*>();
        motorGroups[Action::Wait] = std::vector<Neuron*>();
        
        // Distribute motor neurons to appropriate action groups
        for (const auto& region : regions) {
            for (const auto& population : region->getPopulations()) {
                if (population->getNeuronType() == NeuronType::Motor) {
                    for (Neuron* neuron : population->getNeurons()) {
                        // Use a deterministic distribution based on neuron index
                        size_t neuronIndex = population->getNeurons().size() > 0 ? 
                                             &neuron - &population->getNeurons()[0] : 0;
                        
                        size_t groupIndex = neuronIndex % AgentBrainConfig::MOTOR_GROUP_COUNT;
                        auto it = motorGroups.begin();
                        std::advance(it, groupIndex);
                        it->second.push_back(neuron);
                    }
                }
            }
        }
        
        return motorGroups;
    }
    
private:
    Action action_;
    std::vector<Neuron*>& neurons_;
};

} // namespace nlm
