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
    
    // NEW: Neuromodulation integration
    void setPlasticityModulation(float modulation) { plasticityModulation_ = modulation; }
    void setStressImpact(float impact) { stressImpact_ = impact; }
    void setEmotionalModulation(float modulation) { emotionalModulation_ = modulation; }
    
    // PUBLIC INTERFACE FOR DEVELOPMENT EFFECTS
    
    // Neural excitability development
    void applyNeuralExcitabilityDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Synaptic properties development
    void applySynapticDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Network topology development
    void applyNetworkDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Cognitive systems development
    void applyCognitiveSystemDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Learning rate modulation
    void applyLearningRateModulation(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Critical period effects
    void applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Developmental specialization
    void applySpecializationEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);

private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    // Neuromodulation integration
    float plasticityModulation_;
    float stressImpact_;
    float emotionalModulation_;
    
    // Private helper functions
    void developProcessingSpecialization(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void optimizeForExpertise(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developWisdomFunctions(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
};

} // namespace nlm
