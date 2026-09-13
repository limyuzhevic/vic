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
    
    // Comprehensive update method that applies all developmental effects
    void updateComprehensive(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Affect neural excitability (changes in firing thresholds)
    void affectNeuralExcitability(Brain* brain);
    
    // Affect attention mechanisms (changes in focus/selectivity)
    void affectAttentionSystem(Brain* brain);
    
    // Affect memory consolidation (changes in memory strength)
    void affectMemoryConsolidation(Brain* brain);
    
    // Affect prediction system behavior
    void affectPredictionSystem(Brain* brain);
    
    // Affect working memory properties
    void affectWorkingMemory(Brain* brain);
    
    // Connect to neuromodulation (ACh, NE, 5-HT) for developmental effects
    void affectNeuromodulation(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
