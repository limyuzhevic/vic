#pragma once

#include "BrainIntegration.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Integration system for sensory/motor coordination
    class Integration {
    public:
        Integration();
        ~Integration();
        
        // Initialize sensory and motor neuron collections
        void initializeNeurons(Brain* brain);
        
        // Produce motor actions
        std::unique_ptr<class Action> produceActions(Brain* brain);
        
        // Integrate sensory input
        void integrateSensoryInput(Brain* brain, const class SensoryInput& input);
        
        // Get sensory neuron collection
        const std::vector<Neuron*>& getSensoryNeurons() const;
        
        // Get motor neuron collection
        const std::vector<Neuron*>& getMotorNeurons() const;
        
        // Reset integration state
        void reset(Brain* brain);
        
        // Update integration state
        void update(Brain* brain, SimulationStep currentStep);
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
