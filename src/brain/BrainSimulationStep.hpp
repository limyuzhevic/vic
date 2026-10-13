#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "BrainInitializer.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;
class NeuralRegion;
class Neuron;
class Synapse;

class BrainSimulationStep {
public:
    explicit BrainSimulationStep(std::shared_ptr<Config> config);
    ~BrainSimulationStep() = default;
    
    // Main simulation step implementation
    void executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Individual step phases (modular execution)
    void processDelayedSpikes(Brain* brain, SimulationStep step, Timestamp time);
    void updateNeurons(Brain* brain, Timestamp time, TimestepDuration dt);
    void detectAndQueueSpikes(Brain* brain, SimulationStep step, Timestamp time);
    void processImmediateSpikes(Brain* brain, SimulationStep step);
    void updateWorkingMemory(Brain* brain, TimestepDuration dt);
    void applyNeuromodulation(Brain* brain, TimestepDuration dt);
    void applyPlasticityRules(Brain* brain, Timestamp time);
    void updateEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time);
    void updatePredictionSystem(Brain* brain);
    void updateAttentionSystem(Brain* brain, TimestepDuration dt);
    void updateConceptFormation(Brain* brain);
    void applyStructuralPlasticity(Brain* brain, SimulationStep step);
    void replayMemories(Brain* brain, SimulationStep step);
    void applyDevelopmentEffects(Brain* brain, SimulationStep step);
    void consolidateMemories(Brain* brain, SimulationStep step);
    void updateCheckpoint(Brain* brain, SimulationStep step, Timestamp time);
    
    // Statistics and state management
    void resetStepStatistics();
    void incrementStepStatistics();
    
    // I/O processing
    void processSensoryInput(Brain* brain, const class SensoryInput& input);
    void storeSensoryToWorkingMemory(Brain* brain, const class SensoryInput& input);
    std::unique_ptr<class Action> generateAction(Brain* brain);
    void injectCurrentToNeuron(Brain* brain, NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current);
    
    // Persistence operations
    bool saveBrainState(Brain* brain, const std::string& filepath) const;
    bool loadBrainState(Brain* brain, const std::string& filepath);
    
    // Development operations
    void updateDevelopment(Brain* brain, SimulationStep step);
    void develop(Brain* brain);
    
    // Reset operations
    void resetBrain(Brain* brain);
    
    // Access to initialized components
    std::shared_ptr<Config> getConfig() const { return config; }
    TimestepDuration getTimestep() const { return timestep; }
    DevelopmentStage getDevelopmentalStage() const { return developmentalStage; }
    void setDevelopmentalStage(DevelopmentStage stage) { developmentalStage = stage; }

private:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Reference to the BrainInitializer for accessing initialized components
    std::unique_ptr<BrainInitializer> brainInitializer;
    
    // Simulation parameters
    TimestepDuration timestep;
    
    // Development tracking
    DevelopmentStage developmentalStage;
    
    // Phase 6: Main simulation step orchestration
    void executePhase6Integration(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Helper methods for step processing
    void processSpikeEvents(Brain* brain, SimulationStep step, Timestamp time);
    void updateNeuralDynamics(Brain* brain, Timestamp time, TimestepDuration dt);
    void handleMemoryUpdates(Brain* brain, SimulationStep step, Timestamp time);
    void handleCognitionUpdates(Brain* brain, Timestamp time, TimestepDuration dt);
    void handleDevelopmentUpdates(Brain* brain, SimulationStep step);
    void handlePersistenceOperations(Brain* brain, SimulationStep step, Timestamp time);
    
    // Neuromodulation helper methods
    void updateNovelty(Brain* brain, TimestepDuration dt);
    void updateCuriosity(Brain* brain, TimestepDuration dt);
    void updateDopamine(Brain* brain, TimestepDuration dt);
    void applyDopamineEffects(Brain* brain, float dopamineLevel);
    float calculatePlasticityModulation(Brain* brain);
    
    // Plasticity helper methods
    void applySTDPWithNeuromodulation(Brain* brain, float plasticityMod);
    void applyHebbianLearning(Brain* brain);
    
    // Memory helper methods
    void storeActiveNeuronsToEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time);
    void replayImportantMemories(Brain* brain, SimulationStep step);
    void consolidateMemory(Brain* brain, float consolidationFactor);
    
    // Checkpoint helper methods
    void configureCheckpointManager(Brain* brain);
    
    // Statistics collection
    void collectStepStatistics(Brain* brain);
    void collectSystemStatistics(Brain* brain);
};