#pragma once

#include "Brain.hpp"
#include "../motor/Action.hpp"
#include <memory>

namespace nlm {

class OutputProducer {
public:
    OutputProducer(std::shared_ptr<Brain> brain);
    
    // Generate action based on current neural activity
    std::unique_ptr<Action> produceAction();
    
    // Get motor neuron activity statistics
    float getMotorExcitability() const { return motorExcitability; }
    float getMotorInhibition() const { return motorInhibition; }
    
    // Reset output producer
    void reset() {
        motorExcitability = 0.0f;
        motorInhibition = 0.0f;
    }
    
private:
    std::shared_ptr<Brain> brain;
    float motorExcitability;
    float motorInhibition;
    
    // Analyze motor neuron populations for action selection
    void analyzeMotorActivity();
    
    // Select action based on neural activity patterns
    ActionType selectActionType() const;
};

} // namespace nlm