#pragma once

#include "Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "performance/CheckpointManager.hpp"

namespace nlm {

// Helper functions for Brain class implementation

// Step helper methods
void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime, Brain::Impl* pImpl);
void updateNeuronDynamics(Brain::Impl* pImpl, Timestamp currentTime);
void detectAndScheduleSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);
void processImmediateSpikes(SimulationStep currentStep, Brain::Impl* pImpl);
void updateWorkingMemory(Brain::Impl* pImpl);
void applyNeuromodulationEffects(Brain::Impl* pImpl);
void applyPlasticityRules(Brain::Impl* pImpl, float plasticityMod);
void updateEpisodicMemory(Brain::Impl* pImpl);
void updatePredictionSystem(Brain::Impl* pImpl);
void updateAttentionSystem(Brain::Impl* pImpl);
void updateConceptFormation(Brain::Impl* pImpl);
void applyStructuralPlasticity(Brain::Impl* pImpl, SimulationStep currentStep);
void replayImportantMemories(Brain::Impl* pImpl, SimulationStep currentStep);
void applyDevelopmentEffects(Brain::Impl* pImpl, SimulationStep currentStep);
void consolidateMemories(Brain::Impl* pImpl, SimulationStep currentStep);
void checkpointManagement(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);

// Neuron helper methods
void injectSensoryCurrent(Brain::Impl* pImpl, const class SensoryInput& input);
void injectCurrentToSpecificNeuron(Brain::Impl* pImpl, NeuronId neuron, MembranePotential current);
void injectCurrentToNeuronType(Brain::Impl* pImpl, NeuronType type, MembranePotential current);
void applyDopamineEffects(Brain::Impl* pImpl, float dopamineLevel);

// Memory helper methods
void storeActiveNeuronsToEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep, const EpisodicMemoryItem& episode);
void processMemorySystems(Brain::Impl* pImpl);

// Spike helper methods
void registerSpikeHandlers(Brain::Impl* pImpl);
void registerDelayedSpikeHandler(Brain::Impl* pImpl);

// Plasticity helper methods
void configureSTDP(Brain::Impl* pImpl);
void configureStructuralPlasticity(Brain::Impl* pImpl);

// Statistics and logging helper methods
void logBrainStatus(Brain::Impl* pImpl);
void printRegionStats(const NeuralRegion* region, int regionIndex);

// Development helper methods
declaration DevelopmentStage calculateDevelopmentalStage(Brain::Impl* pImpl);

} // namespace nlm
