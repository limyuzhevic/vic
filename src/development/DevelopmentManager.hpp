#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <unordered_map>
#include <string>

namespace nlm {

// Development statistics structure
struct DevelopmentStats {
    SimulationStep totalUpdates = 0;
    SimulationStep plasticityUpdates = 0;
    SimulationStep stageTransitions = 0;
    std::unordered_map<DevelopmentalStage, SimulationStep> stageDurations;
    SimulationStep criticalPeriodProgress = 0;
    float averagePlasticityModifier = 1.0f;
    SimulationStep synapticEventsCreated = 0;
    SimulationStep synapsesPruned = 0;
    double cumulativeMaturationRate = 0.0;
    
    void reset() {
        totalUpdates = 0;
        plasticityUpdates = 0;
        stageTransitions = 0;
        stageDurations.clear();
        criticalPeriodProgress = 0;
        averagePlasticityModifier = 1.0f;
        synapticEventsCreated = 0;
        synapsesPruned = 0;
        cumulativeMaturationRate = 0.0;
    }
};

// DevelopmentManager orchestrates development system updates and effects
class DevelopmentManager {
public:
    DevelopmentManager(Brain* brain, StructuralPlasticity* structuralPlasticity, 
                      DevelopmentSystem* developmentSystem);
    ~DevelopmentManager();
    
    // Update development system and apply effects
    void update(SimulationStep currentStep, TimestepDuration dt, 
                RandomGenerator& rng, DevelopmentalStage currentStage);
    
    // Apply development effects on plasticity
    void applyPlasticityEffects(DevelopmentalStage stage, 
                               StructuralPlasticity* structuralPlasticity);
    
    // Manage developmental stage transitions
    void manageStageTransition(DevelopmentalStage currentStage, 
                               SimulationStep currentStep);
    
    // Get development statistics
    const DevelopmentStats& getStats() const { return stats_; }
    void resetStats() { stats_.reset(); }
    
    // Configuration
    void setUpdateInterval(SimulationStep interval) { updateInterval_ = interval; }
    SimulationStep getUpdateInterval() const { return updateInterval_; }
    
    // Stage progression thresholds (in simulation time)
    void setStageThresholds(double initial, double criticalPeriod, 
                           double maturation, double adult, double aging);
    
    // Get current developmental age
    double getDevelopmentalAge() const { return developmentalAge_; }
    void setDevelopmentalAge(double age) { developmentalAge_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    DevelopmentStats stats_;
    SimulationStep updateInterval_;
    double developmentalAge_;
    
    // Stage thresholds for progression
    double stageThresholds_[5]; // Initial, CriticalPeriod, Maturation, Adult, Aging
};

} // namespace nlm