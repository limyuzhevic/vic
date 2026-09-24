#pragma once

#include "BrainDevelopment.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Development system for brain maturation and plasticity regulation
    class Development {
    public:
        Development();
        ~Development();
        
        // Update developmental stage
        void updateDevelopment(Brain* brain, RandomGenerator& rng, Timestamp elapsedTime);
        
        // Set developmental stage manually
        void setDevelopmentalStage(DevelopmentalStage stage);
        
        // Get current developmental stage
        DevelopmentalStage getDevelopmentalStage() const;
        
        // Initialize development system
        void initializeDevelopment();
        
        // Modulate plasticity rates based on development
        void modulatePlasticityRates(float& synaptogenesisRate, float& pruningRate);
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
