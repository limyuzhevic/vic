#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes
// Enhanced for Phase 6: Affects multiple brain systems beyond just structural plasticity

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
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Apply developmental effects to all brain systems
    // This is the new Phase 6 enhancement
    void applyDevelopmentalEffects(Brain* brain, SimulationStep currentStep);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Development affects:
    // - Neural excitability
    // - Memory consolidation
    // - Attention modulation
    // - Plasticity rules
    // - Neuromodulation sensitivity
    // - Learning rates
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double age_;
};

// DevelopmentStage: Brain development phases
// Each stage has specific effects on brain systems
enum class DevelopmentalStage {
    Initial,          // Primitive reflexes, high plasticity, basic learning
    CriticalPeriod,   // Sensitive period, rapid learning, strong plasticity
    Maturation,       // Optimization, fine-tuning, refined circuits
    Adult,            // Stable performance, slow learning, maintenance
    Aging            // Decline, reduced plasticity, memory degradation
};

} // namespace nlm
