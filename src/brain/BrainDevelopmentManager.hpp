#pragma once

#include "Brain.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {

class BrainDevelopmentManager {
public:
    BrainDevelopmentManager();
    ~BrainDevelopmentManager();
    
    // Disable copying
    BrainDevelopmentManager(const BrainDevelopmentManager&) = delete;
    BrainDevelopmentManager& operator=(const BrainDevelopmentManager&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update development system
    void updateDevelopment(Brain* brain, SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Apply development effects on plasticity rates
    void applyDevelopmentPlasticityEffects(Brain* brain, DevelopmentalStage stage);
    
    // Get development system pointer
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
