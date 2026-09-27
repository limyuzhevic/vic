#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Developmental regulation: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Apply developmental effects to neurons
    void applyMaturation(Brain* brain);
    
    // Apply developmental effects to synapses
    void applySynaptogenesis(Brain* brain, RandomGenerator& rng);
    
    // Apply developmental effects to pruning
    void applyPruning(Brain* brain, RandomGenerator& rng);
    
    // Get developmental parameters for structural plasticity
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Set developmental parameters
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    // Get developmental milestone progress
    float getMaturationProgress() const;
    float getCriticalPeriodEnd() const;
    
    // Check if development milestones are reached
    bool isMaturationComplete() const;
    bool isCriticalPeriodComplete() const;
    
    // Initialize development system for a brain
    void initializeDevelopment(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
