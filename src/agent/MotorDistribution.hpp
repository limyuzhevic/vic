#pragma once

#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class MotorDistribution {
public:
    MotorDistribution();
    ~MotorDistribution();
    
    // Distribute motor neurons into functional groups
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    
    // Accessor methods
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    // Get total count
    size_t getTotalMotorNeurons() const {
        return motorForward_.size() + motorBackward_.size() + 
               motorTurnLeft_.size() + motorTurnRight_.size() +
               motorInteract_.size() + motorWait_.size();
    }
    
    // Clear all groups
    void clear() {
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
    }

private:
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
};

} // namespace nlm