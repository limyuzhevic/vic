#pragma once

#include <memory>
#include <string>
#include <vector>
#include "SystemIntegrationResult.hpp"

namespace nlm {

// Forward declarations
class Brain;
class NeuronId;
class SynapseId;
class RegionId;
class PopulationId;
class NeuronType;
class SynapseType;
class DevelopmentalStage;
class FiringState;
class ActionType;
class MotorCommand;
class WorldObjectType;
class Config;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class SimpleWorld;
class SelfModel;
class SocialLearning;
class SpatialRepresentation;
class CheckpointSystem;
class MemoryPool;
class EventQueue;
class SparseConnectivity;
class SIMDVectorization;
class WorkingMemory;
class EpisodicMemory;
class SemanticMemory;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Novelty;
class Curiosity;
class PredictionError;

// Memory System Verification Functions
SystemIntegrationResult verifyMemorySystem(const Brain& brain, bool detailed = false, bool includeReplayTests = false);
SystemIntegrationResult verifyEpisodicMemory(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyWorkingMemory(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifySemanticMemory(const Brain& brain, bool detailed = false);

// Cognitive Integration Verification Functions
SystemIntegrationResult verifyCognitiveIntegration(const Brain& brain, bool detailed = false, bool includePlanningTests = false);
SystemIntegrationResult verifyNeuralPlanner(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyConceptFormation(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyPredictionSystem(const Brain& brain, bool detailed = false);

// Neuromodulation Integration Verification Functions
SystemIntegrationResult verifyNeuromodulationIntegration(const Brain& brain, bool detailed = false, bool includeLearningTests = false);
SystemIntegrationResult verifyDopamine(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyAcetylcholine(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyNorepinephrine(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifySerotonin(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyNovelty(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyCuriosity(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyPredictionError(const Brain& brain, bool detailed = false);

// Performance Infrastructure Verification Functions
SystemIntegrationResult verifyPerformanceInfrastructure(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyCheckpointSystem(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyMemoryPool(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyEventQueue(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifySparseConnectivity(const Brain& brain, bool detailed = false);

// Sleep/Rest Cycle Verification Functions
SystemIntegrationResult verifySleepRestCycle(const Brain& brain, bool detailed = false, bool includeConsolidationTests = false);
SystemIntegrationResult verifySleepStageTransitions(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyMemoryConsolidationDuringSleep(const Brain& brain, bool detailed = false);

// Development Verification Functions
SystemIntegrationResult verifyDevelopment(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyDevelopmentalStageProgression(const Brain& brain, bool detailed = false);
SystemIntegrationResult verifyAgeRelatedPlasticity(const Brain& brain, bool detailed = false);

// Complete Integration Verification
SystemIntegrationResult verifyCompleteSystemIntegration(const Brain& brain, bool detailed = false);
SystemIntegrationResult testSystemIntegration(const Brain& brain, const std::string& testSuite);

} // namespace nlm
