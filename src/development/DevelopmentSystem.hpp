#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;
class Synaptogenesis;
class Pruning;
class Maturation;
class Dopamine;
class Reward;
class Curiosity;
class Novelty;
class NeuralEpisodicMemory;
class NeuralWorkingMemory;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Development system: orchestrates neural development
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
    
    // Initialize development system with brain references
    void initializeDevelopment(Brain* brain);
    
    // Integration with neural memory systems
    void integrateWithEpisodicMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void integrateWithWorkingMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Development mechanism accessors
    Synaptogenesis* getSynaptogenesis() { return &pImpl->synaptogenesis; }
    Pruning* getPruning() { return &pImpl->pruning; }
    Maturation* getMaturation() { return &pImpl->maturation; }
    
    // Get development system (for integration)
    static DevelopmentSystem* getDevelopmentSystem();
    
    // Get/set developmental stage via Brain interface
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
