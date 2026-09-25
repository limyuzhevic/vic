// Brain Refactoring Headers - Interface for improved modularity
// This file declares helper functions for Brain.cpp refactoring

#pragma once

#include "Brain.hpp"
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class Logger;
class ConsoleLogger;

// Brain.cpp refactoring helper functions
namespace BrainRefactor {

// Initialization helper functions
void createRegions(Brain::Impl* impl);
void createNeuronsAcrossRegions(Brain::Impl* impl, size_t neuronCount, size_t regionCount);
void initializeConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability);
void initializeMemorySystems(Brain::Impl* impl, size_t neuronCount);
void initializeCognitionSystems(Brain::Impl* impl);
void initializeNeuromodulationSystems(Brain::Impl* impl);
void initializeDevelopmentSystem(Brain::Impl* impl);
void setupSpikeHandlers(Brain::Impl* impl);
void configureCheckpointManager(Brain::Impl* impl);

// Step function helper functions  
void initializeStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
void updateNeuronDynamics(Brain::Impl* impl);
void detectAndScheduleSpikes(Brain::Impl* impl);
void processNeuronSpike(Brain::Impl* impl, Neuron* neuron, NeuralRegion* region);
void createDelayedSpikeEvent(Brain::Impl* impl, Neuron* neuron, Synapse* synapse,
                           SimulationStep currentStep, Timestamp currentTime);
void processSpikes(Brain::Impl* impl);
void updateWorkingMemory(Brain::Impl* impl);
void applyNeuromodulationEffects(Brain::Impl* impl);
void updateEpisodicMemory(Brain::Impl* impl);
void updatePredictionSystem(Brain::Impl* impl);
void updateAttentionSystem(Brain::Impl* impl);
void updateConceptFormation(Brain::Impl* impl);
void applyStructuralPlasticity(Brain::Impl* impl);
void replayImportantMemories(Brain::Impl* impl);
void applyDevelopmentEffects(Brain::Impl* impl);
void periodicMemoryConsolidation(Brain::Impl* impl);
void checkpointManagement(Brain::Impl* impl);

// Memory system helper functions
void setupSensoryNeurons(Brain::Impl* impl, NeuralRegion* region);
void setupMotorNeurons(Brain::Impl* impl, NeuralRegion* region);
void collectMemorySystemMetrics(Brain::Impl* impl);

// Action and sensory processing helper functions
void processSensoryInput(Brain::Impl* impl, const SensoryInput& input);
void injectCurrentToNeurons(Brain::Impl* impl, NeuronId neuron, MembranePotential current);
void injectCurrentToNeuronsByType(Brain::Impl* impl, NeuronType type, MembranePotential current);

// Statistics and logging helper functions
void updateBrainStatistics(Brain::Impl* impl);
void logBrainStatus(const Brain* brain);
void logMemorySystemStatus(Brain::Impl* impl);
void logNeuromodulationStatus(Brain::Impl* impl);
void logDevelopmentStatus(Brain::Impl* impl);

// Checkpoint and save/load helper functions
bool saveBrainState(const Brain* brain, const std::string& filepath);
bool loadBrainState(Brain* brain, const std::string& filepath);
void setupCheckpointManager(Brain::Impl* impl, const std::string& checkpointDir);

// Development and plasticity helper functions
void setupPlasticitySystems(Brain::Impl* impl);
void setupSTDP(Brain::Impl* impl);
void setupHebbian(Brain::Impl* impl);
void setupStructuralPlasticity(Brain::Impl* impl);
void configureDevelopmentStages(Brain::Impl* impl);

// Integration and verification helper functions
bool verifyBrainIntegration(Brain::Impl* impl);
bool testMemoryIntegration(Brain::Impl* impl);
bool testNeuromodulationIntegration(Brain::Impl* impl);
bool testCheckpointing(Brain::Impl* impl);
bool testReplay(Brain::Impl* impl);

// Utility helper functions
template<typename T>
T getConfigValue(const Config* config, const std::string& key, T defaultValue);

std::string getStepPhaseDescription(int stepNumber);
float calculateNeuromodulationFactor(const Dopamine* dopamine);
void applyDopamineEffects(Brain::Impl* impl, float dopamineLevel);

} // namespace BrainRefactor

} // namespace nlm