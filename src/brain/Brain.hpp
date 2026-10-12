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
class AdvancedWorkingMemory;
class AdvancedEpisodicMemory;
class AdvancedSemanticMemory;
class AdvancedProceduralMemory;
class AdvancedAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Synaptogenesis;
class Pruning;
class Maturation;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Reward;

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
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
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
    
// ===== ADVANCED NEURAL MEMORY SYSTEMS =====
    
    // Advanced working memory with capacity limits and LRU eviction
    AdvancedWorkingMemory* getAdvancedWorkingMemory();
    
    // Advanced episodic memory with temporal gap detection and relevance weighting
    AdvancedEpisodicMemory* getAdvancedEpisodicMemory();
    
    // Advanced semantic memory with semantic network and pattern completion
    AdvancedSemanticMemory* getAdvancedSemanticMemory();
    
    // Advanced procedural memory with skill learning and adaptation
    AdvancedProceduralMemory* getAdvancedProceduralMemory();
    
    // Advanced associative memory with spreading activation and chaining
    AdvancedAssociativeMemory* getAdvancedAssociativeMemory();

    // Advanced memory system integration
    void initializeAdvancedMemorySystems();
    void updateAdvancedMemorySystems(TimestepDuration dt);
    void consolidateAdvancedMemory();
    void compressAdvancedMemory();
    void scheduleAdvancedMemoryReplay();
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // Get current sensory state from brain's sensory neurons
    // Returns the current sensory input pattern from neural activity
    const std::vector<float>& getCurrentSensoryState() const;
    
    // Get prediction system state
    float getPredictionError() const;
    float getPredictionConfidence() const;
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    Synaptogenesis* getSynaptogenesis();
    Pruning* getPruning();
    Maturation* getMaturation();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Reward - real reward computation
    Reward* getReward();
    
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
    
    // Log an experience (episodic memory)
    // Creates and stores an episodic memory from sensory input and action
    void logExperience(const class SensoryInput& sensoryInput, const class Action& action);
    
    // Get current neural activity pattern for episodic memory encoding
    std::vector<float> getCurrentNeuralActivityPattern() const;
    
    // Set the current sensory input (for logging)
    void setLastSensoryInput(const class SensoryInput& input);
    
    // Set the current action (for logging)
    void setLastAction(const class Action& action);
    
    // Get current sensory state from brain's sensory neurons
    // Returns the current sensory input pattern from neural activity
    const std::vector<float>& getCurrentSensoryState() const;
    
    // Get prediction system state
    float getPredictionError() const;
    float getPredictionConfidence() const;
    
    // Prediction system methods
    void predictNextState();
    void updatePrediction(float actual);
    void applyPredictionToEpisodicMemory();
    
    // Apply prediction-based reward signals
    void applyPredictionBasedReward();
    
    // Log prediction error for episodic memory
    void logPredictionError();
    
    // ========== CHECKPOINT SYSTEM ==========
    
    // Checkpoint system methods
    bool saveCheckpoint(const std::string& name = "");
    bool loadCheckpoint(const std::string& name);
    std::vector<nlm::CheckpointManager::CheckpointInfo> listCheckpoints() const;
    std::string getCheckpointDir() const;
    std::string getLastCheckpointPath() const;
    bool shouldSaveCheckpoint(SimulationStep currentStep) const;
    void pruneOldCheckpoints(size_t keepCount);
    
    // Checkpoint validation and error handling
    bool validateCheckpoint(const std::string& filepath) const;
    std::string getCheckpointVersion() const;
    bool isCheckpointCompatible(const std::string& filepath) const;
    
    // Checkpoint metadata
    uint64_t getCheckpointNeuronCount() const;
    uint64_t getCheckpointSynapseCount() const;
    uint64_t getCheckpointRegionCount() const;
    SimulationStep getCheckpointStep() const;
    Timestamp getCheckpointSimulationTime() const;
    
    // Checkpoint statistics
    uint64_t getCheckpointFileSize() const;
    double getCheckpointProgress() const;
    uint64_t getCheckpointChecksum() const;
    
    // Checkpoint system status
    bool isCheckpointManagerActive() const;
    uint64_t getCheckpointSaveInterval() const;
    size_t getMaxCheckpoints() const;
    bool isCheckpointCompressionEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
