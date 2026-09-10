// Brain step implementation header
// Phase 6 integration - split step() into focused methods

#pragma once

namespace nlm {

class Brain {
public:
    // Main simulation step methods
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
private:
    // Step 1-14 individual step methods
    void updateNeurons(Timestamp currentTime);
    void processSpikes(Timestamp currentTime, SimulationStep currentStep);
    void updateWorkingMemory();
    void updateNeuromodulation(Timestamp currentTime);
    void updatePlasticity(Timestamp currentTime);
    void updateEpisodicMemory();
    void updatePredictionSystem();
    void updateAttention();
    void updateConceptFormation();
    void updateStructuralPlasticity(Timestamp currentTime);
    void replayMemories();
    void updateDevelopment(Timestamp currentTime);
    void collectStatistics();
};

} // namespace nlm