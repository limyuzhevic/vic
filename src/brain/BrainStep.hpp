// BrainStep.hpp - Main brain stepping logic
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainStep {
public:
    BrainStep(Brain* brain);
    ~BrainStep();
    
    void step(SimulationStep currentStep, Timestamp currentTime);
    
private:
    Brain* brain_;
    
    // Main brain loop with all integrated systems
    void executeMainLoop(SimulationStep currentStep, Timestamp currentTime);
    
    // Individual steps in the brain loop
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime);
    void detectSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateWorkingMemory(SimulationStep currentStep);
    void applyNeuromodulationEffects();
    void applyPlasticityRules();
    void applyHomeostaticScaling(SimulationStep currentStep);
    void updateEpisodicMemory(SimulationStep currentStep);
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void consolidateMemory(SimulationStep currentStep);
    void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
