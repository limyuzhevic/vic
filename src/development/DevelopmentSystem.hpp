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
    
    // ========== DEVELOPMENT SYSTEM ==========

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
    
    // ========== DEVELOPMENT EFFECTS ==========
    
    // Apply developmental effects to neural excitability
    float getNeuralExcitabilityModifier() const;
    
    // Apply developmental effects to attention
    float getAttentionModulator() const;
    
    // Apply developmental effects to memory consolidation
    float getMemoryConsolidationRate() const;
    
    // Apply developmental effects on plasticity rules
    void applyDevelopmentalPlasticityRules(Brain* brain);
    
    // Check if developmental stage is optimal for learning
    bool isOptimalLearningStage() const;
    
    // Get developmental stage-specific learning rate
    float getStageSpecificLearningRate() const;
    
    // Get developmental stage-specific attention span
    float getStageSpecificAttentionSpan() const;
    
    // Get developmental stage-specific memory capacity
    float getStageSpecificMemoryCapacity() const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
