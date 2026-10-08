#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>

namespace nlm {

class InputProcessor {
public:
    InputProcessor(std::shared_ptr<Brain> brain);
    
    // Process sensory input and inject into brain
    void processInput(const SensoryInput& input);
    
    // Get current sensory neuron count
    size_t getSensoryNeuronCount() const { return sensoryNeuronCount; }
    
    // Get current motor neuron count  
    size_t getMotorNeuronCount() const { return motorNeuronCount; }
    
    // Reset processor state
    void reset() {
        lastInputTime = -1.0;
    }
    
private:
    std::shared_ptr<Brain> brain;
    size_t sensoryNeuronCount;
    size_t motorNeuronCount;
    Timestamp lastInputTime;
    
    // Extract sensory neurons from brain regions
    void extractSensorimotorNeurons();
};

} // namespace nlm