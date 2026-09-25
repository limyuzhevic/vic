#pragma once

#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

// BrainDevelopmentManager manages development system
class BrainDevelopmentManager {
public:
    BrainDevelopmentManager() = default;
    ~BrainDevelopmentManager() = default;
    
    // Initialize development system
    void initialize(Brain* brain);
    
    // Update development effects
    void update(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    
    // Apply developmental effects on plasticity
    void applyDevelopmentEffects(Brain* brain);
    
    // Get development statistics
    void collectDevelopmentStats(Brain* brain) const;
};

} // namespace nlm
