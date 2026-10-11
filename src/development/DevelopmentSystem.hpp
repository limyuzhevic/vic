#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class RandomGenerator;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class PredictionError;
class SleepManager;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes
// Provides comprehensive integration across all cognitive systems

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
    
    // === MEMORY SYSTEM INTEGRATION ===
    
    // Development effects on working memory
    void applyMemoryDevelopment(NeuralWorkingMemory* workingMemory, Brain* brain) const;
    
    // Development effects on episodic memory storage and replay
    void applyEpisodicMemoryDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain, SimulationStep currentStep) const;
    
    // Developmental changes in associative memory patterns
    void applyAssociativeMemoryDevelopment(NeuralAssociativeMemory* associativeMemory, Brain* brain) const;
    
    // Development effects on memory consolidation during sleep
    void applySleepMemoryConsolidationDevelopment(NeuralEpisodicMemory* episodicMemory, SleepManager* sleepManager, Brain* brain) const;
    
    // === PREDICTION SYSTEM INTEGRATION ===
    
    // Developmental changes in prediction accuracy and confidence
    void applyPredictionDevelopment(PredictionSystem* predictionSystem, Brain* brain) const;
    
    // Age-dependent prediction horizon and temporal extent
    void applyPredictionHorizonDevelopment(PredictionSystem* predictionSystem, Brain* brain) const;
    
    // Development effects on prediction error sensitivity
    void applyPredictionErrorDevelopment(PredictionSystem* predictionSystem, Brain* brain) const;
    
    // Integration with developmental stages for learning pace
    void applyDevelopmentalLearningPace(PredictionSystem* predictionSystem, Brain* brain) const;
    
    // === COGNITIVE SYSTEM INTEGRATION ===
    
    // Neural planner development (planning depth, horizon, confidence)
    void applyNeuralPlannerDevelopment(NeuralPlanner* neuralPlanner, Brain* brain) const;
    
    // Concept formation development (thresholds, stability requirements)
    void applyConceptFormationDevelopment(ConceptFormation* conceptFormation, Brain* brain) const;
    
    // Attentional selection development (thresholds, inhibition/excitation ratios)
    void applyAttentionalSelectionDevelopment(AttentionalSelection* attentionalSelection, Brain* brain) const;
    
    // Development effects on cognitive flexibility and adaptation
    void applyCognitiveFlexibilityDevelopment(Brain* brain) const;
    
    // === NEUROMODULATION INTEGRATION ===
    
    // Developmental changes in neuromodulator sensitivity and baseline levels
    void applyNeuromodulationDevelopment(Dopamine* dopamine, Acetylcholine* acetylcholine,
                                         Norepinephrine* norepinephrine, Serotonin* serotonin,
                                         Curiosity* curiosity, Novelty* novelty,
                                         PredictionError* predictionError, Brain* brain) const;
    
    // Age-dependent dopamine, acetylcholine, norepinephrine, serotonin effects
    void applyNeuromodulatorSpecificDevelopment(Dopamine* dopamine, Acetylcholine* acetylcholine,
                                                Norepinephrine* norepinephrine, Serotonin* serotonin,
                                                Brain* brain) const;
    
    // Development effects on stress and reward processing
    void applyStressRewardDevelopment(Brain* brain) const;
    
    // Integration with developmental stages for neuromodulation balance
    void applyNeuromodulationBalanceDevelopment(Brain* brain) const;
    
    // === REPLAY INTEGRATION ===
    
    // Development effects on replay frequency and duration
    void applyReplayDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain, SimulationStep currentStep) const;
    
    // Age-dependent replay target selection
    void applyReplayTargetSelectionDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain) const;
    
    // Developmental changes in replay-based learning
    void applyReplayBasedLearningDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain) const;
    
    // Integration with sleep development patterns
    void applySleepReplayDevelopment(NeuralEpisodicMemory* episodicMemory, SleepManager* sleepManager, Brain* brain) const;
    
    // === CHECKPOINT INTEGRATION ===
    
    // Development effects on checkpoint saving/loading
    void applyCheckpointDevelopment(Brain* brain) const;
    
    // Age-dependent checkpoint precision
    void applyCheckpointPrecisionDevelopment(Brain* brain) const;
    
    // Development effects on persistence across sessions
    void applyPersistenceDevelopment(Brain* brain) const;
    
    // === OVERALL DEVELOPMENT COORDINATION ===
    
    // Main development update that applies all effects
    void applyAllDevelopmentEffects(Brain* brain, SimulationStep currentStep, RandomGenerator& rng, TimestepDuration dt);
    
    // Get overall developmental score (0.0-1.0) for all systems
    float getDevelopmentalMaturationScore() const;
    
    // Reset development to initial stage
    void resetToInitial();
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
