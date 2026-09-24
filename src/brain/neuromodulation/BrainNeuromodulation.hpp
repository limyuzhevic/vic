#pragma once

#include "BrainNeuromodulation.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Neuromodulation systems for learning regulation
    class Neuromodulation {
    public:
        Neuromodulation();
        ~Neuromodulation();
        
        // Update dopamine effects
        void updateDopamine(Brain* brain, Timestamp timestep);
        
        // Update novelty/curiosity
        void updateNovelty(Brain* brain, Timestamp timestep);
        
        // Apply neuromodulation parameters
        void applyParameters(Brain* brain);
        
        // Get dopamine level
        float getDopamineLevel() const;
        
        // Get plasticity factor
        float getPlasticityFactor() const;
        
        // Initialize neuromodulation systems
        void initializeNeuromodulation(Brain* brain);
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
