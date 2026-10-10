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
    
    // Initialize development system with brain
    void initialize(Brain* brain);
    
    // Coordinate developmental effects across all systems
    void coordinateDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Stage-specific developmental effects
    void applyStageEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyInitialStageEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyMaturationEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyAdultEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyAgingEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Synaptic development (pruning and strengthening)
    void applySynapticDevelopment(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applySynapticStrengthening(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applySynapticPruning(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Neuromodulatory system development
    void developNeuromodulation(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developDopamine(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developCuriosity(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developNovelty(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Working memory and cognitive function development
    void developWorkingMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developMemoryConsolidation(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developAttention(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developLearningRates(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Behavioral strategy development
    void developBehavioralStrategies(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developExplorationVsExploitation(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developRewardSensitivity(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Ion channel and membrane property development
    void developIonChannelExpression(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void developMembraneProperties(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Plasticity development
    void developPlasticity(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
};
};

} // namespace nlm
