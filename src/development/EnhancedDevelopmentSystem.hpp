#pragma once

#include "Development.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/DevelopmentalStageManager.hpp"
#include "../development/NeuralArchitectureEvolution.hpp"
#include "../development/CognitiveDevelopment.hpp"
#include "../development/BehavioralDevelopment.hpp"
#include "../development/ExperienceDependentDevelopment.hpp"
#include "../development/DevelopmentalMilestones.hpp"
#include "../development/Maturation.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Enhanced DevelopmentSystem that orchestrates all developmental processes
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Initialize the development system
    void initialize(Brain* brain);
    
    // Update all developmental processes
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt, SimulationStep currentStep);
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    
    // Get stage name
    const char* getStageName() const;
    
    // Get stage description
    const char* getStageDescription() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get comprehensive developmental metrics
    DevelopmentalMetrics getMetrics() const;
    
    // Get current cognitive capabilities
    CognitiveCapabilities getCognitiveCapabilities() const;
    
    // Get current behavioral profile
    BehavioralProfile getBehavioralProfile() const;
    
    // Get specialization profile for brain regions
    std::vector<NeuralRegionSpecialization> getSpecializationProfile() const;
    
    // Check if in critical period
    bool isCriticalPeriod() const;
    
    // Get critical period progress
    float getCriticalPeriodProgress() const;
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get stage-specific learning rate
    float getLearningRate() const;
    
    // Get stage-specific synaptogenesis rate
    float getSynaptogenesisRate() const;
    
    // Get stage-specific pruning rate
    float getPruningRate() const;
    
    // Advance to next developmental stage
    void advanceStage();
    
    // Record an experience for experience-dependent development
    void recordExperience(Brain* brain, float reward, double timestamp);
    
    // Check if developmental milestone is achieved
    bool isMilestoneAchieved(const char* milestoneName) const;
    
    // Get next milestone(s) for current stage
    std::vector<const char*> getNextMilestones() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm