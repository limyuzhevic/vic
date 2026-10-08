#pragma once

#include "Brain.hpp"

namespace nlm {

class EnhancedWorkingMemoryIntegration {
public:
    EnhancedWorkingMemoryIntegration();
    ~EnhancedWorkingMemoryIntegration();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Perform enhanced working memory integration throughout brain loop
    void integrateThroughoutBrainLoop();

    // Get current working memory activity
    float getWorkingMemoryActivity() const;

    // Apply working memory state to brain systems
    void applyToAttentionSystem();
    void applyToPredictionSystem();
    void applyToPlanningSystem();
    void applyToConceptFormation();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Integration methods
    void integrateEnhancedSensoryInput();
    void integrateContinuousUpdates();
    void integratePredictionErrorUpdates();
    void integrateActionOutcomeUpdates();
    void integrateNeuromodulationUpdates();
    void integrateWithAttentionSystem();
    void integrateWithCognitionSystems();
    void integrateWithPredictionSystem();
    void integrateWithPlanningSystem();
    void integrateWithConceptFormation();
    void consolidateToEpisodicMemory();
};

} // namespace nlm
