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
    
    // Enhanced development update with plasticity modulation
    void updateDevelopment(Brain* brain, float plasticityRate);
    
    // Apply developmental modulation to plasticity systems
    void applyDevelopmentalModulation(Brain* brain, float plasticityRate);
    
    // Enhance plasticity based on prediction errors
    void enhancePlasticityBasedOnPrediction(Brain* brain, float predictionError, float plasticityRate);
    
    // Get developmental stage specific plasticity boost
    float getStageSpecificPlasticityBoost() const;
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get Hebbian learning rate for developmental stage
    float getHebbianLearningRate(DevelopmentalStage stage) const;
    
    // Get STDP learning rate for developmental stage
    float getSTDPlearningRate(DevelopmentalStage stage) const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Should enhance synaptic connections based on stage?
    bool shouldEnhanceSynapticConnections(DevelopmentalStage stage) const;
    
    // Should prune weak connections based on stage?
    bool shouldPruneWeakConnections(DevelopmentalStage stage) const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
