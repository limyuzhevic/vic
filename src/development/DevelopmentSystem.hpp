#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

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
    
    // Set plasticity modifier for current stage
    void setPlasticityModifier(float modifier);
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get critical period duration
    double getCriticalPeriodDuration() const;
    
    // Set critical period duration
    void setCriticalPeriodDuration(double duration);
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Trigger phase transition (force advancement to next stage)
    void triggerPhaseTransition();
    
    // Get maturation rate (placeholder for Phase 4 implementation)
    double getMaturationRate() const;
    
    // Set maturation rate
    void setMaturationRate(double rate);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
