#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainSimulation {
public:
    explicit BrainSimulation(Brain::Impl* impl) : pImpl(impl) {}
    
    void step(SimulationStep currentStep, Timestamp currentTime, DevelopmentalStage developmentalStage);
    
private:
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime);
    void detectAndQueueSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateWorkingMemory();
    void updateNeuromodulation();
    void applyPlasticityRules(SimulationStep currentStep, Timestamp currentTime);
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep, DevelopmentalStage developmentalStage);
    void consolidateMemories(SimulationStep currentStep);
    void manageCheckpoints(SimulationStep currentStep, Timestamp currentTime);
    void collectStatistics();
    
    Brain::Impl* pImpl;
};

} // namespace nlm
