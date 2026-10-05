#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain;

// Forward declarations for memory systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Forward declarations for cognition systems
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

// Forward declarations for neuromodulation systems
class Dopamine;
class Curiosity;
class PredictionError;
class Novelty;

// Forward declarations for plasticity systems
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Forward declarations for other components
class RandomGenerator;
class Config;
class CheckpointManager;
class DevelopmentSystem;

// BrainCore implementation - Phase 6: Integrated Artificial Brain
// Handles initialization, basic setup, and core configuration
struct BrainCore {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // ========== CORE INITIALIZATION ==========
    BrainCore(std::shared_ptr<Config> cfg);
    
    // Initialize random generator with seed from config
    void initializeRandomGenerator();
    
    // Initialize plasticity systems
    void initializePlasticitySystems();
    
    // Configure STDP parameters
    void configureSTDP();
    
    // Configure structural plasticity
    void configureStructuralPlasticity();
    
    // Get timestep from config
    void configureTimestep();
    
    // Get integration intervals from config
    void configureIntegrationIntervals();
    
    // Initialize checkpoint manager
    void initializeCheckpointManager();
    
    // Create regions based on configuration
    void createRegions(size_t regionCount);
    
    // Create neurons across regions
    void createNeurons(size_t neuronCount, size_t regionCount);
    
    // Initialize connectivity with random weights
    void initializeConnectivity(float connectionProbability);
    
    // Initialize spike system
    void initializeSpikeSystem();
    
    // Initialize memory systems
    void initializeMemorySystems(Brain* parent);
    
    // Initialize prediction system
    void initializePredictionSystem();
    
    // Initialize cognition systems
    void initializeCognitionSystems(Brain* parent);
    
    // Initialize neuromodulation systems
    void initializeNeuromodulationSystems(Brain* parent);
    
    // Register spike handlers for event-driven processing
    void registerSpikeHandlers(Brain* parent);
    
    // Configure checkpoint manager
    void configureCheckpointManager(Brain* parent);
};

} // namespace nlm