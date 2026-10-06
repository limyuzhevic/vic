#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Enhanced checkpoint persistence methods
    
    /**
     * Get comprehensive brain checkpoint data including all neural systems and their states
     * Collects all neural components for checkpointing with validation and integrity checks
     */
    CheckpointData getBrainCheckpointData() const;
    
    /**
     * Restore complete brain state from checkpoint data
     * Reinitializes all neural systems with saved state
     */
    bool restoreBrainState(const CheckpointData& data);
    
    /**
     * Validate checkpoint data integrity
     * Performs comprehensive validation of checkpoint data structure and content
     */
    bool validateCheckpointIntegrity(const CheckpointData& data) const;
    
    /**
     * Create brain-specific checkpoint metadata
     * Generates metadata specific to brain checkpoint for versioning and compatibility
     */
    std::vector<uint8_t> createBrainSpecificMetadata(const CheckpointData& data) const;
    
    // Enhanced checkpoint management methods
    
    /**
     * Save brain state to file (enhanced with comprehensive checkpointing)
     * Saves all neural systems and their state to checkpoint file
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file (enhanced with comprehensive restoration)
     * Restores complete brain state from checkpoint file
     */
    bool load(const std::string& filepath);
    
    /**
     * Enhanced checkpoint management with error recovery
     * Handles checkpoint loading with version compatibility and data corruption recovery
     */
    bool enhancedLoad(const std::string& filepath);
    
    /**
     * Checkpoint integrity validation
     * Comprehensive validation of checkpoint data and file structure
     */
    bool validateCheckpointIntegrity(const std::string& filepath) const;
    
    /**
     * Checkpoint restoration with error recovery
     * Restores checkpoint with comprehensive error handling
     */
    bool restoredWithErrorRecovery(const std::string& filepath);
    
    /**
     * Version compatibility check
     * Ensures checkpoint is compatible with current brain version
     */
    bool isCheckpointCompatible(const std::string& filepath) const;
    
    /**
     * Create checkpoint with incremental save support
     * Implements incremental checkpointing for performance
     */
    bool incrementalSave(const std::string& basePath, uint64_t checkpointStep);
    
    /**
     * Get checkpoint compression for large brains
     * Implements checkpoint compression for performance optimization
     */
    bool compressedSave(const std::string& filepath, CompressionLevel compression);
    
    /**
     * Handle checkpoint fragmentation for very large brains
     * Manages large checkpoints that exceed memory limits
     */
    bool fragmentedSave(const std::string& basePath, size_t maxFragmentSize);
    
    // Checkpoint system integration
    
    /**
     * Get checkpoint statistics
     * Provides information about current checkpoint state
     */
    std::string getCheckpointStats() const;
    
    /**
     * Update checkpoint manager
     * Updates checkpoint state and handles periodic saves
     */
    void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Save checkpoint immediately
     * Forces immediate checkpoint save
     */
    bool saveCheckpointImmediately(const std::string& name);
    
    /**
     * List available checkpoints
     * Returns list of available checkpoints
     */
    std::vector<CheckpointManager::CheckpointInfo> listCheckpoints() const;
    
    /**
     * Prune old checkpoints
     * Removes old checkpoints to save space
     */
    void pruneOldCheckpoints(size_t keepCount);
    
    // Checkpoint data structure for comprehensive state
    struct CheckpointData {
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        std::vector<uint8_t> memoryData;
        std::vector<uint8_t> episodicMemoryData;
        std::vector<uint8_t> associativeMemoryData;
        std::vector<uint8_t> predictionData;
        std::vector<uint8_t> plannerData;
        std::vector<uint8_t> conceptFormationData;
        std::vector<uint8_t> attentionData;
        std::vector<uint8_t> developmentData;
        NeuromodulatorCheckpoint neuromodData;
        SimulationCheckpoint simulationData;
        DevelopmentalCheckpoint developmentalData;
        RandomStateCheckpoint randomData;
        ConfigCheckpoint configData;
        
        struct NeuromodulatorCheckpoint {
            float dopamine;
            float curiosity;
            float novelty;
            float predictionError;
        };
        
        struct SimulationCheckpoint {
            uint64_t step;
            double time;
            double timestep;
            size_t totalSpikesThisStep;
            size_t totalSpikesTotal;
            bool isResting;
            size_t stepsSinceLastEpisode;
            size_t replayInterval;
            size_t consolidationInterval;
            std::vector<float> currentSensoryInput;
            size_t currentConceptId;
        };
        
        struct DevelopmentalCheckpoint {
            DevelopmentalStage stage;
        };
        
        struct RandomStateCheckpoint {
            uint64_t seed;
            std::vector<uint64_t> state;
        };
        
        struct ConfigCheckpoint {
            std::shared_ptr<Config> config;
        };
        
        struct InterRegionConnectionCheckpoint {
            RegionId sourceRegion;
            RegionId targetRegion;
            float weight;
            Delay delay;
        };
        
        struct NeuronCheckpointEntry {
            float membranePotential;
            float restingPotential;
            float threshold;
            float resetPotential;
            float leakConductance;
            FiringState firingState;
            uint32_t refractoryRemaining;
            uint32_t refractoryPeriod;
            float lastSpikeTime;
            NeuronType neuronType;
        };
        
        struct NeuronDynamicsCheckpoint {
            RegionId regionId;
            size_t neuronCount;
            std::vector<NeuronCheckpointEntry> neurons;
        };
        
        std::vector<NeuronDynamicsCheckpoint> neuronDynamicsData;
        std::vector<InterRegionConnectionCheckpoint> connectivityData;
    };
    
private:
    struct Impl;
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
