#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "Development.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Integrates developmental stages, neural excitability, attention mechanisms,
// memory consolidation, neuromodulation, and plasticity rule modulation

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
    
    // Update development with enhanced effects
    void updateEnhanced(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Apply developmental effects to neural excitability
    void applyExcitabilityEffects(Brain* brain);
    
    // Apply developmental effects to attention system
    void applyAttentionEffects(Brain* brain);
    
    // Apply developmental effects to memory consolidation
    void applyMemoryConsolidationEffects(Brain* brain);
    
    // Apply developmental effects to neuromodulation
    void applyNeuromodulationEffects(Brain* brain);
    
    // Apply developmental effects to plasticity rules
    void applyPlasticityRuleEffects(Brain* brain);
    
    // Get developmental metrics for testing
    struct DevelopmentMetrics {
        double age;
        DevelopmentalStage stage;
        float neuralExcitability;
        float attentionCapacity;
        float memoryConsolidation;
        float dopamineRelease;
        float acetylcholineRelease;
        float norepinephrineRelease;
        float serotoninRelease;
        float plasticityStrength;
        float learningRate;
        float explorationBias;
        float memoryRetention;
    };
    
    DevelopmentMetrics getMetrics() const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    std::unique_ptr<Development> development_;
};

} // namespace nlm

