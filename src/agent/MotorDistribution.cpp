// Update MotorDistribution.cpp to use constants
#include "MotorDistribution.hpp"
#include "../core/Constants.hpp"

namespace nlm {

MotorDistribution::MotorDistribution() {}

MotorDistribution::~MotorDistribution() {
    clear();
}

void MotorDistribution::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    clear();
    
    for (Neuron* n : neurons) {
        // Use constant for motor group distribution
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                     motorTurnLeft_.size() + motorTurnRight_.size() +
                     motorInteract_.size() + motorWait_.size();
        
        switch (idx % static_cast<size_t>(MotorConstants::MOTOR_COMMANDS_COUNT)) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
    }
}

} // namespace nlm