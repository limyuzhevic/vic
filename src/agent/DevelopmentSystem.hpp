#pragma once

#include "../core/Types/Types.hpp"
#include "AgentBrainConstants.h"
#include <algorithm>

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem() = default;
    ~DevelopmentSystem() = default;
    
    // Initialize development state
    void initialize();
    
    // Update development system based on timestep
    void updateDevelopment(
        double timestep,
        double& developmentalAge,
        float& plasticityModifier,
        class Brain* brain,
        bool structuralPlasticityEnabled);
    
    // Get current developmental stage based on age
    static DevelopmentalStage getDevelopmentalStageFromAge(double developmentalAge);
    
    // Calculate plasticity modifier for given age
    static float calculatePlasticityModifier(double developmentalAge);
    
    // Calculate structural plasticity rates
    static void calculateStructuralPlasticityRates(
        float plasticityModifier,
        float& synaptogenesisRate,
        float& pruningRate);
    
    // Validate development parameters
    static bool validateDevelopmentParams(double timestep, double developmentalAge);
    
    // Get development statistics
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
private:
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = AgentBrainConstants::PLASTICITY_INITIAL;
};

} // namespace nlm