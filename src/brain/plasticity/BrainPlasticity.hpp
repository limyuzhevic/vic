#pragma once

#include "BrainPlasticity.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Plasticity rules for synaptic modification
    class Plasticity {
    public:
        Plasticity();
        ~Plasticity();
        
        // Apply STDP with neuromodulation
        void applySTDPWithNeuromodulation(float neuromodulationFactor);
        
        // Apply Hebbian learning
        void applyHebbianLearning();
        
        // Apply structural plasticity
        void applyStructuralPlasticity(Brain* brain, RandomGenerator& rng);
        
        // Configure STDP parameters
        void configureSTDP(float ltpWeight, float ltdWeight, float tau);
        
        // Set structural plasticity rates
        void setSynaptogenesisRate(float rate);
        void setPruningRate(float rate);
        
        // Update plasticity based on spike history
        void updatePlasticity(Brain* brain, SimulationStep step, Timestamp time);
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
