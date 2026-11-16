#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Brain;
class NeuralRegion;
class NeuralPopulation;
class Neuron;
class Synapse;
class SpikeSystem;
class NeuralWorkingMemory;
class AttentionalSelection;
class PredictionSystem;
class ConceptFormation;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Serotonin;
class Norepinephrine;
class Acetylcholine;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

class BrainStepOrchestrator {
public:
    BrainStepOrchestrator(Brain* brain);
    ~BrainStepOrchestrator();
    
    // Execute the complete brain step loop
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Individual step methods for better organization and testing
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeuronDynamics(Timestamp currentTime, TimestepDuration timestep);
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateWorkingMemory(TimestepDuration timestep);
    void applyNeuromodulationEffects(TimestepDuration timestep);
    void applyPlasticityRules(float plasticityMod, TimestepDuration timestep);
    void updateEpisodicMemory(SimulationStep currentStep);
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayImportantMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void performMemoryConsolidation(SimulationStep currentStep);
    void checkpointManagement(SimulationStep currentStep, Timestamp currentTime);
    
    // Statistics collection
    void collectStatistics() const;
    
    // Configuration
    void setReplayInterval(size_t interval) { replayInterval_ = interval; }
    void setConsolidationInterval(size_t interval) { consolidationInterval_ = interval; }
    
private:
    // Helper methods for complex operations
    void processSpikes(SimulationStep currentStep);
    void applyDopamineEffects(float dopamineLevel);
    void calculateNeuromodulationFactor(float& plasticityMod);
    void applySynapticScaling(SimulationStep currentStep);
    bool shouldReplayMemories(SimulationStep currentStep) const;
    bool shouldApplyDevelopmentEffects(SimulationStep currentStep) const;
    bool shouldConsolidateMemories(SimulationStep currentStep) const;
    void storeSpikedNeuronsToWorkingMemory(Neuron* neuron);
    void updatePredictionBasedOnSensory();
    void updateConceptsFromWorkingMemory();
    void integrateWithPlanner();
    
    Brain* brain_;
    
    // State tracking
    size_t replayInterval_;
    size_t consolidationInterval_;
    bool isResting_;
    size_t stepsSinceLastEpisode_;
};

} // namespace nlm