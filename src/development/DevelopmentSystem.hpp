#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <vector>

namespace nlm;

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
    
    // Enhanced development system functionality
    
    // Integrate neuromodulatory signals
    void integrateNeuromodulation(float curiosity, float dopamine);
    
    // Integrate prediction signals
    void integratePrediction(float predictionError, float predictionAccuracy);
    
    // Integrate memory consolidation
    void integrateMemory(double consolidationLevel);
    
    // Integrate environmental interaction
    void integrateEnvironmentalInteraction(double interactionStrength);
    
    // Get adaptive development parameters
    float getMaturationRate() const;
    float getCriticalPeriodReopening() const;
    float getAttentionModulation() const;
    float getEnvironmentalInteraction() const;
    
    // Get experience-dependent parameters
    double getLearningExperience() const { return pImpl->learningExperience; }
    double getEnvironmentalInteractionLevel() const { return pImpl->environmentalInteraction; }
    double getPredictionAccuracy() const { return pImpl->predictionAccuracy; }
    double getMemoryConsolidationLevel() const { return pImpl->memoryConsolidation; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
