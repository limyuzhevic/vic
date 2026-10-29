#pragma once

#include "Brain.hpp"

namespace nlm {

// Brain Initialization module
class BrainInitialization {
public:
    static bool initializeBrain(Brain* brain);
    static void setupRegions(Brain* brain, size_t neuronCount, size_t regionCount, float connectionProbability);
    static void setupIntegratedSystems(Brain* brain, size_t neuronCount);
private:
    BrainInitialization() = default;
};

// Brain Step module  
class BrainStep {
public:
    static void executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    static void processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    static void detectAndQueueSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    static void processImmediateSpikes(Brain* brain, SimulationStep currentStep);
    static void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    static void applyNeuromodulationEffects(Brain* brain, TimestepDuration timestep);
    static void applyPlasticityRules(Brain* brain, float plasticityMod, TimestepDuration timestep);
    static void updateEpisodicMemory(Brain* brain, SimulationStep currentStep);
    static void collectStatistics(Brain* brain);
private:
    BrainStep() = default;
};

// Memory systems integration module
class BrainMemory {
public:
    static void initializeWorkingMemory(Brain* brain, size_t neuronCount);
    static void initializeEpisodicMemory(Brain* brain);
    static void initializeAssociativeMemory(Brain* brain);
    static void updateMemorySystems(Brain* brain, TimestepDuration timestep);
    static void storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activation);
    static void replayMemory(Brain* brain);
    static void consolidateMemory(Brain* brain);
private:
    BrainMemory() = default;
};

// Prediction system integration module
class BrainPrediction {
public:
    static void updatePredictionSystem(Brain* brain, TimestepDuration timestep);
    static void computePredictionError(Brain* brain);
    static void updateForwardModel(Brain* brain);
    static void integratePredictionWithAction(Brain* brain);
private:
    BrainPrediction() = default;
};

// Cognition systems integration module
class BrainCognition {
public:
    static void initializeCognitionSystems(Brain* brain);
    static void updatePlanningSystem(Brain* brain, TimestepDuration timestep);
    static void updateConceptFormation(Brain* brain, TimestepDuration timestep);
    static void updateAttentionSystem(Brain* brain, TimestepDuration timestep);
    static void applyCognitiveBiases(Brain* brain);
    static void generateMotorCommands(Brain* brain);
private:
    BrainCognition() = default;
};

// Neuromodulation systems integration module
class BrainNeuromodulation {
public:
    static void updateNeuromodulationSystems(Brain* brain, TimestepDuration timestep);
    static void updateDopamineSystem(Brain* brain, TimestepDuration timestep);
    static void updateCuriositySystem(Brain* brain, TimestepDuration timestep);
    static void updateNoveltySystem(Brain* brain, TimestepDuration timestep);
    static void updatePredictionErrorSystem(Brain* brain, TimestepDuration timestep);
    static float calculatePlasticityModulation(Brain* brain);
private:
    BrainNeuromodulation() = default;
};

// Plasticity systems integration module
class BrainPlasticity {
public:
    static void applySTDPPlasticity(Brain* brain, float plasticityMod, TimestepDuration timestep);
    static void applyHebbianPlasticity(Brain* brain, TimestepDuration timestep);
    static void applyStructuralPlasticity(Brain* brain, TimestepDuration timestep);
    static void updateSynapticStrength(Brain* brain, Synapse* synapse, TimestepDuration timestep);
    static void stabilizeNetwork(Brain* brain);
private:
    BrainPlasticity() = default;
};

// Development system integration module
class BrainDevelopment {
public:
    static void updateDevelopment(Brain* brain, TimestepDuration timestep);
    static void applyDevelopmentalChanges(Brain* brain);
    static void triggerDevelopmentalStage(Brain* brain, DevelopmentalStage stage);
    static void regulateSynaptogenesis(Brain* brain, TimestepDuration timestep);
    static void applyPruning(Brain* brain, TimestepDuration timestep);
private:
    BrainDevelopment() = default;
};

// Checkpoint system integration module
class BrainCheckpoint {
public:
    static void configureCheckpointSystem(Brain* brain);
    static bool saveBrainState(Brain* brain, const std::string& filepath) const;
    static bool loadBrainState(Brain* brain, const std::string& filepath);
    static void manageCheckpoints(Brain* brain);
    static void cleanupOldCheckpoints(Brain* brain);
private:
    BrainCheckpoint() = default;
};

// Statistics and logging module
class BrainStatistics {
public:
    static void collectAndLogStatistics(Brain* brain);
    static void logBrainStatus(const Brain* brain) const;
    static void calculateExcitatoryInhibitoryRatio(Brain* brain);
    static void trackSpikeStatistics(Brain* brain);
    static void trackMemoryStatistics(Brain* brain);
    static void trackPerformanceMetrics(Brain* brain);
private:
    BrainStatistics() = default;
};

} // namespace nlm