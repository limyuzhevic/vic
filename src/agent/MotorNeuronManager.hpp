#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class MotorNeuronManager {
public:
    MotorNeuronManager(std::shared_ptr<Brain> brain);
    
    void distributeAndGroupMotorNeurons();
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
};

} // namespace nlm
