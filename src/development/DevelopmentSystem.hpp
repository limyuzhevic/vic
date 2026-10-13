#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class NeuralPlanner;
class ConceptFormation;
class SocialLearning;
class AttentionalSelection;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, structural changes,
// and comprehensive effects on all brain systems

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
    
    // Update all brain systems with developmental effects
    void updateNeuromodulation(Dopamine* dopamine, Acetylcholine* ach, 
                               Norepinephrine* ne, Serotonin* serotonin,
                               TimestepDuration dt);
    void updateMemorySystems(NeuralWorkingMemory* workingMemory,
                            NeuralEpisodicMemory* episodicMemory,
                            NeuralAssociativeMemory* associativeMemory,
                            TimestepDuration dt);
    void updateCognitiveSystems(NeuralPlanner* planner,
                               ConceptFormation* conceptFormation,
                               SocialLearning* socialLearning,
                               TimestepDuration dt);
    void updateAttentionSystem(AttentionalSelection* attention,
                              TimestepDuration dt);
    
    // Update developmental effects on neural properties
    void updateExcitabilityAndConnectivity(Brain* brain, TimestepDuration dt);
    void updateLearningAndAdaptation(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Stage-specific neuromodulatory effects
    void updateStageSpecificNeuromodulation(Dopamine* dopamine, Acetylcholine* ach,
                                            Norepinephrine* ne, Serotonin* serotonin);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
