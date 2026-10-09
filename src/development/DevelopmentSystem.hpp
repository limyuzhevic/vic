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
    
    // Get excitability modifier for current stage
    float getExcitabilityModifier() const;
    
    // Get attention stability modifier for current stage
    float getAttentionStabilityModifier() const;
    
    // Get working memory capacity modifier for current stage
    float getWorkingMemoryCapacityModifier() const;
    
    // Get memory consolidation strength modifier for current stage
    float getMemoryConsolidationModifier() const;
    
    // Get neuromodulation levels modifier for current stage
    float getNeuromodulationModifier(NeuromodulatorType type) const;
    
    // Get sleep/rest cycle duration modifier for current stage
    float getSleepCycleModifier() const;
    
    // Get life history memory persistence modifier for current stage
    float getLifeHistoryPersistenceModifier() const;
    
    // Get plasticity rule modifier for current stage (beyond structural)
    float getPlasticityRuleModifier() const;
    
    // Update life history tracking
    void updateLifeHistory(Brain* brain, TimestepDuration dt);
    
    // Get current developmental milestone (0.0 to 1.0)
    float getCurrentDevelopmentalMilestone() const;
    
    // Check if a developmental milestone has been reached
    bool hasMilestonePassed(DevelopmentalMilestone milestone) const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    // Life history tracking
    struct LifeHistoryImpl {
        double lastSleepEndTime;
        double totalSleepTime;
        double totalWakeTime;
        double sleepEfficiency;
        
        LifeHistoryImpl() 
            : lastSleepEndTime(0.0), totalSleepTime(0.0), totalWakeTime(0.0), sleepEfficiency(0.0) {}
    };
    LifeHistoryImpl lifeHistory_;
};

} // namespace nlm
