#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <string>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development event types
enum class DevelopmentalEvent {
    CriticalPeriodClosure,
    PlasticityShift,
    MemoryReorganization,
    AttentionCalibration,
    ConceptIntegration
};

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
    
    // Get stage description
    const char* getStageDescription() const;
    
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
    
    // Trigger development event
    void triggerEvent(DevelopmentalEvent event, Brain* brain);
    void triggerEvent(DevelopmentalEvent event, Brain* brain, RandomGenerator& rng);
    
    // Helper function to convert string to developmental event
    static DevelopmentalEvent stringToEvent(const std::string& eventName);
    static const char* getEventName(DevelopmentalEvent event);
    
    // Additional helper methods for specific effects
    void applyExcitabilityChange(Brain* brain, float change);
    void applySynapticPruning(Brain* brain, float factor);
    void applySynapticWeightStabilization(Brain* brain, float stabilization);
    void applyHomeostaticMechanisms(Brain* brain, float homeostasis);
    void applyCompensatoryPlasticity(Brain* brain, float decline);
    
    // Stage-specific effect applications
    void applyInitialStageEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyMaturationEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyAdultEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyAgingEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
