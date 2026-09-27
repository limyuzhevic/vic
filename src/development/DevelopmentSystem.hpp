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
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // ===== CONFIGURATION METHODS =====
    
    // Configure stage duration for a specific stage
    void configureStageDuration(DevelopmentalStage stage, double duration);
    
    // Get stage duration for a specific stage
    double getStageDuration(DevelopmentalStage stage) const;
    
    // Configure critical period duration
    void configureCriticalPeriodDuration(double duration);
    
    // Get critical period duration
    double getCriticalPeriodDuration() const;
    
    // Configure maturation duration
    void configureMaturationDuration(double duration);
    
    // Get maturation duration
    double getMaturationDuration() const;
    
    // ===== STATE MANAGEMENT METHODS =====
    
    // Check if current stage is complete
    bool isStageComplete() const;
    
    // Get progress through current stage (0.0 to 1.0)
    float getStageProgress() const;
    
    // ===== ADVANCED DEVELOPMENT METHODS =====
    
    // Enable experience-based development progression
    void enableExperienceBasedProgression(
        Brain* brain, RandomGenerator& rng, double experienceThreshold);
    
    // Check if development is ready to advance based on brain state
    bool isReadyToAdvance(Brain* brain) const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
