// BrainStep.h - Brain step processing functions
// Handles the main simulation loop and step execution

#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

// Forward declarations
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;

/**
 * Step processing functions for Brain class
 * Splits Brain.cpp 1117 lines into manageable components
 */

namespace BrainStep {

/**
 * Main simulation step implementation
 * Replaces Brain::step() implementation
 */
void stepImpl(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);

/**
 * Process pending delayed spikes
 */
void processDelayedSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);

/**
 * Update all neurons (LIF dynamics)
 */
void updateNeurons(Brain::Impl* pImpl, Timestamp currentTime, TimestepDuration timestep);

/**
 * Detect spikes and schedule spike events
 */
void detectAndScheduleSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);

/**
 * Process immediate spikes
 */
void processSpikes(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Update working memory
 */
void updateWorkingMemory(Brain::Impl* pImpl, TimestepDuration timestep);

/**
 * Apply neuromodulation effects
 */
void applyNeuromodulation(Brain::Impl* pImpl, TimestepDuration timestep);

/**
 * Apply plasticity rules (STDP and Hebbian)
 */
void applyPlasticity(Brain::Impl* pImpl, TimestepDuration timestep, float plasticityMod);

/**
 * Update episodic memory
 */
void updateEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Update prediction system
 */
void updatePredictionSystem(Brain::Impl* pImpl);

/**
 * Update attention system
 */
void updateAttentionSystem(Brain::Impl* pImpl, TimestepDuration timestep);

/**
 * Update concept formation
 */
void updateConceptFormation(Brain::Impl* pImpl);

/**
 * Apply structural plasticity periodically
 */
void applyStructuralPlasticity(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Replay important memories
 */
void replayMemories(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Apply development effects
 */
void applyDevelopmentEffects(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Periodic memory consolidation
 */
void consolidateMemory(Brain::Impl* pImpl, SimulationStep currentStep);

/**
 * Checkpoint management
 */
void manageCheckpoints(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);

/**
 * Collect statistics
 */
void collectStatistics(Brain::Impl* pImpl);

/**
 * Register spike handlers for event-driven processing
 */
void registerSpikeHandlers(Brain::Impl* pImpl);

/**
 * Configure checkpoint manager
 */
void configureCheckpointManager(Brain::Impl* pImpl);

} // namespace BrainStep

// Memory system integration functions
namespace BrainMemory {

void initializeMemorySystems(Brain::Impl* pImpl);
void clearMemorySystems(Brain::Impl* pImpl);
void updateMemorySystems(Brain::Impl* pImpl, SimulationStep currentStep);

} // namespace BrainMemory

// Neuromodulation integration functions
namespace BrainNeuromodulation {

void updateNeuromodulationSystems(Brain::Impl* pImpl, TimestepDuration timestep);
void applyDopamineEffects(Brain::Impl* pImpl, float dopamineLevel);

} // namespace BrainNeuromodulation

// Cognition system integration functions
namespace BrainCognition {

void initializeCognitionSystems(Brain::Impl* pImpl);
void updateCognitionSystems(Brain::Impl* pImpl, TimestepDuration timestep);

} // namespace BrainCognition

// Development system integration functions
namespace BrainDevelopment {

void updateDevelopmentSystem(Brain::Impl* pImpl, SimulationStep currentStep, TimestepDuration timestep);
void applyDevelopmentalPlasticity(Brain::Impl* pImpl, DevelopmentalStage stage);

} // namespace BrainDevelopment

// Input/output functions
namespace BrainIO {

void receiveSensoryInputImpl(Brain::Impl* pImpl, const class SensoryInput& input);
void injectCurrentImpl(Brain::Impl* pImpl, NeuronId neuron, MembranePotential current);
void injectCurrentToNeuronsImpl(Brain::Impl* pImpl, NeuronType type, MembranePotential current);
void produceActionImpl(Brain::Impl* pImpl);
void applyNeuromodulationImpl(Brain::Impl* pImpl, const class Neuromodulator& signal);
void updatePlasticityImpl(Brain::Impl* pImpl);
void developImpl(Brain::Impl* pImpl);
void resetImpl(Brain::Impl* pImpl);

} // namespace BrainIO

// Statistics and logging functions
namespace BrainStats {

void logStatusImpl(const Brain::Impl* pImpl);
float getExcitationInhibitionRatioImpl(const Brain::Impl* pImpl);
size_t getTotalSpikeCountImpl(const Brain::Impl* pImpl);
size_t getPendingSpikeEventCountImpl(const Brain::Impl* pImpl);
size_t getTotalNeuronCountImpl(const Brain::Impl* pImpl);
size_t getTotalSynapseCountImpl(const Brain::Impl* pImpl);
size_t getActiveNeuronCountImpl(const Brain::Impl* pImpl);
size_t getFiringNeuronCountImpl(const Brain::Impl* pImpl);
float getAverageFiringRateImpl(const Brain::Impl* pImpl);

} // namespace BrainStats
