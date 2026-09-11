#pragma once

#include "Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <vector>

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem() = default;
    
    void initialize(Brain* brain);
    
    // Update developmental stages
    void update(Brain& brain, TimestepDuration dt);
    void updateDevelopment(Brain& brain, TimestepDuration dt);
    
    // Synaptogenesis and pruning
    void updateSynaptogenesis(Brain& brain, float rate);
    void updatePruning(Brain& brain, float rate);
    
    // Structural plasticity
    void updateStructuralPlasticity(Brain& brain);
    
    // Developmental stage management
    void setDevelopmentalStage(Brain& brain, DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage(Brain& brain) const;
    
    // Age-dependent dynamics
    void updateAgeDependentDynamics(Brain& brain, Timestamp age);
    float getPlasticityFactorForStage(DevelopmentalStage stage) const;
    
    // Development effects on plasticity
    void applyDevelopmentEffectsToPlasticity(Brain& brain, float& synaptogenesisRate, float& pruningRate);
    
    void logDevelopmentStatus(Brain& brain) const;
    
private:
    // Synaptogenesis (formation of new connections)
    void performSynaptogenesis(Brain& brain, float rate);
    
    // Pruning (elimination of unused connections)
    void performPruning(Brain& brain, float rate);
    
    // Activity-dependent stabilization
    void activityDependentStabilization(Brain& brain);
    
    // Critical period plasticity
    void criticalPeriodPlasticity(Brain& brain, float windowStrength);
    
    // Maturation-dependent changes
    void maturationEffects(Brain& brain, float maturityLevel);
    
    // Age-related decline
    void ageRelatedDecline(Brain& brain, Timestamp age);
};

} // namespace nlm
