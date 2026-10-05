// Brain Step Handler - Manages the complete brain simulation loop
// This component handles the execution of all simulation steps in the correct order
#ifndef BRAIN_STEP_HANDLER_HPP
#define BRAIN_STEP_HANDLER_HPP

#include "Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class BrainStepHandler {
public:
    explicit BrainStepHandler(Brain* brain);
    
    // Execute one complete simulation step
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Get step statistics
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    size_t getTotalSpikesTotal() const { return totalSpikesTotal; }
    
private:
    Brain* brain;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Step execution methods
    void processPendingDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateAllNeurons(Timestamp currentTime);
    void detectSpikesAndScheduleEvents(Timestamp currentTime, SimulationStep currentStep);
    void processSpikes(SimulationStep currentStep);
    void updateWorkingMemory();
    void applyNeuromodulationEffects();
    void applyPlasticityRules(float plasticityMod);
    void updateEpisodicMemory(SimulationStep currentStep);
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayImportantMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void collectStatistics();
    void checkpointManagement(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm

#endif // BRAIN_STEP_HANDLER_HPP